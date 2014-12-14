//
// This demo creates a vortex, which can be moved around in a scene. There is an inclusion
// shape that is kept around the center of the vortex, and an exclusion shape that is held
// in place around a shed. The exclusion shape can be turned on and off (actually switched
// between exclusion and inclusion) by pressing 'x'
//
#include "MyForceField.h"

//This method creates the vortex force field and include/exclude shapes.
//
//This is the most interesting function in this demo, below are other methods for controlling
//the movement of the vortex and turning on/off the exclude shape
//
void SampleVortex::createVortexForceField(const NxVec3& pos, NxActor* actor, NxScene* scene)
{
	assert(scene);

	NxForceFieldDesc ffDesc;
	NxForceFieldLinearKernelDesc	lKernelDesc;
	NxForceFieldLinearKernel*		linearKernel;

	ffDesc.coordinates = NX_FFC_CYLINDRICAL;
	//Attach the vortex in an actor (which we use for moving the field around in the world)
	ffDesc.actor = actor;
	//attach the force field at the center of the actor
	m_forceFieldTransform.id(); 
	ffDesc.pose = m_forceFieldTransform;

	//constant force of 30 towards the center (which is then counter-acted by radial forces specified below)
	//constant force of 4 upwards (creating a constant lift on the objects)
	lKernelDesc.constant = NxVec3(-30, 4.0f, 0); 

	//The target where we want the objects to end up is at radius 3 from the center. We use
	//Y=0 as the target in along the y-axis together with the m(0,1)=-5 to create a force
	//directed outwards from the center of the vortex when objects are floating towards the
	//top of the vortex.
	lKernelDesc.positionTarget = NxVec3(3,0,0);

	//Setup radial forces, depending on where the objects are positioned
	NxMat33 m;
	m.zero();
	m(0,0) = 10; //radial error -> radial force. If outside of target radius, act with a force of 10*distance inwards
	m(0,1) = -5; //axial error -> radial force. If the y component of the object position is above the target y position (0), 
				 //then act with a force of 5*distance outwards. This reduces the force of 30 inwards that we setup earlier,
				 //making the vortex broaden out in the top
	m(0,2) = 0;  //there is no tangential error in cylindrical coordinates, so we just set this to 0
	lKernelDesc.positionMultiplier = m;
	lKernelDesc.noise = NxVec3(5,5,5); //adds a random noise on the forces to make the objects a little more chaotic

	//Set target velocity along the tangent of the vortex to 30 (the other directions to 0)
	lKernelDesc.velocityTarget = NxVec3(0,0,30);
	m.diagonal(NxVec3(1,1,1)); //Acts with a force relative to the current velocity to reach the
							   //target velocities. If the velocity is above 0 in radial direction, then
							   //the radial velocity is decreased. If the velocity is below 30 in tangential
							   //direction, then the velocity is increased until it reaches that velocity.
	lKernelDesc.velocityMultiplier = m;

	//You can try some fall-off forces if you e.g. want the vortex to lose power 
	//along the radial direction when the distance from its center increases:
	lKernelDesc.falloffLinear = NxVec3(5.0f, 0, 0);
	lKernelDesc.falloffQuadratic = NxVec3(5.0f, 0, 0);
	linearKernel = scene->createForceFieldLinearKernel(lKernelDesc);
	ffDesc.kernel = linearKernel;
	ffDesc.flags = 0;

	m_forceField = scene->createForceField(ffDesc);
	assert(m_forceField);

	//Attach an include shape, we position this so that it covers the vortex specified above
	NxForceFieldShape* s = NULL;
	NxBoxForceFieldShapeDesc b;
	b.dimensions = NxVec3(5, 7, 5);
	b.pose.t	 = NxVec3(0, 3.5f, 0);
	s = m_forceField->getIncludeShapeGroup().createShape(b);

	//Create an exclude shape, positioned around the shed
	NxForceFieldShapeGroupDesc sgDesc;
	sgDesc.flags	= NX_FFSG_EXCLUDE_GROUP;
	m_excludeGroup	= scene->createForceFieldShapeGroup(sgDesc);
	
	NxBoxForceFieldShapeDesc exclude;
	exclude.dimensions	= NxVec3(2.25f, 1.5f, 1.75f);
	exclude.pose.t		= NxVec3(8.85f, 1.5f, -10.3f);
	m_excludeShape		= m_excludeGroup->createShape(exclude);

	m_forceField->addShapeGroup(*m_excludeGroup);
}

void SampleVortex::createFluid()
{
	static NxVec3 ParticleBuffer[10000];
	
	// Set structure to pass particles, and receive them after every simulation step
	NxParticleData particles;
	//particles.maxParticles			= gParticleBufferCap;
	particles.numParticlesPtr		= &m_ParticleBufferSize;
	particles.bufferPos				= &ParticleBuffer[0].x;
	particles.bufferPosByteStride	= sizeof(NxVec3);

	// Create a fluid descriptor
	NxFluidDesc fluidDesc;
    fluidDesc.maxParticles                  = 5000;
    fluidDesc.kernelRadiusMultiplier		= 1.2;
    fluidDesc.restParticlesPerMeter			= 10;
	fluidDesc.motionLimitMultiplier			= 3.6;
	fluidDesc.packetSizeMultiplier			= 16;
	fluidDesc.collisionDistanceMultiplier	= 0.12;
    fluidDesc.stiffness						= 20;
    fluidDesc.viscosity						= 6;
	fluidDesc.restDensity					= 50;
    fluidDesc.damping						= 0;
	fluidDesc.restitutionForStaticShapes	= 0.4;
	fluidDesc.dynamicFrictionForStaticShapes= 0.03;
	fluidDesc.simulationMethod				= NX_F_SPH; //NX_F_NO_PARTICLE_INTERACTION;
	fluidDesc.flags						   &= ~NX_FF_HARDWARE;

	fluidDesc.initialParticleData			= particles;
	fluidDesc.particlesWriteData			= particles;

	NxFluid* fluid = m_scene->createFluid(fluidDesc);
	assert(fluid != NULL);

	//Setup structure which wraps the buffers for particle creation.
	ParticleSDK* newParticles = new ParticleSDK[5000];

	NxParticleData newParticleData;
	newParticleData.numParticlesPtr			= new NxU32;
	newParticleData.bufferPos				= &newParticles[0].position.x;
	newParticleData.bufferPosByteStride		= sizeof(ParticleSDK);
	newParticleData.bufferVel				= &newParticles[0].velocity.x;
	newParticleData.bufferVelByteStride		= sizeof(ParticleSDK);
	newParticleData.bufferLife				= &newParticles[0].lifetime;
	newParticleData.bufferLifeByteStride	= sizeof(ParticleSDK);

	fluid->setParticlesWriteData(newParticleData);


	// create Emitter
	NxFluidEmitterDesc emitterDesc;
	emitterDesc.dimensionX	= 0.05;
	emitterDesc.dimensionY	= 0.05;
	emitterDesc.shape		= NX_FE_ELLIPSE;
	emitterDesc.maxParticles = 5000;
	emitterDesc.type		= NX_FE_CONSTANT_FLOW_RATE;
	emitterDesc.randomPos	= NxVec3(0.05f, 0.05f, 0.05f);
	emitterDesc.randomAngle = 0.481710941f;
	emitterDesc.fluidVelocityMagnitude = 5;
	emitterDesc.rate		= 50;
	emitterDesc.particleLifetime = 7;
	emitterDesc.repulsionCoefficient = 1;
	emitterDesc.flags		= NX_FEF_VISUALIZATION | NX_FEF_ADD_BODY_VELOCITY | NX_FEF_ENABLED;

	assert(emitterDesc.isValid());

	NxFluidEmitter* emitter = fluid->createEmitter(emitterDesc);
	assert(emitter);
}

//Startup for the vortex sample, loads the backyard scene and creates a vortex
void SampleVortex::setup(NxScene *scene)
{
	assert(scene);
	m_scene = scene;
	
	createFluid();
	
	//Create a kinematic 
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	bodyDesc.flags			  |= NX_BF_KINEMATIC;
	bodyDesc.massSpaceInertia = NxVec3(1, 1, 1);
	bodyDesc.mass			  = 1.0f;
	actorDesc.body			  = &bodyDesc;
	m_kinematicActor		  = scene->createActor(actorDesc);
	m_forceFieldActor		  = m_kinematicActor;

	//Create the force field for this sample, see code above
	createVortexForceField(NxVec3(0, 0, 0), m_kinematicActor, scene);
}

void SampleVortex::onKeyPress(unsigned char key, int x, int y)
{
	switch(key)
	{
		//Switch the exclude shape between excluding and including (delayed until the update method)
		case 'x':
		{
			m_switchExcludeShape = true;
			break;
		}
	}
}

void SampleVortex::onVirtualKeyPress(int key, int x, int y)
{
	switch(key)
	{
		case GLUT_KEY_LEFT:
			m_forceFieldTransform.t -= NxVec3(0.05f,0,0); 
			m_forceFieldMoved = true;
			break;
		case GLUT_KEY_RIGHT:
			m_forceFieldTransform.t += NxVec3(0.05f,0,0); 
			m_forceFieldMoved = true;
			break;
		case GLUT_KEY_UP:
			m_forceFieldTransform.t -= NxVec3(0,0,0.05f); 
			m_forceFieldMoved = true;
			break;
		case GLUT_KEY_DOWN:
			m_forceFieldTransform.t += NxVec3(0,0,0.05f); 
			m_forceFieldMoved = true;
			break;
	}
}

//This method is called before the scene is simulated, we use it for moving the force field
//and for switching the "exclude shape" between exclude and include
void SampleVortex::update() 
{
	moveForceField();
	if (m_switchExcludeShape && m_excludeGroup != NULL)
	{
		if(m_excludeGroupEnabled)
			m_forceField->removeShapeGroup(*m_excludeGroup);
		else
			m_forceField->addShapeGroup(*m_excludeGroup);
		m_switchExcludeShape  = false;
		m_excludeGroupEnabled = !m_excludeGroupEnabled;
	}
}

//Moves the force field around (by animating a kinematic actor)
void SampleVortex::moveForceField()
{
	if (!m_forceFieldMoved)			return;
	if (m_kinematicActor == NULL)	return;

	m_kinematicActor->moveGlobalPose(m_forceFieldTransform);

	m_forceFieldMoved = false;
}

//Shutdown the demo, remove everything that is not removed by the main application
void SampleVortex::cleanup()
{
	if (m_scene != NULL)
	{
		if (m_forceField != NULL)
		{
			m_scene->releaseForceField(*m_forceField);
			m_forceField = NULL;
		}
		if (m_kinematicActor != NULL)
		{
			m_scene->releaseActor(*m_kinematicActor);
			m_kinematicActor  = NULL;
			m_forceFieldActor = NULL;
		}
	}

	//Also init the state variables for the next setup() call
	init();
}

void SampleVortex::init()
{
	m_forceFieldMoved		= false;
	m_switchExcludeShape	= false;
	m_excludeGroupEnabled	= true;
	m_forceFieldTransform.id();
	m_kinematicActor		= NULL;
	m_excludeShape			= NULL;
	m_forceField			= NULL;
	m_forceFieldActor		= NULL;
	m_ParticleBufferSize	= 0;
}

SampleVortex::SampleVortex()
{
	init();
}
