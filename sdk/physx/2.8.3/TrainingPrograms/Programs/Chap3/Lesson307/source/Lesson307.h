// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//			              LESSON 307: JOINT_NOTIFY
//
// ===============================================================================

#ifndef LESSON307_H
#define LESSON307_H

void PrintControls();
NxD6Joint* CreateD6CylindricalJoint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis);
int main(int argc, char** argv);

#endif



