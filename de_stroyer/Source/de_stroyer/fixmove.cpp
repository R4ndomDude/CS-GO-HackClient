#include "sdk.h"

#define M_PI		3.14159265358979323846f
#define M_RADPI		57.295779513082f

float DotProduct(Vector v1, Vector v2)
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

void AngleVectors(const Vector angles, float* forward, float* right, float* up)
{
	float angle;
	static float sp, sy, cp, cy;

	angle = angles[0] * (M_PI / 180.f);
	sp = sin(angle);
	cp = cos(angle);

	angle = angles[1] * (M_PI / 180.f);
	sy = sin(angle);
	cy = cos(angle);

	if (forward)
	{
		forward[0] = cp * cy;
		forward[1] = cp * sy;
		forward[2] = -sp;
	}
	if (right || up)
	{
		static float sr, cr;

		angle = angles[2] * (M_PI / 180.f);
		sr = sin(angle);
		cr = cos(angle);

		if (right)
		{
			right[0] = -1 * sr * sp * cy + -1 * cr * -sy;
			right[1] = -1 * sr * sp * sy + -1 * cr *cy;
			right[2] = -1 * sr * cp;
		}
		if (up)
		{
			up[0] = cr * sp *cy + -sr * -sy;
			up[1] = cr * sp *sy + -sr * cy;
			up[2] = cr * cp;
		}
	}
}

void Normalize(Vector &vecIn, Vector &vecOut)
{
	float flLen = vecIn.Length();

	if (flLen == 0){
		vecOut.Init(0, 0, 1);
		return;
	}

	flLen = 1 / flLen;
	vecOut.Init(vecIn.x * flLen, vecIn.y * flLen, vecIn.z * flLen);
}

void CFixMove::Start()
{
	Vector angle = G::UserCmd->viewangles;
	angle.x = 0;
	forward = G::UserCmd->forwardmove;
	right = G::UserCmd->sidemove;
	up = G::UserCmd->upmove;
	AngleVectors(angle, (float*)&viewforward, (float*)&viewright, (float*)&viewup);
}

void CFixMove::End()
{
	Vector angle = G::UserCmd->viewangles;
	angle.x = 0;
	AngleVectors(angle, (float*)&aimforward, (float*)&aimright, (float*)&aimup);
	Normalize(viewforward, vForwardNorm);
	Normalize(viewright, vRightNorm);
	Normalize(viewup, vUpNorm);
	G::UserCmd->forwardmove = DotProduct(vForwardNorm * forward, aimforward) + DotProduct(vRightNorm * right, aimforward) + DotProduct(vUpNorm * up, aimforward);
	G::UserCmd->sidemove = DotProduct(vForwardNorm * forward, aimright) + DotProduct(vRightNorm * right, aimright) + DotProduct(vUpNorm * up, aimright);
	//G::UserCmd->upmove = DotProduct(vForwardNorm * forward, aimup) + DotProduct(vRightNorm * right, aimup) + DotProduct(vUpNorm * up, aimup);
}

CFixMove* FixMove = new CFixMove;