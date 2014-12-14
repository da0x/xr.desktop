//C++

/*
----------------------------------------------------
The Desktop Project
------------------

Copyright 2004 Daher Alfawares

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

----------------------------------------------------
*/

#include "NxPhysics.h"
#include "DSys.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment(lib, "PhysXLoader.lib")



namespace DSys { namespace Physics
	{

	NxPhysicsSDK*	gPhysicsSDK = NULL;
	NxScene*		gScene = NULL;
	NxVec3			gDefaultGravity( NxReal(0),NxReal(-9.8),NxReal(0) );

	// debugging
	Var_bool		p_debug						("p_debug", 			"0");
	Var_int			p_angularDamping			("p_angularDamping", 	"40");
	Var_int			p_linearDamping				("p_linearDamping", 	"35");
	Var_float		p_forceFactor				("p_forceFactor", 		"10");
	Var_float		p_releaseDistance			("p_releaseDistance", 	"2.0");
	Var_float		p_hangSpeed					("p_hangSpeed",			"100");

	class DebugRenderer
		{
		public:
			void renderData(const NxDebugRenderable& data) const;

		private:
			static void renderBuffer(float* pVertList, float* pColorList, int type, int num);
		};

	void DebugRenderer::renderBuffer(float* pVertList, float* pColorList, int type, int num)
		{
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3,GL_FLOAT, 0, pVertList);
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4, GL_FLOAT, 0, pColorList);
			glDrawArrays(type, 0, num);
			glDisableClientState(GL_COLOR_ARRAY);
		    glDisableClientState(GL_VERTEX_ARRAY);
		}

	void DebugRenderer::renderData(const NxDebugRenderable& data) const
	{
		glLineWidth(1.0f);
		glPushAttrib(GL_LIGHTING_BIT);
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);

		unsigned int NbPoints = data.getNbPoints();
		if(NbPoints)
		{
			float* pVertList = new float[NbPoints*3];
	    	float* pColorList = new float[NbPoints*4];
	    	int vertIndex = 0;
	    	int colorIndex = 0;
			const NxDebugPoint* Points = data.getPoints();
			while(NbPoints--)
			{
	        	pVertList[vertIndex++] = Points->p.x;
	        	pVertList[vertIndex++] = Points->p.y;
	        	pVertList[vertIndex++] = Points->p.z;
	        	pColorList[colorIndex++] = (float)((Points->color>>16)&0xff)/255.0f;
	        	pColorList[colorIndex++] = (float)((Points->color>>8)&0xff)/255.0f;
	        	pColorList[colorIndex++] = (float)(Points->color&0xff)/255.0f;
	        	pColorList[colorIndex++] = 1.0f;
		      	Points++;
			}
			
			renderBuffer(pVertList, pColorList, GL_POINTS, data.getNbPoints());
	    	
	    	delete[] pVertList;
	    	delete[] pColorList;
		}

		unsigned int NbLines = data.getNbLines();
		if(NbLines)
		{
			float* pVertList = new float[NbLines*3*2];
	    	float* pColorList = new float[NbLines*4*2];
	    	int vertIndex = 0;
	    	int colorIndex = 0;
			const NxDebugLine* Lines = data.getLines();
			while(NbLines--)
			{
	        	pVertList[vertIndex++] = Lines->p0.x;
	        	pVertList[vertIndex++] = Lines->p0.y;
	        	pVertList[vertIndex++] = Lines->p0.z;
	        	pColorList[colorIndex++] = (float)((Lines->color>>16)&0xff)/255.0f;
	        	pColorList[colorIndex++] = (float)((Lines->color>>8)&0xff)/255.0f;
	        	pColorList[colorIndex++] = (float)(Lines->color&0xff)/255.0f;
	        	pColorList[colorIndex++] = 1.0f;

	        	pVertList[vertIndex++] = Lines->p1.x;
	        	pVertList[vertIndex++] = Lines->p1.y;
	        	pVertList[vertIndex++] = Lines->p1.z;
	        	pColorList[colorIndex++] = (float)((Lines->color>>16)&0xff)/255.0f;
	        	pColorList[colorIndex++] = (float)((Lines->color>>8)&0xff)/255.0f;
	        	pColorList[colorIndex++] = (float)(Lines->color&0xff)/255.0f;
	        	pColorList[colorIndex++] = 1.0f;

		      	Lines++;
			}
			
			renderBuffer(pVertList, pColorList, GL_LINES, data.getNbLines()*2);
	    	
	    	delete[] pVertList;
	    	delete[] pColorList;

			glEnable(GL_DEPTH_TEST);
		}

		unsigned int NbTris = data.getNbTriangles();
		if(NbTris)
		{
			float* pVertList = new float[NbTris*3*3];
	    	float* pColorList = new float[NbTris*4*3];
	    	int vertIndex = 0;
	    	int colorIndex = 0;
			const NxDebugTriangle* Triangles = data.getTriangles();
			while(NbTris--)
			{
	        	pVertList[vertIndex++] = Triangles->p0.x;
	        	pVertList[vertIndex++] = Triangles->p0.y;
	        	pVertList[vertIndex++] = Triangles->p0.z;

	        	pVertList[vertIndex++] = Triangles->p1.x;
	        	pVertList[vertIndex++] = Triangles->p1.y;
	        	pVertList[vertIndex++] = Triangles->p1.z;

	        	pVertList[vertIndex++] = Triangles->p2.x;
	        	pVertList[vertIndex++] = Triangles->p2.y;
	        	pVertList[vertIndex++] = Triangles->p2.z;

				for(int i=0;i<3;i++)
				{
	        		pColorList[colorIndex++] = (float)((Triangles->color>>16)&0xff)/255.0f;
	        		pColorList[colorIndex++] = (float)((Triangles->color>>8)&0xff)/255.0f;
	        		pColorList[colorIndex++] = (float)(Triangles->color&0xff)/255.0f;
	        		pColorList[colorIndex++] = 1.0f;
				}

		      	Triangles++;
			}
			
			renderBuffer(pVertList, pColorList, GL_TRIANGLES, data.getNbTriangles()*3);
	  	
	    	delete[] pVertList;
	    	delete[] pColorList;
		}
		glEnable(GL_LIGHTING);
		glColor4f(1.0f,1.0f,1.0f,1.0f);
	}


	DebugRenderer   gDebugRenderer;

	void CreatePlane( Vector Normal, float D )
	{
	    // Create a plane with default descriptor
	    NxPlaneShapeDesc 	planeDesc;
	    NxActorDesc 		actorDesc;

		planeDesc.normal.setx( Normal.x );
		planeDesc.normal.sety( Normal.y );
		planeDesc.normal.setz( Normal.z );
		planeDesc.d = D;

		assert(planeDesc.isValid());
	    actorDesc.shapes.pushBack(&planeDesc);
	    gScene->createActor(actorDesc);
	}

	void dynamic::item::init( Vector Location, Vector Dimensions )
		{ // item is box
		this->actorType = type::box;

	    // Add a single-shape actor to the scene
	    NxActorDesc actorDesc;
	    NxBodyDesc bodyDesc;

		this->dragOn = false;

	    // The actor has one shape, a box, 1m on a side
	    NxBoxShapeDesc boxDesc;
	    boxDesc.dimensions.set(Dimensions.x,Dimensions.y, Dimensions.z);
	    actorDesc.shapes.pushBack(&boxDesc);

	    actorDesc.body		= &bodyDesc;
	    actorDesc.density 	= 10.0f;
	    actorDesc.globalPose.t 	= NxVec3( Location.x, Location.y, Location.z );
	    assert(actorDesc.isValid());
		this->actor = gScene->createActor(actorDesc);
	    assert(this->actor);
		}

	void dynamic::item::init( Vector Location, float Radius )
		{
		this->actorType = type::sphere;

	    // Add a ball
	    NxActorDesc actorDesc;
	    NxBodyDesc bodyDesc;

		this->dragOn = false;

	    // The actor has one shape, a box, 1m on a side
		NxSphereShapeDesc ballDesc;
		ballDesc.radius = Radius;
	    actorDesc.shapes.pushBack(&ballDesc);

	    actorDesc.body		= &bodyDesc;
	    actorDesc.density 	= 4.0f;
	    actorDesc.globalPose.t 	= NxVec3( Location.x, Location.y, Location.z );
	    assert(actorDesc.isValid());
		this->actor = gScene->createActor(actorDesc);
	    assert(this->actor);
		}

	void dynamic::item::hang( Vector Location )
		{
		if( this->jointL || this->jointR )
			return;

		if( this->speed() > p_hangSpeed )
			return; // don't hang if its too fast.

		this->hangVector = Location;

		const float Scale = 1.0f;
        NxDistanceJointDesc distanceDescR, distanceDescL;
		distanceDescL.actor[0] = reinterpret_cast<NxActor*>(this->actor);
	    distanceDescL.actor[1] = 0;
	    distanceDescL.localAnchor[0] = NxVec3(-Scale,0,Scale);
	    distanceDescL.localAnchor[1] = NxVec3(Location.v);
	    distanceDescL.setGlobalAxis(NxVec3(0,-1,0));
		distanceDescR.actor[0] = reinterpret_cast<NxActor*>(this->actor);
	    distanceDescR.actor[1] = 0;
	    distanceDescR.localAnchor[0] = NxVec3(Scale,0,Scale);
	    distanceDescR.localAnchor[1] = NxVec3(Location.v);
	    distanceDescR.setGlobalAxis(NxVec3(0,-1,0));

//	    NxVec3 dist = a1->getGlobalPose()*anchor1 - a0->getGlobalPose()*anchor0;
	    distanceDescL.maxDistance = Scale;  // maximum rest length of the rope or rod between the two anchor points
	    distanceDescL.minDistance = 0;//Scale;  // minimum rest length of the rope or rod between the two anchor points
	    distanceDescR.maxDistance = Scale;  // maximum rest length of the rope or rod between the two anchor points
	    distanceDescR.minDistance = 0;//Scale;  // minimum rest length of the rope or rod between the two anchor points
	    NxSpringDesc spring;
	    spring.spring = 500;
	    spring.damper = 100;
	    distanceDescL.spring = spring;  // makes the joint springy. The spring.targetValue field is not used.
	    distanceDescL.flags = (NX_DJF_MIN_DISTANCE_ENABLED | NX_DJF_MAX_DISTANCE_ENABLED);  // combination of the bits defined by ::NxDistanceJointFlag
	    distanceDescL.flags |= NX_DJF_SPRING_ENABLED;
	    distanceDescR.spring = spring;  // makes the joint springy. The spring.targetValue field is not used.
	    distanceDescR.flags = (NX_DJF_MIN_DISTANCE_ENABLED | NX_DJF_MAX_DISTANCE_ENABLED);  // combination of the bits defined by ::NxDistanceJointFlag
	    distanceDescR.flags |= NX_DJF_SPRING_ENABLED;

	//    distanceDesc.projectionMode = NX_JPM_NONE;
	//    distanceDesc.projectionMode = NX_JPM_POINT_MINDIST;

	    distanceDescL.jointFlags |= NX_JF_COLLISION_ENABLED;
		
	    this->jointL = gScene->createJoint(distanceDescL);
	    this->jointR = gScene->createJoint(distanceDescR);
		}

	void dynamic::item::matrix( float* Mat ) const
		{
		NxActor* Box = reinterpret_cast<NxActor*>(this->actor);
		NxMat34 pose = Box->getGlobalPose();

	    pose.M.getColumnMajorStride4(&(Mat[0]));
	    pose.t.get(&(Mat[12]));

	    //clear the elements we don't need:
	    Mat[3] = Mat[7] = Mat[11] = 0.0f;
	    Mat[15] = 1.0f;
		}

	void dynamic::item::resize( Vector Dimensions )
		{
		// wait fo simulation to end.
		gScene->fetchResults(NX_RIGID_BODY_FINISHED, true);

		NxActor *Actor = reinterpret_cast<NxActor*>(this->actor);
			// release the shape.
		Actor->releaseShape( **Actor->getShapes() );
				
			// create new one
		switch( this->actorType )
			{
			case type::box:
				{
				NxBoxShapeDesc boxDesc;
		    	boxDesc.dimensions.set(Dimensions.x,Dimensions.y, Dimensions.z);
		    	Actor->createShape(boxDesc);
				Actor->updateMassFromShapes(10.0f,0);
				break;
				}
			case type::sphere:
				{
				NxSphereShapeDesc sphereDesc;
				sphereDesc.radius = Dimensions[0];
				Actor->createShape(sphereDesc);
				Actor->updateMassFromShapes(4.0f,0);
				break;
				}
			}
		}

	Vector dynamic::item::position() const
		{
		NxActor *Actor = reinterpret_cast<NxActor*>(this->actor);
		NxVec3 Pos = Actor->getGlobalPosition();
		return Vector( Pos.x, Pos.y, Pos.z );
		}

	void dynamic::item::identity()
		{
		NxMat33 Mat(	
						NxVec3(0,1,0),NxVec3(1,1,0),
						NxVec3(0,0,1));
						
			
		NxActor *Actor = reinterpret_cast<NxActor*>(this->actor);
		Actor->raiseBodyFlag( NX_BF_KINEMATIC );
		Actor->setGlobalOrientation( Mat );
		Actor->clearBodyFlag( NX_BF_KINEMATIC );
		}
	
	bool dynamic::item::hung() const
		{
		if( this->jointL && this->jointR )
			return true;

		return false;
		}

	Vector dynamic::item::hung_vector() const
		{
		return this->hangVector;
		}

	float dynamic::item::speed() const
		{
		NxActor *Actor = reinterpret_cast<NxActor*>(this->actor);
		return Actor->getLinearVelocity().magnitudeSquared();
		}

	void dynamic::item::drag( Vector Location )
		{
		NxActor *Actor = reinterpret_cast<NxActor*>(this->actor);
		Vector DragGlobal; // point being pulled.

		if(!this->dragOn)
			{
				// get local draging point.
			NxVec3 localPos, worldPos;
		    NxMat34 mat, invMat;

		    worldPos = NxVec3(Location.x,Location.y,Location.z);
#if 0
		    mat = box->getGlobalPose();
		    mat.getInverse(invMat);
		    localPos = invMat * worldPos;
#endif
		    localPos = Actor->getGlobalPose() % worldPos;

			this->dragOn = true;
			this->dragLocal.x = localPos.x;
			this->dragLocal.y = localPos.y;
			this->dragLocal.z = localPos.z;
			DragGlobal = Location;

				// save initial draging position.
			dragInitial = Location;

				// set dragging physics parameters.
			Actor->setLinearDamping( NxReal(p_linearDamping) );
			Actor->setAngularDamping( NxReal(p_angularDamping) );
			}
		else
			{ // get the saved local pos, and make it the global location
			NxVec3 localPos, worldPos;

			localPos.x = this->dragLocal.x;
			localPos.y = this->dragLocal.y;
			localPos.z = this->dragLocal.z;
			worldPos = Actor->getGlobalPose() * localPos;

			DragGlobal.Set(worldPos.x,worldPos.y,worldPos.z);
			}

		if( DragGlobal.distancesquared(dragInitial) > p_releaseDistance )
			{ // release any joints if we're pulling too far
							// first step is to release any joints.
			if( this->jointL )
				{
				gScene->releaseJoint( *reinterpret_cast<NxDistanceJoint*>(this->jointL) );
				this->jointL = 0;
				}
			if( this->jointR )
				{
				gScene->releaseJoint( *reinterpret_cast<NxDistanceJoint*>(this->jointR) );
				this->jointR = 0;
				}
			}

		Vector Force = (Location - DragGlobal)*p_forceFactor;
		if( Force.lengthsquared() > 20 )
			{
			Actor->addForceAtLocalPos( NxVec3(Force.x,Force.y,Force.z), NxVec3(this->dragLocal.x,this->dragLocal.y,this->dragLocal.z), NX_VELOCITY_CHANGE );
			}
		}

	void dynamic::item::drag_off()
		{
		NxActor* Actor = reinterpret_cast<NxActor*>(this->actor);
		this->dragOn = false;
		
		Actor->setLinearDamping( 0 );
		Actor->setAngularDamping( 0 );
		}

	void Init()
		{
		DSys::Logger::Print("----- Physics Initializing -----");
		DSys::Logger::Print("Initializing NVIDIA PhysX %d.%d.%d",
				NX_SDK_VERSION_MAJOR,
				NX_SDK_VERSION_MINOR,
				NX_SDK_VERSION_BUGFIX);

	    // Create the physics SDK
		NxSDKCreateError error;
		gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION,0,0,NxPhysicsSDKDesc(),&error);
	    if (!gPhysicsSDK)
			{
			DSys::Logger::Error("Error Initializing Physics");
			return;
			}

			// adjust parameters
		gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.001f);		
		gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 1);
		gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
		gPhysicsSDK->setParameter(NX_VISUALIZE_ACTOR_AXES, 1);
		gPhysicsSDK->setParameter(NX_VISUALIZE_JOINT_LOCAL_AXES, 1);
		gPhysicsSDK->setParameter(NX_VISUALIZE_JOINT_WORLD_AXES, 1);
		gPhysicsSDK->setParameter(NX_VISUALIZE_JOINT_LIMITS,1);

		    // setup the scene
		NxSceneDesc sceneDesc;
		sceneDesc.gravity			= gDefaultGravity;

		if( gPhysicsSDK->getHWVersion() )
			{
			sceneDesc.simType			= NX_SIMULATION_HW;
			DSys::Logger::Good("Hardware Acceleration: Supported!");
			}
		else
			{
			sceneDesc.simType			= NX_SIMULATION_SW;
			DSys::Logger::Warning("Hardware Acceleration: Not Supported!");
			}

		gScene = gPhysicsSDK->createScene(sceneDesc);

		    // Create the default material
	    NxMaterial* defaultMaterial = gScene->getMaterialFromIndex(0); 
	    defaultMaterial->setRestitution(0.5);
	    defaultMaterial->setStaticFriction(0.30);
	    defaultMaterial->setDynamicFriction(0.25);

		DSys::Logger::Print("--------------------------------");
		}

	void Update( float Msec )
		{
		    // Start collision and dynamics for delta time since the last frame
	    gScene->simulate(Msec);
	    gScene->flushStream();
		gScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
		}

	void Render()
		{
		if( p_debug )
			{
			if (gScene)  gDebugRenderer.renderData(*gScene->getDebugRenderable());
			}
		}

	void Shutdown()
		{
	    if (gScene)
		    {
				// wait for process to finish.
	        gScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
	        gPhysicsSDK->releaseScene(*gScene);
		    }
	    if (gPhysicsSDK)  gPhysicsSDK->release();
		}

		
	} /* Physics */ } /* DSys */
