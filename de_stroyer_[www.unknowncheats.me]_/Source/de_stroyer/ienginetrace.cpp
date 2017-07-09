#include "sdk.h"

void IEngineTrace::TraceRay(const Ray_t &ray, unsigned int fMask, ITraceFilter *pTraceFilter, trace_t *pTrace)
{
	typedef void(__thiscall* OriginalFn)(void*, const Ray_t &ray, unsigned int fMask, ITraceFilter *pTraceFilter, trace_t *pTrace);
	return Util::GetVFunc<OriginalFn>(this, 5)(this, ray, fMask, pTraceFilter, pTrace);
}