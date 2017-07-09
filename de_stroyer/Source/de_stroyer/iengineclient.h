#pragma once

class IEngineClient
{
public:
	void	GetScreenSize(int &width, int &height);
	void	GetPlayerInfo(int ent_num, player_info_t* pinfo);
	int		GetLocalPlayer();
	float	GetLastTimeStamp();
	bool	IsInGame();
	const	VMatrix& WorldToScreenMatrix();
	void	ClientCmd_Unrestricted(const char *szCmdString);
};