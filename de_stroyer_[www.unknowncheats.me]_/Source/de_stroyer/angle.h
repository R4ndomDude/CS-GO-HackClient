#define M_PI		3.14159265358979323846f
#define M_RADPI		57.295779513082f
#define RAD2DEG(x)	((float)(x) * (float)(180.f / M_PI))
#define DEG2RAD(x)	((float)(x) / (float)(180.f / M_PI))

class Angle : public Vector
{
public:
	Angle(float _x = 0.f, float _y = 0.f, float _z = 0.f)
	{
		Init(_x, _y, _z);
	}

	float& operator[](int i)
	{
		return ((float*)this)[i];
	}

	float operator[](int i) const
	{
		return ((float*)this)[i];
	}

	Angle operator+(const Angle& in)
	{
		return Angle(this->x + in.x, this->y + in.y, this->z + in.z);
	}

	Angle operator-(const Angle& in)
	{
		return Angle(this->x - in.x, this->y - in.y, this->z - in.z);
	}

	Angle operator*(const Angle& in)
	{
		return Angle(this->x * in.x, this->y * in.y, this->z * in.z);
	}

	Angle operator/(const Angle& in)
	{
		return Angle(this->x / in.x, this->y / in.y, this->z / in.z);
	}

	Angle operator-=(const Angle& in)
	{
		this->x -= in.x;
		this->y -= in.y;
		this->z -= in.z;

		return *this;
	}

	Angle operator+=(const Angle& in)
	{
		this->x += in.x;
		this->y += in.y;
		this->z += in.z;

		return *this;
	}

	Angle operator/=(const Angle& in)
	{
		this->x /= in.x;
		this->y /= in.y;
		this->z /= in.z;

		return *this;
	}

	Angle operator*=(const Angle& in)
	{
		this->x *= in.x;
		this->y *= in.y;
		this->z *= in.z;

		return *this;
	}

	void Clear()
	{
		this->x = this->y = this->z = .0f;
	}

	Angle Normalize()
	{
		if (this->x != this->x)
			this->x = 0;
		if (this->y != this->y)
			this->y = 0;
		if (this->z != this->z)
			this->z = 0;

		if (this->x > 89.f)
			this->x = 89.f;
		if (this->x < -89.f)
			this->x = -89.f;

		while (this->y > 180)
			this->y -= 360;
		while (this->y <= -180)
			this->y += 360;

		if (this->y > 180.f)
			this->y = 180.f;
		if (this->y < -180.f)
			this->y = -180.f;

		this->z = 0;

		return *this;
	}

	float Difference(Angle angDestination)
	{
		bool bX180 = false;
		bool bY180 = false;

		float XDiff = _Normalize(this->x - angDestination.x);
		float YDiff = _Normalize(this->y - angDestination.y);

		bX180 = 180 > XDiff;
		bY180 = 180 > YDiff;

		if (!bX180)
			XDiff -= 360;

		if (!bY180)
			YDiff -= 360;

		if (0 > XDiff)
			XDiff = (XDiff - XDiff) - XDiff;

		if (0 > YDiff)
			YDiff = (YDiff - YDiff) - YDiff;

		float Diff = YDiff + XDiff;

		return Diff;
	}

	/*
	nigga that shit broken af
	float Difference(Angle dest)
	{
		float _x = DEG2RAD(this->x);
		float _y = DEG2RAD(this->y);
		float __x = DEG2RAD(dest.x);
		float __y = DEG2RAD(dest.y);

		Vector a = Vector(sin(_x) * cos(_y), sin(_x) * sin(_y), cos(_x));
		Vector b = Vector(sin(__x) * cos(__y), sin(__x) * sin(__y), cos(__x));

		return RAD2DEG(acos(a.x * b.x + a.y * b.y + a.z * b.z));
	}
	*/
private:
	float _Normalize(float angAngle)
	{
		return (float)(fmod(angAngle + 180, 360) - 180);
	}
};