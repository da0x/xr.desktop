// ===============================================================================
//						     NVIDIA PHYSX SDK TRAINING PROGRAMS
//							 LESSON 603: Forcefield_Kernels
//
//						     Written by Xuezhi Deng, 6-4-2008
// ===============================================================================

#include "Lesson603.h"
#include "Timing.h"

// Physics SDK globals
NxPhysicsSDK*     gPhysicsSDK = NULL;
NxScene*          gScene = NULL;
NxVec3            gDefaultGravity(0,0,0);

// User report globals
DebugRenderer     gDebugRenderer;

// HUD globals
HUD hud;
 
// Display globals
int gMainHandle;
int mx = 0;
int my = 0;

// Camera globals
float gCameraAspectRatio = 1;
NxVec3 gCameraPos(0,25,-20);
NxVec3 gCameraForward(0,-1,1);
NxVec3 gCameraRight(0,0,0);
const NxReal gCameraSpeed = 10;

// Keyboard globals
#define MAX_KEYS 256
bool gKeys[MAX_KEYS];

// Simulation globals
NxReal gDeltaTime = 1.0/60.0;
bool bHardwareScene = false;
bool bPause = false;
bool bShadows = true;
bool bDebugWireframeMode = true;

// Actor globals
NxActor* groundPlane = NULL;
NxActor* box1 = NULL;

NxForceFieldKernelCustom* gCustomKernel;
NxForceFieldLinearKernel* gLinearKernel;
NxForceField* gForceField;

void PrintControls()
{
	printf("\n Flight Controls:\n ----------------\n w = forward, s = back\n a = strafe left, d = strafe right\n q = up, z = down\n");
	printf("\n Miscellaneous:\n --------------\n p = Pause\n b = Toggle Debug Wireframe Mode\n x = Toggle Shadows\n c = Switch between Linear and Custom kernel\n F10 = Reset\n");
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
        DrawActor(actor, NULL, false);

        // Handle shadows
        if (shadows)
        {
			DrawActorShadow(actor, false);
        }
    }
}

void ProcessInputs()
{
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

	// Display kernel Info
	if (gForceField)
	{
		if (gForceField->getForceFieldKernel() == gCustomKernel)
		{
			hud.SetDisplayString(2, "Using CustomKernel", 0.14f, 0.92f);
		}
		else if (gForceField->getForceFieldKernel() == gLinearKernel)
		{
			hud.SetDisplayString(2, "Using LinearKernel", 0.14f, 0.92f);
		}
	}
	
	// Render the HUD
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
		default: { break; }
	}
}

void KeyboardUpCallback(unsigned char key, int x, int y)
{
	gKeys[key] = false;

	switch (key)
	{
		case 'p': { bPause = !bPause; 
					if (bPause)
						hud.SetDisplayString(1, "Paused - Hit \"p\" to Unpause", 0.3f, 0.55f);
					else
						hud.SetDisplayString(1, "", 0.0f, 0.0f);	
					getElapsedTime(); 
					break; }
		case 'x': { bShadows = !bShadows; break; }
		case 'b': { bDebugWireframeMode = !bDebugWireframeMode; break; }
		case 'c': { 
					// Reset the box
					NxMat33 m;
					m.id();
					box1->setGlobalOrientation(m);
					box1->setGlobalPosition(NxVec3(5,3.5,0));
					box1->setLinearVelocity(NxVec3(0,0,0));
					box1->setAngularVelocity(NxVec3(0,0,0));

					ChangeKernel();
					break; 
				  }
		case 27 : { exit(0); break; }
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
    gMainHandle = glutCreateWindow("LESSON 603: Forcefield_Kernels");
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

NxActor* CreateGroundPlane()
{
    // Create a plane with default descriptor
    NxPlaneShapeDesc planeDesc;
    NxActorDesc actorDesc;
    actorDesc.shapes.pushBack(&planeDesc);
    return gScene->createActor(actorDesc);
}

NxActor* CreateBox(const NxVec3& pos)
{
	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// The actor has one shape, a box, 1m on a side
	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions.set(0.5,0.5,0.5);
	actorDesc.shapes.pushBack(&boxDesc);

	actorDesc.body = &bodyDesc;
	actorDesc.density = 10;
	actorDesc.globalPose.t = NxVec3(pos.x,pos.y,pos.z);		
	return gScene->createActor(actorDesc);	
}

NxForceFieldKernelCustom* CreateFFCustomKernel()
{
	// Create a Custom Kernel
	gCustomKernel = new NxForceFieldKernelCustom;
	gCustomKernel->setInitForce(NxVec3(0, 0, 500));
	return gCustomKernel;
}

NxForceFieldLinearKernel* CreateFFLinearKernel()
{
	// Create a Linear kernel
	NxMat33 m;
	m.id();

	NxForceFieldLinearKernelDesc lKernelDesc;
	lKernelDesc.positionMultiplier = m;
	lKernelDesc.positionTarget = NxVec3(0, 3.5, 0);

	gLinearKernel = gScene->createForceFieldLinearKernel(lKernelDesc);
	return gLinearKernel;
}

NxForceField*  CreateForcefield()
{
	// A box forcefield shape descriptor
	NxBoxForceFieldShapeDesc box;
	box.dimensions.set(10, 4, 10);

	// Create the forcefield with the Custom kernel
    NxForceFieldDesc fieldDesc1;
    fieldDesc1.kernel = gCustomKernel;
	fieldDesc1.coordinates = NX_FFC_CARTESIAN;
	box.pose.t.set(0, 4, 0);
	fieldDesc1.includeGroupShapes.push_back(&box);
	return gScene->createForceField(fieldDesc1);
}

void ChangeKernel()
{
	// Switch between Linear and Custom Kernel
	if (gForceField)
	{
		if (gForceField->getForceFieldKernel() == gCustomKernel)
		{
			gForceField->setForceFieldKernel(gLinearKernel);
		}
		else
		{
			gForceField->setForceFieldKernel(gCustomKernel);
		}
	}
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

	// Add Kernel Info to HUD
	hud.AddDisplayString("Using CustomKernel", 0.14f, 0.92f);
}

void InitNx()
{
    // Create the physics SDK
    gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION);
    if (!gPhysicsSDK)  return;

	// Set the debug visualization parameters for forcefield visualization
	gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_FORCE_FIELDS, 1.0f);

    // Create the scene
    NxSceneDesc sceneDesc;
 	sceneDesc.simType = NX_SIMULATION_HW;
    sceneDesc.gravity = gDefaultGravity;
    gScene = gPhysicsSDK->createScene(sceneDesc);	
	if(!gScene)
	{ 
		sceneDesc.simType = NX_SIMULATION_SW; 
		gScene = gPhysicsSDK->createScene(sceneDesc);  
		if(!gScene) return;
	}

	// Create the objects in the scene
	groundPlane = CreateGroundPlane();
	box1 = CreateBox(NxVec3(5,3.5,0));

	// Create a Custome kernel and a Linear kernel
	CreateFFCustomKernel();
	CreateFFLinearKernel();

	//Create the forcefield in the scene
	gForceField = CreateForcefield();

	// Initialize HUD
	InitializeHUD();

	// Get the current time
	getElapsedTime();

	// Start the first frame of the simulation
	if (gScene)  StartPhysics();
}

void ReleaseNx()
{
    if (gScene)
	{
		GetPhysicsResults();  // Make sure to fetchResults() before shutting down
		gPhysicsSDK->releaseScene(*gScene);
	}
	if (gPhysicsSDK) gPhysicsSDK->release();
}

void ResetNx()
{
	ReleaseNx();
	InitNx();
}

void StartPhysics()
{
	// Update the time step
	gDeltaTime = getElapsedTime();

	// Start collision and dynamics for delta time since the last frame
    gScene->simulate(gDeltaTime);
	gScene->flushStream();
}

void GetPhysicsResults()
{
	// Get results from gScene->simulate(gDeltaTime)
	while (!gScene->fetchResults(NX_RIGID_BODY_FINISHED, false));
}

int main(int argc, char** argv)
{
	PrintControls();
    InitGlut(argc, argv);
    InitNx();
    glutMainLoop();
	ReleaseNx();
	return 0;
}
