// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//								LESSON 112 : OVERLAP
//
//						    Written by QA BJ, 6-2-2008
// ===============================================================================

#include "Lesson112.h"
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
float gCameraAspectRatio = 1;
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
NxVec3 gSphereCenter		= NxVec3(0, 2.0, 0.5);
NxVec3 gMIN					= NxVec3(-2, 0, -2);
NxVec3 gMAX					= NxVec3(2, 3, 3);
NxVec3 gBoxCenter			= NxVec3(0, 2, 0);
float  gSphereRadius		= 2.0f;
float  gCapsuleRadius		= 2.0f;
NxSegment	gCapsuleSegment; 
OverlapType gOverlapType	= OVERLAP_AABB;

// Actor globals
NxActor* groundPlane			= NULL;
NxConvexMeshDesc* convexDesc	= NULL;

// Focus actor
NxActor* gSelectedActor = NULL;

void PrintControls()
{
	printf("\n Flight Controls:\n ----------------\n w = forward, s = back\n a = strafe left, d = strafe right\n q = up, z = down\n");
    printf("\n Force Controls:\n ---------------\n i = +z, k = -z\n j = +x, l = -x\n u = +y, m = -y\n");
	printf("\n Wire Controls:\n --------------\n 1 = +z, 2 = -z\n 3 = +x, 4 = -x\n 5 = +y, 6 = -y\n");
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
	
	Update();

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
		case 'r': 
			{ 
				SelectNextActor(); 
				break; 
			}
		case 'c':
			{	// Iterate overlap type
				int i = (int)gOverlapType;
				i = (++i) % 9;
				gOverlapType = (OverlapType)i;
				break;
			}			
		case '1':
			if ((OVERLAP_SPHERE == gOverlapType) ||
				(OVERLAP_CHECK_SPHERE == gOverlapType))
			{
				gSphereCenter.z += 0.2f;
			}
			else if ((OVERLAP_CAPSULE == gOverlapType) ||
					 (OVERLAP_CHECK_CAPSULE == gOverlapType))
			{
				gCapsuleSegment.p0.z += 0.2f;
				gCapsuleSegment.p1.z += 0.2f;
			}
			else if ((OVERLAP_AABB == gOverlapType) ||
					 (OVERLAP_CHECK_AABB == gOverlapType))
			{
				gMIN.z += 0.2f;
				gMAX.z += 0.2f;		
			}
			else if ((OVERLAP_OBB == gOverlapType) ||
					 (OVERLAP_CHECK_OBB == gOverlapType))
			{
				gBoxCenter += NxVec3(0, 0, 0.2f);
			}
			break;
		case '2':
			if ((OVERLAP_SPHERE == gOverlapType) ||
				(OVERLAP_CHECK_SPHERE == gOverlapType))
			{
				gSphereCenter.z -= 0.2f;
			}
			else if ((OVERLAP_CAPSULE == gOverlapType) ||
					 (OVERLAP_CHECK_CAPSULE == gOverlapType))
			{
				gCapsuleSegment.p0.z -= 0.2f;
				gCapsuleSegment.p1.z -= 0.2f;
			}
			else if ((OVERLAP_AABB == gOverlapType) ||
					 (OVERLAP_CHECK_AABB == gOverlapType))
			{
				gMIN.z -= 0.2f;
				gMAX.z -= 0.2f;		
			}
			else if ((OVERLAP_OBB == gOverlapType) ||
					 (OVERLAP_CHECK_OBB == gOverlapType))
			{
				gBoxCenter += NxVec3(0, 0, -0.2f);
			}
			break;
		case '3':
			if ((OVERLAP_SPHERE == gOverlapType) ||
				(OVERLAP_CHECK_SPHERE == gOverlapType))
			{
				gSphereCenter.x += 0.2f;
			}
			else if ((OVERLAP_CAPSULE == gOverlapType) ||
					 (OVERLAP_CHECK_CAPSULE == gOverlapType))
			{
				gCapsuleSegment.p0.x += 0.2f;
				gCapsuleSegment.p1.x += 0.2f;
			}
			else if ((OVERLAP_AABB == gOverlapType) ||
					 (OVERLAP_CHECK_AABB == gOverlapType))
			{
				gMIN.x += 0.2f;
				gMAX.x += 0.2f;		
			}
			else if ((OVERLAP_OBB == gOverlapType) ||
					 (OVERLAP_CHECK_OBB == gOverlapType))
			{
				gBoxCenter += NxVec3(0.2f, 0, 0);
			}
			break;
		case '4':
			if ((OVERLAP_SPHERE == gOverlapType) ||
				(OVERLAP_CHECK_SPHERE == gOverlapType))
			{
				gSphereCenter.x -= 0.2f;
			}
			else if ((OVERLAP_CAPSULE == gOverlapType) ||
					 (OVERLAP_CHECK_CAPSULE == gOverlapType))
			{
				gCapsuleSegment.p0.x -= 0.2f;
				gCapsuleSegment.p1.x -= 0.2f;
			}
			else if ((OVERLAP_AABB == gOverlapType) ||
					 (OVERLAP_CHECK_AABB == gOverlapType))
			{
				gMIN.x -= 0.2f;
				gMAX.x -= 0.2f;		
			}
			else if ((OVERLAP_OBB == gOverlapType) ||
					 (OVERLAP_CHECK_OBB == gOverlapType))
			{
				gBoxCenter += NxVec3(-0.2f, 0, 0);
			}
			break;
		case '5':
			if ((OVERLAP_SPHERE == gOverlapType) ||
				(OVERLAP_CHECK_SPHERE == gOverlapType))
			{
				gSphereCenter.y += 0.2f;
			}
			else if ((OVERLAP_CAPSULE == gOverlapType) ||
					 (OVERLAP_CHECK_CAPSULE == gOverlapType))
			{
				gCapsuleSegment.p0.y += 0.2f;
				gCapsuleSegment.p1.y += 0.2f;
			}
			else if ((OVERLAP_AABB == gOverlapType) ||
					 (OVERLAP_CHECK_AABB == gOverlapType))
			{
				gMIN.y += 0.2f;
				gMAX.y += 0.2f;		
			}
			else if ((OVERLAP_OBB == gOverlapType) ||
					 (OVERLAP_CHECK_OBB == gOverlapType))
			{
				gBoxCenter += NxVec3(0, 0.2f, 0);
			}
			break;
		case '6':
			if ((OVERLAP_SPHERE == gOverlapType) ||
				(OVERLAP_CHECK_SPHERE == gOverlapType))
			{
				gSphereCenter.y -= 0.2f;
			}
			else if ((OVERLAP_CAPSULE == gOverlapType) ||
					 (OVERLAP_CHECK_CAPSULE == gOverlapType))
			{
				gCapsuleSegment.p0.y -= 0.2f;
				gCapsuleSegment.p1.y -= 0.2f;
			}
			else if ((OVERLAP_AABB == gOverlapType) ||
					 (OVERLAP_CHECK_AABB == gOverlapType))
			{
				gMIN.y -= 0.2f;
				gMAX.y -= 0.2f;		
			}
			else if ((OVERLAP_OBB == gOverlapType) ||
					 (OVERLAP_CHECK_OBB == gOverlapType))
			{
				gBoxCenter += NxVec3(0, -0.2f, 0);
			}
			break;
		default:  
			{
				break;
			}
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
    gMainHandle = glutCreateWindow("Lesson 112: OVERLAP");
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

NxActor* CreateSphere(const NxVec3 &pos, int flag)
{
	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// The actor has one shape, a sphere
	NxSphereShapeDesc sphereDesc;
	sphereDesc.radius		= 1.5f;
	sphereDesc.localPose.t	= NxVec3(0,2.0f,0);
	assert(sphereDesc.isValid());
	actorDesc.shapes.pushBack(&sphereDesc);

	if (0 == flag)
	{	//Dynamic actor
		bodyDesc.flags	   |= NX_BF_DISABLE_GRAVITY;
		actorDesc.body		= &bodyDesc;
		actorDesc.density	= 1.0f;
	}
	else if (1 == flag)
	{	//Static actor
		actorDesc.body		= NULL;
		actorDesc.density	= 1.0f;
	}
	else if (2 == flag)
	{	// Kinematic actor
		bodyDesc.flags	   |= NX_BF_KINEMATIC;
		actorDesc.body		= &bodyDesc;
		actorDesc.density	= 1.0f;
	}
	actorDesc.globalPose.t	= pos;	//NxVec3(-6.5f, 0, 0);
	assert(actorDesc.isValid());
	NxActor *pActor = gScene->createActor(actorDesc);
	assert(pActor);
	return pActor;
}

NxActor* CreateBox(const NxVec3 &pos, int flag)
{
	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// The actor has one shape, a box
	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions.set(0.8f, 1.0f, 0.8f);
	boxDesc.localPose.t = NxVec3(0,2.0f,0);
	actorDesc.shapes.pushBack(&boxDesc);

	if (0 == flag)
	{	//Dynamic actor
		bodyDesc.flags	   |= NX_BF_DISABLE_GRAVITY;
		actorDesc.body		= &bodyDesc;
		actorDesc.density	= 1.0f;
	}
	else if (1 == flag)
	{	//Static actor
		actorDesc.body		= NULL;
		actorDesc.density	= 1.0f;
	}
	else if (2 == flag)
	{	// Kinematic actor
		bodyDesc.flags	   |= NX_BF_KINEMATIC;
		actorDesc.body		= &bodyDesc;
		actorDesc.density	= 1.0f;
	}

	actorDesc.globalPose.t	= pos; //NxVec3(-2.0f, 0, 0);
	NxActor* pActor = gScene->createActor(actorDesc);
	assert(pActor);
	return pActor;
}

NxActor* CreateCapsule(const NxVec3 &pos, int flag)
{
	NxActorDesc actorDesc;
	NxBodyDesc  bodyDesc;

	NxCapsuleShapeDesc capsuleDesc;
	capsuleDesc.radius		= 0.7f;
	capsuleDesc.height		= 1.0f;
	capsuleDesc.localPose.t	= NxVec3(0, 2.0f, 0);
	actorDesc.shapes.pushBack(&capsuleDesc);

	if (0 == flag)
	{	//Dynamic actor
		bodyDesc.flags	   |= NX_BF_DISABLE_GRAVITY;
		actorDesc.body		= &bodyDesc;
		actorDesc.density	= 1.0f;
	}
	else if (1 == flag)
	{	//Static actor
		actorDesc.body		= NULL;
		actorDesc.density	= 1.0f;
	}
	else if (2 == flag)
	{	// Kinematic actor
		bodyDesc.flags	   |= NX_BF_KINEMATIC;
		actorDesc.body		= &bodyDesc;
		actorDesc.density	= 1.0f;
	}
	
	actorDesc.globalPose.t	= pos; //NxVec3(2.0f, 0, 0);
	NxActor* pActor = gScene->createActor(actorDesc);
	assert(pActor);
	return pActor;
}

NxActor* CreateConvex(const NxVec3 &pos, int flag)
{
   NxActorDesc actorDesc;
    NxBodyDesc bodyDesc;
	
	NxVec3 boxDim(1,0.8,1.5);
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
		convexDesc = new  NxConvexMeshDesc();
		assert(convexDesc);
	 }

    convexDesc->numVertices			= 8;
    convexDesc->pointStrideBytes	= sizeof(NxVec3);
    convexDesc->points				= verts;
	convexDesc->flags				= NX_CF_COMPUTE_CONVEX;

	NxConvexShapeDesc convexShapeDesc;
	convexShapeDesc.localPose.t		= NxVec3(0, 2.0f, boxDim.z * 0.4);
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
		NxActorDesc actorDesc;
		assert(convexShapeDesc.isValid());
		actorDesc.shapes.pushBack(&convexShapeDesc);
		if (0 == flag)
		{	//Dynamic actor
			bodyDesc.flags	   |= NX_BF_DISABLE_GRAVITY;
			actorDesc.body		= &bodyDesc;
			actorDesc.density	= 1.0f;
		}
		else if (1 == flag)
		{	//Static actor
			actorDesc.body		= NULL;
			actorDesc.density	= 1.0f;
		}
		else if (2 == flag)
		{	// Kinematic actor
			bodyDesc.flags	   |= NX_BF_KINEMATIC;
			actorDesc.body		= &bodyDesc;
			actorDesc.density	= 1.0f;
		}

		actorDesc.globalPose.t  = pos; //NxVec3(6.5f, 0.0f, 0.0f);
		assert(actorDesc.isValid());
		NxActor* actor = gScene->createActor(actorDesc);
		assert(actor);	
		return actor;
    }

    return NULL;
}

void Update()
{
	NxMat34 mat34;
	NxMat33 mat;
	NxQuat quat(0.0f,NxVec3(0,1,0));  
	mat.fromQuat(quat);
	NxBox worldBox;
	worldBox.extents	= NxVec3(2, 2, 2);
	worldBox.rot		= mat;

	NxSphere	worldSphere;
	NxBounds3	worldBounds;
	NxCapsule	worldCapsule;
	worldCapsule.radius = 2.0f;

	NxU32 nbPlanes = 2;
	NxPlane worldPlanes[2];
	worldPlanes[0].set(NxVec3(-2,0,2), NxVec3(0,0,1));
	worldPlanes[1].set(NxVec3(-2,0,2), NxVec3(1,0,0));

	NxU32 nbDynamicShapes	= gScene->getNbDynamicShapes();
	NxU32 nbStaticShapes	= gScene->getNbStaticShapes();
	NxU32 nbShapes = 0;
	NxShapesType type;

	int i = 0;
	for (i = 0; i < 3; ++ i)
	{
		if (i == 0)
		{
			nbShapes = nbDynamicShapes;
			type	 = NX_DYNAMIC_SHAPES;
			switch(gOverlapType)
			{
			case OVERLAP_AABB:
			case OVERLAP_CHECK_AABB:
				worldBounds.set(gMIN, gMAX);
				break;
			case OVERLAP_OBB:
			case OVERLAP_CHECK_OBB:
				worldBox.center = gBoxCenter;
				break;
			case OVERLAP_CAPSULE:
			case OVERLAP_CHECK_CAPSULE:
				worldCapsule = NxCapsule(gCapsuleSegment, gCapsuleRadius);
				break;
			case OVERLAP_SPHERE:
			case OVERLAP_CHECK_SPHERE:
				worldSphere = NxSphere(gSphereCenter, gSphereRadius);
				break;
			}
		}
		else if (i == 1)
		{
			nbShapes = nbStaticShapes;
			type	 = NX_STATIC_SHAPES;
			switch(gOverlapType)
			{
			case OVERLAP_AABB:
			case OVERLAP_CHECK_AABB:
				worldBounds.set(gMIN+NxVec3(-6.0f,0,0),gMAX+NxVec3(-6.0f,0,0));
				break;
			case OVERLAP_OBB:
			case OVERLAP_CHECK_OBB:
				worldBox.center = gBoxCenter+NxVec3(-6,0,0);
				break;
			case OVERLAP_CAPSULE:
			case OVERLAP_CHECK_CAPSULE:
				worldCapsule.p0.x = gCapsuleSegment.p0.x - 6.0f;
				worldCapsule.p1.x = gCapsuleSegment.p1.x - 6.0f;
				break;
			case OVERLAP_SPHERE:
			case OVERLAP_CHECK_SPHERE:
				worldSphere = NxSphere(gSphereCenter + NxVec3(-6,0,0), gSphereRadius);
				break;
			}
		}
		else if (i == 2)
		{
			nbShapes = nbStaticShapes + nbDynamicShapes;
			type	 = NX_ALL_SHAPES;
			switch(gOverlapType)
			{
			case OVERLAP_AABB:
			case OVERLAP_CHECK_AABB:
				worldBounds.set(gMIN+NxVec3(6.0f,0,0),gMAX+NxVec3(6.0f,0,0));
				break;
			case OVERLAP_OBB:
			case OVERLAP_CHECK_OBB:
				worldBox.center = gBoxCenter+NxVec3(6,0,0);
				break;
			case OVERLAP_CAPSULE:
			case OVERLAP_CHECK_CAPSULE:
				worldCapsule.p0.x = gCapsuleSegment.p0.x + 6.0f;
				worldCapsule.p1.x = gCapsuleSegment.p1.x + 6.0f;
				break;
			case OVERLAP_SPHERE:
			case OVERLAP_CHECK_SPHERE:
				worldSphere = NxSphere(gSphereCenter + NxVec3(6,0,0), gSphereRadius);
				break;
			}
		}

		NxShape** shapes = (NxShape**)NxAlloca(nbShapes*sizeof(NxShape*));
		for (NxU32 j = 0; j < nbShapes; j++)  shapes[j] = NULL;
		NxU32 activeGroups = 0xffffffff;
		NxGroupsMask* groupsMask = NULL;
		bool bResult	= true;
		float linewidth = 1.0f;
		switch(gOverlapType)
		{
		case OVERLAP_AABB:
			gScene->overlapAABBShapes(worldBounds, type, nbShapes, shapes, &gShapeReport, activeGroups, groupsMask, true);
			NxCreateBox(worldBox, worldBounds, mat34);
			DrawWireBox(worldBox, NxVec3(1,0,0), linewidth);
			break;
		case OVERLAP_CHECK_AABB:
			bResult = gScene->checkOverlapAABB(worldBounds, type, activeGroups, groupsMask);
			NxCreateBox(worldBox, worldBounds, mat34);
			if (bResult == true)
				DrawWireBox(worldBox, NxVec3(1,0,0), linewidth);
			else
				DrawWireBox(worldBox, NxVec3(0,1,0), linewidth);
			break;
		case OVERLAP_OBB:
			gScene->overlapOBBShapes(worldBox, type, nbShapes, shapes, &gShapeReport, activeGroups, groupsMask);
			DrawWireBox(worldBox, NxVec3(1,0,0), linewidth);
			break;
		case OVERLAP_CHECK_OBB:
			if (gScene->checkOverlapOBB(worldBox, type, activeGroups, groupsMask) == true)
				DrawWireBox(worldBox, NxVec3(1,0,0), linewidth);
			else
				DrawWireBox(worldBox, NxVec3(0,1,0), linewidth);
			break;
		case OVERLAP_CAPSULE:
			gScene->overlapCapsuleShapes(worldCapsule, type, nbShapes, shapes, &gShapeReport, activeGroups, groupsMask);
			DrawWireCapsule(worldCapsule, NxVec3(1,0,0));
			break;
		case OVERLAP_CHECK_CAPSULE:
			if (gScene->checkOverlapCapsule(worldCapsule, type,activeGroups, groupsMask) == true)
				DrawWireCapsule(worldCapsule, NxVec3(1,0,0));
			else
				DrawWireCapsule(worldCapsule, NxVec3(0,1,0));
			break;
		case OVERLAP_SPHERE:
			gScene->overlapSphereShapes(worldSphere, type, nbShapes, shapes, &gShapeReport, activeGroups, groupsMask);
			DrawWireSphere(&worldSphere, NxVec3(1,0,0));
			break;
		case OVERLAP_CHECK_SPHERE:
			if (gScene->checkOverlapSphere(worldSphere, type,activeGroups, groupsMask) == true)
				DrawWireSphere(&worldSphere, NxVec3(1,0,0));
			else
				DrawWireSphere(&worldSphere, NxVec3(0,1,0));
			break;
		case OVERLAP_CULL:
			gScene->cullShapes(nbPlanes, worldPlanes, type, nbShapes, shapes, &gShapeReport, activeGroups, groupsMask);
			DrawLine(NxVec3(-20,0,2), NxVec3(-2,0,2),NxVec3(1,0,0), linewidth);
			DrawLine(NxVec3(-2,0,-20), NxVec3(-2,0,2),NxVec3(1,0,0), linewidth);
			break;
		}
	}
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
	gPhysicsSDK->setParameter(NX_DEFAULT_SLEEP_LIN_VEL_SQUARED, 0.15*0.15);
	gPhysicsSDK->setParameter(NX_DEFAULT_SLEEP_ANG_VEL_SQUARED, 0.14*0.14);

	// Set the debug visualization parameters
	gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_ACTOR_AXES, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_AABBS, 1);

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

	// Create dynamic actors
	CreateSphere(NxVec3(6.0f, 0.0f, 24.0f), 0);
	CreateBox(NxVec3(6.0f, 0.0f, 16.0f), 0);
	CreateCapsule(NxVec3(6.0f, 0.0f, 6.0f), 0);
	gSelectedActor = CreateConvex(NxVec3(6.0f, 0.0f, 0.0f), 0);
	
	// Create static actors
	CreateSphere(NxVec3(0.0f, 0.0f, 24.0f), 0);
	CreateBox(NxVec3(0.0f, 0.0f, 16.0f), 0);
	CreateCapsule(NxVec3(0.0f, 0.0f, 6.0f), 0);
	CreateConvex(NxVec3(0.0f, 0.0f, 0.0f), 0);

	// Create kinematic actors
	CreateSphere(NxVec3(-6.0f, 0.0f, 24.0f), 0);
	CreateBox(NxVec3(-6.0f, 0.0f, 16.0f), 0);
	CreateCapsule(NxVec3(-6.0f, 0.0f, 6.0f), 0);
	CreateConvex(NxVec3(-6.0f, 0.0f, 0.0f), 0);

	bPause = false;

	gCapsuleSegment.p0 = NxVec3(0, 1.2f, 0.5f);
	gCapsuleSegment.p1 = NxVec3(0, 3.2f, 0.5f);

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



