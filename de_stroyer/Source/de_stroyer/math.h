#pragma once

// i kind of just pasted this together...whatever no one cares :^)

#define M_PI		3.14159265358979323846f
#define M_RADPI		57.295779513082f
#define RAD2DEG(x)	((float)(x) * (float)(180.f / M_PI))

namespace Math
{
	extern void MakeVector(Angle angAngle, Angle& vecVector);
	extern float GetFov(Vector vecOrigin, Vector vecDestination, Vector vecForward);
}