// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//			                     LESSON 1003: DAMPING
//
//                     Written by Matthias M�ller-Fischer, 3-8-05
//						     Edited by Bob Schade, 5-1-06
//							 Edited by Xueyan Gong, 6-5-08
// ===============================================================================

#ifndef LESSON1003_H
#define LESSON1003_H

#include "NxPhysics.h"
#include "DrawObjects.h"
#include "UserData.h"
#include "HUD.h"

#include "Stream.h"
#include "NxCooking.h"

#include "DebugRenderer.h"
#include "UserAllocator.h"
#include "ErrorStream.h"

#include "Actors.h"
#include "MyCloth.h"

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

void RenderCallback();
void ReshapeCallback(int width, int height);
void IdleCallback();
void KeyboardCallback(unsigned char key, int x, int y);
void KeyboardUpCallback(unsigned char key, int x, int y);
void SpecialCallback(int key, int x, int y);
void MouseCallback(int button, int state, int x, int y);
void MotionCallback(int x, int y);
void ExitCallback();
void InitGlut(int argc, char** argv, char* lessonTitle);

void InitializeHUD();
void InitializeSpecialHUD();

void InitNx();
void ReleaseNx();
void ResetNx();

void StartPhysics();
void GetPhysicsResults();

int main(int argc, char** argv);

#endif  // LESSON1003_H



