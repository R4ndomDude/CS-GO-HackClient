class Vector
{
public:
	float x, y, z;

	Vector(float _x = 0, float _y = 0, float _z = 0)
	{
		Init(_x, _y, _z);
	}

	void Init(float _x = 0, float _y = 0, float _z = 0)
	{
		this->x = _x;
		this->y = _y;
		this->z = _z;
	}

	float& operator[](int i)
	{
		return ((float*)this)[i];
	}

	float operator[](int i) const
	{
		return ((float*)this)[i];
	}

	Vector operator+(const float& in)
	{
		return Vector(this->x + in, this->y + in, this->z + in);
	}

	Vector operator-(const float& in)
	{
		return Vector(this->x - in, this->y - in, this->z - in);
	}

	Vector operator*(const float& in)
	{
		return Vector(this->x * in, this->y * in, this->z * in);
	}

	Vector operator/(const float& in)
	{
		return Vector(this->x / in, this->y / in, this->z / in);
	}

	Vector operator+(const Vector& in)
	{
		return Vector(this->x + in.x, this->y + in.y, this->z + in.z);
	}

	Vector operator-(const Vector& in)
	{
		return Vector(this->x - in.x, this->y - in.y, this->z - in.z);
	}

	Vector operator*(const Vector& in)
	{
		return Vector(this->x * in.x, this->y * in.y, this->z * in.z);
	}

	Vector operator/(const Vector& in)
	{
		return Vector(this->x / in.x, this->y / in.y, this->z / in.z);
	}

	Vector operator-=(const Vector& in)
	{
		this->x -= in.x;
		this->y -= in.y;
		this->z -= in.z;

		return *this;
	}

	Vector operator+=(const Vector& in)
	{
		this->x += in.x;
		this->y += in.y;
		this->z += in.z;

		return *this;
	}

	Vector operator/=(const Vector& in)
	{
		this->x /= in.x;
		this->y /= in.y;
		this->z /= in.z;

		return *this;
	}

	Vector operator*=(const Vector& in)
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

	float Length()
	{
		return sqrtf(this->x * this->x + this->y * this->y + this->z * this->z);
	}

	float LengthSqr() const
	{
		return (this->x * this->x + this->y * this->y + this->z * this->z);
	}
};

struct VMatrix
{
	float m[4][4];

	inline float* operator[](int i)
	{
		return m[i];
	}

	inline const float* operator[](int i) const
	{
		return m[i];
	}
};