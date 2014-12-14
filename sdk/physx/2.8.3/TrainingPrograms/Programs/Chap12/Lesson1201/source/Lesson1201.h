// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//						  LESSON 1201: CHARACTER CONTROLLER
//
//						    Written by Bob Schade, 5-1-06
// ===============================================================================

#ifndef LESSON1201_H
#define LESSON1201_H

#include <GL/glut.h>
#include <stdio.h>

#include "NxPhysics.h"
#include "DrawObjects.h"
#include "Actors.h"
#include "UserData.h"
#include "HUD.h"

#include "DebugRenderer.h"
#include "UserAllocator.h"
#include "ErrorStream.h"

#include "NxController.h"
#include "MyCharacterController.h"
#include "NxControllerManager.h"

#include "Terrain.h"
#include "TerrainRender.h"

#include "Stream.h"

void StartJump(NxF32 v0);
void StopJump();
NxF32 GetHeight(NxF32 elapsedTime);

void PrintControls();
void ProcessKeys();

void SetupCamera();
void RenderActors(bool shadows);

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

void CreateRandomTerrain();

void InitializeHUD();

void InitNx();
void ReleaseNx();
void ResetNx();

NxReal getElapsedTime();
void UpdateCharacter(NxReal deltaTime);
void RunPhysics();

int main(int argc, char** argv);

#endif  // LESSON1201_H


