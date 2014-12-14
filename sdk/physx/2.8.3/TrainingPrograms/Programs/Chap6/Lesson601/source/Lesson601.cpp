// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//						  LESSON 601 - Forcefield_Creation
//
//						  Written by Xuezhi Deng, 6-4-2008
// ===============================================================================

#include "Lesson601.h"
#include "Timing.h"

// Physics SDK globals
NxPhysicsSDK*     gPhysicsSDK = NULL;
NxScene*          gScene = NULL;
NxVec3            gDefaultGravity(0,-9.8,0);

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
NxVec3 gCameraPos(0,15,-30);
NxVec3 gCameraForward(0,0,1);
NxVec3 gCameraRight(-1,0,0);
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
NxActor* box2 = NULL;

void PrintControls()
{
	printf("\n Flight Controls:\n ----------------\n w = forward, s = back\n a = strafe left, d = strafe right\n q = up, z = down\n");
	printf("\n Miscellaneous:\n --------------\n p = Pause\n b = Toggle Debug Wireframe Mode\n x = Toggle Shadows\n F10 = Reset\n");
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
    gMainHandle = glutCreateWindow("Lesson 601: Forcefield_Creation");
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

NxForceField* CreateForcefield()
{
	// Create a Forcefield kernel
    NxForceFieldLinearKernelDesc linearKernelDesc;
    linearKernelDesc.constant = NxVec3(0.0f,19.6f,0.0f); 
    NxForceFieldLinearKernel* pLinearKernel;
    pLinearKernel = gScene->createForceFieldLinearKernel(linearKernelDesc);

	// The Forcefield descriptor
    NxForceFieldDesc fieldDesc;
    fieldDesc.kernel = pLinearKernel;
	fieldDesc.rigidBodyType = NX_FF_TYPE_GRAVITATIONAL;

	// A box forcefield shape descriptor
	NxBoxForceFieldShapeDesc box;
	box.dimensions.set(2, 2, 2);
	box.pose.t.set(6, 2, 0);

	// Create a Forcefield
	// There is one special include group per force field which cannot be shared among other force fields and which is moving with the force field.
	// The pose of the shapes in this include group is relative to the force field pose and the shapes are include volumes only. 
	// The special include group contains a box shape
	fieldDesc.includeGroupShapes.push_back(&box);
    NxForceField *pForceField; 
    pForceField = gScene->createForceField(fieldDesc);

	// A capsule forcefield shape descriptor
	NxCapsuleForceFieldShapeDesc capsule;
	capsule.height = 1;
	capsule.radius = 2.5f;
	capsule.pose.t.set(6, 9, 0);

	// A sphere forcefield shape descriptor
	NxSphereForceFieldShapeDesc sphere;
	sphere.radius = 4;
	sphere.pose.t.set(6, 24, 0);

	// Add two shapes to the first shape group
	NxForceFieldShapeGroupDesc sgDesc1;
	sgDesc1.shapes.pushBack(&capsule);
	sgDesc1.shapes.pushBack(&sphere);
	NxForceFieldShapeGroup* pIncludeGroup;
    pIncludeGroup = gScene->createForceFieldShapeGroup(sgDesc1);

	box.pose.t.set(-6, 2, 0);
	capsule.pose.t.set(-6, 9, 0);

	// Add two shapes to the second shape group
	// The group flag (NX_FFSG_EXCLUDE_GROUP) defines that all shapes inside this group are exclude volumes.
	NxForceFieldShapeGroupDesc sgDesc2;
	sgDesc2.flags = NX_FFSG_EXCLUDE_GROUP;
	sgDesc2.shapes.pushBack(&box);
	sgDesc2.shapes.pushBack(&capsule);
	NxForceFieldShapeGroup* pExcludeGroup;
    pExcludeGroup = gScene->createForceFieldShapeGroup(sgDesc2);

	// Add these two shape groups to the forcefield
	pForceField->addShapeGroup(*pIncludeGroup);
	pForceField->addShapeGroup(*pExcludeGroup);

	return pForceField;
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
	box1 = CreateBox(NxVec3(6,3.5,0));
	box2 = CreateBox(NxVec3(-6,3.5,0));

	//Create the forcefield in the scene
	CreateForcefield();

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
	if (gPhysicsSDK)  gPhysicsSDK->release();
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
