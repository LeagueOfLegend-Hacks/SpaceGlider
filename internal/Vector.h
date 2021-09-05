#pragma once
#include <cmath>
#include <cfloat>
#define M_PI 3.14159265358979323846f
__forceinline float ToDegree(float Args)
{
	const float flPi = 3.141592654f;
	return (Args * (180.f / flPi));
}
struct Vector2 {
	Vector2() {};
	Vector2(float _x, float _y) {
		x = _x;
		y = _y;
	}

	float x;
	float y;
	float operator*(Vector2& a) const
	{
		return ((x * a.x) + (y * a.y));
	}
	inline bool operator == (const Vector2& A) const
	{
		return A.x == x && A.y == y;
	}

	inline Vector2 operator + (const Vector2& A) const
	{
		return Vector2(x + A.x, y + A.y);
	}

	inline Vector2 operator + (const float A) const
	{
		return Vector2(x + A, y + A);
	}

	inline Vector2 operator * (const float A) const
	{
		return Vector2(A * x, A * y);
	}

	inline Vector2 operator * (const Vector2& A) const
	{
		return Vector2(A.x * x, A.y * y);
	}

	inline Vector2 operator - (const float A) const
	{
		return Vector2(A * x, A * y);
	}

	inline Vector2 operator - (const Vector2& A) const
	{
		return Vector2(A.x - x, A.y - y);
	}

	inline Vector2 operator / (const float A) const
	{
		return Vector2(A / x, A / y);
	}

	inline Vector2 operator / (const Vector2& A) const
	{
		return Vector2(A.x / x, A.y / y);
	}
	Vector2& operator*=(const float a)
	{
		x *= a;
		y *= a;
		return *this;
	}
	friend Vector2 operator*(const float a, const Vector2 b)
	{
		return Vector2(b.x * a, b.y * a);
	}
	Vector2 Rotated(float Angle)
	{
		float c = cosf(Angle);
		float s = sinf(Angle);

		return Vector2(x * c - y * s, y * c + x * s);
	}

	Vector2 Perpendicular()
	{
		return Vector2(y, -x);
	}

	float length() {
		return sqrt(x * x + y * y);
	}

	float distance(const Vector2& o) {
		return sqrt(pow(x - o.x, 2) + pow(y - o.y, 2));
	}

	Vector2 vscale(const Vector2& s) {
		return Vector2(x * s.x, y * s.y);
	}

	Vector2 scale(float s) {
		return Vector2(x * s, y * s);
	}

	Vector2 normalize() {
		float l = length();
		return Vector2(x / l, y / l);
	}

	Vector2 add(const Vector2& o) {
		return Vector2(x + o.x, y + o.y);
	}

	Vector2 sub(const Vector2& o) {
		return Vector2(x - o.x, y - o.y);
	}

	Vector2 clone() {
		return Vector2(x, y);
	}
	bool Intersects(Vector2 const& Seg1End, Vector2 const& Seg2Start, Vector2 const& Seg2End, Vector2* IntersectionOut)
	{
		double deltaACy = y - Seg2Start.y;
		double deltaDCx = Seg2End.x - Seg2Start.x;
		double deltaACx = x - Seg2Start.x;
		double deltaDCy = Seg2End.y - Seg2Start.y;
		double deltaBAx = Seg1End.x - x;
		double deltaBAy = Seg1End.y - y;

		auto denominator = deltaBAx * deltaDCy - deltaBAy * deltaDCx;
		auto numerator = deltaACy * deltaDCx - deltaACx * deltaDCy;

		if (fabs(denominator) < FLT_EPSILON)
		{
			if (fabs(numerator) < FLT_EPSILON)
			{
				// collinear. Potentially infinite intersection points.
				// Check and return one of them.
				if (x >= Seg2Start.x && x <= Seg2End.x)
				{
					if (IntersectionOut)
						*IntersectionOut = *this;

					return true;
				}

				if (Seg2Start.x >= x && Seg2Start.x <= Seg1End.x)
				{
					if (IntersectionOut)
						*IntersectionOut = Seg2Start;

					return true;
				}

				return false;
			}

			// parallel
			return false;
		}

		auto r = numerator / denominator;

		if (r < 0 || r > 1)
			return false;

		auto s = (deltaACy * deltaBAx - deltaACx * deltaBAy) / denominator;

		if (s < 0 || s > 1)
			return false;

		if (IntersectionOut)
			*IntersectionOut = Vector2(x + r * deltaBAx, y + r * deltaBAy);

		return true;
	}
	bool Close(float a, float b, float eps) const
	{
		if (fabs(eps) < FLT_EPSILON)
			eps = (float)1e-9;

		return fabs(a - b) <= eps;
	}
	float Polar() const
	{
		if (Close(x, 0, 0))
		{
			if (y > 0)
				return 90;

			return y < 0 ? 270.f : 0.f;
		}

		float flTheta = ToDegree(atanf(y / x));

		if (x < 0)
			flTheta += 180.f;

		if (flTheta < 0)
			flTheta += 360.f;

		return flTheta;
	}
	float AngleBetween(Vector2 const& Other) const
	{
		float flTheta = Polar() - Other.Polar();

		if (flTheta < 0)
			flTheta += 360.f;

		if (flTheta > 180.f)
			flTheta = 360.f - flTheta;

		return flTheta;
	}
};
struct Vector3 {
	Vector3() {};
	Vector3(float _x, float _y, float _z) {
		x = _x;
		y = _y;
		z = _z;
	}

	float x;
	float y;
	float z;
	Vector3 Rotated(float angle) const
	{
		auto const c = cos(angle);
		auto const s = sin(angle);

		return { static_cast<float>(x * c - z * s), y, static_cast<float>(z * c + x * s) };
	}
	bool Close(float a, float b, float eps) const
	{
		if (abs(eps) < FLT_EPSILON)
		{
			eps = static_cast<float>(1e-9);
		}
		return abs(a - b) <= eps;
	}
	float Polar() const
	{
		if (this->Close(x, 0.f, 0.f))
		{
			if (y > 0.f)
			{
				return 90.f;
			}
			return y < 0.f ? 270.f : 0.f;
		}

		auto theta = atan(y / x) * 180.f / M_PI;
		if (x < 0.f)
		{
			theta = theta + 180.f;
		}
		if (theta < 0.f)
		{
			theta = theta + 360.f;
		}
		return theta;
	}
	float AngleBetween(Vector3 const& other) const
	{
		auto theta = Polar() - other.Polar();
		if (theta < 0.f)
		{
			theta = theta + 360.f;
		}
		if (theta > 180.f)
		{
			theta = 360.f - theta;
		}
		return theta;
	}
	Vector3 Perpendicular()
	{
		return Vector3(z, y, -x);
	}
	Vector2 To2D() const
	{
		return Vector2(x, z);
	}

	float length() {
		return sqrt(x * x + y * y + z * z);
	}

	float distance(const Vector3& o) {
		return sqrt(pow(x - o.x, 2) + pow(y - o.y, 2) + pow(z - o.z, 2));
	}

	inline bool operator == (const Vector3& A) const
	{
		return A.x == x && A.y == y && A.z == z;
	}

	inline Vector3 operator + (const Vector3& A) const
	{
		return Vector3(x + A.x, y + A.y, z + A.z);
	}

	inline Vector3 operator + (const float A) const
	{
		return Vector3(x + A, y + A, z + A);
	}

	inline Vector3 operator * (float A) const
	{
		return Vector3(A * x, A * y, A * z);
	}
	float operator*(Vector3& a) const
	{
		return ((x * a.x) + (y * a.y) + (z * a.z));
	}

	inline Vector3 operator * (const Vector3& A) const
	{
		return Vector3(A.x * x, A.y * y, A.z * z);
	}

	inline Vector3 operator - (const float A) const
	{
		return Vector3(A * x, A * y, A * z);
	}

	inline Vector3 operator - (const Vector3& A) const
	{
		return Vector3(A.x - x, A.y - y, A.z - z);
	}

	inline Vector3 operator / (const float A) const
	{
		return Vector3(A / x, A / y, A / z);
	}

	inline Vector3 operator / (const Vector3& A) const
	{
		return Vector3(A.x / x, A.y / y, A.z / z);
	}

	Vector3 rotate_x(float angle) {
		return Vector3(
			x,
			y * cos(angle) - z * sin(angle),
			y * sin(angle) + z * cos(angle)
		);
	}

	Vector3 rotate_y(float angle) {
		return Vector3(
			x * cos(angle) + z * sin(angle),
			y,
			-x * sin(angle) + z * cos(angle)
		);
	}

	Vector3 rotate_z(float angle) {
		return Vector3(
			x * cos(angle) - y * sin(angle),
			x * sin(angle) + y * cos(angle),
			z
		);
	}

	Vector3 vscale(const Vector3& s) {
		return Vector3(x * s.x, y * s.y, z * s.z);
	}

	Vector3 scale(float s) {
		return Vector3(x * s, y * s, z * s);
	}

	Vector3 normalize() {
		float l = length();
		return Vector3(x / l, y / l, z / l);
	}

	Vector3 add(const Vector3& o) {
		return Vector3(x + o.x, y + o.y, z + o.z);
	}

	Vector3 sub(const Vector3& o) {
		return Vector3(x - o.x, y - o.y, z - o.z);
	}

	Vector3 clone() {
		return Vector3(x, y, z);
	}

	Vector3 extend(const Vector3& dir, double distance) {
		return add(dir).scale(distance);
	}
};
struct Vector4 {
	Vector4() {};
	Vector4(float _x, float _y, float _z, float _w) {
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}

	float x;
	float y;
	float z;
	float w;

	float length() {
		return sqrt(x * x + y * y + z * z + w * w);
	}

	float distance(const Vector4& o) {
		return sqrt(pow(x - o.x, 2) + pow(y - o.y, 2) + pow(z - o.z, 2) + pow(w - o.w, 2));
	}

	Vector4 vscale(const Vector4& s) {
		return Vector4(x * s.x, y * s.y, z * s.z, w * s.w);
	}

	Vector4 scale(float s) {
		return Vector4(x * s, y * s, z * s, w * s);
	}

	Vector4 normalize() {
		float l = length();
		return Vector4(x / l, y / l, z / l, w / l);
	}

	Vector4 add(const Vector4& o) {
		return Vector4(x + o.x, y + o.y, z + o.z, w + o.w);
	}

	Vector4 sub(const Vector4& o) {
		return Vector4(x - o.x, y - o.y, z - o.z, w - o.w);
	}

	Vector4 clone() {
		return Vector4(x, y, z, w);
	}
};