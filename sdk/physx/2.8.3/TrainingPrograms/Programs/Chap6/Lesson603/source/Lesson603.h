// ===============================================================================
//				  NVIDIA PHYSX SDK TRAINING PROGRAMS
//			      LESSON 603: Forcefield_Kernels
//
//				  Written by Xuezhi Deng, 6-4-2008
// ===============================================================================

#ifndef LESSON603_H
#define LESSON603_H

#include <GL/glut.h>

#include <stdio.h>

#include "NxPhysics.h"
#include "DrawObjects.h"
#include "HUD.h"

#include "DebugRenderer.h"



//define a custom forcefield
#include "NxForceFieldKernelDefs.h"
NX_START_FORCEFIELD(Custom)

NxVConst(InitForce);

NX_START_FUNCTION
	// If an object's velocity is zero, give it the initial force
	force = InitForce;
	NxVec3 v = Velocity;
	NxFinishIf(v == NxVec3(0,0,0));

	// While the object is moving, give it the force that is relative to its position
	NxVec3 p = Position;
	force = NxVec3(0,3.5,0) - p;
NX_END_FUNCTION

NX_END_FORCEFIELD(Custom)



void PrintControls();
void ProcessCameraKeys();
void SetupCamera();
void RenderActors(bool shadows);

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

NxActor* CreateGroundPlane();
NxActor* CreateBox(const NxVec3& pos);
NxForceFieldKernelCustom* CreateFFCustomKernel();
NxForceFieldLinearKernel* CreateFFLinearKernel();
NxForceField * CreateForcefield();
void ChangeKernel();

void InitializeHUD();

void InitNx();
void ReleaseNx();
void ResetNx();

void StartPhysics();
void GetPhysicsResults();

int main(int argc, char** argv);

#endif  // LESSON603_H


