// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//			      LESSON 111: DYNAMIC CONTINUOUS COLLISION DETECTION
//
//						    Written by QA BJ, 6-2-2008
// ===============================================================================

#ifndef LESSON111_H
#define LESSON111_H

#include <GL/glut.h>
#include <stdio.h>

#include "NxPhysics.h"
#include "DrawObjects.h"
#include "HUD.h"

#include "Stream.h"
#include "NxCooking.h"

#include "DebugRenderer.h"
#include "Actors.h"

void PrintControls();
bool IsSelectable(NxActor* actor);
void SelectNextActor();
void ProcessCameraKeys();
void SetupCamera();
void RenderActors(bool shadows);
void DrawForce(NxActor* actor, NxVec3& forceVec, const NxVec3& color);

NxVec3 ApplyForceToActor(NxActor* actor, const NxVec3& forceDir, const NxReal forceStrength, bool forceMode);
void ProcessForceKeys();
void ProcessInputs();
void SpecialKeys(int key, int x, int y);

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

void CreateStack(int size);
void CreateTower(int size);
NxCCDSkeleton* CreateCCDSkeleton(float size);
NxActor* CreateCCDBox(const NxVec3& pos, const NxVec3& boxDim, NxReal density, bool doDynamicCCD);
NxActor* CreateGroundplane();

void InitializeHUD();

void InitNx();
void ReleaseNx();
void ResetNx();

void StartPhysics();
void GetPhysicsResults();

int main(int argc, char** argv);

#endif  // LESSON119_H



