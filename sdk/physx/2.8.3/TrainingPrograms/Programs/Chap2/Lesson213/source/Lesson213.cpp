// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//			         LESSON 213: 6DOF Motion Controller
//
//						    Written by Bob Schade, 5-1-06
//							Edited by Yongdong, 7-9-08
// ===============================================================================

// SIMULATE ABILITY TO SWITCH FROM ONE TYPE OF JOINT TO ANOTHER...

#include <GL/glut.h>
#include <stdio.h>

#include "NxPhysics.h"
#include "CommonCode2.h"
#include "Lesson213.h"

// Physics SDK globals
extern NxPhysicsSDK*     gPhysicsSDK;
extern NxScene*          gScene;
extern NxVec3            gDefaultGravity;

// User report globals
extern DebugRenderer     gDebugRenderer;
extern UserAllocator*	 gAllocator;

// HUD globals
extern HUD hud;
NxU32 gJointType = 0;
const NxU32 gNumJointConfigurations = 6;
char* gJointTypeString[gNumJointConfigurations] = {"Translation Limited","Translation Soft Limited",
												   "Rotation Limited","Rotation Soft Limited",
												   "Translation Motored", "Rotation Motored"};
const NxU32 gNumJointDegreesOfFreedom = 6;
NxD6JointMotion gJointMotion[gNumJointDegreesOfFreedom] = 
{ 
    NX_D6JOINT_MOTION_LIMITED,
    NX_D6JOINT_MOTION_LIMITED,
    NX_D6JOINT_MOTION_LIMITED,
    NX_D6JOINT_MOTION_LOCKED,
    NX_D6JOINT_MOTION_LOCKED,
    NX_D6JOINT_MOTION_LOCKED
};
const NxU32 gNumJointTypesOfFreedom = 3;
char* gJointMotionString[gNumJointTypesOfFreedom] = {"Locked","Limited","Free"};
char* gOnOffString[2] = {"Off","On"};

// Camera globals
extern NxReal gCameraSpeed;

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
NxActor* capsule1;
NxActor* capsule2;

NxActor* ball;


NxActor* box;
NxActor* sphere;
NxActor* capsule;
NxActor* pyramid;

// Focus actor
extern NxActor* gSelectedActor;

// Joint globals
NxD6Joint* d6Joint = NULL;

bool bReconfigureD6Joint = false;


extern bool bForceMode;

NxVec3 gJointStartPos = NxVec3(0,5,0);
NxVec3 gBallStartPos = NxVec3(3,5,0);

void PrintControls()
{
	printf("\n Flight Controls:\n ----------------\n w = forward, s = back\n a = strafe left, d = strafe right\n q = up, z = down\n");
    printf("\n Force Controls:\n ---------------\n i = +z, k = -z\n j = +x, l = -x\n u = +y, m = -y\n");
	printf("\n Miscellaneous:\n --------------\n p = Pause\n r = Select Next Actor\n f = Toggle Force Mode\n b = Toggle Debug Wireframe Mode\n x = Toggle Shadows\n");
	printf("\n Special:\n --------------\n t = Move Focus Actor to (0,5,0)\n");
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

void MatchTarget()
{
    NxD6JointDesc d6Desc;

	d6Joint->saveToDesc(d6Desc);

//    d6Desc.xDrive.driveType = NX_D6JOINT_DRIVE_POSITION | NX_D6JOINT_DRIVE_VELOCITY;
    d6Desc.xDrive.driveType = NX_D6JOINT_DRIVE_POSITION;
	d6Desc.xDrive.spring = 200;  // FLT_MAX
	d6Desc.xDrive.damping = 10;
#if 0
	d6Desc.xDrive.driveType = NX_D6JOINT_DRIVE_VELOCITY;
    d6Desc.xDrive.forceLimit = FLT_MAX;
#endif  // 0

//    d6Desc.yDrive.driveType = NX_D6JOINT_DRIVE_POSITION | NX_D6JOINT_DRIVE_VELOCITY;
    d6Desc.yDrive.driveType = NX_D6JOINT_DRIVE_POSITION;
	d6Desc.yDrive.spring = 200;  // FLT_MAX
	d6Desc.yDrive.damping = 10;
#if 0
    d6Desc.yDrive.driveType = NX_D6JOINT_DRIVE_VELOCITY;
    d6Desc.yDrive.forceLimit = FLT_MAX;
#endif  // 0

//    d6Desc.xDrive.driveType = NX_D6JOINT_DRIVE_POSITION | NX_D6JOINT_DRIVE_VELOCITY;
    d6Desc.zDrive.driveType = NX_D6JOINT_DRIVE_POSITION;
	d6Desc.zDrive.spring = 200;  // FLT_MAX
	d6Desc.zDrive.damping = 10;
#if 0
    d6Desc.zDrive.driveType = NX_D6JOINT_DRIVE_VELOCITY;
    d6Desc.zDrive.forceLimit = FLT_MAX;
#endif  // 0

	NxVec3 pos = ball->getGlobalPosition();
	NxVec3 vel = ball->getLinearVelocity();

    NxVec3 controlPos = pos - gBallStartPos;
	controlPos = NxVec3(controlPos.y, controlPos.z, controlPos.x);
	d6Desc.drivePosition.set(controlPos);

//	NxVec3 controlVel = NxVec3(vel.y, vel.z, vel.x);
//	d6Desc.driveLinearVelocity.set(controlVel);

	d6Desc.flags = NX_D6JOINT_SLERP_DRIVE;
//    d6Desc.slerpDrive.driveType = NX_D6JOINT_DRIVE_POSITION | NX_D6JOINT_DRIVE_VELOCITY;
    d6Desc.slerpDrive.driveType = NX_D6JOINT_DRIVE_POSITION;
//    d6Desc.slerpDrive.driveType = NX_D6JOINT_DRIVE_VELOCITY;
//    d6Desc.slerpDrive.forceLimit = FLT_MAX;
	d6Desc.slerpDrive.spring = 200;  // FLT_MAX
	d6Desc.slerpDrive.damping = 10;

	NxMat33 orient = ball->getGlobalOrientation();
	NxMat33 controlOrient;
	controlOrient.setRow(0, orient.getRow(1));
	controlOrient.setRow(1, orient.getRow(2));
	controlOrient.setRow(2, orient.getRow(0));
	NxQuat q(controlOrient);
	d6Desc.driveOrientation = q;

//	NxVec3 angVel = ball->getAngularVelocity();
//	d6Desc.driveAngularVelocity.set(angVel);

	d6Joint->loadFromDesc(d6Desc);
}

void ProcessInputs()
{
    ProcessMoveKeys();

	MatchTarget();

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
//	    case 't': { bReconfigureD6Joint = true; break; }
	//	case 'g': { bToggleLowerActorGravity = true; break; }
	}
}

NxD6Joint* CreateD6Joint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
{
	NxD6JointDesc d6Desc;
	d6Desc.actor[0] = a0;
	d6Desc.actor[1] = a1;
	d6Desc.setGlobalAnchor(globalAnchor);
	d6Desc.setGlobalAxis(globalAxis);

	d6Desc.twistMotion = NX_D6JOINT_MOTION_FREE;
	d6Desc.swing1Motion = NX_D6JOINT_MOTION_FREE;
	d6Desc.swing2Motion = NX_D6JOINT_MOTION_FREE;

	d6Desc.xMotion = NX_D6JOINT_MOTION_FREE;
	d6Desc.yMotion = NX_D6JOINT_MOTION_FREE;
	d6Desc.zMotion = NX_D6JOINT_MOTION_FREE;

	d6Desc.projectionMode = NX_JPM_NONE;

	return (NxD6Joint*)gScene->createJoint(d6Desc);
}

NxActor* CreateFollowBall(const NxVec3& pos, const NxReal radius, const NxReal density)
{
	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// The actor has one shape, a sphere
	NxSphereShapeDesc sphereDesc;
	sphereDesc.radius = radius;
	actorDesc.shapes.pushBack(&sphereDesc);

	if (density)
	{
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t = pos;
	return gScene->createActor(actorDesc);	
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
	gPhysicsSDK->setParameter(NX_VISUALIZE_JOINT_LIMITS, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_JOINT_LOCAL_AXES, 1);

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
	CreateGroundPlane();

	// Slider Joint
	capsule1 = CreateCapsule(NxVec3(0,5,0), 1, 0.5, 10);
	capsule1->raiseBodyFlag(NX_BF_KINEMATIC);
	capsule2 = CreateCapsule(NxVec3(0,3,0), 1, 0.5, 10);
    capsule2->raiseBodyFlag(NX_BF_DISABLE_GRAVITY);

	NxVec3 globalAnchor = gJointStartPos;
	NxVec3 globalAxis = NxVec3(0,1,0);
	d6Joint = CreateD6Joint(capsule1, capsule2, globalAnchor, globalAxis);

	ball = CreateFollowBall(gBallStartPos,0.25,1);
	ball->raiseBodyFlag(NX_BF_KINEMATIC);

	NxMat33 orient = ball->getGlobalOrientation();
	NxMat33 controlOrient;
	controlOrient.setRow(0, orient.getRow(2));
	controlOrient.setRow(1, orient.getRow(0));
	controlOrient.setRow(2, orient.getRow(1));
	ball->setGlobalOrientation(controlOrient);

	gSelectedActor = ball;
	gForceStrength = 1500;
	gCameraSpeed = 10;

	box = CreateBox(NxVec3(3,0,0), NxVec3(0.5,1,0.5), 20);
	sphere = CreateSphere(NxVec3(0,0,3), 1, 10);
	capsule = CreateCapsule(NxVec3(-3,0,0), 2, 0.5, 10);
	pyramid = CreatePyramid(NxVec3(0,0,-3), NxVec3(0.75,1.5,0.75), 10);

	box->setLinearDamping(0.5);
	sphere->setLinearDamping(0.5);
	capsule->setLinearDamping(0.5);
	pyramid->setLinearDamping(0.5);

	AddUserDataToActors(gScene);

	// Initialize HUD
	InitializeHUD();

	// Get the current time
	getElapsedTime();

	// Start the first frame of the simulation
	if (gScene)  StartPhysics();
}

int main(int argc, char** argv)
{
	PrintControls();
	InitGlut(argc, argv, "Lesson 213: 6DOF Motion Controller");
    InitNx();
    glutMainLoop();
	ReleaseNx();
	return 0;
}
