// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//			                    LESSON 503: CORE DUMP
//
//                          Written by Bob Schade, 5-1-06
// ===============================================================================

#include <GL/glut.h>
#include <stdio.h>

#include "NxPhysics.h"
#include "CommonCode.h"
#include "Actors.h"
#include "Lesson503.h"
#include "MediaPath.h"

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

// Data file
char*fnameCD="MyCoreDump.xml";

// Actor globals

extern NxActor* gSelectedActor;


bool bCoreDump = false;
bool bLoadCore = false;
bool bInstantiateCore = false;

NXU::NxuPhysicsCollection* gPhysicsCollection = NULL;

// Core dump globals
NxPhysicsSDK* CreatePhysics()
{
    NxPhysicsSDK* pSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, gAllocator);
	if (!pSDK)  return NULL;

	// Set the physics parameters
	pSDK->setParameter(NX_SKIN_WIDTH, 0.01);

	// Set the debug visualization parameters
	pSDK->setParameter(NX_VISUALIZATION_SCALE, 1);
	pSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
	pSDK->setParameter(NX_VISUALIZE_ACTOR_AXES, 1);
	pSDK->setParameter(NX_VISUALIZE_COLLISION_FNORMALS, 1);
	return pSDK;
}

NXU::NxuPhysicsCollection* LoadCoreDump(const char * strFileName, NXU::NXU_FileType fileType = NXU::FT_XML)
{
	return NXU::loadCollection(strFileName, fileType);
}


bool InstantiateCoreDump(NxPhysicsSDK* pSDK, NXU::NxuPhysicsCollection* c)
{
	class MyNotify : public NXU_userNotify
	{
	};
	bool success = false;
	MyNotify notify;
	success = NXU::instantiateCollection( c, *pSDK, NULL, NULL, &notify );
	//NXU::releaseCollection(c);
	return success;
}

void PrintControls()
{
	printf("\n Flight Controls:\n ----------------\n w = forward, s = back\n a = strafe left, d = strafe right\n q = up, z = down\n");
    printf("\n Force Controls:\n ---------------\n i = +z, k = -z\n j = +x, l = -x\n u = +y, m = -y\n");
	printf("\n Miscellaneous:\n --------------\n p = Pause\n r = Select Next Actor\n f = Toggle Force Mode\n b = Toggle Debug Wireframe Mode\n x = Toggle Shadows\n t = Move Focus Actor to (0,5,0)\n");
	printf("\n Special:\n --------\n c = Save Core Dump\n v = Load Core Dump into container\n n = Instantiate Core Dump\n");
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

	// Core dump
	if (bCoreDump)
	{
		bool ok = NXU::coreDump(gPhysicsSDK, fnameCD, NXU::FT_XML, true, false);
		if(ok) {
			printf("Output core dump successfully!\n");
		} else {
			printf("Output core dump failed!\n");
		}
		bCoreDump = false;
	}

	// Load core to core container
	if (bLoadCore)
	{
		if(gPhysicsCollection) {
			NXU::releaseCollection(gPhysicsCollection);
			gPhysicsCollection = NULL;
		}
		gPhysicsCollection = LoadCoreDump(fnameCD);
		if(!gPhysicsCollection) {
			printf("Unable to load the core dump, please first save a core dump.\n");
		}
		else
		{
			printf("Core dump has been loaded into container.\n");
		}
		bLoadCore = false;
	}

	// instantiate a core dump
	if(bInstantiateCore) {
		if(gPhysicsCollection) {
			if(gPhysicsSDK) {
				ReleasePhysicsSDK(gPhysicsSDK);
				gPhysicsSDK = CreatePhysics();
			}
			if(InstantiateCoreDump(gPhysicsSDK, gPhysicsCollection)) {
				if(gPhysicsSDK->getNbScenes()) {
					gScene = gPhysicsSDK->getScene(0);
					AddUserDataToActors(gScene);
					NxActor** actors = gScene->getActors();
					gSelectedActor = *actors;
					while(!IsSelectable(gSelectedActor))
					{
						gSelectedActor = *actors++;
					}
				}
				printf("Core dump instantiated\n");
			}
			else
			{
				printf("Error in instantiating the core dump\n");
			}
		}
		else
		{
				printf("Unable to instantiate the core dump with an empty container\n");
		}
		bInstantiateCore = false;
	}

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
		case 'c': { printf("Coredumping...\n"); bCoreDump = true; break; }
		case 'v': { printf("Loading Core Dump to Container...\n"); bLoadCore = true; break; }
		case 'n': { printf("Instantiating Core...\n"); bInstantiateCore = true; break; }
	}
}

void InitNx()
{
	// Create a memory allocator
    gAllocator = new UserAllocator;

    // Create the physics SDK
	gPhysicsSDK = CreatePhysics();

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

    // Set Core Dump directory
	char buff[512];
	FindMediaFile(fnameCD, buff);
#ifdef WIN32
	SetCurrentDirectory(buff);
#elif LINUX
	chdir(buff);
#endif

	// Create the objects in the scene
	NxActor* groundPlane = CreateGroundPlane();

	NxActor* box = CreateBox(NxVec3(5,0,0), NxVec3(0.5,1,0.5), 20);
	NxActor* sphere = CreateSphere(NxVec3(0,0,0), 1, 10);
	NxActor* capsule = CreateCapsule(NxVec3(-5,0,0), 2, 0.5, 10);
//	pyramid = CreatePyramid(NxVec3(0,0,0), NxVec3(1,0.5,1.5), 10);

	AddUserDataToActors(gScene);

	gSelectedActor = capsule;
//	gSelectedActor = pyramid;

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
	InitGlut(argc, argv, "Lesson 503: Core Dump");
    InitNx();
    glutMainLoop();
	ReleaseNx();
	return 0;
}



