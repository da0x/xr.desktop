// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//						   LESSON 116: RIGIDBODY FORCEFIELD
//
//						    Written by QA BJ, 6-2-2008
// ===============================================================================

#include "Lesson116.h"
#include "Timing.h"

// Physics SDK globals
NxPhysicsSDK*     gPhysicsSDK	= NULL;
NxScene*          gScene		= NULL;
NxVec3            gDefaultGravity(0,-9.8,0);
bool			  gSceneRunning = false;

// User report globals
DebugRenderer     gDebugRenderer;
int				  gDebugRenderState = 0;

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
unsigned gSampleIndex		= 0;
NxArray<MyForceField*>	gSamples;

// Actor globals
NxActor* groundPlane	= NULL;

// Focus actor
NxActor* gSelectedActor = NULL;

void PrintControls()
{
	printf("\n Flight Controls:\n ----------------\n w = forward, s = back\n a = strafe left, d = strafe right\n q = up, z = down\n");
    printf("\n Force Controls:\n ---------------\n i = +z, k = -z\n j = +x, l = -x\n u = +y, m = -y\n");
	printf("\n Miscellaneous:\n --------------\n p   = Pause\n r   = Select Next Actor\n f   = Toggle Force Mode\n b   = Toggle Debug Wireframe Mode\n x   = Toggle Shadows\n t   = Move Focus Actor to (0,5,0)\n F10 = Reset Scene\n <space> = Shot balls\n");
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
	gCameraAspectRatio = ((float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT));
	
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
	if (!actor)		return;
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
	if (!gSelectedActor)	return;
	
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
        ProcessInputs();
		StartPhysics();
	}

    // Display scene
 	RenderActors(bShadows);

	RenderFluid();

//	RenderForceField();

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

void RenderFluid()
{
	NxFluid** fluids = gScene->getFluids();
	int nbFluids = gScene->getNbFluids();
	while (nbFluids--)
	{
		NxFluid* fluid = *fluids++;
		DrawFluid(fluid);
	}
}

void DrawFluid(NxFluid* fluid)
{
#ifndef NX_DISABLE_FLUIDS
	NxParticleData particles = fluid->getParticlesWriteData();
	if (!particles.numParticlesPtr)
		return;

	NxU32 numParticles = *particles.numParticlesPtr;
	if (numParticles == 0)
		return;

#ifndef __CELLOS_LV2__
	glPushAttrib(GL_ALL_ATTRIB_BITS);
#endif

	glPointSize(5.0f);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, particles.bufferPosByteStride, particles.bufferPos);

	glDrawArrays(GL_POINTS, 0, numParticles);

	glDisableClientState(GL_VERTEX_ARRAY);

#ifndef __CELLOS_LV2__
	glPopAttrib();
#endif
#endif // NX_DISABLE_FLUIDS
}

void RenderForceField()
{
	if (gDebugRenderState >= 2)
		SampleAndVisualizeForceField();
	
	NxU32 nbForceFields = gScene->getNbForceFields();
	NxForceField *forceField		= NULL;
	NxU32 nbGroups					= 0;
	NxForceFieldShapeGroup*	group	= NULL;
	while (nbForceFields--)
	{
		forceField = gScene->getForceFields()[nbForceFields];
		assert(forceField);
		forceField->resetShapeGroupsIterator();
		nbGroups = forceField->getNbShapeGroups();
		while (nbGroups--)
		{
			group = forceField->getNextShapeGroup();
			assert(group);
			RenderForceFieldGroup(group);
		}
		RenderForceFieldGroup(&forceField->getIncludeShapeGroup());
	}	
}

void RenderForceFieldGroup(NxForceFieldShapeGroup* group)
{
		const NxReal excludeCol[4] = {1.0f, 0.0f, 0.0f, 0.3f};
		const NxReal includeCol[4] = {0.0f, 1.0f, 0.0f, 0.3f};
		const NxReal* col = (group->getFlags() & NX_FFSG_EXCLUDE_GROUP) ? excludeCol : includeCol;
	
		group->resetShapesIterator();
		NxU32 nbShapes = group->getNbShapes();

		NxMat34 ffPose;
		ffPose.id();
		NxForceField* ff = group->getForceField();
		if(ff)
		{
			ffPose = ff->getPose();
			NxActor* actor =  ff->getActor();
			if(actor)
				ffPose = actor->getGlobalPose() * ffPose;
		}
		while (nbShapes--)
		{
			NxForceFieldShape* shape = group->getNextShape();
			NxMat34 pose = ffPose * shape->getPose();
			
			glPushMatrix();
			SetupGLMatrix(pose.t, pose.M);
			glColor4fv(col);

			switch(shape->getType())
			{
			case NX_SHAPE_BOX:
				{
				NxBoxForceFieldShape* boxShape = shape->isBox();
				NxVec3 boxDim = boxShape->getDimensions();
				glScalef(boxDim.x, boxDim.y, boxDim.z);
				glutSolidCube(2);
				break;
				}
			case NX_SHAPE_SPHERE:
				{
				NxSphereForceFieldShape* sphereShape = shape->isSphere();
				NxReal radius = sphereShape->getRadius();
				glScalef(radius, radius, radius);
				glutSolidSphere(1.0f, 12, 12);
				break;
				}
			}
			glPopMatrix();
		}
}

void SampleAndVisualizeForceField()
{
	NxU32 nbForceFields = gScene->getNbForceFields();
	NxForceField *field = NULL;
	NxU32 nbGroups		= 0;
	NxForceFieldShapeGroup* group = NULL;
	while (nbForceFields--)
	{
		field = gScene->getForceFields()[nbForceFields];
		assert(field);
#ifndef __CELLOS_LV2__
		glPushAttrib(GL_ALL_ATTRIB_BITS);
#endif
		glDisable(GL_LIGHTING);
		glLineWidth(1.0f);
		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
		glEnableClientState(GL_VERTEX_ARRAY);
		glLoadIdentity();

		// visualize all shape groups
		field->resetShapeGroupsIterator();
		nbGroups = field->getNbShapeGroups();
		while (nbGroups--)
		{
			group = field->getNextShapeGroup();
			SampleAndVisualizeForceFieldShapeGroup(field, group);
		}
		SampleAndVisualizeForceFieldShapeGroup(field, &field->getIncludeShapeGroup());

		glDisableClientState(GL_VERTEX_ARRAY);

#ifndef __CELLOS_LV2__
		glPopAttrib();
#else
		glEnable(GL_LIGHTING);
		glColor4f(1.0f,1.0f,1.0f,1.0f);
#endif
	}
}

void SampleAndVisualizeForceFieldShapeGroup(NxForceField* field, NxForceFieldShapeGroup* group)
{
	group->resetShapesIterator();
	NxU32 nbShapes = group->getNbShapes();
	while (nbShapes--)
	{
		NxForceFieldShape* shape = group->getNextShape();
		assert(shape);
		NxBoxForceFieldShape* boxShape = shape->isBox();
		assert(boxShape);
		if (!boxShape) continue;

		NxVec3 dim = boxShape->getDimensions();
	

		NxMat34 pose = boxShape->getPose();
		if (boxShape->getForceField() != NULL)
		{
  			pose = field->getPose() * pose;
			if (gSamples[gSampleIndex]->getForceFieldActor() != NULL)
  				pose = gSamples[gSampleIndex]->getForceFieldActor()->getGlobalPose() * pose;
		}
			
		NxVec3 start = pose.t - (pose.M*dim);
		int resolution = 10;
		//NxVec3 step = dim * (2.0f / (float) resolution);
		NxVec3 stepX = pose.M.getColumn(0) * dim.x * (2.0f / (float) resolution);
		NxVec3 stepY = pose.M.getColumn(1) * dim.y * (2.0f / (float) resolution);
		NxVec3 stepZ = pose.M.getColumn(2) * dim.z * (2.0f / (float) resolution);
		NxVec3 samplePoint;
		NxVec3 sampleForce;
		NxVec3 sampleTorque;
		for (int i = 0; i < resolution; i++)
		{
			for (int j = 0; j < resolution; j++)
			{
				for (int k = 0; k < resolution; k++)
				{
					samplePoint = start + stepX*i + stepY*j + stepZ*k;

					field->samplePoints(1, &samplePoint, NULL, &sampleForce, &sampleTorque);
					MyDrawArrow(samplePoint, samplePoint + sampleForce * 0.005f);
				}
			}
		}
	}
}

void MyDrawArrow(const NxVec3& posA, const NxVec3& posB)
{
	NxVec3 v3ArrowShape[] = {
		NxVec3(posA), NxVec3(posB)
	};
	glVertexPointer(3, GL_FLOAT, sizeof(NxVec3), &v3ArrowShape[0].x);
	glDrawArrays(GL_LINES, 0, sizeof(v3ArrowShape)/sizeof(NxVec3));
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
	case ' ':	//SPACE
		CreateGrid(NxVec3(0, 5, 0), 1, 0, 1); 
		CreateGrid(NxVec3(0, 2, 0), 1, 0, 1);
		CreateGrid(NxVec3(0, 1, 1), 1, 0, 1);
		CreateGrid(NxVec3(0, 3, 0), 1, 0, 1);
		CreateGrid(NxVec3(0, 4, 1), 1, 0, 1);
		break;
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
    gMainHandle = glutCreateWindow("Lesson 116: Rigidbody and ForceField");
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

NxActor* CreateBall(const NxVec3& pos, NxReal size, bool trigger)
{
	NxSphereShapeDesc	sphereDesc;
	NxBodyDesc			bodyDesc;
	NxActorDesc			actorDesc;

	sphereDesc.radius			= 0.5f;
	sphereDesc.localPose.t.x	= 1.0f;
	actorDesc.shapes.pushBack(&sphereDesc);

	NxMat34 mat(NxMat33(NX_IDENTITY_MATRIX), pos);
	actorDesc.globalPose.t	= mat.t;
	actorDesc.globalPose.M	= mat.M;
	actorDesc.body			= &bodyDesc;
	actorDesc.density		= 2.0f;

	NxActor *pActor			= gScene->createActor(actorDesc);
	assert(pActor);

	return pActor;	
}

void CreateGrid(NxVec3 pos, int size, int shapeType, int shapeSize)
{
	NxReal spacing = 1.1f * shapeSize;
	int z = 0;
	int x = 0;
	NxReal delta = 0.0f;
	NxActor *newActor = NULL;
	NxVec3 newPos;
	for (z=0; z<size; z++)
	{
		for (x=0; x<size; x++)
		{
			delta  = shapeSize + spacing;
			newPos = pos + NxVec3(x * delta, 0, z * delta);
			newActor =  CreateBall(pos, shapeSize);
			gSelectedActor = newActor;
		}
	}
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
	gPhysicsSDK->setParameter(NX_DEFAULT_SLEEP_LIN_VEL_SQUARED, 0.15*0.15);
	gPhysicsSDK->setParameter(NX_DEFAULT_SLEEP_ANG_VEL_SQUARED, 0.14*0.14);

	// Set the debug visualization parameters
	gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_ACTOR_AXES, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_FORCE_FIELDS, 1.0f);

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

	// Create ForceField
	gSamples[gSampleIndex]->setup(gScene);

	CreateGrid(NxVec3(0, 1, 0), 1, 0, 1);

	// Initialize HUD
	InitializeHUD();

	// Get the current time
	getElapsedTime();

	// Start the first frame of the simulation
	if (gScene)  StartPhysics();
}

void ReleaseNx()
{
	GetPhysicsResults();

	gSamples[gSampleIndex]->cleanup();

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
	if (gSceneRunning)
		return;

	// Update the time step
	gDeltaTime = getElapsedTime();

	gSamples[gSampleIndex]->update();
	// Start collision and dynamics for delta time since the last frame
    gScene->simulate(gDeltaTime);
	gSceneRunning = true;
	GetPhysicsResults();
}

void GetPhysicsResults()
{
	if (gSceneRunning)
	{
		NxU32 error;
		gScene->flushStream();
		gScene->fetchResults(NX_ALL_FINISHED, true, &error);
		assert(error == 0);
		gSceneRunning = false;
	}
}

int main(int argc, char** argv)
{
	//Make a list of the demos in this sample
	gSamples.pushBack(new SampleVortex());
	gSamples.pushBack(new SampleWind());
	gSamples.pushBack(new SampleExplosion());
	
	PrintControls();
    InitGlut(argc, argv);
    InitNx();
    glutMainLoop();
	ReleaseNx();
	return 0;
}

