#include "SDK.h"

class CPlayer;
class CAimbot
{
public:
	CAimbot();
	~CAimbot();

	void			Main(CUserCmd* pCmd, float frametime);

private:
	void			DropTarget(void);
	void			GetBestTarget(void);
	bool			Valid(int index);

private:
	CUserCmd*		m_pCmd;
	CPlayer*		m_pLocal;
	Vector3			m_vEnd;
	int				m_iBestIndex;
	float			m_flBestTarget;
	int				m_iTimeCounter;
	int				m_iWeaponId;
};