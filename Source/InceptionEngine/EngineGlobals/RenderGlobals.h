
#pragma once

/*
double buffer or tripple buffer.
Not supprt tripple buffer yet
*/
constexpr unsigned int NumOfRenderBuffers = 2U;

/*
Design a single shader for both skeletal mesh and 
static mesh. The first two mat4 in uniform buffer
is not anim pose.
*/
constexpr unsigned int AnimPoseOffsetInUBuffer = 2U;

/*
Every vertex can be affected by at most 4 bones
*/
constexpr unsigned int MaxBonePerVertex = 4U;

/*
A skeleton can have at most 198 bones. Can be increase if the gpu 
support larger uniform buffer.
*/
constexpr unsigned int MaxBonePerSkeleton = 198U;