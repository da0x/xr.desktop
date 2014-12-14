// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//                              LESSON 105 : MATERIALS
//
//						    Written by QA BJ, 6-2-2008
// ===============================================================================

#ifndef LESSON105_H
#define LESSON105_H

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

NxActor* CreateGroundPlane();
NxActor* CreateCompoundActor();
NxActor* CreateTriangleActor();

void InitializeHUD();

void InitNx();
void ReleaseNx();
void ResetNx();

void StartPhysics();
void GetPhysicsResults();
void ComputeTerrain(bool* done, NxVec3* field, NxU32 x0, NxU32 y0, NxU32 size, NxF32 value);
float trand();
void smoothTriangle(NxU32 a, NxU32 b, NxU32 c, NxVec3* TerrainVerts);
void chooseTrigMaterial(NxU32 faceIndex, NxVec3* TerrainVerts, NxU32* TerrainFaces, NxMaterialIndex* TerrainMaterials);

int main(int argc, char** argv);


#endif  // LESSON105_H


