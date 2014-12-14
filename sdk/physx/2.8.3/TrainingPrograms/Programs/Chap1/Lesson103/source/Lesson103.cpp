// ===============================================================================
//						    NVIDIA PHYSX SDK TRAINING PROGRAMS
//							LESSON 103: COMPOUND (MULTI SHAPES)
//
//						    Written by QA BJ, 6-2-2008
// ===============================================================================

#include "Lesson103.h"
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
NxActor* groundPlane			= NULL;
NxConvexMeshDesc* convexDesc	= NULL;

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
    gMainHandle = glutCreateWindow("Lesson 103: Compound (Multi Shapes)");
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

NxActor* CreatePrimaryMultiShape()
{	
	// Create an actor which is composed of Box, Capsule and Sphere.

	NxActorDesc actorDesc;
	NxBodyDesc  bodyDesc;	
	
	//Box Shape
	NxBoxShapeDesc boxShape;
	boxShape.dimensions		= NxVec3(1.0f, 1.0f, 1.0f);
	boxShape.localPose.t	= NxVec3(1.5f, 0.0f, 0.0f);
	assert(boxShape.isValid());
	actorDesc.shapes.pushBack(&boxShape);

	//Capsule Shape
	NxCapsuleShapeDesc capsuleDesc;
	capsuleDesc.radius		= 0.8f;
	capsuleDesc.height		= 1.0f;
	capsuleDesc.localPose.t = NxVec3(0.0f, 0.0f, 0.0f);
	//Rotate capsule 90 degree around z axis
	NxQuat quat(90.0f, NxVec3(0, 0, 1));
	NxMat33 m;
	m.id();
	m.fromQuat(quat);
	capsuleDesc.localPose.M = m;
	assert(capsuleDesc.isValid());
	actorDesc.shapes.pushBack(&capsuleDesc);

	//Sphere Shape
	NxSphereShapeDesc sphereDesc;
	sphereDesc.radius		= 1.0f;
	sphereDesc.localPose.t	= NxVec3(-1.5f, 0.0f, 0.0f);
	assert(sphereDesc.isValid());
	actorDesc.shapes.pushBack(&sphereDesc);

	actorDesc.body			= &bodyDesc;
	actorDesc.density		= 1.0f;
	actorDesc.globalPose.t	= NxVec3(5.0f, 0.0f, 0.0f);
	assert(actorDesc.isValid());

	NxActor *pActor = gScene->createActor(actorDesc);
	assert(pActor);

	return pActor;
}


NxActor* CreateMeshMultiShape()
{
	// Create an actor which is composed of Convex and Sphere.

	NxActorDesc actorDesc;
	NxBodyDesc  bodyDesc;

	//Convex Shape
	NxVec3 boxDim(2.0f, 2.0f, 2.0f);
    
    NxVec3 verts[8] =	{	NxVec3(boxDim.x,      -boxDim.y, -boxDim.z), 
							NxVec3(-boxDim.x,     -boxDim.y, -boxDim.z), 
							NxVec3(-boxDim.x,     -boxDim.y, boxDim.z),
							NxVec3(boxDim.x,      -boxDim.y, boxDim.z), 
							NxVec3(boxDim.x*0.5,  boxDim.y,  -boxDim.z*0.5), 
							NxVec3(-boxDim.x*0.5, boxDim.y,  -boxDim.z*0.5),
							NxVec3(-boxDim.x*0.5, boxDim.y,  boxDim.z*0.5), 
							NxVec3(boxDim.x*0.5,  boxDim.y,  boxDim.z*0.5)
						};


	// Create descriptor for convex mesh
    if (!convexDesc)
		convexDesc	= new NxConvexMeshDesc();
	assert(convexDesc);
    convexDesc->numVertices			= 8;
    convexDesc->pointStrideBytes	= sizeof(NxVec3);
    convexDesc->points				= verts;
	convexDesc->flags				= NX_CF_COMPUTE_CONVEX;

	NxConvexShapeDesc convexShapeDesc;
	convexShapeDesc.localPose.t		= NxVec3(0,boxDim.y,0);
	convexShapeDesc.userData		= convexDesc;
 
	NxInitCooking();
	// Cooking from memory
    MemoryWriteBuffer buf;
    bool status = NxCookConvexMesh(*convexDesc, buf);
	//
	// Please note about the created Convex Mesh, user needs to release it when no one uses it to save memory. It can be detected
	// by API "meshData->getReferenceCount() == 0". And, the release API is "gPhysicsSDK->releaseConvexMesh(*convexShapeDesc.meshData);"
	//
	NxConvexMesh *pMesh			= gPhysicsSDK->createConvexMesh(MemoryReadBuffer(buf.data));
	assert(pMesh);
    convexShapeDesc.meshData	= pMesh;
	NxCloseCooking();

    if (pMesh)
    {
        // Save mesh in userData for drawing.
		pMesh->saveToDesc(*convexDesc);
		//
		assert(convexShapeDesc.isValid());
        actorDesc.shapes.pushBack(&convexShapeDesc);
    }

	//Sphere Shape
	NxSphereShapeDesc sphereDesc;
	sphereDesc.radius		= 1.8f;
	sphereDesc.localPose.t	= NxVec3(0.0f, 5.4f, 0.0f);
	assert(sphereDesc.isValid());
	actorDesc.shapes.pushBack(&sphereDesc);

	actorDesc.body			= &bodyDesc;
	actorDesc.density		= 1.0f;
	actorDesc.globalPose.t  = NxVec3(-5.0f, 0.0f, 0.0f);
	assert(actorDesc.isValid());
	NxActor *pActor = gScene->createActor(actorDesc);
	assert(pActor);

	return pActor;
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
		sceneDesc.simType			= NX_SIMULATION_SW; 
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

	// Create primary multi shapes.
	gSelectedActor = CreatePrimaryMultiShape();

	// Create mesh multi shapes
	CreateMeshMultiShape();

	// Initialize HUD
	InitializeHUD();

	// Get the current time
	getElapsedTime();

	// Start the first frame of the simulation
	if (gScene)  StartPhysics();
}

void ReleaseNx()
{
   if (convexDesc)
   {
		delete convexDesc;
		convexDesc = NULL;
   }
	
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
