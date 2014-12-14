// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//		                       LESSON 305: Raycast Report
//
//						    Written by Bob Schade, 5-1-06
//							Updated by Yang Liu, 7-1-08
// ===============================================================================

#ifndef LESSON305_H
#define LESSON305_H

#include "CommonCode.h"
#include "RaycastReport.h"
#include "UserData.h"

void PrintControls();
void AddTerrainLight();
void RaycastClosestShapeFromActor(NxActor* actor, NxU32 groupFlag);
void RaycastAllShapesFromActor(NxActor* actor, NxU32 groupFlag);
int main(int argc, char** argv);

#endif



