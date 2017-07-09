class VectorAligned : public Vector
{
public:
	VectorAligned() {}

	VectorAligned(const Vector &vec)
	{
		this->x = vec.x;
		this->y = vec.y;
		this->z = vec.z;
	}

	float w;
};

enum TraceType_t
{
	TRACE_EVERYTHING = 0,
	TRACE_WORLD_ONLY,				// NOTE: This does *not* test static props!!!
	TRACE_ENTITIES_ONLY,			// NOTE: This version will *not* test static props
	TRACE_EVERYTHING_FILTER_PROPS,	// NOTE: This version will pass the IHandleEntity for props through the filter, unlike all other filters
};

struct Ray_t
{
	Ray_t() { }

	VectorAligned		m_Start;
	VectorAligned		m_Delta;
	VectorAligned		m_StartOffset;
	VectorAligned		m_Extents;
	const matrix3x4_t	*m_pWorldAxisTransform;
	bool				m_IsRay;
	bool				m_IsSwept;

	void Init(Vector vecStart, Vector vecEnd)
	{
		m_Delta = VectorAligned(vecEnd - vecStart);
		m_IsSwept = (m_Delta.LengthSqr() != 0);
		m_Extents.Clear();
		m_pWorldAxisTransform = NULL;
		m_IsRay = true;
		m_StartOffset.Clear();
		m_Start = vecStart;
	}
};

class ITraceFilter
{
public:
	virtual bool ShouldHitEntity(CBaseEntity* pEntityHandle, int contentsMask)
	{
		return !(pEntityHandle == pSkip);
	}
	virtual TraceType_t	GetTraceType()
	{
		return TRACE_EVERYTHING;
	}
	void* pSkip;
};

class CTraceFilter : public ITraceFilter
{
public:
	virtual bool ShouldHitEntity(CBaseEntity* pEntityHandle, int contentsMask)
	{
		return !(pEntityHandle == pSkip);
	}

	virtual TraceType_t	GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}
	void* pSkip;
};

class CTraceFilterSkipTwoEntities : public ITraceFilter
{
public:
	CTraceFilterSkipTwoEntities(void *pPassEnt1, void *pPassEnt2)
	{
		pPassEntity1 = pPassEnt1;
		pPassEntity2 = pPassEnt2;
	}

	virtual bool ShouldHitEntity(CBaseEntity *pEntityHandle, int contentsMask)
	{
		return !(pEntityHandle == pPassEntity1 || pEntityHandle == pPassEntity2);
	}

	virtual TraceType_t GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}

	void *pPassEntity1;
	void *pPassEntity2;
};

struct cplane_t
{
	Vector	normal;
	float	dist;
	byte	type;
	byte	signbits;
	byte	pad[2];
};

class CBaseTrace
{
public:
	Vector			startpos;	
	Vector			endpos;		
	cplane_t		plane;		

	float			fraction;	

	int				contents;	
	unsigned short	dispFlags;	

	bool			allsolid;	
	bool			startsolid;	

	CBaseTrace() {}
};

struct csurface_t
{
	const char*		name;
	short			surfaceProps;
	unsigned short	flags;
};

class CGameTrace : public CBaseTrace
{
public:
	float			fractionleftsolid;
	csurface_t		surface;			

	int				hitgroup;			

	short			physicsbone;		
	unsigned short	worldSurfaceIndex;

	CBaseEntity*	m_pEnt;
	int				hitbox;	

	CGameTrace() {}
}; typedef CGameTrace trace_t;