// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//					  LESSON 215: 6DOF Linear Driver
//
//						    Written by Bob Schade, 5-1-06
//							Edited by Yongdong, 7-9-08
// ===============================================================================

/**
	\brief show linear driver of D6Joint

	Create only one d6jont in the scene, two capsule actors
	all rotation DOFs are disabled, other three translation DOFs are limited.

	Launch the tutorial, you will see two capsules forming a d6joint,
	and the lower capsule is dynamic and is moving from left to right 
	motivated by linear driver, when it hit the limit (value is 5) it will 
	move backward from right to left, and then back and forth, for details
	please see the function UpdateLinearDriverVelocity()
*/

#include <GL/glut.h>
#include <stdio.h>

#include "NxPhysics.h"
#include "CommonCode.h"
#include "Lesson215.h"
#include "Actors.h"
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
const NxU32 gNumJointConfigurations = 3;

const NxU32 gNumJointTypesOfFreedom = 3;
char* gJointMotionString[gNumJointTypesOfFreedom] = {"Locked","Limited","Free"}; 

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
NxActor* capsule1 = NULL;
NxActor* capsule2 = NULL;

// Focus actor
extern NxActor* gSelectedActor;

// Joint globals
NxD6Joint* d6Joint = NULL;

bool bReconfigureD6Joint = false;

void PrintControls()
{
	printf("\n Flight Controls:\n ----------------\n w = forward, s = back\n a = strafe left, d = strafe right\n q = up, z = down\n");
	printf("\n Force Controls:\n ---------------\n i = +z, k = -z\n j = +x, l = -x\n u = +y, m = -y\n");
	printf("\n Miscellaneous:\n --------------\n p = Pause\n r = Select Next Actor\n f = Toggle Force Mode\n b = Toggle Debug Wireframe Mode\n x = Toggle Shadows\n");
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
	DisableAKey('t');
	ProcessForceKeys();

	// Show debug wireframes
	if (bDebugWireframeMode)
	{
		if (gScene)  gDebugRenderer.renderData(*gScene->getDebugRenderable());
	}
}

void UpdateLinearDriverVelocity()
{
	if (capsule2->getGlobalPosition().x < -5.0f)
		d6Joint->setDriveLinearVelocity(NxVec3(0, 2, 0));
	else if (capsule2->getGlobalPosition().x > 5.0f)
		d6Joint->setDriveLinearVelocity(NxVec3(0, -2, 0));
}

void RenderCallback()
{
	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ProcessCameraKeys();
	SetupCamera();

	UpdateLinearDriverVelocity();

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
}

NxD6Joint* CreateD6Joint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
{
	NxD6JointDesc d6Desc;
	d6Desc.actor[0] = a0;
	d6Desc.actor[1] = a1;
	d6Desc.driveLinearVelocity = NxVec3(0, 0, 0);
	d6Desc.setGlobalAnchor(globalAnchor);
	d6Desc.setGlobalAxis(globalAxis);

	d6Desc.twistMotion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc.swing1Motion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc.swing2Motion = NX_D6JOINT_MOTION_LOCKED;

	d6Desc.linearLimit.value = 5;
	d6Desc.linearLimit.damping = 50;
	d6Desc.linearLimit.restitution = 0.8f;
	d6Desc.linearLimit.spring = 100;

	//d6Desc.xDrive.driveType = NX_D6JOINT_DRIVE_VELOCITY;
	//d6Desc.xDrive.forceLimit = 500.0f;
	d6Desc.yDrive.driveType = NX_D6JOINT_DRIVE_VELOCITY;
	d6Desc.yDrive.forceLimit = 500.0f;

	d6Desc.xMotion = NX_D6JOINT_MOTION_LIMITED;
	d6Desc.yMotion = NX_D6JOINT_MOTION_LIMITED;
	d6Desc.zMotion = NX_D6JOINT_MOTION_LIMITED;

	d6Desc.projectionMode = NX_JPM_NONE;

	return (NxD6Joint*)gScene->createJoint(d6Desc);
}

void InitializeSpecialHUD()
{
	char ds[512];

	// Add rotation title to HUD
	sprintf(ds, "ROTATION:");
	hud.AddDisplayString(ds, 0.015f, 0.87f); 

	// Add rotation motions to HUD
	sprintf(ds, "   twistMotion: %s", gJointMotionString[0]);
	hud.AddDisplayString(ds, 0.015f, 0.82f); 
	sprintf(ds, "   swing1Motion: %s", gJointMotionString[0]);
	hud.AddDisplayString(ds, 0.015f, 0.77f); 
	sprintf(ds, "   swing2Motion: %s", gJointMotionString[0]);
	hud.AddDisplayString(ds, 0.015f, 0.72f); 

	// Add translation title to HUD
	sprintf(ds, "TRANSLATION:");
	hud.AddDisplayString(ds, 0.015f, 0.67f); 

	// Add translation motions to HUD
	sprintf(ds, "   xMotion: %s", gJointMotionString[1]);
	hud.AddDisplayString(ds, 0.015f, 0.62f); 
	sprintf(ds, "   yMotion: %s", gJointMotionString[1]);
	hud.AddDisplayString(ds, 0.015f, 0.57f); 
	sprintf(ds, "   zMotion: %s", gJointMotionString[1]);
	hud.AddDisplayString(ds, 0.015f, 0.52f);
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
	//	gPhysicsSDK->setParameter(NX_VISUALIZE_ACTOR_AXES, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_JOINT_LIMITS, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_JOINT_LOCAL_AXES, 1);

	// Create the scene
	NxSceneDesc sceneDesc;
	sceneDesc.gravity               = gDefaultGravity;
	sceneDesc.simType				= NX_SIMULATION_HW;
	gScene = gPhysicsSDK->createScene(sceneDesc);	
	if(gScene == NULL)
	{
		sceneDesc.simType = NX_SIMULATION_SW;
		gScene = gPhysicsSDK->createScene(sceneDesc);	
	}

	// Create the default material
	NxMaterial* defaultMaterial = gScene->getMaterialFromIndex(0); 
	defaultMaterial->setRestitution(0.5);
	defaultMaterial->setStaticFriction(0.5);
	defaultMaterial->setDynamicFriction(0.5);

	// Create the objects in the scene
	CreateGroundPlane();

	capsule1 = CreateCapsule(NxVec3(0,5,0), 1, 0.5, 10);
	capsule1->raiseBodyFlag(NX_BF_KINEMATIC);
	capsule2 = CreateCapsule(NxVec3(0,3,0), 1, 0.5, 10);
	capsule2->raiseBodyFlag(NX_BF_DISABLE_GRAVITY);

	NxVec3 globalAnchor = NxVec3(0,5,0);
	NxVec3 globalAxis = NxVec3(0,0,-1);
	d6Joint = CreateD6Joint(capsule1, capsule2, globalAnchor, globalAxis);

	gSelectedActor = capsule2;
	gForceStrength = 50000;
	gCameraSpeed = 10;

	// Initialize HUD
	InitializeHUD();
	InitializeSpecialHUD();

	// Get the current time
	getElapsedTime();

	// Start the first frame of the simulation
	if (gScene)  StartPhysics();
}

int main(int argc, char** argv)
{
	PrintControls();
	InitGlut(argc, argv, "Lesson 215: 6DOF Linear Driver");
	InitNx();
	glutMainLoop();
	ReleaseNx();
	return 0;
}

