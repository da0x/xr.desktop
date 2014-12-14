// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//							LESSON 115 : RIGIDBODY SOFTBODY
//
//						    Written by QA BJ, 6-2-2008
// ===============================================================================

#ifndef LESSON115_H
#define LESSON115_H

#include <GL/glut.h>
#include <stdio.h>

#include "NxPhysics.h"
#include "DrawObjects.h"
#include "HUD.h"

#include "Stream.h"
#include "NxCooking.h"

#include "DebugRenderer.h"
#include "MySoftBody.h"
#include "ObjMesh.h"
#include "MediaPath.h"

void PrintControls();
bool IsSelectable(NxActor* actor);
void SelectNextActor();
void ProcessCameraKeys();
void SetupCamera();
void RenderActors(bool shadows);
void DrawForce(NxActor* actor, NxVec3& forceVec, const NxVec3& color);
void RenderSoftBody();

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

void SetupAnimalScene();
void CreateStack();
NxActor* CreateGroundPlane();
NxActor* CreateCapsule(const NxVec3& pos, NxReal height, NxReal radius, NxReal density);
NxActor* CreateBox(const NxVec3& pos, const NxVec3& boxDim, const NxReal density);

void InitializeHUD();

void InitNx();
void ReleaseNx();
void ResetNx();

void StartPhysics();
void GetPhysicsResults();

int main(int argc, char** argv);

#endif  // LESSON115_H
