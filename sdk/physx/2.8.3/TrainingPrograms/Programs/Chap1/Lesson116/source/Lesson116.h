// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//						   LESSON 116: RIGIDBODY FORCEFIELD
//
//						    Written by QA BJ, 6-2-2008
// ===============================================================================

#ifndef LESSON116_H
#define LESSON116_H

#include <GL/glut.h>
#include <stdio.h>

#include "NxPhysics.h"
#include "DrawObjects.h"
#include "HUD.h"

#include "Stream.h"
#include "NxCooking.h"

#include "DebugRenderer.h"
#include "MyForceField.h"

void PrintControls();
bool IsSelectable(NxActor* actor);
void SelectNextActor();
void ProcessCameraKeys();
void SetupCamera();
void RenderActors(bool shadows);
void DrawForce(NxActor* actor, NxVec3& forceVec, const NxVec3& color);
void RenderForceField();
void RenderForceFieldGroup(NxForceFieldShapeGroup *group);
void SampleAndVisualizeForceField();
void SampleAndVisualizeForceFieldShapeGroup(NxForceField* field, NxForceFieldShapeGroup* group);
void MyDrawArrow(const NxVec3& posA, const NxVec3& posB);
void RenderFluid();
void DrawFluid(NxFluid* fluid);

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

void CreateGrid(NxVec3 pos, int size=1, int shapeType=0, int shapeSize=1);
NxActor* CreateBall(const NxVec3& pos, NxReal size = 2.0f, bool trigger = false);

void InitializeHUD();

void InitNx();
void ReleaseNx();
void ResetNx();

void StartPhysics();
void GetPhysicsResults();

int main(int argc, char** argv);

#endif  // LESSON116_H



