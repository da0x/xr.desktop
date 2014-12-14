// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//						   LESSON 209: CYLINDRICAL JOINTS
//
//						    Written by Bob Schade, 5-1-06
// ===============================================================================

/**
	\brief show cylindrical and fixed joint

	By default the tutorial will show a cylindrical joint, press key 'v'
	to switch to a fixed joint.

	Tow functions to create joint are:
	CreateFixedJoint
	CreateCylindricalJoint

	For key handling logic please refer to function SpecialKeys()
 */

#include <GL/glut.h>
#include <stdio.h>

#include "NxPhysics.h"
#include "CommonCode.h"
#include "Actors.h"
#include "Lesson205.h"

// Physics SDK globals
extern NxPhysicsSDK*     gPhysicsSDK;
extern NxScene*          gScene;
extern NxVec3            gDefaultGravity;

// User report globals
extern DebugRenderer     gDebugRenderer;
extern UserAllocator*	 gAllocator;

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
static NxActor* groundPlane;
NxActor* capsule1 = NULL;
NxActor* capsule2 = NULL;

// Joint globals
NxCylindricalJoint* cylJoint = NULL;
NxFixedJoint*		fxJoint = NULL;

// Focus actor
extern NxActor* gSelectedActor;

void PrintControls()
{
	printf("\n Flight Controls:\n ----------------\n w = forward, s = back\n a = strafe left, d = strafe right\n q = up, z = down\n");
    printf("\n Force Controls:\n ---------------\n i = +z, k = -z\n j = +x, l = -x\n u = +y, m = -y\n");
	printf("\n Miscellaneous:\n --------------\n p = Pause\n r = Select Next Actor\n f = Toggle Force Mode\n b = Toggle Debug Wireframe Mode\n x = Toggle Shadows\n t = Move Focus Actor to (0,5,0)\n v = Switch Between Fixed Joint and Cylindrical Joint\n");
}

void RenderActors(bool shadows)
{
    // Render all the actors in the scene
    NxU32 nbActors = gScene->getNbActors();
    NxActor** actors = gScene->getActors();
    while (nbActors--)
    {
        NxActor* actor = *actors++;
        DrawActor(actor, gSelectedActor, false);

        // Handle shadows
        if (shadows)
        {
			DrawActorShadow(actor, false);
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

NxFixedJoint* CreateFixedJoint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
{
	NxFixedJointDesc fixedDesc;
	fixedDesc.actor[0] = a0;
	fixedDesc.actor[1] = a1;
	fixedDesc.setGlobalAnchor(globalAnchor);
	fixedDesc.setGlobalAxis(globalAxis);

	return (NxFixedJoint*)gScene->createJoint(fixedDesc);
}

NxCylindricalJoint* CreateCylindricalJoint(NxActor* a0, NxActor* a1, NxVec3 globalAnchor, NxVec3 globalAxis)
{
	NxCylindricalJointDesc cylDesc;
	cylDesc.actor[0] = a0;
	cylDesc.actor[1] = a1;
	cylDesc.setGlobalAnchor(globalAnchor);
	cylDesc.setGlobalAxis(globalAxis);

	NxJoint* joint = gScene->createJoint(cylDesc);

	joint->setLimitPoint(globalAnchor);

	// Add up-down limiting planes
	joint->addLimitPlane(-globalAxis, globalAnchor + 1*globalAxis);
	joint->addLimitPlane(globalAxis, globalAnchor - 1*globalAxis);

	return (NxCylindricalJoint*)joint;
}


void ResetScene()
{
	if (gScene) {
		if (cylJoint){
			gScene->releaseJoint(*cylJoint);
			cylJoint = NULL;
		}
		else if (fxJoint){
			gScene->releaseJoint(*fxJoint);
			fxJoint = NULL;
		}
		gScene->releaseActor(*capsule1);
		gScene->releaseActor(*capsule2);

		capsule1 = CreateCapsule(NxVec3(0,5,0), 4, 0.85, 10);
		capsule1->raiseBodyFlag(NX_BF_KINEMATIC);
		capsule2 = CreateCapsule(NxVec3(0,3,0), 3, 0.5, 10);
		capsule2->setLinearDamping(0.5);
		capsule2->setAngularDamping(0.5);

		gSelectedActor = capsule2;
	}
}

void Pause(bool isSpring)
{
	bPause = true;
	hud.SetDisplayString(1, "Paused - Hit \"p\" to Unpause", 0.3f, 0.55f);
	if (cylJoint)
		hud.SetDisplayString(2, "Cylindrical Joint.", .74f, .88f);
	else if (fxJoint)
		hud.SetDisplayString(2, "Fixed Joint.", .74f, .88f);/* motor enabled */
}

#define MAX_KEYS 256
extern bool gKeys[MAX_KEYS];

void SpecialKeys(unsigned char key, int x, int y)
{
	gKeys[key] = true;

	switch (key)
	{
	case 'v': 
		{ 
			{
				NxVec3 globalAnchor = NxVec3(0,7,0);
				NxVec3 globalAxis = NxVec3(0,1,0);
				if (cylJoint)
				{
					ResetScene();
					fxJoint = CreateFixedJoint(capsule1, capsule2, globalAnchor, globalAxis);
					Pause(false);
				}
				else if (fxJoint)
				{
					ResetScene();
					cylJoint = CreateCylindricalJoint(capsule1, capsule2, globalAnchor, globalAxis);
					Pause(true);
				}

			}
			break; 
		}
	}

	gKeys[key] = false;
}

void InitNx()
{
    // Create the physics SDK
    gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION);
    if (!gPhysicsSDK)  return;

	// Set the physics parameters
	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.01);

	// Set the debug visualization parameters
	gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_ACTOR_AXES, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_JOINT_LIMITS, 1);

    // Create the scene
    NxSceneDesc sceneDesc;
    sceneDesc.gravity               = gDefaultGravity;
    sceneDesc.simType				= NX_SIMULATION_HW;
    gScene = gPhysicsSDK->createScene(sceneDesc);	
 if(!gScene){ 
		sceneDesc.simType				= NX_SIMULATION_SW; 
		gScene = gPhysicsSDK->createScene(sceneDesc);  
		if(!gScene) return;
	}

	// Create the default material
	NxMaterial* defaultMaterial = gScene->getMaterialFromIndex(0); 
	defaultMaterial->setRestitution(0.5);
	defaultMaterial->setStaticFriction(0.5);
	defaultMaterial->setDynamicFriction(0.5);

	// Create the objects in the scene
	groundPlane = CreateGroundPlane();

	capsule1 = CreateCapsule(NxVec3(0,5,0), 4, 0.85, 10);
	capsule1->raiseBodyFlag(NX_BF_KINEMATIC);
	capsule2 = CreateCapsule(NxVec3(0,3,0), 3, 0.5, 10);
	capsule2->setLinearDamping(0.5);
	capsule2->setAngularDamping(0.5);

	NxVec3 globalAnchor = NxVec3(0,7,0);
	NxVec3 globalAxis = NxVec3(0,1,0);
	cylJoint = CreateCylindricalJoint(capsule1, capsule2, globalAnchor, globalAxis);

	gSelectedActor = capsule2;
	bPause = true;

	// Initialize HUD
	InitializeHUD();
	hud.AddDisplayString("Cylindrical Joint.", 0.74f, 0.88f);

	// Get the current time
	getElapsedTime();

	// Start the first frame of the simulation
	if (gScene)  StartPhysics();
}

int main(int argc, char** argv)
{
	PrintControls();
	InitGlut(argc, argv, "Lesson 205: Cylindrical and Fixed Joints");
    InitNx();
    glutMainLoop();
	ReleaseNx();
	return 0;
}
