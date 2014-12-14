// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//			              LESSON 906: FLUIDS USERDATA
//
// ===============================================================================

#include "Lesson906.h"
#include "Timing.h"

// Physics SDK globals
NxPhysicsSDK*     gPhysicsSDK = NULL;
NxScene*          gScene = NULL;
NxVec3            gDefaultGravity(0,-9.8,0);
 
// User report globals
DebugRenderer     gDebugRenderer;
UserAllocator*	  gAllocator = NULL;

// HUD globals
HUD hud;

// Display globals
int gMainHandle;
int mx = 0;
int my = 0;

// Camera globals
float gCameraAspectRatio = 1;
//NxVec3 gCameraPos(0,2,-6);
NxVec3 gCameraPos(0,5,-12);
NxVec3 gCameraForward(0,0,1);
NxVec3 gCameraRight(-1,0,0);
const NxReal gCameraSpeed = 0.1;

// Force globals
NxVec3 gForceVec(0,0,0);
NxReal gForceStrength = 750;
bool bForceMode = true;

// Keyboard globals
#define MAX_KEYS 256
bool gKeys[MAX_KEYS];

// Simulation globals
NxReal gDeltaTime = 1.0/60.0;
NxReal gTotalTime = 0;
bool bHardwareScene = false;
bool bHardwareFluid = true;
bool bPause = false;
bool bShadows = true;
bool bDebugWireframeMode = false;

// Actor globals
NxActor* groundPlane = NULL;
NxActor* box = NULL;
NxActor* flatSurface = NULL;

NxActor* drain = NULL;

// Focus actor
NxActor* gSelectedActor = NULL;

// Fluid particle globals
NxVec3 gParticleBuffer[10000];
NxU32 gParticleBufferCap = 10000;
NxU32 gParticleBufferNum = 0;

// Mesh globals
NxTriangleMeshDesc flatSurfaceTriangleMeshDesc;

// Fluid particle macros
#define MAX_PARTICLES 32767

// Fluid globals
NxFluid*                fluid = NULL;

bool bEmitterWireframe = true;

/**
Stores elements which can be received from the SDK.
*/
struct ParticleSDK
{
	NxVec3	position;
	NxVec3  velocity;
	NxReal	density;
	NxReal  lifetime;
	NxU32	id;
	NxVec3	collisionNormal;
};

/**
Stores user specific data.
*/
struct MyParticle
{
	NxVec3 color;
};

// Fluid particle globals
static unsigned			gNewParticlesNum = 0;
static ParticleSDK*		gNewParticles = NULL;
static NxParticleData	gNewParticleData;
static unsigned			gNewParticlesMax = MAX_PARTICLES/5;

static NxU32*			gCreatedIdsBuffer;
static NxU32			gCreatedIdsBufferNum;

static NxU32*			gDeletedIdsBuffer;
static NxU32			gDeletedIdsBufferNum;

static MyParticle*				gParticlesColorBuffer;

static NxParticleIdData		gCreatedIds;
static NxParticleIdData		gDeletedIds;

static float			gDistance = 0.1f;
static unsigned			gSideNum = 14;
static float			gLifetime = 100.0f;


void PrintControls()
{
	printf("\n Flight Controls:\n ----------------\n w = forward, s = back\n a = strafe left, d = strafe right\n q = up, z = down\n");
    printf("\n Force Controls:\n ---------------\n i = +z, k = -z\n j = +x, l = -x\n u = +y, m = -y\n");
	printf("\n Miscellaneous:\n --------------\n p = Pause\n r = Select Next Actor\n f = Toggle Force Mode\n");
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
	// Process camera keys
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

void RenderActors(bool shadows)
{
    // Render all the actors in the scene
    NxU32 nbActors = gScene->getNbActors();
    NxActor** actors = gScene->getActors();
    while (nbActors--)
    {
        NxActor* actor = *actors++;

		ActorUserData* ud = (ActorUserData*)(actor->userData);

		if (ud && ud->flags & UD_IS_DRAIN)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDepthMask(GL_FALSE);
			glColor4f(0.1f, 0.8f, 1.0f, 0.8f);

 			DrawActor(actor, gSelectedActor, true);

			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		}
		else
		{
 			DrawActor(actor, gSelectedActor, true);

			// Handle shadows
            if (shadows)
            {
		        DrawActorShadow(actor, true);
            }
		}
    }
}

void RenderFluid()
{ 
    // Render fluid
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	for (NxU32 p=0; p<gNewParticlesNum; p++)
    {
        
		const ParticleSDK& particle = gNewParticles[p];
		const MyParticle& particlesColorBuffer = gParticlesColorBuffer[particle.id];
		glColor4f(particlesColorBuffer.color.x, particlesColorBuffer.color.y, particlesColorBuffer.color.z, 1.0f);
        glPushMatrix();
        glTranslatef(particle.position.x,particle.position.y,particle.position.z);
        glutSolidCube(0.1);  // glutSolidSphere(0.1,8,5);
        glPopMatrix();

    }
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void DrawForce(NxActor* actor, NxVec3& forceVec, const NxVec3& color)
{
	// Draw only if the force is large enough
	NxReal force = forceVec.magnitude();
	if (force < 0.1f)  return;

	forceVec = 3*forceVec/force;

	NxVec3 pos = actor->getCMassGlobalPosition();
	DrawArrow(pos, pos + forceVec, color);
}

NxVec3 ApplyForceToActor(NxActor* actor, const NxVec3& forceDir, const NxReal forceStrength, bool forceMode)
{
	NxVec3 forceVec = forceStrength*forceDir;

	if (forceMode)
		actor->addForce(forceVec);
	else 
		actor->addTorque(forceVec);

	return forceVec;
}

void ProcessForceKeys()
{
    if (!gSelectedActor)  return;

	// Process force keys
	for (int i = 0; i < MAX_KEYS; i++)
	{	
		if (!gKeys[i])  { continue; }

		switch (i)
		{
			// Force controls
			case 'i': {gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(0,0,1),gForceStrength,bForceMode); break; }
			case 'k': {gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(0,0,-1),gForceStrength,bForceMode); break; }
			case 'j': {gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(1,0,0),gForceStrength,bForceMode); break; }
			case 'l': {gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(-1,0,0),gForceStrength,bForceMode); break; }
			case 'u': {gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(0,1,0),gForceStrength,bForceMode); break; }
			case 'm': {gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(0,-1,0),gForceStrength,bForceMode); break; }
		}
	}
}

void ProcessInputs()
{
    ProcessForceKeys();

    // Show debug wireframes
	if (bDebugWireframeMode)
	{
		if (gScene)
		{
			gDebugRenderer.renderData(*gScene->getDebugRenderable());
		}
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

	RenderFluid();

	if (bForceMode)
		DrawForce(gSelectedActor, gForceVec, NxVec3(1,1,0));
	else
		DrawForce(gSelectedActor, gForceVec, NxVec3(0,1,1));
	gForceVec = NxVec3(0,0,0);

	if (!bPause)
	{
	    if (gDeltaTime < 10)  gTotalTime += gDeltaTime;
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
		case 'r': { SelectNextActor(); break; }
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
/*		case 'x': { bShadows = !bShadows; break; }
		case 'b': { bDebugWireframeMode = !bDebugWireframeMode; break; }	*/	
		case 'f': { bForceMode = !bForceMode; break; }
		case 'v': { bEmitterWireframe = !bEmitterWireframe; break; }
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

void InitGlut(int argc, char** argv, char* lessonTitle)
{
    glutInit(&argc, argv);
    glutInitWindowSize(800, 600);
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
void CreateParticleSphere(NxParticleData& pd, unsigned maxParticles, bool append, const NxVec3& pos, const NxVec3 vel, float lifetime, float distance, unsigned sideNum)
{
	float rad = sideNum*distance*0.5f;

	char* bufferPos = reinterpret_cast<char*>(pd.bufferPos);
	char* bufferVel = reinterpret_cast<char*>(pd.bufferVel);
	char* bufferLife = reinterpret_cast<char*>(pd.bufferLife);

	if(bufferPos == NULL && bufferVel == NULL && bufferLife == NULL)
		return;

	if(!append)
		(*pd.numParticlesPtr) = 0;
	else
	{
		bufferPos += pd.bufferPosByteStride * (*pd.numParticlesPtr);
		bufferVel += pd.bufferVelByteStride * (*pd.numParticlesPtr);
		bufferLife += pd.bufferLifeByteStride * (*pd.numParticlesPtr);
	}

	for(unsigned i=0; i<sideNum; i++)
		for(unsigned j=0; j<sideNum; j++)
			for(unsigned k=0; k<sideNum; k++)
			{
				if(*pd.numParticlesPtr >= maxParticles)
					break;

				NxVec3 p = NxVec3(i*distance,j*distance,k*distance);
				if(p.distance(NxVec3(rad,rad,rad)) < rad)
				{
					p += pos;

					if(bufferPos)
					{
						NxVec3& position = *reinterpret_cast<NxVec3*>(bufferPos);
						position = p;
						bufferPos += pd.bufferPosByteStride;
					}

					if(bufferVel)
					{
						NxVec3& velocity = *reinterpret_cast<NxVec3*>(bufferVel);
						velocity = vel;
						bufferVel += pd.bufferVelByteStride;
					}

					if(bufferLife)
					{
						NxReal& life = *reinterpret_cast<NxReal*>(bufferLife);
						life = lifetime;
						bufferLife += pd.bufferLifeByteStride;
					}

					(*pd.numParticlesPtr)++;
				}
			}
}
void CreateFluid()
{
	gNewParticles			= new ParticleSDK[gNewParticlesMax];
	gCreatedIdsBuffer		= new NxU32[gNewParticlesMax];
	gDeletedIdsBuffer		= new NxU32[gNewParticlesMax];
	gParticlesColorBuffer	= new MyParticle[gNewParticlesMax];

	//Setup structure which wraps the buffers for particle creation.
	gNewParticleData.numParticlesPtr		= &gNewParticlesNum;
	gNewParticleData.bufferPos				= &gNewParticles[0].position.x;
	gNewParticleData.bufferPosByteStride	= sizeof(ParticleSDK);
	gNewParticleData.bufferVel				= &gNewParticles[0].velocity.x;
	gNewParticleData.bufferVelByteStride	= sizeof(ParticleSDK);
	gNewParticleData.bufferLife				= &gNewParticles[0].lifetime;
	gNewParticleData.bufferLifeByteStride	= sizeof(ParticleSDK);
	gNewParticleData.bufferId				= &gNewParticles[0].id;
	gNewParticleData.bufferIdByteStride		= sizeof(ParticleSDK);

	gCreatedIds.numIdsPtr					= &gCreatedIdsBufferNum;
	gCreatedIds.bufferId					= &gCreatedIdsBuffer[0];
	gCreatedIds.bufferIdByteStride			= sizeof(NxU32);

	gDeletedIds.numIdsPtr					= &gDeletedIdsBufferNum;
	gDeletedIds.bufferId					= &gDeletedIdsBuffer[0];
	gDeletedIds.bufferIdByteStride			= sizeof(NxU32);

	//Setup fluid descriptor
	NxFluidDesc fluidDesc;
	fluidDesc.maxParticles                  = MAX_PARTICLES;
	fluidDesc.kernelRadiusMultiplier		= 2.0f;
	fluidDesc.restParticlesPerMeter			= 10.0f;
	fluidDesc.motionLimitMultiplier			= 3.0f;
	fluidDesc.packetSizeMultiplier			= 8;
	fluidDesc.collisionDistanceMultiplier   = 0.1;
	fluidDesc.stiffness						= 50.0f;
	fluidDesc.viscosity						= 40.0f;
	fluidDesc.restDensity					= 1000.0f;
	fluidDesc.damping						= 0.0f;
// There are some API changes since 280 version, Fluid collision coefficients have been renamed, 
// E.g. NxFluidDesc::dynamicCollisionAdhesion is named NxFluidDesc::dynamicFrictionForDynamicShapes. 
#if NX_SDK_VERSION_NUMBER < 280
	fluidDesc.staticCollisionRestitution = 0.162f;
	fluidDesc.staticCollisionAdhesion = 0.146f;
	fluidDesc.dynamicCollisionRestitution = 0.5f;
	fluidDesc.dynamicCollisionAdhesion = 0.5f;
#else 
	fluidDesc.restitutionForStaticShapes = 0.162f;
	fluidDesc.dynamicFrictionForStaticShapes  = 0.146f;
	fluidDesc.restitutionForDynamicShapes = 0.5f;
	fluidDesc.dynamicFrictionForDynamicShapes = 0.5f;
#endif
	fluidDesc.simulationMethod				= NX_F_SPH;

	//Switch to priority queue modus, in order to always be able to add a scoop of particles.
	fluidDesc.flags |= NX_FF_PRIORITY_MODE;
	fluidDesc.numReserveParticles = gNewParticlesMax;

	if (!bHardwareFluid)
		fluidDesc.flags &= ~NX_FF_HARDWARE;

	//Add the first particle scoop to the fluidDesc.
	NxVec3 fluidPos;
	NxVec3 fluidVel;

	fluidPos.set(0,4,0);
	fluidVel.set(0,0,0);

	// initialize the particles' data buffer
	CreateParticleSphere(gNewParticleData, gNewParticlesMax, false, fluidPos, fluidVel, gLifetime, gDistance, gSideNum);

	fluidDesc.initialParticleData = gNewParticleData;
	fluidDesc.particlesWriteData  = gNewParticleData;

	fluidDesc.particleCreationIdWriteData	= gCreatedIds;
	fluidDesc.particleDeletionIdWriteData	= gDeletedIds;

	NxFluid* fluid = gScene->createFluid(fluidDesc);
	if(!fluid)
	{
		fluidDesc.flags &= ~NX_FF_HARDWARE;
		bHardwareFluid = false;
		fluid = gScene->createFluid(fluidDesc);
	}

	//Set color for first scoop --> red
	//The NxScene::createFluid() function adds the NxFluidDesc::initialParticleData to the SDK. 
	//It also writes to the MyFluid::mCreatedParticleIds buffer, which was passed to the SDK
	//(NxFluidDesc::particleCreationIdWriteData). 
	for (unsigned i=0; i< gCreatedIdsBufferNum; i++)
	{
		unsigned id = gCreatedIdsBuffer[i];
		gParticlesColorBuffer[id].color =  NxVec3(0.8f, 0.1f,0.1f);
	}

	//Create another two scoops. This time with NxFluid::addParticles().
	//The NxFluid::addParticles() function adds new particles to the SDK. 
	//It also writes to the MyFluid::mCreatedParticleIds buffer, which was passed to the SDK
	//(NxFluidDesc::particleCreationIdWriteData). 
	fluidPos.set(-2,2,0);
	fluidVel.set(5,0,0);
	CreateParticleSphere(gNewParticleData, gNewParticlesMax, false, fluidPos, fluidVel, gLifetime, gDistance, gSideNum);
	fluid->addParticles(gNewParticleData);

	//Set scoop color --> blue.
	for (unsigned i=0; i<gCreatedIdsBufferNum; i++)
	{
		unsigned id = gCreatedIdsBuffer[i];
		gParticlesColorBuffer[id].color =  NxVec3(0.3f, 0.4f,1.0f);
	}

	fluidPos.set(2,2,0);
	fluidVel.set(-5,0,0);
	CreateParticleSphere(gNewParticleData, gNewParticlesMax, false, fluidPos, fluidVel, gLifetime, gDistance, gSideNum);
	fluid->addParticles(gNewParticleData);

	//Set scoop color --> green.
	for (unsigned i=0; i< gCreatedIdsBufferNum; i++)
	{
		unsigned id = gCreatedIdsBuffer[i];
		gParticlesColorBuffer[id].color =  NxVec3(0.3f, 1.0f,0.3f);
	}

}


void ReleaseFluid()
{
    if (fluid)
    {
        // Destroy the Fluid.
        if (gScene)
        {
            gScene->releaseFluid(*fluid);
        }
        fluid = 0;
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

	// Add hardware/software fluid to HUD
	if (bHardwareFluid)
		hud.AddDisplayString("Hardware Fluid", 0.74f, 0.88f);
	else
		hud.AddDisplayString("Software Fluid", 0.74f, 0.88f);	

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
    gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, gAllocator);
    if (!gPhysicsSDK)  return;

	// Set the physics parameters
	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.01);

	// Set the debug visualization parameters
	gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 1);
	//gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
	//gPhysicsSDK->setParameter(NX_VISUALIZE_ACTOR_AXES, 1);
	//gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_FNORMALS, 1);

    // Create the scene
    NxSceneDesc sceneDesc;
	sceneDesc.simType				= NX_SIMULATION_SW;
    sceneDesc.gravity               = gDefaultGravity;
    gScene = gPhysicsSDK->createScene(sceneDesc);	

	// Create the default material
	NxMaterial* defaultMaterial = gScene->getMaterialFromIndex(0); 
	defaultMaterial->setRestitution(0.5);
	defaultMaterial->setStaticFriction(0.5);
	defaultMaterial->setDynamicFriction(0.5);

	// Create the objects in the scene
	groundPlane = CreateGroundPlane();

	CreateFluid();

	// Initialize HUD
	InitializeHUD();

	// Get the current time
	getElapsedTime();

	// Start the first frame of the simulation
	if (gScene)  StartPhysics();
}

void ReleaseNx()
{
	GetPhysicsResults();  // make sure to fetchResults() before shutting down
	ReleaseFluid();
	if (gScene) gPhysicsSDK->releaseScene(*gScene);
	if (gPhysicsSDK)  gPhysicsSDK->release();
	NX_DELETE_SINGLE(gAllocator);
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
	InitGlut(argc, argv, "Lesson 906: Fluid UserData");
    InitNx();
    glutMainLoop();
	ReleaseNx();
	return 0;
}
