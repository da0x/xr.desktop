// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//						    LESSON 109 : COLLISION GROUPS
//
//						    Written by QA BJ, 6-2-2008
// ===============================================================================

#include "Lesson109.h"
#include "Timing.h"

// Physics SDK globals
NxPhysicsSDK*     gPhysicsSDK	= NULL;
NxScene*          gScene		= NULL;
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
float  gCameraAspectRatio = 1;
NxVec3 gCameraPos(0,5,-15);
NxVec3 gCameraForward(0,0,1);
NxVec3 gCameraRight(-1,0,0);
const NxReal gCameraSpeed = 10;

// Force globals
NxVec3 gForceVec(0,0,0);
NxReal gForceStrength	= 40000;
bool bForceMode			= true;

// Keyboard globals
#define MAX_KEYS 256
bool gKeys[MAX_KEYS];

// Simulation globals
NxReal gDeltaTime			= 1.0/60.0;
bool bHardwareScene			= false;
bool bPause					= false;
bool bShadows				= true;
bool bDebugWireframeMode	= false;

// Actor globals
NxActor* groundPlane	= NULL;
NxActor* box1			= NULL;
NxActor* box2			= NULL;
NxActor* box3			= NULL;
NxActor* sphere1		= NULL;
NxActor* sphere2		= NULL;
NxActor* sphere3		= NULL;
NxActor* capsule1		= NULL;
NxActor* capsule2		= NULL;
NxActor* capsule3		= NULL;

// Focus actor
NxActor* gSelectedActor = NULL;


void PrintControls()
{
	printf("\n Flight Controls:\n ----------------\n w = forward, s = back\n a = strafe left, d = strafe right\n q = up, z = down\n");
    printf("\n Force Controls:\n ---------------\n i = +z, k = -z\n j = +x, l = -x\n u = +y, m = -y\n");
	printf("\n Miscellaneous:\n --------------\n p   = Pause\n r   = Select Next Actor\n f   = Toggle Force Mode\n b   = Toggle Debug Wireframe Mode\n x   = Toggle Shadows\n t   = Move Focus Actor to (0,5,0)\n F10 = Reset Scene\n");
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
	gCameraAspectRatio = (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT);
	
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
			case 'i': { gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(0,0,1),gForceStrength,bForceMode); break; }
			case 'k': { gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(0,0,-1),gForceStrength,bForceMode); break; }
			case 'j': { gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(1,0,0),gForceStrength,bForceMode); break; }
			case 'l': { gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(-1,0,0),gForceStrength,bForceMode); break; }
			case 'u': { gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(0,1,0),gForceStrength,bForceMode); break; }
			case 'm': { gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(0,-1,0),gForceStrength,bForceMode); break; }

		    // Return focus actor to (0,5,0)
			case 't': 
				{ 
					gSelectedActor->setGlobalPosition(NxVec3(0,5,0)); 
					gScene->flushCaches();
					break; 
				}
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
		case 'r': { SelectNextActor(); break; }
		default:  { break; }
	}
}

void KeyboardUpCallback(unsigned char key, int x, int y)
{
	gKeys[key] = false;

	switch (key)
	{
		case 'p': { bPause = !bPause; 
					if (bPause)
						hud.SetDisplayString(0, "Paused - Hit \"p\" to Unpause", 0.3f, 0.55f);
					else
						hud.SetDisplayString(0, "", 0.0f, 0.0f);	
					getElapsedTime(); 
					break; }
		case 'x': { bShadows = !bShadows; break; }
		case 'b': { bDebugWireframeMode = !bDebugWireframeMode; break; }		
		case 'f': { bForceMode = !bForceMode; break; }
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
    gMainHandle = glutCreateWindow("Lesson 109: Collision Groups");
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
	NxMaterial* defaultMaterial = gScene->getMaterialFromIndex(0); 
	planeDesc.materialIndex	= defaultMaterial->getMaterialIndex();
    actorDesc.shapes.pushBack(&planeDesc);
    return gScene->createActor(actorDesc);
}

NxActor* CreateBox(const NxVec3& pos, const NxVec3& boxDim, NxReal density)
{
	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// The actor has one shape, a box
	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions.set(boxDim.x,boxDim.y,boxDim.z);
	boxDesc.localPose.t = NxVec3(0,boxDim.y,0);
	actorDesc.shapes.pushBack(&boxDesc);

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

	NxActor* pActor=gScene->createActor(actorDesc);
	assert(pActor);

	return pActor;
}

NxActor* CreateSphere(const NxVec3& pos, NxReal radius, NxReal density)
{
	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// The actor has one shape, a sphere
	NxSphereShapeDesc sphereDesc;
	sphereDesc.radius		= radius;
	sphereDesc.localPose.t	= NxVec3(0,radius,0);
	assert(sphereDesc.isValid());
	actorDesc.shapes.pushBack(&sphereDesc);

	if (density)
	{
		actorDesc.body		= &bodyDesc;
		actorDesc.density	= density;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t	= pos;

	NxActor* pActor = gScene->createActor(actorDesc);
	assert(pActor);

	return 	pActor;
}

NxActor* CreateCapsule(const NxVec3& pos, NxReal height, NxReal radius, NxReal density)
{
	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// The actor has one shape, a capsule
	NxCapsuleShapeDesc capsuleDesc;
	capsuleDesc.height = height;
	capsuleDesc.radius = radius;
	capsuleDesc.localPose.t = NxVec3(0,radius+0.5*height,0);
	assert(capsuleDesc.isValid());
	actorDesc.shapes.pushBack(&capsuleDesc);

	if (density)
	{
		actorDesc.body		= &bodyDesc;
		actorDesc.density	= density;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t = pos;
	
	NxActor* pActor = gScene->createActor(actorDesc);
	assert(pActor);
	
	return 	pActor;
}

void InitializeHUD()
{
	bHardwareScene = (gScene->getSimType() == NX_SIMULATION_HW);

	hud.Clear();

	//// Add hardware/software to HUD
	//if (bHardwareScene)
	//    hud.AddDisplayString("Hardware Scene", 0.74f, 0.92f);
	//else
	//	hud.AddDisplayString("Software Scene", 0.74f, 0.92f);

	// Add pause to HUD
	if (bPause)  
		hud.AddDisplayString("Paused - Hit \"p\" to Unpause", 0.3f, 0.55f);
	else
		hud.AddDisplayString("", 0.0f, 0.0f);
}

void SetActorCollisionGroup(NxActor *actor, NxCollisionGroup group)
{
	NxU32 nbShapes = actor->getNbShapes();
	NxShape*const* shapes = actor->getShapes();

	while (nbShapes--)
	{
		shapes[nbShapes]->setGroup(group);
	}
}

void InitActorCollisionGroups()
{
    SetActorCollisionGroup(box1, 1);
    SetActorCollisionGroup(box2, 1);
    SetActorCollisionGroup(box3, 1);

    SetActorCollisionGroup(capsule1, 2);
    SetActorCollisionGroup(capsule2, 2);
    SetActorCollisionGroup(capsule3, 2);

    SetActorCollisionGroup(sphere1, 3);
    SetActorCollisionGroup(sphere2, 3);
    SetActorCollisionGroup(sphere3, 3);

	gScene->setGroupCollisionFlag(1,2,true);
	gScene->setGroupCollisionFlag(2,3,true);
	gScene->setGroupCollisionFlag(1,3,false);
}

void InitContactReports()
{
	gScene->setActorPairFlags(*box1, *sphere1, NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_TOUCH | NX_NOTIFY_ON_END_TOUCH);
	gScene->setActorPairFlags(*box1, *sphere2, NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_TOUCH | NX_NOTIFY_ON_END_TOUCH);
	gScene->setActorPairFlags(*box1, *sphere3, NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_TOUCH | NX_NOTIFY_ON_END_TOUCH);
	gScene->setActorPairFlags(*box2, *sphere1, NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_TOUCH | NX_NOTIFY_ON_END_TOUCH);
	gScene->setActorPairFlags(*box2, *sphere2, NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_TOUCH | NX_NOTIFY_ON_END_TOUCH);
	gScene->setActorPairFlags(*box2, *sphere3, NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_TOUCH | NX_NOTIFY_ON_END_TOUCH);
	gScene->setActorPairFlags(*box3, *sphere1, NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_TOUCH | NX_NOTIFY_ON_END_TOUCH);
	gScene->setActorPairFlags(*box3, *sphere2, NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_TOUCH | NX_NOTIFY_ON_END_TOUCH);
	gScene->setActorPairFlags(*box3, *sphere3, NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_TOUCH | NX_NOTIFY_ON_END_TOUCH);
}

void InitContactReportsPerPair()
{
	gScene->setActorPairFlags(*box1, *sphere1, NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_TOUCH | NX_NOTIFY_ON_END_TOUCH);
	gScene->setActorPairFlags(*box1, *sphere2, NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_TOUCH | NX_NOTIFY_ON_END_TOUCH);
	gScene->setActorPairFlags(*box1, *sphere3, NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_TOUCH | NX_NOTIFY_ON_END_TOUCH);
	gScene->setActorPairFlags(*box2, *sphere1, NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_TOUCH | NX_NOTIFY_ON_END_TOUCH);
	gScene->setActorPairFlags(*box2, *sphere2, NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_TOUCH | NX_NOTIFY_ON_END_TOUCH);
	gScene->setActorPairFlags(*box2, *sphere3, NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_TOUCH | NX_NOTIFY_ON_END_TOUCH);
	gScene->setActorPairFlags(*box3, *sphere1, NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_TOUCH | NX_NOTIFY_ON_END_TOUCH);
	gScene->setActorPairFlags(*box3, *sphere2, NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_TOUCH | NX_NOTIFY_ON_END_TOUCH);
	gScene->setActorPairFlags(*box3, *sphere3, NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_TOUCH | NX_NOTIFY_ON_END_TOUCH);
}

void InitContactReportsPerGroup()
{
	box1->setGroup(1);
	box2->setGroup(1);
	box3->setGroup(1);

	capsule1->setGroup(2);
	capsule2->setGroup(2);
	capsule3->setGroup(2);

	gScene->setActorGroupPairFlags(1, 2, NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_TOUCH | NX_NOTIFY_ON_END_TOUCH);
}

void DisableCollisionResponse()
{
	box1->raiseBodyFlag(NX_BF_KINEMATIC);
	box2->raiseBodyFlag(NX_BF_KINEMATIC);
	box3->raiseBodyFlag(NX_BF_KINEMATIC);

	box1->raiseActorFlag(NX_AF_DISABLE_RESPONSE);
	box2->raiseActorFlag(NX_AF_DISABLE_RESPONSE);
	box3->raiseActorFlag(NX_AF_DISABLE_RESPONSE);
}

void InitNx()
{
	// Initialize Camera Parameters
	gCameraAspectRatio	= 1.0f;
	gCameraPos			= NxVec3(0,5,-15);
	gCameraForward		= NxVec3(0,0,1);
	gCameraRight		= NxVec3(-1,0,0);	
	
	// Create the physics SDK
    gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION);
    if (!gPhysicsSDK)  return;

	// Set the physics parameters
	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.01);
	gPhysicsSDK->setParameter(NX_DEFAULT_SLEEP_LIN_VEL_SQUARED, 0.15*0.15);
	gPhysicsSDK->setParameter(NX_DEFAULT_SLEEP_ANG_VEL_SQUARED, 0.14*0.14);

	// Set the debug visualization parameters
	gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_ACTOR_AXES, 1);

    // Create the scene
    NxSceneDesc sceneDesc;
    sceneDesc.gravity               = gDefaultGravity;
	sceneDesc.simType				= NX_SIMULATION_SW;
    gScene = gPhysicsSDK->createScene(sceneDesc);	
	if(!gScene)
	{ 
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

	//
	box1 = CreateBox(NxVec3(6,0,1), NxVec3(0.5,1,0.5), 20);
	box2 = CreateBox(NxVec3(5,0,-0.25), NxVec3(0.5,1,0.5), 20);
	box3 = CreateBox(NxVec3(4,0,1), NxVec3(0.5,1,0.5), 20);

	capsule1 = CreateCapsule(NxVec3(1,0,6), 1.5, 0.5, 20);
	capsule2 = CreateCapsule(NxVec3(0,0,5), 1.5, 0.5, 20);
	capsule3 = CreateCapsule(NxVec3(-1,0,6), 1.5, 0.5, 20);

	sphere1 = CreateSphere(NxVec3(-6,0,1), 0.8, 20);
	sphere2 = CreateSphere(NxVec3(-5,0,-0.25), 0.8, 20);
	sphere3 = CreateSphere(NxVec3(-4,0,1), 0.8, 20);

	gSelectedActor = box3;

	InitActorCollisionGroups();
	InitContactReportsPerPair();

#if 0
	InitContactReportsPerGroup();
	DisableCollisionResponse();
#endif  // 0

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

