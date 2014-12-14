// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//			               LESSON 110 : COLLISION FILTERING
//
//						    Written by QA BJ, 6-2-2008
// ===============================================================================

#ifndef LESSON110_H
#define LESSON110_H

#include <GL/glut.h>
#include <stdio.h>

#include "NxPhysics.h"
#include "DrawObjects.h"
#include "HUD.h"

#include "Stream.h"
#include "NxCooking.h"

#include "DebugRenderer.h"

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
void InitGlut(int argc, char** argv);

NxActor* CreateBox(const NxVec3& pos, const NxVec3& boxDim, NxReal density);
void CreateStack();
void SetActorGroupMask(NxActor* actor, NxGroupsMask gm);
void RandomizeActorCollisionFilters(NxScene* scene);

void InitializeHUD();

void InitNx();
void ReleaseNx();
void ResetNx();

void StartPhysics();
void GetPhysicsResults();

int main(int argc, char** argv);

#endif  // LESSON110_H



