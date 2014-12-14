// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//						     LESSON 101 : PRIMARY SHAPE
//
//						    Written by QA BJ, 6-2-2008
// ===============================================================================

#ifndef LESSON101_H
#define LESSON101_H

#include <GL/glut.h>

#include <stdio.h>

#include "NxPhysics.h"
#include "DrawObjects.h"
#include "HUD.h"

#include "DebugRenderer.h"
#include "CommonCode.h"

void PrintControls();
void ProcessCameraKeys();
void SetupCamera();
void RenderActors(bool shadows);
void DrawForce(NxActor* actor, NxVec3& forceVec, const NxVec3& color);

NxVec3 ApplyForceToActor(NxActor* actor, const NxVec3& forceDir, const NxReal forceStrength);
void ProcessForceKeys();
void ProcessInputs();
void SelectNextActor();
bool IsSelectable(NxActor* actor);

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
NxActor* CreateBox();
NxActor* CreateSphere();
NxActor* CreateCapsule();
NxActor* CreateBounds3();
NxActor* CreateSegment();
NxActor* CreateRay();

void InitializeHUD();

void InitNx();
void ReleaseNx();
void ResetNx();

void StartPhysics();
void GetPhysicsResults();

int main(int argc, char** argv);

#endif  // LESSON101_H
