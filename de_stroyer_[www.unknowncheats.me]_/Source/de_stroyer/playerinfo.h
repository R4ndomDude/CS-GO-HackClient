struct player_info_t
{
	char __pad0[0x8];

	int xuidlow;
	int xuidhigh;

	char name[128];
	int userid;
	char guid[33];

	char __pad1[0x17B];
};

/*typedef struct player_info_s
{
private:
	char __pad[2];
public:
	int xuidlow;
	int xuidhigh;
	char name[128];
	int userid;
	char guid[33];
	UINT m_nSteam3ID;
	char m_szFriendsName[128];
	bool m_bIsFakePlayer;
	bool m_bIsHLTV;
	DWORD m_dwCustomFiles[4];
	BYTE m_FilesDownloaded;
private:
	BYTE __pad1;
} player_info_t;*/