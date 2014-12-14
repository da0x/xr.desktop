// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//						     LESSON 102 : MESH SHAPES
//
//						    Written by QA BJ, 6-2-2008
// ===============================================================================

#include "Lesson102.h"
#include "Timing.h"

// Physics SDK globals
NxPhysicsSDK*     gPhysicsSDK	= NULL;
NxScene*          gScene		= NULL;
NxVec3            gDefaultGravity(0,-9.8,0);
UserAllocator*	  gAllocator;

// User report globals
DebugRenderer     gDebugRenderer;

// HUD globals
HUD hud;
 
// Display globals
int gMainHandle;
int mx = 0;
int my = 0;

// Camera globals
float	gCameraAspectRatio = 1;
NxVec3	gCameraPos(0,5,-15);
NxVec3	gCameraForward(0,0,1);
NxVec3	gCameraRight(-1,0,0);
const NxReal gCameraSpeed = 10;

// Force globals
NxVec3 gForceVec(0,0,0);
NxReal gForceStrength	= 20000;
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
NxTriangleMeshDesc* triangleMeshDesc = NULL;
NxTriangleMeshDesc* concaveDesc		 = NULL;

// Focus actor
NxActor* gSelectedActor = NULL;


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

   if(!actor->isDynamic())
	   return false;

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

void PrintControls()
{
	printf("\n Flight Controls:\n ----------------\n w = forward, s = back\n a = strafe left, d = strafe right\n q = up, z = down\n");
    printf("\n Force Controls:\n ---------------\n i = +z, k = -z\n j = +x, l = -x\n u = +y, m = -y\n");
	printf("\n Miscellaneous:\n --------------\n p   = Pause\n b   = Toggle Debug Wireframe Mode\n x   = Toggle Shadows\n r   = Select Actor\n F10 = Reset Scene");
}

NxVec3 ApplyForceToActor(NxActor* actor, const NxVec3& forceDir, const NxReal forceStrength)
{
	NxVec3 forceVec = forceStrength*forceDir*gDeltaTime;
	actor->addForce(forceVec);
	return forceVec;
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

void ProcessForceKeys()
{
	// Process force keys
	for (int i = 0; i < MAX_KEYS; i++)
	{	
		if (!gKeys[i])  { continue; }

		switch (i)
		{
			// Force controls
			case 'i': { gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(0,0,1),gForceStrength); break; }
			case 'k': { gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(0,0,-1),gForceStrength); break; }
			case 'j': { gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(1,0,0),gForceStrength); break; }
			case 'l': { gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(-1,0,0),gForceStrength); break; }
			case 'u': { gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(0,1,0),gForceStrength); break; }
			case 'm': { gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(0,-1,0),gForceStrength); break; }

		    // Return box to (0,5,0)
			case 't': 
				{ 
					if (gSelectedActor)
					{
						gSelectedActor->setGlobalPosition(NxVec3(0,5,0)); 
						gScene->flushCaches();
					}
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

	DrawForce(gSelectedActor, gForceVec, NxVec3(1,1,0));
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
		case 'r':	{ SelectNextActor(); break; }
		default:	{ break; }
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
    gMainHandle = glutCreateWindow("Lesson 102: Mesh Shapes");
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
	gCameraPos			= NxVec3(0, 5, -15);
	gCameraForward		= NxVec3(0,0,1);
	gCameraRight		= NxVec3(-1,0,0);
		
	// Create a memory allocator
    if (!gAllocator)
	{
		gAllocator = new UserAllocator;
		assert(gAllocator);
	}
	
	// Create the physics SDK
    gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, gAllocator);
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
 	sceneDesc.simType				= NX_SIMULATION_SW;
    sceneDesc.gravity               = gDefaultGravity;
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

	// Create Convex Mesh
	gSelectedActor = CreateConvexMesh();

	// Create Triangle Mesh
	CreateTriangleMesh();

	// Create Concave Mesh, Actually it is too Triangle Mesh.
	CreateConcaveMesh();

	// Initialize HUD
	InitializeHUD();

	// Get the current time
	getElapsedTime();

	// Start the first frame of the simulation
	if (gScene)  StartPhysics();
}

NxActor* CreateGroundPlane()
{
    // Create a plane with default descriptor
    NxPlaneShapeDesc planeDesc;
    NxActorDesc actorDesc;
    actorDesc.shapes.pushBack(&planeDesc);
    return gScene->createActor(actorDesc);
}

NxActor* CreateConvexMesh()
{
    NxActorDesc actorDesc;
    NxBodyDesc bodyDesc;
	
	NxVec3 boxDim(1.5,0.75,1.5);
    // Pyramid
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
	{
		convexDesc	= new NxConvexMeshDesc();
		assert(convexDesc);
	}
    convexDesc->numVertices			= 8;
    convexDesc->pointStrideBytes	= sizeof(NxVec3);
    convexDesc->points				= verts;
	convexDesc->flags				= NX_CF_COMPUTE_CONVEX;

	NxConvexShapeDesc convexShapeDesc;
	convexShapeDesc.localPose.t		= NxVec3(0,boxDim.y,0);
	convexShapeDesc.userData		= convexDesc;
 
	
	// Two ways on cooking mesh: 1. Saved in memory, 2. Saved in file	
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
		NxActorDesc actorDesc;
		assert(convexShapeDesc.isValid());
        actorDesc.shapes.pushBack(&convexShapeDesc);
	    actorDesc.body		= &bodyDesc;
		actorDesc.density	= 1.0f;
	   
        actorDesc.globalPose.t  = NxVec3(0.0f, 0.0f, 0.0f);
		assert(actorDesc.isValid());
		NxActor* actor = gScene->createActor(actorDesc);
		assert(actor);	
		return actor;
    }

    return NULL;
}

NxActor* CreateTriangleMesh()
{
	NxVec3 boxDim(1.0f, 1.0f, 1.0f);
	// Supply hull
	 NxVec3 verts[8] = {NxVec3(-boxDim.x, -boxDim.y, -boxDim.z), 
						NxVec3(boxDim.x,  -boxDim.y, -boxDim.z), 
						NxVec3(-boxDim.x, boxDim.y,  -boxDim.z), 
						NxVec3(boxDim.x,  boxDim.y,  -boxDim.z),
						NxVec3(-boxDim.x, -boxDim.y, boxDim.z), 
						NxVec3(boxDim.x,  -boxDim.y, boxDim.z), 
						NxVec3(-boxDim.x, boxDim.y,  boxDim.z), 
						NxVec3(boxDim.x,  boxDim.y,  boxDim.z),
						};

	// Triangles is 12*3
	 NxU32 indices[12*3] = {1,2,3,        
							0,2,1,  
							5,4,1,    
							1,4,0,    
							1,3,5,    
							3,7,5,    
							3,2,7,    
							2,6,7,    
							2,0,6,    
							4,6,0,
							7,4,5,
							7,6,4
							};

	 NxU16 indices16[12*3] = {	1,2,3,        
								0,2,1,  
								5,4,1,    
								1,4,0,    
								1,3,5,    
								3,7,5,    
								3,2,7,    
								2,6,7,    
								2,0,6,    
								4,6,0,
								7,4,5,
								7,6,4
							};

	// Create descriptor for triangle mesh
    if (!triangleMeshDesc)
	{
		triangleMeshDesc	= new NxTriangleMeshDesc();
		assert(triangleMeshDesc);
	}
	triangleMeshDesc->numVertices			= 8;
	triangleMeshDesc->pointStrideBytes		= sizeof(NxVec3);
	triangleMeshDesc->points				= verts;
	triangleMeshDesc->numTriangles			= 12;
	triangleMeshDesc->flags					= 0;
	triangleMeshDesc->triangles				= indices;
	triangleMeshDesc->triangleStrideBytes	= 3 * sizeof(NxU32);
	
	//Alternative:	see NxMeshFlags
	//triangleMeshDesc->flags				= NX_MF_16_BIT_INDICES
	//triangleMeshDesc->triangles			= indices16;
	//triangleMeshDesc->triangleStrideBytes	= 3 * sizeof(NxU16);

	// The actor has one shape, a triangle mesh
	NxInitCooking();
	MemoryWriteBuffer buf;

	bool status = NxCookTriangleMesh(*triangleMeshDesc, buf);
	NxTriangleMesh* pMesh;
	if (status)
	{
		pMesh = gPhysicsSDK->createTriangleMesh(MemoryReadBuffer(buf.data));
	}
	else
	{
		assert(false);
		pMesh = NULL;
	}
	NxCloseCooking();
	// Create TriangleMesh above code segment.

	NxTriangleMeshShapeDesc tmsd;
	tmsd.meshData		= pMesh;
	tmsd.userData		= triangleMeshDesc;
	tmsd.localPose.t	= NxVec3(0, boxDim.y, 0);
	tmsd.meshPagingMode = NX_MESH_PAGING_AUTO;
	
	NxActorDesc actorDesc;
	NxBodyDesc  bodyDesc;
	
	assert(tmsd.isValid());
	actorDesc.shapes.pushBack(&tmsd);
	//Dynamic triangle mesh don't be supported anymore. So body = NULL
	actorDesc.body			= NULL;		
	actorDesc.globalPose.t	= NxVec3(3.0f, 0.0f, 0.0f);

	if (pMesh)
	{
		// Save mesh in userData for drawing
		pMesh->saveToDesc(*triangleMeshDesc);
		//
		assert(actorDesc.isValid());
		NxActor *actor = gScene->createActor(actorDesc);
		assert(actor);

		return actor;
	}

	return NULL;
}

NxActor* CreateConcaveMesh()
{
	NxBodyDesc  bodyDesc;
	NxActorDesc actorDesc;

	NxVec3 boxDim(1.0f, 1.0f, 1.0f);
	// Supply hull
	NxVec3 verts[16] = 
	{	
		NxVec3(-boxDim.x, -boxDim.y, -boxDim.z), 
		NxVec3(boxDim.x,  -boxDim.y, -boxDim.z), 
		NxVec3(-boxDim.x, boxDim.y,  -boxDim.z), 
		NxVec3(boxDim.x,  boxDim.y,  -boxDim.z),

		NxVec3(-boxDim.x, -boxDim.y, boxDim.z), 
		NxVec3(boxDim.x,  -boxDim.y, boxDim.z), 
		NxVec3(-boxDim.x, boxDim.y,  boxDim.z), 
		NxVec3(boxDim.x,  boxDim.y,  boxDim.z),
		
		NxVec3(-boxDim.x, -boxDim.y, 3*boxDim.z), 
		NxVec3(boxDim.x,  -boxDim.y, 3*boxDim.z), 
		NxVec3(-boxDim.x, boxDim.y,  3*boxDim.z), 
		NxVec3(boxDim.x,  boxDim.y,  3*boxDim.z),
		
		NxVec3(3*boxDim.x, -boxDim.y, 3*boxDim.z), 
		NxVec3(3*boxDim.x, -boxDim.y, boxDim.z), 
		NxVec3(3*boxDim.x, boxDim.y,  3*boxDim.z), 
		NxVec3(3*boxDim.x, boxDim.y,  boxDim.z) 
	};

	// Triangles is 12*3 - 8
	NxU32 indices[(12*3 - 8)*3] =
	{ 
		// BOX 1
		1,2,3,        
		0,2,1,  
		5,4,1,    
		1,4,0,    
		1,3,5,    
		3,7,5,    
		3,2,7,    
		2,6,7,    
		2,0,6,    
		4,6,0,
		// BOX 2  
		8,9,10,
		11,10,9,
		7,6,11,
		6,10,11,
		5,8,4,
		5,9,8,
		10,6,4,
		4,8,10,
		// BOX 3
		5,7,13,
		13,7,15,
		12,14,11,
		12,11,9,
		13,12,9,
		5,13,9,
		13,15,12,
		15,14,12,
		15,7,14,
		7,11,14,
	};

	// Create descriptor for triangle mesh
	if (!concaveDesc)
	{
		concaveDesc		= new NxTriangleMeshDesc();
		assert(concaveDesc);
	}	
	concaveDesc->numVertices			= 16;
	concaveDesc->pointStrideBytes		= sizeof(NxVec3);
	concaveDesc->points					= verts;
	concaveDesc->numTriangles			= 12*3 - 8;
	concaveDesc->triangles				= indices;
	concaveDesc->triangleStrideBytes	= 3 * sizeof(NxU32);
	concaveDesc->flags					= 0;
	assert(concaveDesc->isValid());

	// The actor has one shape, a triangle mesh
	NxTriangleMeshShapeDesc concaveShapeDesc;
	concaveShapeDesc.localPose.t = NxVec3(0,boxDim.y,0);
	concaveShapeDesc.userData	 = concaveDesc;
	
	NxInitCooking();
	MemoryWriteBuffer buf;
	NxTriangleMesh * pMesh = NULL;
	bool status = NxCookTriangleMesh(*concaveDesc, buf);
	assert(status);
	pMesh = gPhysicsSDK->createTriangleMesh(MemoryReadBuffer(buf.data));
	assert(pMesh);
	concaveShapeDesc.meshData = pMesh;
	NxCloseCooking();


	if (pMesh)
	{
		// Save mesh in userData for drawing
		pMesh->saveToDesc(*concaveDesc);
		//

		assert(concaveShapeDesc.isValid());
		actorDesc.shapes.pushBack(&concaveShapeDesc);
		//Dynamic triangle mesh don't be supported anymore. So body = NULL
		actorDesc.body			= NULL;
		actorDesc.globalPose.t	= NxVec3(-6.0f, 0.0f, 0.0f);
		assert(actorDesc.isValid());
		NxActor *pActor = gScene->createActor(actorDesc);
		assert(pActor);

		return pActor;	
	}

	return NULL;
}

void ReleaseNx()
{
	GetPhysicsResults();  // Make sure to fetchResults() before shutting down  

	if (convexDesc)
	{
		delete convexDesc;
		convexDesc = NULL;
	}

	if (triangleMeshDesc)
	{
		delete triangleMeshDesc;
		triangleMeshDesc = NULL;
	}

	if (concaveDesc)
	{
		delete concaveDesc;
		concaveDesc = NULL;
	}
	
	if (gScene)
	{
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
