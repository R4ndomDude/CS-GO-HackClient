#pragma once

class ICVar
{
public:
	void RegisterConCommand(ConVar *pCvar);
	ConVar* FindVar(const char *var_name);
};