#pragma once

class CFixMove
{
public:
	void Start();
	void End();
private:
	float forward, right, up;
	Vector viewforward, viewright, viewup, aimforward, aimright, aimup, vForwardNorm, vRightNorm, vUpNorm;
}; extern CFixMove* FixMove;