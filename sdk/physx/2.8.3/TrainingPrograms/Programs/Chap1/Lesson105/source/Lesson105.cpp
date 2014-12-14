// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//                              LESSON 105 : MATERIALS
//
//						    Written by QA BJ, 6-2-2008
// ===============================================================================

#include "Lesson105.h"
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

// Focus actor
NxActor* gSelectedActor = NULL;

// Material Index
static NxMaterialIndex	boxIndex;
static NxMaterialIndex	capsuleIndex;
static NxMaterialIndex	sphereIndex;

static NxMaterialIndex	iceIndex;
static NxMaterialIndex	rockIndex;
static NxMaterialIndex	mudIndex;
static NxMaterialIndex	grassIndex;

// Define Terrain(Triangle)
const NxU32 TERRAIN_SIZE		= 33;
const NxU32 TERRAIN_NB_VERTS	= TERRAIN_SIZE*TERRAIN_SIZE;
const NxU32 TERRAIN_NB_FACES	= (TERRAIN_SIZE-1)*(TERRAIN_SIZE-1)*2;
const NxU32 TERRAIN_OFFSET		= -20.0f;
const NxU32 TERRAIN_WIDTH		= 20.0f;
const NxU32 TERRAIN_CHAOS		= 150.0f;

class Random
{
	unsigned long next;

public:

	static const unsigned int rand_max = 32767;

	Random(unsigned seed = 1)
	{
		next = 0;
		srand(seed);
	}

	int rand(void)  /* RAND_MAX assumed to be 32767. */
	{
	    next = next * 1103515245 + 12345;
	    return((unsigned)(next/65536) % 32768);
	}

	void srand(unsigned seed)
	{
	    next = seed;
	}
};

float trand()
{
	static Random random(4095);
	return ((float)random.rand()/((float)random.rand_max+1))*(2.0f) -1.0f;
}

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
    gMainHandle = glutCreateWindow("Lesson 105: Materials");
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

NxActor* CreateCompoundActor()
{
	NxActorDesc actorDesc;
	NxBodyDesc  bodyDesc;	
	
	//Box Shape
	NxBoxShapeDesc boxShape;
	boxShape.dimensions		= NxVec3(1.0f, 1.0f, 1.0f);
	boxShape.localPose.t	= NxVec3(1.5f, 0.0f, 0.0f);
	boxShape.materialIndex	= boxIndex;
	assert(boxShape.isValid());
	actorDesc.shapes.pushBack(&boxShape);

	//Capsule Shape
	NxCapsuleShapeDesc capsuleDesc;
	capsuleDesc.radius		= 0.8f;
	capsuleDesc.height		= 1.0f;
	capsuleDesc.localPose.t = NxVec3(0.0f, 0.0f, 0.0f);
	capsuleDesc.materialIndex = capsuleIndex;
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
	sphereDesc.materialIndex = sphereIndex;
	assert(sphereDesc.isValid());
	actorDesc.shapes.pushBack(&sphereDesc);

	actorDesc.body			= &bodyDesc;
	actorDesc.density		= 1.0f;
	actorDesc.globalPose.t	= NxVec3(0.0f, 8.0f, 0.0f);
	assert(actorDesc.isValid());

	NxActor *pActor = gScene->createActor(actorDesc);
	assert(pActor);

	return pActor;
}


NxActor* CreateTriangleActor()
{
	// Initialize terrain vertices
	NxVec3* TerrainVerts = new NxVec3[TERRAIN_NB_VERTS];
	assert(TerrainVerts);
	NxU32 x = 0;
	NxU32 y = 0;
	NxVec3 v;
	for(y=0;y<TERRAIN_SIZE;y++)
	{
		for(x=0;x<TERRAIN_SIZE;x++)
		{
			v.set(NxF32(x)-(NxF32(TERRAIN_SIZE-1)*0.5f), 0.0f, NxF32(y)-(NxF32(TERRAIN_SIZE-1)*0.5f));
			v *= TERRAIN_WIDTH;
			TerrainVerts[x+y*TERRAIN_SIZE] = v;
		}
	}

	// Fractalize
	bool* done = new bool[TERRAIN_NB_VERTS];
	memset(done,0,TERRAIN_NB_VERTS);
	TerrainVerts[0].y								= 10.0f;
	TerrainVerts[TERRAIN_SIZE-1].y					= 10.0f;
	TerrainVerts[TERRAIN_SIZE*(TERRAIN_SIZE-1)].y	= 10.0f;
	TerrainVerts[TERRAIN_NB_VERTS-1].y				= 10.0f;
	ComputeTerrain(done, TerrainVerts, 0, 0, TERRAIN_SIZE, TERRAIN_CHAOS);
	for(x=0; x<TERRAIN_NB_VERTS; x++)	
		TerrainVerts[x].y += TERRAIN_OFFSET;
	delete[] done;

	// Initialize terrain faces
	NxU32 *TerrainFaces = new NxU32[TERRAIN_NB_FACES*3];

	NxU32 k = 0;
	for(y=0; y<TERRAIN_SIZE-1; y++)
	{
		for(x=0; x<TERRAIN_SIZE-1; x++)
		{
			// Create first triangle
			TerrainFaces[k]		= x   + y*TERRAIN_SIZE;
			TerrainFaces[k+1]	= x   + (y+1)*TERRAIN_SIZE;
			TerrainFaces[k+2]	= x+1 + (y+1)*TERRAIN_SIZE;

			//while we're at it do some smoothing of the random terrain because its too rough to do a good demo of this effect.
			smoothTriangle(TerrainFaces[k], TerrainFaces[k+1], TerrainFaces[k+2], TerrainVerts);
			k+=3;
			// Create second triangle
			TerrainFaces[k]		= x   + y*TERRAIN_SIZE;
			TerrainFaces[k+1]	= x+1 + (y+1)*TERRAIN_SIZE;
			TerrainFaces[k+2]	= x+1 + y*TERRAIN_SIZE;

			smoothTriangle(TerrainFaces[k], TerrainFaces[k+1], TerrainFaces[k+2], TerrainVerts);
			k+=3;
		}
	}

	//allocate terrain materials -- one for each face.
	NxMaterialIndex *TerrainMaterials = new NxMaterialIndex[TERRAIN_NB_FACES];

	for(x=0; x<TERRAIN_NB_FACES; x++)
	{
		//new: generate material indices for all the faces
		chooseTrigMaterial(x, TerrainVerts, TerrainFaces, TerrainMaterials);
	}
	// Build vertex normals
	NxVec3 *TerrainNormals = new NxVec3[TERRAIN_NB_VERTS];
	NxBuildSmoothNormals(TERRAIN_NB_FACES, TERRAIN_NB_VERTS, TerrainVerts, TerrainFaces, NULL, TerrainNormals, true);

	// Build physical model
	NxTriangleMeshDesc* terrainDesc		= new NxTriangleMeshDesc;
	assert(terrainDesc);
	terrainDesc->numVertices			= TERRAIN_NB_VERTS;
	terrainDesc->numTriangles			= TERRAIN_NB_FACES;
	terrainDesc->pointStrideBytes		= sizeof(NxVec3);
	terrainDesc->triangleStrideBytes	= 3*sizeof(NxU32);
	terrainDesc->points					= TerrainVerts;
	terrainDesc->triangles				= TerrainFaces;							
	terrainDesc->flags					= 0;
	//add the mesh material data:
	terrainDesc->materialIndexStride	= sizeof(NxMaterialIndex);
	terrainDesc->materialIndices		= TerrainMaterials;

	NxInitCooking();
	MemoryWriteBuffer buf;
	bool status = NxCookTriangleMesh(*terrainDesc, buf);
	assert(status);
	NxTriangleMesh *pMesh = NULL;
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
 
	NxTriangleMeshShapeDesc terrainShapeDesc;
	terrainShapeDesc.meshData		= pMesh;
	terrainShapeDesc.userData		= terrainDesc;
	terrainShapeDesc.meshPagingMode = NX_MESH_PAGING_AUTO;
	terrainShapeDesc.shapeFlags		|= NX_SF_FEATURE_INDICES;
	assert(terrainShapeDesc.isValid());

 	NxActorDesc ActorDesc;
	ActorDesc.shapes.pushBack(&terrainShapeDesc);
	if (pMesh)
	{
		pMesh->saveToDesc(*terrainDesc);

		assert(ActorDesc.isValid());
		NxActor *pActor = gScene->createActor(ActorDesc);
		assert(pActor);

		return pActor;

	}
	
	return NULL;
}


void ComputeTerrain(bool* done, NxVec3* field, NxU32 x0, NxU32 y0, NxU32 size, NxF32 value)
{
	// Compute new size
	size >>= 1;
	if(!size) return;

	// Compute new heights
	NxF32 v0 = value * trand();
	NxF32 v1 = value * trand();
	NxF32 v2 = value * trand();
	NxF32 v3 = value * trand();
	NxF32 v4 = value * trand();

	NxU32 x1 = (x0+size)		% TERRAIN_SIZE;
	NxU32 x2 = (x0+size+size)	% TERRAIN_SIZE;
	NxU32 y1 = (y0+size)		% TERRAIN_SIZE;
	NxU32 y2 = (y0+size+size)	% TERRAIN_SIZE;

	if(!done[x1 + y0*TERRAIN_SIZE])	field[x1 + y0*TERRAIN_SIZE].y = v0 + 0.5f * (field[x0 + y0*TERRAIN_SIZE].y + field[x2 + y0*TERRAIN_SIZE].y);
	if(!done[x0 + y1*TERRAIN_SIZE])	field[x0 + y1*TERRAIN_SIZE].y = v1 + 0.5f * (field[x0 + y0*TERRAIN_SIZE].y + field[x0 + y2*TERRAIN_SIZE].y);
	if(!done[x2 + y1*TERRAIN_SIZE])	field[x2 + y1*TERRAIN_SIZE].y = v2 + 0.5f * (field[x2 + y0*TERRAIN_SIZE].y + field[x2 + y2*TERRAIN_SIZE].y);
	if(!done[x1 + y2*TERRAIN_SIZE])	field[x1 + y2*TERRAIN_SIZE].y = v3 + 0.5f * (field[x0 + y2*TERRAIN_SIZE].y + field[x2 + y2*TERRAIN_SIZE].y);
	if(!done[x1 + y1*TERRAIN_SIZE])	field[x1 + y1*TERRAIN_SIZE].y = v4 + 0.5f * (field[x0 + y1*TERRAIN_SIZE].y + field[x2 + y1*TERRAIN_SIZE].y);

	done[x1 + y0*TERRAIN_SIZE] = true;
	done[x0 + y1*TERRAIN_SIZE] = true;
	done[x2 + y1*TERRAIN_SIZE] = true;
	done[x1 + y2*TERRAIN_SIZE] = true;
	done[x1 + y1*TERRAIN_SIZE] = true;

	// Recurse through 4 corners
	value *= 0.5f;
	ComputeTerrain(done, field, x0,	y0,	size, value);
	ComputeTerrain(done, field, x0,	y1,	size, value);
	ComputeTerrain(done, field, x1,	y0,	size, value);
	ComputeTerrain(done, field, x1,	y1,	size, value);
}

void smoothTriangle(NxU32 a, NxU32 b, NxU32 c, NxVec3* TerrainVerts)
{
	NxVec3 & v0 = TerrainVerts[a];
	NxVec3 & v1 = TerrainVerts[b];
	NxVec3 & v2 = TerrainVerts[c];

	NxReal avg = (v0.y + v1.y + v2.y) * 0.333f;
	avg *= 0.5f;
	v0.y = v0.y * 0.5f + avg;
	v1.y = v1.y * 0.5f + avg;
	v2.y = v2.y * 0.5f + avg;
}

void chooseTrigMaterial(NxU32 faceIndex, NxVec3* TerrainVerts, NxU32* TerrainFaces, NxMaterialIndex* TerrainMaterials)
{
	NxVec3 & v0 = TerrainVerts[TerrainFaces[faceIndex * 3]];
	NxVec3 & v1 = TerrainVerts[TerrainFaces[faceIndex * 3 + 1]];
	NxVec3 & v2 = TerrainVerts[TerrainFaces[faceIndex * 3 + 2]];

	NxVec3 edge0 = v1 - v0;
	NxVec3 edge1 = v2 - v0;

	NxVec3 normal = edge0.cross(edge1);
	normal.normalize();
	NxReal steepness = 1.0f - normal.y;

	if	(steepness > 0.25f)
	{
		TerrainMaterials[faceIndex] = iceIndex;
	}
	else if (steepness > 0.2f)
	{
		TerrainMaterials[faceIndex] = rockIndex;
	}
	else if (steepness > 0.1f)
	{
		TerrainMaterials[faceIndex] = mudIndex;
	}
	else
		TerrainMaterials[faceIndex] = grassIndex;
}

void InitializeHUD()
{
	bHardwareScene = (gScene->getSimType() == NX_SIMULATION_HW);

	hud.Clear();

	// Add hardware/software to HUD
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
	gPhysicsSDK->setParameter(NX_BOUNCE_THRESHOLD, -100);
	gPhysicsSDK->setParameter(NX_DYN_FRICT_SCALING, 0.5);
    gPhysicsSDK->setParameter(NX_STA_FRICT_SCALING, 0.5);

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
	defaultMaterial->setRestitution(0.75);
	defaultMaterial->setStaticFriction(0.5);
	defaultMaterial->setDynamicFriction(0.1);
	defaultMaterial->setFlags(NX_MF_ANISOTROPIC);
	defaultMaterial->setDynamicFrictionV(1.0f);
	defaultMaterial->setStaticFrictionV(0.95f);
	defaultMaterial->setDirOfAnisotropy(NxVec3(0, 0, 1));

	//Create Compound Materials
	NxMaterialDesc materialDesc;
	materialDesc.restitution		= 1.0f;
	materialDesc.dynamicFriction	= 0.8f;
	materialDesc.staticFriction		= 0.5f;
	boxIndex						= gScene->createMaterial(materialDesc)->getMaterialIndex();

	materialDesc.restitution		= 0.1f;
	materialDesc.dynamicFriction	= 1.0f;
	materialDesc.staticFriction		= 1.0f;
	capsuleIndex					= gScene->createMaterial(materialDesc)->getMaterialIndex();
	
	materialDesc.restitution		= 0.5f;
	materialDesc.dynamicFriction	= 0.5f;
	materialDesc.staticFriction		= 0.8f;
	sphereIndex						= gScene->createMaterial(materialDesc)->getMaterialIndex();
	
	// Create Triangle Materials
	materialDesc.restitution		= 1.0f;
	materialDesc.staticFriction		= 0.0f;
	materialDesc.dynamicFriction	= 0.0f;
	iceIndex						= gScene->createMaterial(materialDesc)->getMaterialIndex();

	materialDesc.restitution		= 0.3f;
	materialDesc.staticFriction		= 1.2f;
	materialDesc.dynamicFriction	= 1.0f;
	rockIndex						= gScene->createMaterial(materialDesc)->getMaterialIndex();

	materialDesc.restitution		= 0.0f;
	materialDesc.staticFriction		= 3.0f;
	materialDesc.dynamicFriction	= 1.0f;
	mudIndex						= gScene->createMaterial(materialDesc)->getMaterialIndex();

	materialDesc.restitution		= 0.0f;
	materialDesc.staticFriction		= 0.0f;
	materialDesc.dynamicFriction	= 0.0f;
	grassIndex						= gScene->createMaterial(materialDesc)->getMaterialIndex();

	// Create the objects in the scene
	groundPlane = CreateGroundPlane();

	// Create Compound with different materials
	gSelectedActor = CreateCompoundActor();

	// Create Triangle Actor
	CreateTriangleActor();

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
