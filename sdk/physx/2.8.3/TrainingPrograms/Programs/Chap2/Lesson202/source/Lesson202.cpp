// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//						      LESSON 204: JOINT SPRINGS
//
//						    Written by Bob Schade, 5-1-06
// ===============================================================================

/**
	\brief show a NxRevoluteJoint with spring enabled and a NxRevoluteJoint with 
	angular driver

	Press 'v' to switch between spring and driver, for details please refer to
	the function SpecialKeys()

	A main function to create revolute joint is CreateRevoluteJoint()
 */

#include <GL/glut.h>
#include <stdio.h>

#include "NxPhysics.h"
#include "CommonCode.h"
#include "Actors.h"
#include "Lesson202.h"

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
NxActor* box1 = NULL;
NxActor* box2 = NULL;

// Joint globals
NxRevoluteJoint* revJoint = NULL;

// Focus actor
extern NxActor* gSelectedActor;

void PrintControls()
{
	printf("\n Flight Controls:\n ----------------\n w = forward, s = back\n a = strafe left, d = strafe right\n q = up, z = down\n");
    printf("\n Force Controls:\n ---------------\n i = +z, k = -z\n j = +x, l = -x\n u = +y, m = -y\n");
	printf("\n Miscellaneous:\n --------------\n p = Pause\n r = Select Next Actor\n f = Toggle Force Mode\n b = Toggle Debug Wireframe Mode\n x = Toggle Shadows\n t = Move Focus Actor to (0,5,0)\n v = Switch Between Spring and Motor\n");
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

void ResetScene()
{
	if (gScene) {
		gScene->releaseJoint(*revJoint);
		gScene->releaseActor(*box1);
		gScene->releaseActor(*box2);

		box1 = CreateBox(NxVec3(0,5,0), NxVec3(0.5,2,1), 10);
		box1->raiseBodyFlag(NX_BF_KINEMATIC);
		box2 = CreateBox(NxVec3(0,1,0), NxVec3(0.5,2,1), 10);
		box2->raiseBodyFlag(NX_BF_DISABLE_GRAVITY);

		gSelectedActor = box2;
	}
}

NxRevoluteJoint* CreateRevoluteJoint(NxActor* a0, NxActor* a1, NxVec3 globalAnchor, NxVec3 globalAxis, bool isSpring)
{
	NxRevoluteJointDesc revDesc;
	revDesc.actor[0] = a0;
	revDesc.actor[1] = a1;
	revDesc.setGlobalAnchor(globalAnchor);
	revDesc.setGlobalAxis(globalAxis);

	if (isSpring)
	{
		revDesc.flags |= NX_RJF_SPRING_ENABLED;
		NxSpringDesc springDesc;
		springDesc.spring = 5000;
		springDesc.damper = 50;
		springDesc.targetValue = 0.5*NxPi;
		revDesc.spring = springDesc;
	}
	else // motor enabled
	{
		revDesc.flags |= NX_RJF_MOTOR_ENABLED;
		NxMotorDesc motorDesc;
		motorDesc.setToDefault();
		motorDesc.velTarget = 10;
		motorDesc.maxForce = 1000;
		motorDesc.freeSpin = true;
		revDesc.motor = motorDesc;
	}
	return (NxRevoluteJoint*)gScene->createJoint(revDesc);
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

void Pause(bool isSpring)
{
	bPause = true;
	hud.SetDisplayString(1, "Paused - Hit \"p\" to Unpause", 0.3f, 0.55f);
	if (isSpring)
		hud.SetDisplayString(2, "spring enabled.", .74f, .88f);
	else 
		hud.SetDisplayString(2, "motor enabled.", .74f, .88f);/* motor enabled */
}

#define MAX_KEYS 256
extern bool gKeys[MAX_KEYS];
/**
\brief process key 'x'

press key 'x' to switch between spring and motor
by default the revolute joint has spring enabled, 
press 'x' to switch to motor enabled, at the same time,
disable spring for the revolute joint; press 'x' again
will switch to spring again.
*/
void SpecialKeys(unsigned char key, int x, int y)
{
	gKeys[key] = true;

	switch (key)
	{
	case 'v': 
		{ 
			{
				NxVec3 globalAnchor = NxVec3(0.5,5,-1);
				NxVec3 globalAxis = NxVec3(0,0,-1);
				NxU32 flags = revJoint->getFlags();
				if (flags & NX_RJF_SPRING_ENABLED)
				{
					ResetScene();
					revJoint = CreateRevoluteJoint(box1, box2, globalAnchor, globalAxis, false);/* motor enabled */
					Pause(false);/* pause for motor joint */
				}
				else if (flags & NX_RJF_MOTOR_ENABLED)
				{
					ResetScene();
					revJoint = CreateRevoluteJoint(box1, box2, globalAnchor, globalAxis, true);/* spring enabled */
					Pause(true);/* pause for spring joint */
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
	groundPlane = CreateGroundPlane();

	box1 = CreateBox(NxVec3(0,5,0), NxVec3(0.5,2,1), 10);
	box1->raiseBodyFlag(NX_BF_KINEMATIC);
	box2 = CreateBox(NxVec3(0,1,0), NxVec3(0.5,2,1), 10);
	box2->raiseBodyFlag(NX_BF_DISABLE_GRAVITY);

	NxVec3 globalAnchor = NxVec3(0.5,5,-1);
	NxVec3 globalAxis = NxVec3(0,0,-1);
	revJoint = CreateRevoluteJoint(box1, box2, globalAnchor, globalAxis, true);
	
	gSelectedActor = box2;
	gForceStrength = 250000;
	bPause = true;

	// Initialize HUD
	InitializeHUD();
	hud.AddDisplayString("spring enabled.", 0.74f, 0.88f);

	// Get the current time
	getElapsedTime();

	// Start the first frame of the simulation
	if (gScene)  StartPhysics();
}

int main(int argc, char** argv)
{
	PrintControls();
	InitGlut(argc, argv, "Lesson 202: Joint Springs and Motors");
    InitNx();
    glutMainLoop();
	ReleaseNx();
	return 0;
}

