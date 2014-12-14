// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//			              LESSON 905: FLUIDS EVENTS
//
// ===============================================================================

#include "Lesson905.h"
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

// Mesh globals
NxTriangleMeshDesc flatSurfaceTriangleMeshDesc;

// Fluid number
#define FLUID_NUMBER   5
// Fluid particle globals
NxFluid*		  gFluid[FLUID_NUMBER];
NxFluidEmitter*	  gEmitter[FLUID_NUMBER];
NxVec3*			  gParticleBuffer[FLUID_NUMBER];
unsigned		  gParticleBufferCap[FLUID_NUMBER];
unsigned		  gParticleBufferNum[FLUID_NUMBER];
NxParticleData    gParticles[FLUID_NUMBER];
NxFluidDesc       gFluidDesc[FLUID_NUMBER];
static int gEmitterArray[FLUID_NUMBER];
static int gFluidArray[FLUID_NUMBER];

// Fluid particle macros
#define MAX_PARTICLES 32767

// Fluid particle globals
NxU32            gNumParticles   = 0;
//NxVec3*          gParticles      = 0;

bool bEmitterWireframe = true;

class MyFluidUserNotify : public NxFluidUserNotify
{
public:
	virtual bool onEmitterEvent(NxFluidEmitter& emitter, NxFluidEmitterEventType eventType);
	virtual bool onEvent(NxFluid& fluid, NxFluidEventType eventType);

	virtual ~MyFluidUserNotify() {}  
};

bool MyFluidUserNotify::onEvent(NxFluid& fluid, NxFluidEventType eventType)
{
	int a = (int)&fluid;
	if(eventType ==NX_FET_NO_PARTICLES_LEFT)
	{
		for(int i =0;i< FLUID_NUMBER;i++)
		{
			if(a == gFluidArray[i])
				printf("Fluid %d is empty now\n",i);
		}
	}

	return false;
}

bool MyFluidUserNotify::onEmitterEvent(NxFluidEmitter& emitter, NxFluidEmitterEventType eventType)
{
	int a = (int)&emitter;
	if(eventType ==NX_FEET_EMITTER_EMPTY)
	{
		for(int i =0;i< FLUID_NUMBER;i++)
		{
			if(a == gEmitterArray[i])
				printf("Emitter %d is empty now\n",i);
		}
	}

	return false;
}

static MyFluidUserNotify* gUserNotify;

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
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	for(int i = 0 ; i < FLUID_NUMBER; i++)
	{
		for (NxU32 p=0; p<gParticleBufferNum[i]; p++)
		{
			NxVec3& particle = gParticleBuffer[i][p];
			glPushMatrix();
			glTranslatef(particle.x,particle.y,particle.z);
			glutSolidCube(0.1);  // glutSolidSphere(0.1,8,5);
			glPopMatrix();
		}
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

	//if (bEmitterWireframe)
	//{
	//	NxMat34 fluidEmitterMat = fluidEmitter->getGlobalPose();
	//  	 	NxVec3 fluidEmitterPos, fluidEmitterDir;
	//	fluidEmitterPos = fluidEmitterMat.t;
	//	fluidEmitterMat.M.getColumn(2,fluidEmitterDir);

	//	DrawArrow(fluidEmitterPos, fluidEmitterPos + fluidEmitterDir, NxVec3(0,0,1)); 
	//       DrawEllipse(20, fluidEmitter->getGlobalPose(), NxVec3(0.5,0,0.5), fluidEmitter->getDimensionX(), fluidEmitter->getDimensionY());
	//}

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

void CreateFluid(NxVec3 pos)
{
	// Create fluid mEmitter 
	NxFluidEmitterDesc feDesc;
	feDesc.relPose.t = NxVec3(0, 0, 0);
	NxMat33 m1, m2;
	m1.rotY(0.5f*NxPi);
	feDesc.relPose.M = m1;
	feDesc.frameShape = NULL;
	feDesc.type = NX_FE_CONSTANT_PRESSURE;
	feDesc.rate = 100;
	feDesc.maxParticles = 300;
	feDesc.shape = NX_FE_RECTANGULAR;
	feDesc.dimensionX = 0.25;
	feDesc.dimensionY = 0.25;
	feDesc.randomPos = NxVec3(0.02,0.02,0.01);
	feDesc.randomAngle = 0;
	feDesc.particleLifetime = 10;
	feDesc.fluidVelocityMagnitude = 3;

	for(int i = 0; i < FLUID_NUMBER; i++)
	{    
		gParticleBufferCap[i] = MAX_PARTICLES;
		gParticleBuffer[i] = new NxVec3[MAX_PARTICLES];	
		gParticleBufferNum[i] = 0;

		gParticles[i].numParticlesPtr		= &gParticleBufferNum[i];
		gParticles[i].bufferPos				= &gParticleBuffer[i][0].x;
		gParticles[i].bufferPosByteStride	= sizeof(NxVec3);

		gFluidDesc[i].flags |= NX_FF_VISUALIZATION;
		gFluidDesc[i].maxParticles = 300 + 50*i;
		gFluidDesc[i].simulationMethod = NX_F_SPH;
		gFluidDesc[i].restParticlesPerMeter = 6;
		gFluidDesc[i].viscosity = 80;
		gFluidDesc[i].stiffness = 15;
		gFluidDesc[i].kernelRadiusMultiplier = 1.6;
		gFluidDesc[i].packetSizeMultiplier			= 16;
		gFluidDesc[i].motionLimitMultiplier          = 0.2f;
		// There are some API changes since 280 version, Fluid collision coefficients have been renamed, 
		// E.g. NxFluidDesc::dynamicCollisionAdhesion is named NxFluidDesc::dynamicFrictionForDynamicShapes. 
#if NX_SDK_VERSION_NUMBER < 280
		gFluidDesc[i].staticCollisionRestitution = 0.162f;
		gFluidDesc[i].staticCollisionAdhesion = 0.146f;
		gFluidDesc[i].dynamicCollisionRestitution = 0.5f;
		gFluidDesc[i].dynamicCollisionAdhesion = 0.5f;
#else 
		gFluidDesc[i].restitutionForStaticShapes = 0.162f;
		gFluidDesc[i].dynamicFrictionForStaticShapes  = 0.146f;
		gFluidDesc[i].restitutionForDynamicShapes = 0.5f;
		gFluidDesc[i].dynamicFrictionForDynamicShapes = 0.5f;
#endif
		gFluidDesc[i].initialParticleData			= gParticles[i];
		gFluidDesc[i].particlesWriteData			= gParticles[i];

		if(!gFluidDesc[i].isValid())	
			return;
		if(!bHardwareFluid)
			gFluidDesc[i].flags &= ~NX_FF_HARDWARE;

		//set max particles number for each emitter
		feDesc.maxParticles		= 300 + 50*i;

		gFluid[i] = gScene->createFluid(gFluidDesc[i]);
		if(!gFluid[i])
		{
			gFluidDesc[i].flags &= ~NX_FF_HARDWARE;
			bHardwareFluid = false;
			gFluid[i] = gScene->createFluid(gFluidDesc[i]);
		}

		gEmitter[i] = gFluid[i]->createEmitter(feDesc);
		pos.x = pos.x + 2.5;	
		gEmitter[i]->setGlobalPosition(pos);
	}
	for(int i =0;i< FLUID_NUMBER;i++)
	{
		gEmitterArray[i] = (int)((gEmitter[i]));
		gFluidArray[i]   = (int)((gFluid[i]));
	}
}

void ReleaseFluid()
{
	for (int i = 0; i< FLUID_NUMBER; i++)
	{
		gScene->releaseFluid(*gFluid[i]);
		delete[] gParticleBuffer[i];
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

	gUserNotify = new MyFluidUserNotify();
	gScene->setFluidUserNotify(gUserNotify);

	// Create the default material
	NxMaterial* defaultMaterial = gScene->getMaterialFromIndex(0); 
	defaultMaterial->setRestitution(0.5);
	defaultMaterial->setStaticFriction(0.5);
	defaultMaterial->setDynamicFriction(0.5);

	// Create the objects in the scene
	groundPlane = CreateGroundPlane();

	// Create multi fluids
	CreateFluid(NxVec3(-7,2,0));

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
	InitGlut(argc, argv, "Lesson 905: Fluid Events");
	InitNx();
	glutMainLoop();
	ReleaseNx();
	return 0;
}
