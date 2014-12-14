// ===============================================================================
//						 NVIDIA PHYSX SDK TRAINING PROGRAMS
//						 LESSON 804: USER CONTROLLED THREADS
//
//						    Written by Bob Schade, 5-1-06
//							Updated by Yang Liu, 7-1-08
// ===============================================================================

/**
	This lesson demonstrates the use of the threading interface available in
	the PhysX SDK 2.4+

	There are 4 different approaches to threading:-

		* Application supplied scheduler
		* Polling, ie the SDK manages the work queue and the application can poll a function to perform work
		* SDK managed threading (At present this is likely to yield the best performance)
		* No threading

	See below for defines to control which method this lesson uses.

	NOTE: Fine grained threading is only available (at present) on the XBox 360. While these interfaces are available 
	on other platforms, they will not provide any performance gains.

	NOTE: The graphics wrapper used on the xbox is quite slow and performance comparisons should not be made with 
	graphics enabled.
 */

#include "Lesson804.h"
#include "Timing.h"

// Physics SDK globals
NxPhysicsSDK*     gPhysicsSDK = NULL;
NxScene*          gScene = NULL;
NxVec3            gDefaultGravity(0,-9.81,0);

// User report globals
UserAllocator*    gAllocator = NULL;
ErrorStream       gErrorStream;
DebugRenderer     gDebugRenderer;
PerfRenderer      gPerfRenderer;

// HUD globals
HUD hud;

// Thread mode
bool gbThreadScheduler = false;
bool gbThreadPolling   = false;
bool gbThreadSDKManage = false;
bool gbNoThread        = false;
 
// Display globals
int gMainHandle;
int mx = 0;
int my = 0;

// Camera globals
float gCameraAspectRatio = 1;
NxVec3 gCameraPos(0,5,-15);
NxVec3 gCameraForward(0,0,1);
NxVec3 gCameraRight(-1,0,0);
const NxReal gCameraSpeed = 10;

// Force globals
NxVec3 gForceVec(0,0,0);
NxReal gForceStrength = 50000;
bool bForceMode = true;

// Keyboard globals
#define MAX_KEYS 256
bool gKeys[MAX_KEYS];

// Simulation globals
NxReal gDeltaTime = 1.0/60.0;
bool bHardwareScene = false;
bool bPause = false;
bool bShadows = true;
bool bDebugWireframeMode = false;

// Actor globals
NxActor* groundPlane = NULL;
NxActor* box = NULL;
NxActor* sphere = NULL;
NxActor* capsule = NULL;
NxActor* pyramid = NULL;

// Focus actor
NxActor* gSelectedActor = NULL;

// Thread stuff
static CustomScheduler gCustomScheduler;
static PollingThreads gPollingThreads;

void PrintControls()
{
	printf("\n Flight Controls:\n ----------------\n w = forward, s = back\n a = strafe left, d = strafe right\n q = up, z = down\n");
    printf("\n Force Controls:\n ---------------\n i = +z, k = -z\n j = +x, l = -x\n u = +y, m = -y\n");
	printf("\n Miscellaneous:\n --------------\n F10 = Restart\n p = Pause\n r = Select Next Actor\n f = Toggle Force Mode\n b = Toggle Debug Wireframe Mode\n x = Toggle Shadows\n c = Move Focus Actor to (0,5,0)\n");
	printf(" t = Thread custom scheduler\n y = Thread polling\n g = Thread SDK management\n h = No thread\n");
}

bool IsSelectable(NxActor* actor)
{
   NxShape*const* shapes = gSelectedActor->getShapes();
   NxU32 nShapes = gSelectedActor->getNbShapes();
   while (nShapes--)
   {
       if (shapes[nShapes]->getFlag(NX_TRIGGER_ENABLE)) 
	   {
		   return false;
	   }
   }
   if (actor == groundPlane)
	   return false;

   return true;
}

void SelectNextActor()
{
   NxU32 nbActors = gScene->getNbActors();
   NxActor** actors = gScene->getActors();
   for (NxU32 i = 0; i < nbActors; i++)
   {
       if (actors[i] == gSelectedActor)
       {
           NxU32 j = 1;
           gSelectedActor = actors[(i+j)%nbActors];
           while (!IsSelectable(gSelectedActor))
           {
               j++;
               gSelectedActor = actors[(i+j)%nbActors];
           }
           break;
       }
   }
}

void ProcessCameraKeys()
{
    NxReal deltaTime;

    if (bPause) deltaTime = 0.0005; else deltaTime = gDeltaTime;  

	// Process camera keys
	for (int i = 0; i < MAX_KEYS; i++)
	{	
		if (!gKeys[i])  { continue; }

		switch (i)
		{
			// Camera controls
			case 'w':{ gCameraPos += gCameraForward*gCameraSpeed*deltaTime; break; }
			case 's':{ gCameraPos -= gCameraForward*gCameraSpeed*deltaTime; break; }
			case 'a':{ gCameraPos -= gCameraRight*gCameraSpeed*deltaTime; break; }
			case 'd':{ gCameraPos += gCameraRight*gCameraSpeed*deltaTime; break; }
			case 'z':{ gCameraPos -= NxVec3(0,1,0)*gCameraSpeed*deltaTime; break; }
			case 'q':{ gCameraPos += NxVec3(0,1,0)*gCameraSpeed*deltaTime; break; }
		}
	}
}

void SetupCamera()
{
	// Setup camera
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, gCameraAspectRatio, 1.0f, 10000.0f);
	gluLookAt(gCameraPos.x,gCameraPos.y,gCameraPos.z,gCameraPos.x + gCameraForward.x, gCameraPos.y + gCameraForward.y, gCameraPos.z + gCameraForward.z, 0.0f, 1.0f, 0.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
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

void DrawForce(NxActor* actor, NxVec3& forceVec, const NxVec3& color)
{
	// Draw only if the force is large enough
	NxReal force = forceVec.magnitude();
	if (force < 0.1)  return;

	forceVec = 3*forceVec/force;

	NxVec3 pos = actor->getCMassGlobalPosition();
	DrawArrow(pos, pos + forceVec, color);
}

NxVec3 ApplyForceToActor(NxActor* actor, const NxVec3& forceDir, const NxReal forceStrength, bool forceMode)
{
	NxVec3 forceVec = forceStrength*forceDir*gDeltaTime;

	if (forceMode)
		actor->addForce(forceVec);
	else 
		actor->addTorque(forceVec);

	return forceVec;
}

void ProcessForceKeys()
{
	// Process force keys
	for (int i = 0; i < MAX_KEYS; i++)
	{	
		if (!gKeys[i])  { continue; }

		switch (i)
		{
			// Force controls
			case 'i': { gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(0,0,1),gForceStrength,bForceMode); break; }
			case 'k': { gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(0,0,-1),gForceStrength,bForceMode); break; }
			case 'j': { gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(1,0,0),gForceStrength,bForceMode); break; }
			case 'l': { gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(-1,0,0),gForceStrength,bForceMode); break; }
			case 'u': { gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(0,1,0),gForceStrength,bForceMode); break; }
			case 'm': { gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(0,-1,0),gForceStrength,bForceMode); break; }

			// Return focus actor to (0,5,0)
		    case 'c': { gSelectedActor->setGlobalPosition(NxVec3(0,5,0)); break; }
		}
	}
}

void ProcessInputs()
{
    // Print profile results (if enabled)
	gPerfRenderer.render(gScene->readProfileData(true), glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

	if (gbThreadPolling)
	{
		// We must reset the polling threads so they are ready for the next run
		gPollingThreads.ResetPollForWork();
	}

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

void ReshapeCallback(int width, int height)
{
    glViewport(0, 0, width, height);
    gCameraAspectRatio = float(width)/float(height);
}

void IdleCallback()
{
    glutPostRedisplay();
}

void KeyboardCallback(unsigned char key, int x, int y)
{
	gKeys[key] = true;

	switch (key)
	{
		case 'r': { SelectNextActor(); break; }
		default: { break; }
	}
}

void KeyboardUpCallback(unsigned char key, int x, int y)
{
	gKeys[key] = false;

	switch (key)
	{
	    case '0': { gPerfRenderer.toggleEnable(); break; }
		case 'p': { bPause = !bPause; 
					if (bPause)
						hud.SetDisplayString(1, "Paused - Hit \"p\" to Unpause", 0.3f, 0.55f);
					else
						hud.SetDisplayString(1, "", 0.0f, 0.0f);	
					getElapsedTime(); 
					break; }
		case 'x': { bShadows = !bShadows; break; }
		case 'b': { bDebugWireframeMode = !bDebugWireframeMode; break; }		
		case 'f': { bForceMode = !bForceMode; break; }
		case 27 : { exit(0); break; }

		// Switch simulation threads mode
		case 't': {	ReleaseNx();
					gbThreadScheduler = gbThreadPolling = gbThreadSDKManage = gbNoThread = false;
					gbThreadScheduler = true;
					InitNx();
					break; }
		case 'y': {	ReleaseNx();
					gbThreadScheduler = gbThreadPolling = gbThreadSDKManage = gbNoThread = false;
					gbThreadPolling = true;
					InitNx();
					break; }
		case 'g': {	ReleaseNx();
					gbThreadScheduler = gbThreadPolling = gbThreadSDKManage = gbNoThread = false;
					gbThreadSDKManage = true;
					InitNx();
					break; }
		case 'h': {	ReleaseNx();
					gbThreadScheduler = gbThreadPolling = gbThreadSDKManage = gbNoThread = false;
					gbNoThread = true;
					InitNx();
					break; }
		default : { break; }
	}
}

void SpecialCallback(int key, int x, int y)
{
	switch (key)
    {
		// Reset PhysX
		case GLUT_KEY_F10: ResetNx(); return; 
	}
}

void MouseCallback(int button, int state, int x, int y)
{
    mx = x;
    my = y;
}

void MotionCallback(int x, int y)
{
    int dx = mx - x;
    int dy = my - y;
    
    gCameraForward.normalize();
    gCameraRight.cross(gCameraForward,NxVec3(0,1,0));

    NxQuat qx(NxPiF32 * dx * 20 / 180.0f, NxVec3(0,1,0));
    qx.rotate(gCameraForward);
    NxQuat qy(NxPiF32 * dy * 20 / 180.0f, gCameraRight);
    qy.rotate(gCameraForward);

    mx = x;
    my = y;
}

void ExitCallback()
{
	ReleaseNx();
}

void InitGlut(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(512, 512);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    gMainHandle = glutCreateWindow("Lesson 804: User Controlled Threads");
    glutSetWindow(gMainHandle);
    glutDisplayFunc(RenderCallback);
    glutReshapeFunc(ReshapeCallback);
    glutIdleFunc(IdleCallback);
    glutKeyboardFunc(KeyboardCallback);
    glutKeyboardUpFunc(KeyboardUpCallback);
	glutSpecialFunc(SpecialCallback);
    glutMouseFunc(MouseCallback);
    glutMotionFunc(MotionCallback);
	MotionCallback(0,0);
	atexit(ExitCallback);

    // Setup default render states
    glClearColor(0.0f, 0.0f, 0.0f, 1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_CULL_FACE);

    // Setup lighting
    glEnable(GL_LIGHTING);
    float AmbientColor[]    = { 0.0f, 0.1f, 0.2f, 0.0f };         glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);
    float DiffuseColor[]    = { 0.2f, 0.2f, 0.2f, 0.0f };         glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);
    float SpecularColor[]   = { 0.5f, 0.5f, 0.5f, 0.0f };         glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularColor);
    float Position[]        = { 100.0f, 100.0f, -400.0f, 1.0f };  glLightfv(GL_LIGHT0, GL_POSITION, Position);
    glEnable(GL_LIGHT0);
}

void InitializeHUD()
{
	bHardwareScene = (gScene->getSimType() == NX_SIMULATION_HW);

	hud.Clear();

	// Add hardware/software to HUD
	if (bHardwareScene)
	    hud.AddDisplayString("Hardware Scene", 0.74f, 0.92f);
	else
		hud.AddDisplayString("Software Scene", 0.74f, 0.92f);

	// Add pause to HUD
	if (bPause)  
		hud.AddDisplayString("Paused - Hit \"p\" to Unpause", 0.3f, 0.55f);
	else
		hud.AddDisplayString("", 0.0f, 0.0f);

	if (gbThreadScheduler)
	{
		hud.AddDisplayString("Simulation threads custom scheduler", 0.01f, 0.92f);
	}
	else if (gbThreadPolling)
	{
		hud.AddDisplayString("Simulation threads polling", 0.01f, 0.92f);
	}
	else if (gbThreadSDKManage)
	{
		hud.AddDisplayString("Simulation threads managed by SDK", 0.01f, 0.92);
	}
	else if (gbNoThread)
	{
		hud.AddDisplayString("No simulation threads", 0.01f, 0.92f);
	}
	else
		assert(0);
}

void InitNx()
{
	// Create a memory allocator
    gAllocator = new UserAllocator;

    // Create the physics SDK
	gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, gAllocator, &gErrorStream);
	if (0 == gPhysicsSDK) return;

	// Set the physics parameters
	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.01);

	// Set the debug visualization parameters
	gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_ACTOR_AXES, 1);

    // Create the scene
    NxSceneDesc sceneDesc;
    sceneDesc.gravity = gDefaultGravity;
    sceneDesc.simType = NX_SIMULATION_HW;
	
	if (gbThreadScheduler)
	{
		sceneDesc.flags |= NX_SF_ENABLE_MULTITHREAD;
		sceneDesc.customScheduler = &gCustomScheduler;
		gCustomScheduler.CreateThreads(2);
	}
	else if (gbThreadPolling)
	{
		sceneDesc.flags |= NX_SF_ENABLE_MULTITHREAD;
	}
	else if (gbThreadSDKManage)
	{
		sceneDesc.flags |= NX_SF_ENABLE_MULTITHREAD;
		sceneDesc.internalThreadCount = 2;
	}
	else if (gbNoThread)
	{
		sceneDesc.flags &= ~NX_SF_SIMULATE_SEPARATE_THREAD;
	}
	else
		assert(0);

    gScene = gPhysicsSDK->createScene(sceneDesc);	
	if(0 == gScene)
	{ 
		sceneDesc.simType = NX_SIMULATION_SW; 
		gScene = gPhysicsSDK->createScene(sceneDesc);  
		if(0 == gScene) return;
	}
	if (gbThreadPolling)
	{
		gPollingThreads.CreateThreads(2, gScene);
		// We must reset the polling threads so they are ready for the next run
		gPollingThreads.ResetPollForWork();
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
	pyramid = CreateHalfPyramid(NxVec3(0,0,0), NxVec3(1,0.5,1.5), 10);
	gSelectedActor = pyramid;

	AddUserDataToActors(gScene);

	// Initialize HUD
	InitializeHUD();

	// Start the first frame of the simulation
	if (gScene)  StartPhysics();
}

void ReleaseNx()
{
    if (gScene)
	{
		GetPhysicsResults();  // Make sure to fetchResults() before shutting down
		if (gbThreadPolling)
		{
			// We must call shutdownWorkerThreads() before releasing the scene if we have the polling threads.
			gPollingThreads.ShutDownWorkerThreads(); 

		}
		gPhysicsSDK->releaseScene(*gScene);
	}
	if (gPhysicsSDK)  gPhysicsSDK->release();
    NX_DELETE_SINGLE(gAllocator);
	if (gbThreadScheduler)
		gCustomScheduler.KillThreads();
	else if (gbThreadPolling)
		gPollingThreads.KillThreads();
}

void ResetNx()
{
	ReleaseNx();
	InitNx();
}

void StartPhysics()
{
	if (0 == gScene) return;

	// Update the time step
	gDeltaTime = getElapsedTime();

	// Start collision and dynamics for delta time since the last frame
    gScene->simulate(gDeltaTime);
	gScene->flushStream();
}

void GetPhysicsResults()
{
	// Get results from gScene->simulate(gDeltaTime)
	if(0 != gScene)
		gScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
}

int main(int argc, char** argv)
{
	gbThreadScheduler = gbThreadPolling = gbThreadSDKManage = gbNoThread = false;
	gbThreadScheduler = true;
	PrintControls();
    InitGlut(argc, argv);
    InitNx();
    glutMainLoop();
	ReleaseNx();
	return 0;
}
