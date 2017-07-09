#pragma once

#define M_PI		3.14159265358979323846f
#define M_RADPI		57.295779513082f
#define RAD2DEG(x)	((float)(x) * (float)(180.f / M_PI))

namespace Math
{
	extern void MakeVector(Angle angAngle, Angle& vecVector);
	extern float GetFov(Vector vecOrigin, Vector vecDestination, Vector vecForward);
	void SnapRotate(int x, int y, int _x, int _y, int& ox, int& oy) {
		ox = _x - x;
		oy = _y - y;

		double ang = atan2(oy, ox);

		double xx = cos(ang - 1.57f);
		double yy = -cos(ang);

		ox = (int)(xx > 0 ? ceil(xx) : floor(xx));
		oy = (int)(yy > 0 ? ceil(yy) : floor(yy));
	}
}