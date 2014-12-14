// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//			              LESSON 307: VISUAL REMOTE DEBUGGER
//
//						    Written by Bob Schade, 5-1-06
// ===============================================================================

#include <GL/glut.h>
#include <stdio.h>

#include "NxPhysics.h"
#include "CommonCode.h"
#include "Actors.h"
#include "Lesson307.h"

// Physics SDK globals
extern NxPhysicsSDK*     gPhysicsSDK;
extern NxScene*          gScene;
extern NxVec3            gDefaultGravity;

// User report globals
extern DebugRenderer     gDebugRenderer;
extern UserAllocator*	 gAllocator;
extern ErrorStream       gErrorStream;

// HUD globals
extern HUD hud;

// Force globals
extern NxVec3 gForceVec;
extern NxReal gForceStrength;
extern bool bForceMode;

// Simulation globals
extern bool bHardwareScene;
extern bool bPause;
extern bool bShadows;
extern bool bDebugWireframeMode;

// Actor globals
NxActor* groundPlane = 0;
extern NxActor* box;
extern NxActor* sphere;
extern NxActor* capsule;
NxActor* pyramid = 0;

extern bool bSelectEnable;
extern bool bSetFocusPosEnable;
extern NxActor* gSelectedActor;

NxD6Joint* pD6Joint = 0;

class MyJointNotify : public NxUserNotify
{
public:
	virtual void onWake(NxActor** actors, NxU32 count)
	{
		hud.SetDisplayString(3, "actor is awake", 0.05f, 0.88f);
	}
	virtual void onSleep(NxActor** actors, NxU32 count)
	{
		hud.SetDisplayString(3, "actor is sleeping", 0.05f, 0.88f);
	}

	virtual bool onJointBreak(NxReal breakingImpulse, NxJoint& brokenJoint)
	{
		hud.SetDisplayString(2, "Joint broken!", 0.05f, 0.92f);
		return true;
	}
} gJointNotify;

void PrintControls()
{
	printf("\n Flight Controls:\n ----------------\n w = forward, s = back\n a = strafe left, d = strafe right\n q = up, z = down\n");
    printf("\n Force Controls:\n ---------------\n i = +z, k = -z\n j = +x, l = -x\n u = +y, m = -y\n");
	printf("\n Miscellaneous:\n --------------\n p = Pause\n f = Toggle Force Mode\n b = Toggle Debug Wireframe Mode\n x = Toggle Shadows\n");
}

void RenderActors(bool shadows)
{
    // Render all the actors in the scene
    NxU32 nbActors = gScene->getNbActors();
    NxActor** actors = gScene->getActors();
    while (nbActors--)
    {
        NxActor* actor = *actors++;
	    DrawActor(actor, gSelectedActor, true);

        // Handle shadows
        if (shadows)
        {
			DrawActorShadow(actor, true);
        }
    }
}

void ProcessInputs()
{
    ProcessForceKeys();

    // Show debug wireframes
	if (bDebugWireframeMode)
	{
		if (gScene)  gDebugRenderer.renderData(*gScene->getDebugRenderable());
	}
}

void RenderCallback()
{
    // Clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ProcessCameraKeys();
	SetupCamera();

    if (gScene && !bPause)
	{
		GetPhysicsResults();
        ProcessInputs();
		StartPhysics();
	}

    // Display scene
 	RenderActors(bShadows);

	if (bForceMode)
		DrawForce(gSelectedActor, gForceVec, NxVec3(1,1,0));
	else
		DrawForce(gSelectedActor, gForceVec, NxVec3(0,1,1));
	gForceVec = NxVec3(0,0,0);

	// Render HUD
	hud.Render();

    glFlush();
    glutSwapBuffers();
}

void SpecialKeys(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'c': { 
		NxReal maxForce = 100.0f;
		NxReal maxTorque = 50.0f;
		if (0 != pD6Joint)
			pD6Joint->setBreakable(maxForce, maxTorque);

		hud.SetDisplayString(2, "Joints are breakable. 'u' -- break it.", 0.05f, 0.92f);
		break;
			  }
	}
}

void InitNx()
{
	// Create a memory allocator
    gAllocator = new UserAllocator;

    // Create the physics SDK
    gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, gAllocator);
	if (!gPhysicsSDK)  return;

	// Set the physics parameters
	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.01);

	// Set the debug visualization parameters
	gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_ACTOR_AXES, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_FNORMALS, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_JOINT_LOCAL_AXES, 1);

    // Create the scene
    NxSceneDesc sceneDesc;
    sceneDesc.gravity = gDefaultGravity;
	sceneDesc.simType = NX_SIMULATION_HW;

    gScene = gPhysicsSDK->createScene(sceneDesc);	
	if(0 == gScene)
	{ 
		sceneDesc.simType = NX_SIMULATION_SW; 
		gScene = gPhysicsSDK->createScene(sceneDesc);  
		if(0 == gScene) return;
	}
	gScene->setUserNotify(&gJointNotify);

	// Create the default material
	NxMaterial* defaultMaterial = gScene->getMaterialFromIndex(0); 
	defaultMaterial->setRestitution(0.5);
	defaultMaterial->setStaticFriction(0.5);
	defaultMaterial->setDynamicFriction(0.5);

	// Create the objects in the scene
	groundPlane = CreateGroundPlane();

	NxActor *pCapsuleK = CreateCapsule(NxVec3(5, 5, 0), 3, 1, 1);
	pCapsuleK->raiseBodyFlag(NX_BF_KINEMATIC);
	NxActor *pCapsuleD = CreateCapsule(NxVec3(5, 3, 0), 1.5, 0.5, 1);
	pD6Joint = CreateD6CylindricalJoint(pCapsuleK, pCapsuleD, NxVec3(5, 3, 0), NxVec3(0,2,0));
	pD6Joint->setLimitPoint(NxVec3(6, 3, 0));
	pD6Joint->addLimitPlane(NxVec3(0,1,0), NxVec3(0, 3, 0) );
	pD6Joint->addLimitPlane(NxVec3(0,-1,0), NxVec3(0, 5, 0) );

	AddUserDataToActors(gScene);
	gSelectedActor = pCapsuleD;
	bSelectEnable = false;
	bSetFocusPosEnable = false;

	// Initialize HUD
	InitializeHUD();
	hud.AddDisplayString("Joints are unbreakable", 0.05f, 0.92f);
	hud.AddDisplayString("", 0.05f, 0.88f);

	// Start the first frame of the simulation
	if (gScene)  StartPhysics();
}

NxD6Joint* CreateD6CylindricalJoint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
{
	assert(0 != gScene);

	NxD6JointDesc d6Desc;
	d6Desc.actor[0] = a0;
	d6Desc.actor[1] = a1;
	d6Desc.setGlobalAnchor(globalAnchor);
	d6Desc.setGlobalAxis(globalAxis);

	d6Desc.xMotion = NX_D6JOINT_MOTION_FREE;
	d6Desc.yMotion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc.zMotion = NX_D6JOINT_MOTION_LOCKED;

	d6Desc.twistMotion = NX_D6JOINT_MOTION_FREE;
	d6Desc.swing1Motion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc.swing2Motion = NX_D6JOINT_MOTION_LOCKED;

	d6Desc.twistDrive.driveType = NX_D6JOINT_DRIVE_POSITION;
	d6Desc.twistDrive.damping =0.5;
	d6Desc.twistDrive.spring =5000;	

	d6Desc.projectionMode = NX_JPM_NONE;

	if (d6Desc.isValid())
	{
		NxJoint * pJoint = gScene->createJoint(d6Desc);
		if (0 != pJoint)
			return pJoint->isD6Joint();
	}
	return 0;
}

int main(int argc, char** argv)
{
	PrintControls();
	InitGlut(argc, argv, "Lesson 307: Notify");
    InitNx();
    glutMainLoop();
	ReleaseNx();
	return 0;
}



