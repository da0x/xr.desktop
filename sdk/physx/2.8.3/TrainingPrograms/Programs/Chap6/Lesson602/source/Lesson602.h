// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//					      LESSON 602: Forcefield_Coordinate
//
//						  Written by Xuezhi Deng, 6-4-2008
// ===============================================================================

#ifndef LESSON602_H
#define LESSON602_H

#include <GL/glut.h>

#include <stdio.h>

#include "NxPhysics.h"
#include "DrawObjects.h"
#include "HUD.h"

#include "DebugRenderer.h"

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
void CreateForcefield();

void InitializeHUD();

void InitNx();
void ReleaseNx();
void ResetNx();

void StartPhysics();
void GetPhysicsResults();

int main(int argc, char** argv);

#endif  // LESSON602_H
