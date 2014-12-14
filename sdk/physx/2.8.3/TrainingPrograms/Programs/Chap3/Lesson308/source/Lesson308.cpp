// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//		                    LESSON 308: OUTPUT STREAM
//
//						    Written by Yang Liu, 7-22-08
// ===============================================================================

#include <GL/glut.h>
#include <stdio.h>

#include "NxPhysics.h"
#include "CommonCode.h"
#include "Actors.h"
#include "Lesson308.h"

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
NxActor* box = 0;
NxActor* sphere  = 0;
NxActor* capsule = 0;
NxActor* pyramid = 0;

extern NxActor* gSelectedActor;

bool bCreateWarning = false;
bool bCreateError = false;

void PrintControls()
{
	printf("\n Flight Controls:\n ----------------\n w = forward, s = back\n a = strafe left, d = strafe right\n q = up, z = down\n");
    printf("\n Force Controls:\n ---------------\n i = +z, k = -z\n j = +x, l = -x\n u = +y, m = -y\n");
	printf("\n Miscellaneous:\n --------------\n p = Pause\n r = Select Next Actor\n f = Toggle Force Mode\n b = Toggle Debug Wireframe Mode\n x = Toggle Shadows\n t = Move Focus Actor to (0,5,0)\n");
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

	if (bCreateWarning)
	{
		CreateWarning();
		bCreateWarning = false;
	}
	if (bCreateError)
	{
		CreateError();
		bCreateError = false;
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
	case 'v': {	
		bCreateWarning = true;
		break; }
	case 'c': {
		bCreateError = true;
		break; }
	default: { break; }
	}
}

void InitNx()
{
	// Create a memory allocator
    gAllocator = new UserAllocator;

    // Create the physics SDK
    gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, gAllocator, &gErrorStream);
	if (!gPhysicsSDK)  return;

	// Set the physics parameters
	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.01);

	// Set the debug visualization parameters
	gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_ACTOR_AXES, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_FNORMALS, 1);

    // Create the scene
    NxSceneDesc sceneDesc;
    sceneDesc.gravity = gDefaultGravity;
    sceneDesc.simType = NX_SIMULATION_HW;
    gScene = gPhysicsSDK->createScene(sceneDesc);	
	if(!gScene)
	{ 
		sceneDesc.simType = NX_SIMULATION_SW; 
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

	box = CreateBox(NxVec3(5,0,0), NxVec3(0.5,1,0.5), 20);
	sphere = CreateSphere(NxVec3(0,0,5), 1, 10);
	capsule = CreateCapsule(NxVec3(-5,0,0), 2, 0.5, 10);
	pyramid = CreatePyramid(NxVec3(0,0,0), NxVec3(1,0.5,1.5), 10);
	gSelectedActor = pyramid;
	AddUserDataToActors(gScene);

	// Initialize HUD
	InitializeHUD();
	hud.AddDisplayString("'v' -- create warning", 0.05f, 0.92f);
	hud.AddDisplayString("'c' -- create error", 0.05f, 0.88f);
	bCreateWarning = false;
	bCreateError = false;

	// Get the current time
	getElapsedTime();

	// Start the first frame of the simulation
	if (gScene)  StartPhysics();
}

void CreateError()
{
	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions.set(2, 2, 2);
	boxDesc.localPose.t = NxVec3(0, 2, 0);

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&boxDesc);
	actorDesc.globalPose.t = NxVec3(7, 0, 0);

	NxBodyDesc bodyDesc;
	actorDesc.body = &bodyDesc;
	actorDesc.density = -2;
	gScene->createActor(actorDesc);
}

void CreateWarning()
{
	NxBoxShapeDesc boxDesc1;
	boxDesc1.dimensions.set(2, 2, 2);
	boxDesc1.localPose.t = NxVec3(0, 2, 0);

	NxBoxShapeDesc boxDesc2;
	boxDesc2.dimensions.set(1, 1, 1);
	boxDesc2.localPose.t = NxVec3(1, 1, 0);

	NxActorDesc actorDesc;
	actorDesc.globalPose.t = NxVec3(7, 0, 0);
	actorDesc.shapes.pushBack(&boxDesc1);
	actorDesc.shapes.pushBack(&boxDesc2);

	NxBodyDesc bodyDesc;
	actorDesc.body = 0;
	gScene->createActor(actorDesc);
}

int main(int argc, char** argv)
{
	PrintControls();
	InitGlut(argc, argv, "Lesson 308: Output Stream");
    InitNx();
    glutMainLoop();
	ReleaseNx();
	return 0;
}
