/*
Syn's AyyWare Framework 2015
*/

#pragma once

#include "SDK.h"

namespace GameUtils
{
	void NormaliseViewAngle(Vector &angle);
	void CL_FixMove(CUserCmd* cmd, Vector viewangles);
	bool IsVisible(IClientEntity* pLocal, IClientEntity* pEntity, int BoneID);
	bool IsBallisticWeapon(void* weapon);
	bool IsPistol(void* weapon);
	bool IsSniper(void* weapon);
	int GetPlayerCompRank(IClientEntity* pEntity);
};

// Trace Line Memes
void UTIL_TraceLine(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask,
	const IClientEntity *ignore, int collisionGroup, trace_t *ptr);

void UTIL_ClipTraceToPlayers(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, ITraceFilter* filter, trace_t* tr);

bool IsBreakableEntity(IClientEntity* ent);

bool TraceToExit(Vector& end, trace_t& tr, Vector start, Vector vEnd, trace_t* trace);

void SayInChat(const char *text);

Vector GetHitboxPosition(IClientEntity* pEntity, int Hitbox);

void ForceUpdate();