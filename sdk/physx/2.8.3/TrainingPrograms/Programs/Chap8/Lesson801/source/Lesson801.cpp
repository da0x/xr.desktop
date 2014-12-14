// ===============================================================================
//						   NVIDIA PHYSX SDK TRAINING PROGRAMS
//			               LESSON 801: Hardware_Scene_Manager
//
//						    Written by Bob Schade, 5-1-06
//							Updated by Yang Liu, 7-1-08
// ===============================================================================

#include "Lesson801.h"
#include "Timing.h"

// Physics SDK globals
NxPhysicsSDK*     gPhysicsSDK = NULL;
NxScene*          gScene = NULL;
NxCompartment*	  gCompartmentHW = NULL;
NxCompartment*	  gCompartmentSW = NULL;
NxVec3            gDefaultGravity(0, -9.81, 0);

// User report globals
DebugRenderer     gDebugRenderer;
UserAllocator*	  gAllocator = NULL;
ErrorStream       gErrorStream;

// HUD globals
HUD hud;

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
bool bHWScene = true;
bool bPause = false;
bool bShadows = true;
bool bDebugWireframeMode = false;

// Actor globals
NxActor* groundPlane = NULL;
NxActor* boxHW = NULL;
NxActor* boxSW = NULL;
NxActor* sphereHW = NULL;
NxActor* sphereSW = NULL;
NxActor* capsule = NULL;

// Focus actor
NxActor* gSelectedActor = NULL;

void PrintControls()
{
	printf("\n Flight Controls:\n ----------------\n w = forward, s = back\n a = strafe left, d = strafe right\n q = up, z = down\n");
    printf("\n Force Controls:\n ---------------\n i = +z, k = -z\n j = +x, l = -x\n u = +y, m = -y\n");
	printf("\n Miscellaneous:\n --------------\n F10 = Restart\n p = Pause\n r = Select Next Actor\n f = Toggle Force Mode\n b = Toggle Debug Wireframe Mode\n x = Toggle Shadows\n t = Move Focus Actor to (0,5,0)\n c = Toggle Primary Scene Simulation Type\n");
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
   for(NxU32 i = 0; i < nbActors; i++)
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
        DrawActor(actor, gSelectedActor, false);

        // Handle shadows
        if (shadows)
        {
			DrawActorShadow(actor, false);
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
			case 'i': {gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(0,0,+1),gForceStrength,bForceMode); break; }
			case 'k': {gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(0,0,-1),gForceStrength,bForceMode); break; }
			case 'j': {gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(+1,0,0),gForceStrength,bForceMode); break; }
			case 'l': {gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(-1,0,0),gForceStrength,bForceMode); break; }
			case 'u': {gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(0,+1,0),gForceStrength,bForceMode); break; }
			case 'm': {gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(0,-1,0),gForceStrength,bForceMode); break; }

		    // Return focus actor to (0,5,0)
		    case 't': { gSelectedActor->setGlobalPosition(NxVec3(0,5,0)); break; }
		}
	}
}

void ProcessInputs()
{
    ProcessForceKeys();

    // Show debug wireframes
    if (bDebugWireframeMode && (0 != gScene))
		gDebugRenderer.renderData(*gScene->getDebugRenderable());
}

void RenderCallback()
{
    // Clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ProcessCameraKeys();
	SetupCamera();

    if (!bPause)
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

void KeyboardCallback(unsigned char key, int /*x*/, int /*y*/)
{
	gKeys[key] = true;

	switch (key)
	{
		case 'r': { SelectNextActor(); break; }
		case 'c': { bHWScene = !bHWScene; CreateScenario(); break; }
		default: { break; }
	}
}

void KeyboardUpCallback(unsigned char key, int /*x*/, int /*y*/)
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
		case 'f': { bForceMode = !bForceMode; break; }
		case 27 : { exit(0); break; }
		default : { break; }
	}
}

void SpecialCallback(int key, int /*x*/, int /*y*/)
{
	switch (key)
    {
		// Reset PhysX
		case GLUT_KEY_F10: ResetNx(); return; 
	}
}

void MouseCallback(int /*button*/, int /*state*/, int x, int y)
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

void InitGlut(int argc, char** argv, char* lessonTitle)
{
    glutInit(&argc, argv);
    glutInitWindowSize(512, 512);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	gMainHandle = glutCreateWindow(lessonTitle);
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

NxActor* CreateManagedCapsule(const NxVec3& pos, const NxReal fHeight, const NxReal fRadius, const NxReal fDensity, NxCompartment *pCompartment = 0)
{
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// The actor has one shape, a capsule
	NxCapsuleShapeDesc capsuleDesc;
	capsuleDesc.height = fHeight;
	capsuleDesc.radius = fRadius;
	capsuleDesc.localPose.t = NxVec3(0,fRadius+0.5*fRadius,0);
	actorDesc.shapes.pushBack(&capsuleDesc);

	if (fDensity)
	{
		actorDesc.body = &bodyDesc;
		actorDesc.density = fDensity;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t = pos;
	actorDesc.compartment = pCompartment;

	NxActor* actor = gScene->createActor(actorDesc);
	if (0 == actor) 
		printf("\nUnable to create a actor. Please make sure that you have correctly installed the latest version of the NVIDIA PhysX SDK.\n");

	return actor;	
}

NxActor* CreateManagedSphere(const NxVec3& pos, const NxReal fRadius, const NxReal fDensity, NxCompartment *pCompartment = 0)
{
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// The actor has one shape, a sphere
	NxSphereShapeDesc sphereDesc;
	sphereDesc.radius = fRadius;
	sphereDesc.localPose.t = NxVec3(0,fRadius,0);
	actorDesc.shapes.pushBack(&sphereDesc);

	if (fDensity)
	{
		actorDesc.body = &bodyDesc;
		actorDesc.density = fDensity;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t = pos;
	actorDesc.compartment = pCompartment;

	NxActor* actor = gScene->createActor(actorDesc);
	if (0 == actor) 
		printf("\nUnable to create a actor. Please make sure that you have correctly installed the latest version of the NVIDIA PhysX SDK.\n");

	return actor;	
}

NxActor* CreateManagedBox(const NxVec3& pos, const NxVec3& boxDim, const NxReal fDensity, NxCompartment *pCompartment = 0)
{
	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// The actor has one shape, a box
	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions.set(boxDim.x,boxDim.y,boxDim.z);
	boxDesc.localPose.t = NxVec3(0,boxDim.y,0);
	actorDesc.shapes.pushBack(&boxDesc);

	if (fDensity)
	{
		actorDesc.body = &bodyDesc;
		actorDesc.density = fDensity;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t = pos;
	actorDesc.compartment = pCompartment;

	NxActor* actor = gScene->createActor(actorDesc);
	if (0 == actor) 
		printf("\nUnable to create a actor. Please make sure that you have correctly installed the latest version of the NVIDIA PhysX SDK.\n");

	return actor;
}

void InitializeHUD()
{
	hud.Clear();
	if (0 != gScene)
	{
		if (NX_SIMULATION_HW == gScene->getSimType())
			hud.AddDisplayString("Hardware Scene", 0.74f, 0.92f);
		else
			hud.AddDisplayString("Software Scene", 0.74f, 0.92f);
	}

	// Add pause to HUD
	if (bPause)  
		hud.AddDisplayString("Paused - Hit \"p\" to Unpause", 0.3f, 0.55f);
	else
		hud.AddDisplayString("", 0.0f, 0.0f);
}

void DestroyScenario()
{
	if (0 != gPhysicsSDK && 0 != gScene)
	{
		GetPhysicsResults();  // make sure to fetchResults() before shutting down
		gPhysicsSDK->releaseScene(*gScene);
		gScene = 0;
		gCompartmentHW = 0;
		gCompartmentSW = 0;
		gSelectedActor = 0;
		groundPlane = 0;

		boxHW = 0;
		boxSW = 0;
		sphereHW = 0;
		sphereSW = 0;
		capsule = 0;
	}
}

bool CreateScenario()
{
	DestroyScenario();

	// Create the scene
	NxSceneDesc sceneDesc;
	sceneDesc.gravity = gDefaultGravity;
	sceneDesc.simType = bHWScene? NX_SIMULATION_HW : NX_SIMULATION_SW;
	gScene = gPhysicsSDK->createScene(sceneDesc);	
	if(0 == gScene)
	{ 
		bHWScene = !bHWScene;
		sceneDesc.simType = bHWScene? NX_SIMULATION_HW : NX_SIMULATION_SW;
		gScene = gPhysicsSDK->createScene(sceneDesc);  
		if(0 == gScene) return false;
	}

	// Create the default material
	NxMaterial* defaultMaterial = gScene->getMaterialFromIndex(0); 
	defaultMaterial->setRestitution(0.5);
	defaultMaterial->setStaticFriction(0.5);
	defaultMaterial->setDynamicFriction(0.5);

	// Create the plane in primary scene
	groundPlane = CreateGroundPlane();

	// Create compartment(HSM, managed hardware scene) attached to this scene
	NxCompartmentDesc desc;
	desc.type = NX_SCT_RIGIDBODY;
	desc.deviceCode = NxU32(NX_DC_PPU_0);
	gCompartmentHW = gScene->createCompartment(desc);

	desc.deviceCode = NxU32(NX_DC_CPU);
	gCompartmentSW = gScene->createCompartment(desc);

	// Create objects
	boxHW = CreateManagedBox(NxVec3(6, 0, 0), NxVec3(0.5, 1, 0.5), 20, gCompartmentHW);
	boxSW = CreateManagedBox(NxVec3(3, 0, 0), NxVec3(0.5, 1, 0.5), 20, gCompartmentSW);
	sphereHW = CreateManagedSphere(NxVec3(-6,0,0), 1, 10, gCompartmentHW);
	sphereHW = CreateManagedSphere(NxVec3(-3,0,0), 1, 10, gCompartmentSW);
	capsule = CreateManagedCapsule(NxVec3(0,0,0), 2, 1, 5, 0);

	gSelectedActor = boxHW;

	// Initialize HUD
	InitializeHUD();

	// Start the first frame of the simulation
	StartPhysics();
	return true;
}

void InitNx()
{
	// Create a memory allocator
    gAllocator = new UserAllocator;

    // Create the physics SDK
    gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, gAllocator, &gErrorStream);
    if (0 == gPhysicsSDK) return;

	//This should be update for compatible with GPU and PPU!!!
	// Check for hardware availability
	NxHWVersion hwCheck = gPhysicsSDK->getHWVersion();
	if (hwCheck == NX_HW_VERSION_NONE) 
	{
		printf("Unable to find any NVIDIA PhysX hardware, this sample requires hardware physics to run. Please make sure that you have correctly installed the latest version of the NVIDIA PhysX SDK.\n");
		gPhysicsSDK->release();
		gPhysicsSDK = 0;
		NX_DELETE_SINGLE(gAllocator);
		exit(1);
	}

	// Set the physics parameters
	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.05);

	// Set the debug visualization parameters
	gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_ACTOR_AXES, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_FNORMALS, 1);

	CreateScenario();
}

void ReleaseNx()
{
	DestroyScenario();
	if (0 != gPhysicsSDK)
	{
		gPhysicsSDK->release();
		gPhysicsSDK = 0;
	}
    NX_DELETE_SINGLE(gAllocator);
}

void ResetNx()
{
	ReleaseNx();
	bHWScene = true;
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
	// Get results from gScene->simulate(deltaTime)
	if(0 != gScene)
		gScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
}

int main(int argc, char** argv)
{
	PrintControls();
    InitGlut(argc, argv, "Lesson 801: Hardware Scene Manager");
    InitNx();
    glutMainLoop();
	ReleaseNx();
	return 0;
}
