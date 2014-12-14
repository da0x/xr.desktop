// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//						  LESSON 1201: CHARACTER CONTROLLER
//
//						    Written by Bob Schade, 5-1-06
//							Edited by Xueyan Gong, 6-25-08
// ===============================================================================

#include "Lesson1201.h"
#include "Timing.h"

// Physics SDK globals
NxPhysicsSDK*   gPhysicsSDK = NULL;
NxScene*        gScene = NULL;

// User report globals
DebugRenderer   gDebugRenderer;
UserAllocator*	gAllocator = NULL;

// HUD globals
HUD hud;

// Display globals
int gMainHandle;
int mx = 0;
int my = 0;

// Camera globals
float gCameraAspectRatio = 1;
NxVec3 gCameraPos(0,10,-30);
NxVec3 gCameraForward(0,0,1);
NxVec3 gCameraRight(-1,0,0);
const NxReal gCameraSpeed = 0.2;

// Keyboard globals
#define MAX_KEYS 256
bool gKeys[MAX_KEYS];

// Simulation globals
bool bHardwareScene = true;
bool bPause = false;
bool bDebugWireframeMode = false;

// Terrain globals
#define TERRAIN_SIZE		33
#define TERRAIN_NB_VERTS	TERRAIN_SIZE*TERRAIN_SIZE
#define TERRAIN_NB_FACES	(TERRAIN_SIZE-1)*(TERRAIN_SIZE-1)*2
#define TERRAIN_OFFSET		-20
#define TERRAIN_WIDTH		10
#define TERRAIN_CHAOS		150

NxVec3* gTerrainVerts = NULL;
NxVec3* gTerrainNormals = NULL;
NxU32* gTerrainFaces = NULL;

TerrainData* gTerrainData = NULL;
NxActor* gTerrain = NULL;

// Character controller
NxControllerManager* gManager = NULL;
MyCharacterController* gBoxController = NULL;
MyCharacterController* gCapsuleController = NULL;
MyCharacterController* gSelectedController = NULL;

NxF32 gCharacterSpeed = 10;
NxVec3 gCharacterVec = NxVec3(1,0,0);
bool bPushCharacter = false;
/*
a(t) = cte = g
v(t) = g*t + v0
y(t) = 1/2*g*t^2 + v*t
*/
bool gJump = false;
NxF32 gV0;
NxF32 gJumpTime;

NxF32 G = -9.8;
NxVec3 gDefaultGravity(0,-9.8,0);

void StartJump(NxF32 v0)
{
	if (gJump)  return;
	gJumpTime = 0.0f;
	gV0	= v0;
	gJump = true;
}

void StopJump()
{
	gJump = false;
}

NxF32 GetHeight(NxF32 elapsedTime)
{
	if (!gJump)  return 0.0f;
	NxF32 Vt = gV0 + G*gJumpTime; // Vt = Vo + GT
	gJumpTime += elapsedTime;
	return Vt*elapsedTime + 1/2*G*elapsedTime*elapsedTime; // S = VtT + 1/2GT^2
}

void PrintControls()
{
	printf("\n Flight Controls:\n ----------------\n w = forward, s = back\n a = strafe left, d = strafe right\n q = up, z = down\n");
	printf("\n Character Controls:\n ---------------\n i = +z, k = -z\n j = +x, l = -x\n u = +y\n");
	printf("\n Miscellaneous:\n --------------\n p = Pause\n n = Select Next Character\n t = Expand Character Extent\n r = Shrink Character Extent\n 1,2,3 = Add New Objects to Scene\n");
}

void ProcessKeys()
{
	// Process keys
	for (int i = 0; i < MAX_KEYS; i++)
	{	
		if (!gKeys[i])  { continue; }

		switch (i)
		{
		// Camera controls
		case 'w':{ gCameraPos += gCameraForward*gCameraSpeed; break; }
		case 's':{ gCameraPos -= gCameraForward*gCameraSpeed; break; }
		case 'a':{ gCameraPos -= gCameraRight*gCameraSpeed; break; }
		case 'd':{ gCameraPos += gCameraRight*gCameraSpeed; break; }
		case 'z':{ gCameraPos -= NxVec3(0,1,0)*gCameraSpeed; break; }
		case 'q':{ gCameraPos += NxVec3(0,1,0)*gCameraSpeed; break; }

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

void RenderActors()
{
	// Render all the actors in the scene
	NxU32 nbActors = gScene->getNbActors();
	NxActor** actors = gScene->getActors();
	while (nbActors--)
	{
		NxActor* actor = *actors++;

		// Skip actors with UD_NO_RENDER flag set
		ActorUserData* ud = (ActorUserData*)(actor->userData);
		if (ud && (ud->flags & UD_NO_RENDER))  continue;


		// Draw controller with special color
		if (gBoxController->GetCharacterActor() == actor || gCapsuleController->GetCharacterActor() == actor)
			glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	
		DrawActor(actor, NULL, true);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}
}

void RenderCallback()
{
	if(gScene && !bPause)  RunPhysics();

	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ProcessKeys();
	SetupCamera();
 	RenderActors();

	// Show debug wireframes
	if (bDebugWireframeMode)
	{
		if (gScene)  gDebugRenderer.renderData(*gScene->getDebugRenderable());
	}

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
	// Move character control
	case 'i': {gCharacterVec = NxVec3(0,0,1); bPushCharacter = true; break; }
	case 'k': {gCharacterVec = NxVec3(0,0,-1); bPushCharacter = true; break; }
	case 'j': {gCharacterVec = NxVec3(1,0,0); bPushCharacter = true; break; }
	case 'l': {gCharacterVec = NxVec3(-1,0,0); bPushCharacter = true; break; }

	default: { break; }
	}
}

void KeyboardUpCallback(unsigned char key, int x, int y)
{
	gKeys[key] = false;

	switch (key)
	{
	case 'i':  // Stop moving
	case 'k': 
	case 'j': 
	case 'l': bPushCharacter = false; break; 

	case 'p': { bPause = !bPause; 
		if (bPause)
			hud.SetDisplayString(1, "Paused - Hit \"p\" to Unpause", 0.3f, 0.55f);
		else
			hud.SetDisplayString(1, "", 0.0f, 0.0f);	
		getElapsedTime(); 
		break; }
	case 'b': { bDebugWireframeMode = !bDebugWireframeMode; break; }		
	case 'u': { StartJump(15.0f); break; }
	case 'g': gSelectedController->SetCharacterPos(NxVec3(0.0f, 5.0f, 0.0f)); break;
	case '1': 
		{
			NxActor* box = CreateBox(NxVec3(0,20,0), NxVec3(1,1,1), 10); 
			box->setAngularDamping(0.5);
			SetActorCollisionGroup(box, GROUP_COLLIDABLE_PUSHABLE);
			break;
		}
	case '2': 
		{
			NxActor* sphere = CreateSphere(NxVec3(0,20,0), 1, 10);
			sphere->setAngularDamping(0.5);
			SetActorCollisionGroup(sphere, GROUP_COLLIDABLE_PUSHABLE);
			break;
		}
	case '3': 
		{
			NxActor* capsule = CreateCapsule(NxVec3(0,20,0), 4, 1, 10);
			capsule->setAngularDamping(0.5);
			SetActorCollisionGroup(capsule, GROUP_COLLIDABLE_PUSHABLE);
			break;
		}
	case 27 : { exit(0); break; }
	case 't':
		{
			gSelectedController->UpdateCharacterExtents(gScene, NxVec3(0.3f));
			break;
		}
	case 'r':
		{
			gSelectedController->UpdateCharacterExtents(gScene, NxVec3(-0.3f));
			break;
		}
	case 'n':
		{
			if (gSelectedController == gBoxController)
				gSelectedController = gCapsuleController;
			else
				gSelectedController = gBoxController;
			break;
		}
	case ' ': 
		{
			NxActor* sphere = CreateSphere(gCameraPos, 1, 1);
			sphere->setLinearVelocity(gCameraForward * 20);
			break; 
		}
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

NxActor* CreateTerrain()
{
	gTerrainData = new TerrainData;
	gTerrainData->init(TERRAIN_SIZE, TERRAIN_OFFSET, TERRAIN_WIDTH, TERRAIN_CHAOS, true);

	// Build physical model
	NxTriangleMeshDesc terrainDesc;
	terrainDesc.numVertices					= TERRAIN_NB_VERTS;
	terrainDesc.numTriangles				= TERRAIN_NB_FACES;
	terrainDesc.pointStrideBytes			= sizeof(NxVec3);
	terrainDesc.triangleStrideBytes			= 3*sizeof(NxU32);
	terrainDesc.points						= gTerrainData->verts;
	terrainDesc.triangles					= gTerrainData->faces;
	terrainDesc.flags = (gScene->getSimType() == NX_SIMULATION_HW) ?  NX_MF_HARDWARE_MESH :0;

	NxTriangleMeshShapeDesc terrainShapeDesc;

	NxInitCooking();
	MemoryWriteBuffer writeBuffer;
	bool status = NxCookTriangleMesh(terrainDesc, writeBuffer);
	terrainShapeDesc.meshData = gPhysicsSDK->createTriangleMesh(MemoryReadBuffer(writeBuffer.data));
	NxCloseCooking();

	terrainShapeDesc.group					= GROUP_COLLIDABLE_NON_PUSHABLE;
	if (gScene->getSimType() == NX_SIMULATION_HW)
		terrainShapeDesc.meshPagingMode = NX_MESH_PAGING_AUTO;

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&terrainShapeDesc);
	return gScene->createActor(actorDesc);
}

void SetupScene()
{
	// Create randomized terrain
	gTerrain = CreateTerrain();

	// Create stairs
	NxU32 i;
    for(i=0;i<20;i++)
	{	
		NxActor* box = CreateBox(NxVec3(-10 - NxF32(i), -3.6 + NxF32(i)*0.5, 0), NxVec3(2,2,2), 0);
		SetActorCollisionGroup(box, GROUP_COLLIDABLE_NON_PUSHABLE);
	}

	// Create cube too high for autostep
	NxActor* box1 = CreateBox(NxVec3(-10, -1-2, -10), NxVec3(2,2,2), 0);
	SetActorCollisionGroup(box1, GROUP_COLLIDABLE_NON_PUSHABLE);

	// Create rotated cube
	NxActor* box2 = CreateBox(NxVec3(10, 4-2, -10), NxVec3(2,2,2), 0);
	SetActorCollisionGroup(box2, GROUP_COLLIDABLE_NON_PUSHABLE);
	NxMat33 M = box2->getGlobalOrientation();
	M.rotX(0.5);
	box2->setGlobalOrientation(M);

	NxActor* box3 = CreateBox(NxVec3(10, 10-2, 10), NxVec3(2,2,2), 0);
	SetActorCollisionGroup(box3, GROUP_COLLIDABLE_NON_PUSHABLE);

	NxActor* box4 = CreateBox(NxVec3(-10, 5.5-2, 10), NxVec3(2,2,2), 0);
	SetActorCollisionGroup(box4, GROUP_COLLIDABLE_NON_PUSHABLE);

	NxActor* sphere1 = CreateSphere(NxVec3(0, 0-1, 10), 1, 0);
	SetActorCollisionGroup(sphere1, GROUP_COLLIDABLE_NON_PUSHABLE);

	NxActor* sphere2 = CreateSphere(NxVec3(0, 1-1, 5), 1, 0);
	SetActorCollisionGroup(sphere2, GROUP_COLLIDABLE_NON_PUSHABLE);

	NxActor* capsule1 = CreateCapsule(NxVec3(0, -1.5-2, -10), 3, 2, 0);
	SetActorCollisionGroup(capsule1, GROUP_COLLIDABLE_NON_PUSHABLE);

	// Create walls
	NxU32 numBoxes = 20;
	NxReal halfBoxSize = 2;
	NxReal wallLength = numBoxes*halfBoxSize*2;
	for(i=0;i<numBoxes;i++)
	{
		NxActor* box = CreateBox(NxVec3((NxF32(i)/NxF32(numBoxes-1))*wallLength - wallLength*0.5f, 0, wallLength*0.5f), NxVec3(halfBoxSize,halfBoxSize,halfBoxSize), 0);
		SetActorCollisionGroup(box, GROUP_COLLIDABLE_NON_PUSHABLE);
	}
	for(i=0;i<numBoxes;i++)
	{
		NxActor* box = CreateBox(NxVec3((NxF32(i)/NxF32(numBoxes-1))*wallLength - wallLength*0.5f, 0, -wallLength*0.5f), NxVec3(halfBoxSize,halfBoxSize,halfBoxSize), 0);
		SetActorCollisionGroup(box, GROUP_COLLIDABLE_NON_PUSHABLE);
	}
	for(i=0;i<numBoxes;i++)
	{
		NxActor* box = CreateBox(NxVec3(-wallLength*0.5f, 0, (NxF32(i)/NxF32(numBoxes-1))*wallLength - wallLength*0.5f), NxVec3(halfBoxSize,halfBoxSize,halfBoxSize), 0);
		SetActorCollisionGroup(box, GROUP_COLLIDABLE_NON_PUSHABLE);
	}
	for(i=0;i<numBoxes;i++)
	{
		NxActor* box = CreateBox(NxVec3(wallLength*0.5f, 0, (NxF32(i)/NxF32(numBoxes-1))*wallLength - wallLength*0.5f), NxVec3(halfBoxSize,halfBoxSize,halfBoxSize), 0);
		SetActorCollisionGroup(box, GROUP_COLLIDABLE_NON_PUSHABLE);
	}

	// Initialize controller manager
	gManager = NxCreateControllerManager(gAllocator);
	// Create character controllers
	gBoxController = new MyCharacterController(gManager, gScene, NxVec3(0, 10, 0), NxVec3(0.8,1.5,0.8));
	gCapsuleController = new MyCharacterController(gManager, gScene, NxVec3(0, 8, 0), 0.8, 1.5);
	gSelectedController = gCapsuleController;

	AddUserDataToActors(gScene);

	// Set the heightfield to use light #1 for rendering
	((ActorUserData*)(gTerrain->userData))->flags |= UD_RENDER_USING_LIGHT1;
}

void InitGlut(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(512, 512);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	gMainHandle = glutCreateWindow("Lesson 1201: Character Controller");
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
	glClearColor(0.3f, 0.3f, 0.3f, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_CULL_FACE);

	// Setup lighting
	glEnable(GL_LIGHTING);
	NxF32 AmbientColor[]	= { 0.0f, 0.1f, 0.2f, 0.0f };		   glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);
	NxF32 DiffuseColor[]	= { 1.0f, 1.0f, 1.0f, 0.0f };		   glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);
	NxF32 SpecularColor[]	= { 0.0f, 0.0f, 0.0f, 0.0f };		   glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularColor);
	NxF32 Position[]		= { -10.0f, 100.0f, -4.0f, 1.0f };	   glLightfv(GL_LIGHT0, GL_POSITION, Position);
	glEnable(GL_LIGHT0);

	NxF32 AmbientColor1[]    = { 0.0f, 0.1f, 0.2f, 0.0f };         glLightfv(GL_LIGHT1, GL_AMBIENT, AmbientColor1);
	NxF32 DiffuseColor1[]    = { 0.9f, 0.9f, 0.9f, 0.0f };         glLightfv(GL_LIGHT1, GL_DIFFUSE, DiffuseColor1);
	NxF32 SpecularColor1[]   = { 0.9f, 0.9f, 0.9f, 0.0f };         glLightfv(GL_LIGHT1, GL_SPECULAR, SpecularColor1);
	NxF32 Position1[]        = { 0.0f, 5.0f, 0.0f, 1.0f };	       glLightfv(GL_LIGHT1, GL_POSITION, Position1);
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
	// Create a memory allocator
	gAllocator = new UserAllocator;

	// Create the physics SDK
	gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, gAllocator, NULL);
	if (!gPhysicsSDK)  return;

	// Set the physics parameters
	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.05);

	// Set the debug visualization parameters
	gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_ACTOR_AXES, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_FNORMALS, 1);

	// Create the scene
	NxSceneDesc sceneDesc;
	sceneDesc.gravity = gDefaultGravity;
	sceneDesc.simType = NX_SIMULATION_HW;
	gScene = gPhysicsSDK->createScene(sceneDesc);	
	if(!gScene){ 
		sceneDesc.simType = NX_SIMULATION_SW; 
		gScene = gPhysicsSDK->createScene(sceneDesc);  
		if(!gScene) return;
	}

	float TimeStep = 1.0f / 60.0f;
	gScene->setTiming(TimeStep, 1, NX_TIMESTEP_VARIABLE);

	// Create the default material
	NxMaterial* defaultMaterial = gScene->getMaterialFromIndex(0); 
	defaultMaterial->setRestitution(0.0);
	defaultMaterial->setStaticFriction(0.5);
	defaultMaterial->setDynamicFriction(0.5);

	SetupScene();

	// Initialize HUD
	InitializeHUD();

	// Get the current time
	getElapsedTime();
}

void ReleaseNx()
{
	delete gCapsuleController;
	delete gBoxController;
	gCapsuleController = NULL;
	gBoxController = NULL;
	gSelectedController = NULL;

	NxReleaseControllerManager(gManager);

	if (gScene) gPhysicsSDK->releaseScene(*gScene);
	if (gPhysicsSDK)  NxReleasePhysicsSDK(gPhysicsSDK);

	delete gTerrainData;
	NX_DELETE_SINGLE(gAllocator);
}

void ResetNx()
{
	ReleaseNx();
	InitNx();
}

void UpdateCharacter(NxReal deltaTime)
{
	NxVec3 disp = gDefaultGravity;

	if (bPushCharacter)
	{
		NxVec3 horizontalDisp = gCharacterVec;
		horizontalDisp.y = 0.0f;
		horizontalDisp.normalize();
		disp += horizontalDisp * gCharacterSpeed;
	}

	disp *= deltaTime;
	NxF32 height = GetHeight(deltaTime); // compute height(Y) in jumping
	if (height != 0.0f)
	{
		disp.y += height;
	}

	NxU32 collisionFlags;
	gSelectedController->Move(disp, collisionFlags);
	if (collisionFlags & NXCC_COLLISION_DOWN)  
		StopJump();

	gManager->updateControllers();
}

void RunPhysics()
{
	// Update the time step
	NxReal deltaTime = getElapsedTime();

	// Update the box character's position according to by testing its
	// axis-aligned bounding box against a copy of the current scene.
	UpdateCharacter(deltaTime);

	// Run collision and dynamics for delta time since the last frame
	gScene->simulate(deltaTime);
	gScene->flushStream();
	gScene->fetchResults(NX_RIGID_BODY_FINISHED, true);

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
