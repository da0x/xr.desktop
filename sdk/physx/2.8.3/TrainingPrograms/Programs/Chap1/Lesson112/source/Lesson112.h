// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//								 LESSON 112 : OVERLAP
//
//						    Written by QA BJ, 6-2-2008
// ===============================================================================

#ifndef LESSON112_H
#define LESSON112_H

#include <GL/glut.h>
#include <stdio.h>

#include "NxPhysics.h"
#include "DrawObjects.h"
#include "HUD.h"

#include "Stream.h"
#include "NxCooking.h"

#include "DebugRenderer.h"

class ShapeReport : public NxUserEntityReport<NxShape*>
{
public:
	virtual bool onEvent(NxU32 nbShapes, NxShape** shapes)
	{
		return true;
	}

} gShapeReport;

enum OverlapType
{
	OVERLAP_AABB = 0,
	OVERLAP_CAPSULE,
	OVERLAP_OBB,
	OVERLAP_SPHERE,
	OVERLAP_CHECK_AABB,
	OVERLAP_CHECK_CAPSULE,
	OVERLAP_CHECK_OBB,
	OVERLAP_CHECK_SPHERE,
	OVERLAP_CULL
};

void PrintControls();
bool IsSelectable(NxActor* actor);
void SelectNextActor();
void ProcessCameraKeys();
void SetupCamera();
void RenderActors(bool shadows);
void DrawForce(NxActor* actor, NxVec3& forceVec, const NxVec3& color);
void Update();

NxVec3 ApplyForceToActor(NxActor* actor, const NxVec3& forceDir, const NxReal forceStrength, bool forceMode);
void ProcessForceKeys();
void ProcessInputs();

void RenderCallback();
void ReshapeCallback(int width, int height);
void IdleCallback();
void KeyboardCallback(unsigned char key, int x, int y);
void KeyboardUpCallback(unsigned char key, int x, int y);
void SpecialCallback(int key, int x, int y);
void MouseCallback(int button, int state, int x, int y);
void MotionCallback(int x, int y);
void ExitCallback();
void InitGlut(int argc, char** argv);

NxActor* CreateSphere(const NxVec3 &pos, int flag);	// flag= 0: DYNAMIC; 1: STATIC; 2: KINEMATIC
NxActor* CreateBox(const NxVec3 &pos, int flag);	// flag= 0: DYNAMIC; 1: STATIC; 2: KINEMATIC
NxActor* CreateCapsule(const NxVec3 &pos, int flag);// flag= 0: DYNAMIC; 1: STATIC; 2: KINEMATIC
NxActor* CreateConvex(const NxVec3 &pos, int flag); // flag= 0: DYNAMIC; 1: STATIC; 2: KINEMATIC

void InitializeHUD();

void InitNx();
void ReleaseNx();
void ResetNx();

void StartPhysics();
void GetPhysicsResults();

int main(int argc, char** argv);

#endif  // LESSON112_H


