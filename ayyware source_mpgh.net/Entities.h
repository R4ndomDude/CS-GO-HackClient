/*
Syn's AyyWare Framework 2015
*/

#pragma once

#include "MiscDefinitions.h"
#include "ClientRecvProps.h"
#include "offsets.h"
#include "Vector.h"

#define TEAM_CS_T 2
#define TEAM_CS_CT 3

#define BONE_USED_BY_HITBOX			0x00000100

class IClientRenderable;
class IClientNetworkable;
class IClientUnknown;
class IClientThinkable;
class IClientEntity;
class CSWeaponInfo;

class CSWeaponInfo
{
public:
	virtual void Function0(); //
	virtual void Function1(); //
	virtual void Parse(KeyValues *pKeyValuesData, const char *szWeaponName); //
	virtual void RefreshDynamicParameters(void); //
	virtual void Function4(); //
	virtual void Function5(); //
	virtual void Function6(); //
	virtual void Function7(); //
	virtual void Function8(); //

	char pad_0x0004[0x2]; //0x0004
	char m_pszName[32]; //0x0006 
	char pad_0x0026[0x7E]; //0x0026
	__int16 N0000002E; //0x00A4 
	char m_pszModelName[32]; //0x00A6 
	char pad_0x00C6[0x6DE]; //0x00C6
	BYTE m_IsFullAuto; //0x07A4 
	char pad_0x07A5[0x7]; //0x07A5
	__int32 m_iPrice; //0x07AC 
	float m_flArmorRatio; //0x07B0 
	char pad_0x07B4[0x10]; //0x07B4
	float m_flPenetration; //0x07C4 
	__int32 m_iDamage; //0x07C8 
	float m_flRange; //0x07CC 
	float m_flRangeModifier; //0x07D0 
	char pad_0x07D4[0x4]; //0x07D4
	float m_flCycleTime; //0x07D8 

};

enum class CSGOClassID
{
	CTestTraceline = 189,
	CTEWorldDecal = 190,
	CTESpriteSpray = 187,
	CTESprite = 186,
	CTESparks = 185,
	CTESmoke = 184,
	CTEShowLine = 182,
	CTEProjectedDecal = 179,
	CTEPlayerDecal = 178,
	CTEPhysicsProp = 175,
	CTEParticleSystem = 174,
	CTEMuzzleFlash = 173,
	CTELargeFunnel = 171,
	CTEKillPlayerAttachments = 170,
	CTEImpact = 169,
	CTEGlowSprite = 168,
	CTEShatterSurface = 181,
	CTEFootprintDecal = 165,
	CTEFizz = 164,
	CTEExplosion = 162,
	CTEEnergySplash = 161,
	CTEEffectDispatch = 160,
	CTEDynamicLight = 159,
	CTEDecal = 157,
	CTEClientProjectile = 156,
	CTEBubbleTrail = 155,
	CTEBubbles = 154,
	CTEBSPDecal = 153,
	CTEBreakModel = 152,
	CTEBloodStream = 151,
	CTEBloodSprite = 150,
	CTEBeamSpline = 149,
	CTEBeamRingPoint = 148,
	CTEBeamRing = 147,
	CTEBeamPoints = 146,
	CTEBeamLaser = 145,
	CTEBeamFollow = 144,
	CTEBeamEnts = 143,
	CTEBeamEntPoint = 142,
	CTEBaseBeam = 141,
	CTEArmorRicochet = 140,
	CTEMetalSparks = 172,
	CSteamJet = 135,
	CSmokeStack = 128,
	DustTrail = 238,
	CFireTrail = 62,
	SporeTrail = 244,
	SporeExplosion = 243,
	RocketTrail = 241,
	SmokeTrail = 242,
	CPropVehicleDriveable = 117,
	ParticleSmokeGrenade = 240,
	CParticleFire = 96,
	MovieExplosion = 239,
	CTEGaussExplosion = 167,
	CEnvQuadraticBeam = 55,
	CEmbers = 45,
	CEnvWind = 59,
	CPrecipitation = 111,
	CPrecipitationBlocker = 112,
	CBaseTempEntity = 18,
	NextBotCombatCharacter = 0,
	CBaseAttributableItem = 4,
	CEconEntity = 44,
	CWeaponXM1014 = 236,
	CWeaponTaser = 231,
	CSmokeGrenade = 126,
	CWeaponSG552 = 228,
	CWeaponSawedoff = 224,
	CWeaponNOVA = 220,
	CIncendiaryGrenade = 85,
	CMolotovGrenade = 93,
	CWeaponM3 = 212,
	CKnifeGG = 90,
	CKnife = 89,
	CHEGrenade = 82,
	CFlashbang = 64,
	CWeaponElite = 203,
	CDecoyGrenade = 40,
	CDEagle = 39,
	CWeaponUSP = 235,
	CWeaponM249 = 211,
	CWeaponUMP45 = 234,
	CWeaponTMP = 233,
	CWeaponTec9 = 232,
	CWeaponSSG08 = 230,
	CWeaponSG556 = 229,
	CWeaponSG550 = 227,
	CWeaponScout = 226,
	CWeaponSCAR20 = 225,
	CSCAR17 = 122,
	CWeaponP90 = 223,
	CWeaponP250 = 222,
	CWeaponP228 = 221,
	CWeaponNegev = 219,
	CWeaponMP9 = 218,
	CWeaponMP7 = 217,
	CWeaponMP5Navy = 216,
	CWeaponMag7 = 215,
	CWeaponMAC10 = 214,
	CWeaponM4A1 = 213,
	CWeaponHKP2000 = 210,
	CWeaponGlock = 209,
	CWeaponGalilAR = 208,
	CWeaponGalil = 207,
	CWeaponG3SG1 = 206,
	CWeaponFiveSeven = 205,
	CWeaponFamas = 204,
	CWeaponBizon = 199,
	CWeaponAWP = 198,
	CWeaponAug = 197,
	CAK47 = 1,
	CWeaponCSBaseGun = 201,
	CWeaponCSBase = 200,
	CC4 = 29,
	CBaseCSGrenade = 8,
	CSmokeGrenadeProjectile = 127,
	CMolotovProjectile = 94,
	CDecoyProjectile = 41,
	CFireCrackerBlast = 60,
	CInferno = 86,
	CChicken = 31,
	CFootstepControl = 66,
	CCSGameRulesProxy = 34,
	CWeaponCubemap = 0,
	CWeaponCycler = 202,
	CTEPlantBomb = 176,
	CTEFireBullets = 163,
	CTERadioIcon = 180,
	CPlantedC4 = 104,
	CCSTeam = 38,
	CCSPlayerResource = 36,
	CCSPlayer = 35,
	CCSRagdoll = 37,
	CTEPlayerAnimEvent = 177,
	CHostage = 83,
	CHostageCarriableProp = 84,
	CBaseCSGrenadeProjectile = 9,
	CHandleTest = 81,
	CTeamplayRoundBasedRulesProxy = 139,
	CSpriteTrail = 133,
	CSpriteOriented = 132,
	CSprite = 131,
	CRagdollPropAttached = 120,
	CRagdollProp = 119,
	CPredictedViewModel = 113,
	CPoseController = 109,
	CGameRulesProxy = 80,
	CInfoLadderDismount = 87,
	CFuncLadder = 72,
	CTEFoundryHelpers = 166,
	CEnvDetailController = 51,
	CWorld = 237,
	CWaterLODControl = 196,
	CWaterBullet = 195,
	CVoteController = 194,
	CVGuiScreen = 193,
	CPropJeep = 116,
	CPropVehicleChoreoGeneric = 0,
	CTriggerSoundOperator = 192,
	CBaseVPhysicsTrigger = 22,
	CTriggerPlayerMovement = 191,
	CBaseTrigger = 20,
	CTest_ProxyToggle_Networkable = 188,
	CTesla = 183,
	CBaseTeamObjectiveResource = 17,
	CTeam = 138,
	CSunlightShadowControl = 137,
	CSun = 136,
	CParticlePerformanceMonitor = 97,
	CSpotlightEnd = 130,
	CSpatialEntity = 129,
	CSlideshowDisplay = 125,
	CShadowControl = 124,
	CSceneEntity = 123,
	CRopeKeyframe = 121,
	CRagdollManager = 118,
	CPhysicsPropMultiplayer = 102,
	CPhysBoxMultiplayer = 100,
	CPropDoorRotating = 115,
	CBasePropDoor = 16,
	CDynamicProp = 43,
	CProp_Hallucination = 114,
	CPostProcessController = 110,
	CPointCommentaryNode = 108,
	CPointCamera = 107,
	CPlayerResource = 106,
	CPlasma = 105,
	CPhysMagnet = 103,
	CPhysicsProp = 101,
	CStatueProp = 134,
	CPhysBox = 99,
	CParticleSystem = 98,
	CMovieDisplay = 95,
	CMaterialModifyControl = 92,
	CLightGlow = 91,
	CInfoOverlayAccessor = 88,
	CFuncTrackTrain = 79,
	CFuncSmokeVolume = 78,
	CFuncRotating = 77,
	CFuncReflectiveGlass = 76,
	CFuncOccluder = 75,
	CFuncMoveLinear = 74,
	CFuncMonitor = 73,
	CFunc_LOD = 68,
	CTEDust = 158,
	CFunc_Dust = 67,
	CFuncConveyor = 71,
	CFuncBrush = 70,
	CBreakableSurface = 28,
	CFuncAreaPortalWindow = 69,
	CFish = 63,
	CFireSmoke = 61,
	CEnvTonemapController = 58,
	CEnvScreenEffect = 56,
	CEnvScreenOverlay = 57,
	CEnvProjectedTexture = 54,
	CEnvParticleScript = 53,
	CFogController = 65,
	CEnvDOFController = 52,
	CCascadeLight = 30,
	CEnvAmbientLight = 50,
	CEntityParticleTrail = 49,
	CEntityFreezing = 48,
	CEntityFlame = 47,
	CEntityDissolve = 46,
	CDynamicLight = 42,
	CColorCorrectionVolume = 33,
	CColorCorrection = 32,
	CBreakableProp = 27,
	CBeamSpotlight = 25,
	CBaseButton = 5,
	CBaseToggle = 19,
	CBasePlayer = 15,
	CBaseFlex = 12,
	CBaseEntity = 11,
	CBaseDoor = 10,
	CBaseCombatCharacter = 6,
	CBaseAnimatingOverlay = 3,
	CBoneFollower = 26,
	CBaseAnimating = 2,
	CAI_BaseNPC = 0,
	CBeam = 24,
	CBaseViewModel = 21,
	CBaseParticleEntity = 14,
	CBaseGrenade = 13,
	CBaseCombatWeapon = 7,
	CBaseWeaponWorldModel = 23
};

enum class CSGOHitboxID
{
	Head = 0,
	Neck,
	NeckLower,
	Pelvis,
	Stomach,
	LowerChest,
	Chest,
	UpperChest,
	RightThigh,
	LeftThigh,
	RightShin,
	LeftShin,
	RightFoot,
	LeftFoot,
	RightHand,
	LeftHand,
	RightUpperArm,
	RightLowerArm,
	LeftUpperArm,
	LeftLowerArm
};

class ScriptCreatedItem
{
public:
	CPNETVAR_FUNC(int*, ItemDefinitionIndex, 0xE67AB3B8); //m_iItemDefinitionIndex
	CPNETVAR_FUNC(int*, ItemIDHigh, 0x714778A); //m_iItemIDHigh
	CPNETVAR_FUNC(int*, ItemIDLow, 0x3A3DFC74); //m_iItemIDLow
};

class AttributeContainer
{
public:
	CPNETVAR_FUNC(ScriptCreatedItem*, m_Item, 0x7E029CE5);
};


class CBaseCombatWeapon
{
public:
	CNETVAR_FUNC(float, GetNextPrimaryAttack, 0xDB7B106E); //m_flNextPrimaryAttack
	CNETVAR_FUNC(int, GetAmmoInClip, 0x97B6F70C); //m_iClip1
	CNETVAR_FUNC(HANDLE, GetOwnerHandle, 0xC32DF98D); //m_hOwner
	CNETVAR_FUNC(Vector, GetOrigin, 0x1231CE10); //m_vecOrigin

	CPNETVAR_FUNC(int*, FallbackPaintKit, 0xADE4C870); // m_nFallbackPaintKit
	CPNETVAR_FUNC(int*, FallbackSeed, 0xC2D0683D); // m_nFallbackSeed
	CPNETVAR_FUNC(float*, FallbackWear, 0xA263576C); //m_flFallbackWear
	CPNETVAR_FUNC(int*, FallbackStatTrak, 0x1ED78768); //m_nFallbackStatTrak
	CPNETVAR_FUNC(int*, OwnerXuidLow, 0xAD8D897F);
	CPNETVAR_FUNC(int*, OwnerXuidHigh, 0x90511E77);
	CPNETVAR_FUNC(int*, ViewModelIndex, 0x7F7C89C1);
	CPNETVAR_FUNC(int*, WorldModelIndex, 0x4D8AD9F3);

	CPNETVAR_FUNC(AttributeContainer*, m_AttributeManager, 0xCFFCE089);

	float GetInaccuracy()
	{
		typedef float(__thiscall* oGetSpread)(PVOID);
		return call_vfunc< oGetSpread>(this, Offsets::VMT::Weapon_GetSpread)(this);
	}

	float GetSpread()
	{
		typedef float(__thiscall* oGetInac)(PVOID);
		return call_vfunc< oGetInac>(this, Offsets::VMT::Weapon_GetSpread + 1)(this);
	}

	void UpdateAccuracyPenalty()
	{
		typedef void(__thiscall* oUpdateAccuracyPenalty)(PVOID);
		return call_vfunc< oUpdateAccuracyPenalty>(this, Offsets::VMT::Weapon_GetSpread + 2)(this);
	}

	CSWeaponInfo* GetCSWpnData()
	{
		static DWORD GetCSWpnDataAddr = Utilities::Memory::FindPattern("client.dll", (PBYTE)"\x55\x8B\xEC\x81\xEC\x00\x00\x00\x00\xB8\x00\x00\x00\x00\x57", "xxxxx????x????x");
		if (GetCSWpnDataAddr)
		{
			CSWeaponInfo* retData;
			__asm
			{
				mov ecx, this
				call GetCSWpnDataAddr
				mov retData, eax
			}
			return retData;
		}
		else
		{
			return nullptr;
		}
	}
};

class CCSBomb
{
public:
	CNETVAR_FUNC(HANDLE, GetOwnerHandle, 0xC32DF98D); //m_hOwner
	CNETVAR_FUNC(float, GetC4BlowTime, 0xB5E0CA1C); //m_flC4Blow
	CNETVAR_FUNC(float, GetC4DefuseCountDown, 0xB959B4A6); //m_flDefuseCountDown
};

class CLocalPlayerExclusive
{
public:
	CNETVAR_FUNC(Vector, GetViewPunchAngle, 0x68F014C0);//m_viewPunchAngle
	CNETVAR_FUNC(Vector, GetAimPunchAngle, 0xBF25C290);//m_aimPunchAngle
	CNETVAR_FUNC(Vector, GetAimPunchAngleVel, 0x8425E045);//m_aimPunchAngleVel
};

class CollisionProperty
{
public:
	CNETVAR_FUNC(Vector, GetMins, 0xF6F78BAB);//m_vecMins
	CNETVAR_FUNC(Vector, GetMaxs, 0xE47C6FC4);//m_vecMaxs
	CNETVAR_FUNC(unsigned char, GetSolidType, 0xB86722A1);//m_nSolidType
	CNETVAR_FUNC(unsigned short, GetSolidFlags, 0x63BB24C1);//m_usSolidFlags
	CNETVAR_FUNC(int, GetSurroundsType, 0xB677A0BB); //m_nSurroundType

	bool IsSolid()
	{
		return (GetSolidType() != SOLID_NONE) && ((GetSolidFlags() & FSOLID_NOT_SOLID) == 0);
	}
};

class IClientRenderable
{
public:
	//virtual void*					GetIClientUnknown() = 0;
	virtual Vector const&			GetRenderOrigin(void) = 0;
	virtual Vector const&			GetRenderAngles(void) = 0;
	virtual bool					ShouldDraw(void) = 0;
	virtual bool					IsTransparent(void) = 0;
	virtual bool					UsesPowerOfTwoFrameBufferTexture() = 0;
	virtual bool					UsesFullFrameBufferTexture() = 0;
	virtual void					GetShadowHandle() const = 0;
	virtual void*					RenderHandle() = 0;
	virtual const model_t*				GetModel() const = 0;
	virtual int						DrawModel(int flags) = 0;
	virtual int						GetBody() = 0;
	virtual void					ComputeFxBlend() = 0;

	bool SetupBones(matrix3x4 *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
	{
		typedef bool(__thiscall* oSetupBones)(PVOID, matrix3x4*, int, int, float);
		return call_vfunc< oSetupBones>(this, 13)(this, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
	}
};

class IClientNetworkable
{
public:
	virtual IClientUnknown*	GetIClientUnknown() = 0;
	virtual void			Release() = 0;
	virtual ClientClass*	GetClientClass() = 0;// FOR NETVARS FIND YOURSELF ClientClass* stuffs
	virtual void			NotifyShouldTransmit( /* ShouldTransmitState_t state*/) = 0;
	virtual void			OnPreDataChanged( /*DataUpdateType_t updateType*/) = 0;
	virtual void			OnDataChanged( /*DataUpdateType_t updateType*/) = 0;
	virtual void			PreDataUpdate( /*DataUpdateType_t updateType*/) = 0;
	virtual void			PostDataUpdate( /*DataUpdateType_t updateType*/) = 0;
	virtual void			unknown();
	virtual bool			IsDormant(void) = 0;
	virtual int				GetIndex(void) const = 0;
	virtual void			ReceiveMessage(int classID /*, bf_read &msg*/) = 0;
	virtual void*			GetDataTableBasePtr() = 0;
	virtual void			SetDestroyedOnRecreateEntities(void) = 0;
};

class IClientUnknown
{
public:
	virtual void*		GetCollideable() = 0;
	virtual IClientNetworkable*	GetClientNetworkable() = 0;
	virtual IClientRenderable*	GetClientRenderable() = 0;
	virtual IClientEntity*		GetIClientEntity() = 0;
	virtual IClientEntity*		GetBaseEntity() = 0;
	virtual IClientThinkable*	GetClientThinkable() = 0;
};

class IClientThinkable
{
public:
	virtual IClientUnknown*		GetIClientUnknown() = 0;
	virtual void				ClientThink() = 0;
	virtual void*				GetThinkHandle() = 0;
	virtual void				SetThinkHandle(void* hThink) = 0;
	virtual void				Release() = 0;
};

class __declspec (novtable)IClientEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable, public IClientThinkable
{
public:
public:
	virtual void			Release(void) = 0;
	virtual void			blahblahpad(void) = 0;
	virtual Vector&	GetAbsOrigin(void) const = 0;//in broken place use GetOrigin Below
	virtual const Vector&	GetAbsAngles(void) const = 0;

	//---                 NetVars                  ---//

	CPNETVAR_FUNC(CLocalPlayerExclusive*, localPlayerExclusive, 0x7177BC3E);// m_Local
	CPNETVAR_FUNC(CollisionProperty*, collisionProperty, 0xE477CBD0);//m_Collision

	CNETVAR_FUNC(int, GetFlags, 0xE456D580); //m_fFlags
	CNETVAR_FUNC(Vector, GetOrigin, 0x1231CE10); //m_vecOrigin
	CNETVAR_FUNC(Vector, GetRotation, 0x6BEA197A); //m_angRotation
	CNETVAR_FUNC(int, GetTeamNum, 0xC08B6C6E); //m_iTeamNum
	CNETVAR_FUNC(int, GetMaxHealth, 0xC52E1C28); //m_iMaxHealth
	CNETVAR_FUNC(int, GetHealth, 0xA93054E3); //m_iHealth
	CNETVAR_FUNC(unsigned char, GetLifeState, 0xD795CCFC); //m_lifeState
	CNETVAR_FUNC(HANDLE, GetActiveWeaponHandle, 0xB4FECDA3); //m_hActiveWeapon
	CNETVAR_FUNC(int, GetTickBase, 0xD472B079); //m_nTickBase
	CNETVAR_FUNC(Vector, GetViewOffset, 0xA9F74931); //m_vecViewOffset[0]
	CNETVAR_FUNC(Vector, GetVelocity, 0x40C1CA24); //m_vecVelocity[0]
	CNETVAR_FUNC(bool, HasGunGameImmunity, 0x6AD6FA0D); //m_bGunGameImmunity
	CNETVAR_FUNC(bool, IsDefusing, 0xA2C14106); //m_bIsDefusing
	CNETVAR_FUNC(int, ArmorValue, 0x3898634); //m_ArmorValue
	CNETVAR_FUNC(bool, HasHelmet, 0x7B97F18A); //m_bHasHelmet
	CNETVAR_FUNC(bool, IsScoped, 0x61B9C22C); //m_bIsScoped
	CNETVAR_FUNC(int, GetMoney, 0xF4B3E183); //m_iAccount
	CNETVAR_FUNC(HANDLE, GetObserverTargetHandle, 0x8660FD83); //m_hObserverTarget
	// ----------------------------------------------//

	bool IsAlive()
	{
		return (GetLifeState() == LIFE_ALIVE && GetHealth() > 0 );
	}

	Vector GetBonePos(int i)
	{
		matrix3x4 boneMatrix[128];
		if (this->SetupBones(boneMatrix, 128, BONE_USED_BY_HITBOX, GetTickCount64()))
		{
			return Vector(boneMatrix[i][0][3], boneMatrix[i][1][3], boneMatrix[i][2][3]);
		}
		return Vector(0, 0, 0);
	}

	Vector GetHeadPos()
	{
		return this->GetBonePos(6);
	}

};