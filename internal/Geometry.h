#pragma once
#include <list>
#include <vector>
#include "Vector.h"
#include "ImRender.h"
#include "clipper.h"
#define CircleLineSegmentN 16
namespace Geometry
{
	class Polygon
	{
	public:
		std::vector<Vector3> Points;

		void Add(const Vector3& point)
		{
			Points.push_back(point);
		}

		void Draw(ImColor Color, int Width = 1)
		{
			for (size_t i = 0; i <= Points.size() - 1; i++)
			{
				auto iNextIdx = (Points.size() - 1 == i) ? 0 : (i + 1);
				render.draw_line(Points[i].To2D(), Points[iNextIdx].To2D(), Color, Width);
			}
		}

		bool IsInside(const Vector3& point)
		{
			return !IsOutside(point);
		}

		bool IsOutside(const Vector3& point)
		{
			const auto p = ClipperLib::IntPoint(point.x, point.z);
			return ClipperLib::PointInPolygon(p, ToClipperPath()) != 1;
		}

		int PointInPolygon(const Vector3& point)
		{
			const auto p = ClipperLib::IntPoint(point.x, point.z);
			return ClipperLib::PointInPolygon(p, ToClipperPath());
		}

		std::vector<ClipperLib::IntPoint> ToClipperPath()
		{
			std::vector<ClipperLib::IntPoint> result;

			for (const auto& point : Points)
				result.emplace_back(point.x, point.z);

			return result;
		}
	};

	class Arc
	{
	public:
		Vector3 End;
		int HitBox;
		Vector3 Start;
		float distanceTo;

		Arc(const Vector3& start, const Vector3& end, int hitbox)
		{
			Start = start;
			End = end;
			HitBox = hitbox;
			distanceTo = Start.distance(End);
		}

		std::vector<Vector3> CircleCircleIntersection(Vector3& center1, Vector3& center2, float radius1, float radius2)
		{
			std::vector<Vector3> result;

			float d = center1.distance(center2);

			if (d > radius1 + radius2 || d <= abs(radius1 - radius2))
			{
				return result;
			}

			float a = (radius1 * radius1 - radius2 * radius2 + d * d) / (2 * d);
			float h = (float)sqrt(radius1 * radius1 - a * a);
			Vector3 direction = (center2 - center1).normalize();
			Vector3 pa = center1 + direction * a;
			Vector3 s1 = pa + direction.Perpendicular() * h;
			Vector3 s2 = pa - direction.Perpendicular() * h;

			result.push_back(s1);
			result.push_back(s2);
			return result;
		}

		Polygon ToPolygon(int offset = 0)
		{
			offset += HitBox;
			Polygon result = Polygon();

			float innerRadius = -0.1562f * distanceTo + 600.f;
			float outerRadius = 0.45256f * distanceTo + 50.f;

			outerRadius = outerRadius / (float)cos(2 * IM_PI / CircleLineSegmentN);

			auto innerCenters = CircleCircleIntersection(Start, End, innerRadius, innerRadius);
			auto outerCenters = CircleCircleIntersection(Start, End, outerRadius, outerRadius);

			Vector3 innerCenter = innerCenters[0];
			Vector3 outerCenter = outerCenters[0];

			Vector3 direction = (End - outerCenter).normalize();
			Vector3 end = (Start - outerCenter).normalize();
			float maxAngle = (float)(direction.AngleBetween(end) * IM_PI / 180.0f);

			float step = -maxAngle / CircleLineSegmentN;

			for (int i = 0; i < CircleLineSegmentN; i++)
			{
				float angle = step * i;
				Vector3 point = outerCenter + direction.Rotated(angle) * (outerRadius + 150 + offset);
				result.Add(point);
			}

			direction = (Start - innerCenter).normalize();
			end = (End - innerCenter).normalize();
			maxAngle = (float)(direction.AngleBetween(end) * M_PI / 180.0f);
			step = maxAngle / CircleLineSegmentN;

			for (int i = 0; i < CircleLineSegmentN; i++)
			{
				float angle = step * i;
				Vector3 point = innerCenter + direction.Rotated(angle) * fmax(0, innerRadius - offset);
				result.Add(point);
			}

			return result;
		}
	};

	class Circle
	{
	public:
		Vector3 Center;
		float Radius;

		Circle(const Vector3& center, float radius)
		{
			Center = center;
			Radius = radius;
		}

		Polygon ToPolygon(int offset = 0, float overrideWidth = -1)
		{
			Polygon result = Polygon();
			float outRadius = overrideWidth > 0
				? overrideWidth
				: (offset + Radius) / (float)cos(2 * M_PI / CircleLineSegmentN);

			double step = 2 * M_PI / CircleLineSegmentN;
			double angle = Radius;
			for (int i = 0; i <= CircleLineSegmentN; i++)
			{
				angle += step;
				Vector3 point = Vector3(Center.x + outRadius * (float)cos(angle),
					this->Center.y, Center.z + outRadius * (float)sin(angle));
				result.Add(point);
			}

			return result;
		}
	};

	class Rectangle
	{
	public:
		Vector3 Direction;
		Vector3 Perpendicular;
		Vector3 REnd;
		Vector3 RStart;
		float Width;

		Rectangle(const Vector3& start, const Vector3& end, float widthStart)
		{
			RStart = start;
			REnd = end;
			Width = widthStart;
			Direction = (end - start).normalize();
			Perpendicular = Direction.Perpendicular();
		}

		Polygon ToPolygon(int offset = 0, float overrideWidth = -1)
		{
			Polygon result = Polygon();

			result.Add(RStart +
				Perpendicular * (overrideWidth > 0 ? overrideWidth : Width + offset) -
				Direction * offset);
			result.Add(RStart -
				Perpendicular * (overrideWidth > 0 ? overrideWidth : Width + offset) -
				Direction * offset);
			result.Add(REnd -
				Perpendicular * (overrideWidth > 0 ? overrideWidth : Width + offset) +
				Direction * offset);
			result.Add(REnd +
				Perpendicular * (overrideWidth > 0 ? overrideWidth : Width + offset) +
				Direction * offset);

			return result;
		}
	};

	class Ring
	{
	public:
		Vector3 Center;
		float Radius;
		float RingRadius;

		Ring(const Vector3& center, float radius, float ringRadius)
		{
			Center = center;
			Radius = radius;
			RingRadius = ringRadius;
		}

		Polygon ToPolygon(int offset = 0)
		{
			Polygon result;

			float outRadius = (float)(offset + this->Radius + this->RingRadius) / cosf(2 * M_PI / CircleLineSegmentN);
			float innerRadius = this->Radius - this->RingRadius - offset;

			for (int i = 0; i <= CircleLineSegmentN; i++)
			{
				float angle = i * 2 * M_PI / CircleLineSegmentN;
				auto point = Vector3(
					this->Center.x - outRadius * cosf(angle),
					this->Center.y, this->Center.z - outRadius * sinf(angle));

				result.Add(point);
			}
			for (int i = 0; i <= CircleLineSegmentN; i++)
			{
				float angle = i * 2 * M_PI / CircleLineSegmentN;
				auto point = Vector3(
					this->Center.x - innerRadius * cosf(angle),
					this->Center.y, this->Center.z - innerRadius * sinf(angle));

				result.Add(point);
			}
			return result;
		}
	};

	class Sector
	{
	public:
		float Angle;
		Vector3 Center;
		Vector3 Direction;
		float Radius;

		Sector(const Vector3& center, const Vector3& direction, float angle, float radius)
		{
			Center = center;
			Direction = direction;
			Angle = angle;
			Radius = radius;
		}

		Polygon ToPolygon(int offset = 0)
		{
			Polygon result = Polygon();
			float outRadius = (Radius + offset) / (float)cos(2 * IM_PI / CircleLineSegmentN);

			result.Add(Center);
			Vector3 side1 = Direction.Rotated(-Angle * 0.5f);

			for (int i = 0; i <= CircleLineSegmentN; i++)
			{
				Vector3 cDirection = side1.Rotated(i * Angle / CircleLineSegmentN).normalize();
				result.Add(Vector3(Center.x + outRadius * cDirection.x,
					Center.y, Center.z + outRadius * cDirection.z));
			}

			return result;
		}
	};

	class Geometry
	{
	public:

		static std::vector<std::vector<ClipperLib::IntPoint>> ClipPolygons(std::vector<Polygon> polygons)
		{
			std::vector<std::vector<ClipperLib::IntPoint>> subj(polygons.size());
			std::vector<std::vector<ClipperLib::IntPoint>> clip(polygons.size());

			for (Polygon polygon : polygons)
			{
				subj.push_back(polygon.ToClipperPath());
				clip.push_back(polygon.ToClipperPath());
			}

			auto solution = std::vector<std::vector<ClipperLib::IntPoint>>();
			ClipperLib::Clipper c;
			c.AddPaths(subj, ClipperLib::PolyType::ptSubject, true);
			c.AddPaths(clip, ClipperLib::PolyType::ptClip, true);
			c.Execute(ClipperLib::ClipType::ctUnion, solution, ClipperLib::PolyFillType::pftNegative, ClipperLib::PolyFillType::pftNonZero);

			return solution;
		}

		static std::vector<Vector3> CutPath(std::vector<Vector3> path, float distanceTo)
		{
			std::vector<Vector3> result;

			if (distanceTo < 0)
			{
				path[0] = path[0] + (path[1] - path[0]).normalize() * distanceTo;
				return path;
			}

			for (auto i = 0; i < path.size() - 1; i++)
			{
				auto dist = path[i].distance(path[i + 1]);
				if (dist > distanceTo)
				{
					result.push_back(path[i] + (path[i + 1] - path[i]).normalize() * distanceTo);
					for (auto j = i + 1; j < path.size(); j++)
					{
						result.push_back(path[j]);
					}
					break;
				}
				distanceTo -= dist;
			}
			return result.size() > 0 ? result : std::vector<Vector3>{ path.back() };
		}

		static float PathLength(std::vector<Vector3> path)
		{
			float distanceTo = 0.0f;
			for (int i = 0; i < path.size() - 1; i++)
			{
				distanceTo += path[i].distance(path[i + 1]);
			}
			return distanceTo;
		}

		static Vector3 Vector3MovementCollision(const Vector3& startPoint1, const Vector3& endPoint1, float v1, const Vector3& startPoint2, float v2, float& t1, float delay = 0.f)
		{
			auto sP1x = startPoint1.x;
			auto sP1y = startPoint1.z;
			auto eP1x = endPoint1.x;
			auto eP1y = endPoint1.z;
			auto sP2x = startPoint2.x;
			auto sP2y = startPoint2.z;

			float d = eP1x - sP1x, e = eP1y - sP1y;
			float dist = (float)sqrt(d * d + e * e);
			t1 = NAN;
			float S = abs(dist) > FLT_EPSILON ? v1 * d / dist : 0.f,
				K = (abs(dist) > FLT_EPSILON) ? v1 * e / dist : 0.f;

			float r = sP2x - sP1x, j = sP2y - sP1y;
			auto c = r * r + j * j;

			if (dist > 0.f)
			{
				if (abs(v1 - FLT_MAX) < FLT_EPSILON)
				{
					auto t = dist / v1;
					t1 = v2 * t >= 0.f ? t : NAN;
				}
				else if (abs(v2 - FLT_MAX) < FLT_EPSILON)
				{
					t1 = 0.f;
				}
				else
				{
					float a = S * S + K * K - v2 * v2, b = -r * S - j * K;

					if (abs(a) < FLT_EPSILON)
					{
						if (abs(b) < FLT_EPSILON)
						{
							t1 = (abs(c) < FLT_EPSILON) ? 0.f : NAN;
						}
						else
						{
							auto t = -c / (2 * b);
							t1 = (v2 * t >= 0.f) ? t : NAN;
						}
					}
					else
					{
						auto sqr = b * b - a * c;
						if (sqr >= 0)
						{
							auto nom = (float)sqrt(sqr);
							auto t = (-nom - b) / a;
							t1 = v2 * t >= 0.f ? t : NAN;
							t = (nom - b) / a;
							auto t2 = (v2 * t >= 0.f) ? t : NAN;

							if (!isnan(t2) && !isnan(t1))
							{
								if (t1 >= delay && t2 >= delay)
								{
									t1 = fmin(t1, t2);
								}
								else if (t2 >= delay)
								{
									t1 = t2;
								}
							}
						}
					}
				}
			}
			else if (abs(dist) < FLT_EPSILON)
			{
				t1 = 0.f;
			}

			return (!isnan(t1)) ? Vector3(sP1x + S * t1, 0, sP1y + K * t1) : Vector3(0, 0, 0);
		}

		static Vector3 PositionAfter(std::vector<Vector3> self, int t, int speed, int delay = 0)
		{
			int distanceTo = fmax(0, t - delay) * speed / 1000;

			for (int i = 0; i <= self.size() - 2; i++)
			{
				Vector3 from = self[i];
				Vector3 to = self[i + 1];
				int d = (int)to.distance(from);
				if (d > distanceTo)
				{
					return from + (to - from).normalize() * distanceTo;
				}
				distanceTo -= d;
			}

			return self[self.size() - 1];
		}

		static Polygon ToPolygon(std::vector<ClipperLib::IntPoint> v)
		{
			Polygon polygon = Polygon();
			for (ClipperLib::IntPoint& point : v)
				polygon.Add(Vector3(point.X, 0, point.Y));
			return polygon;
		}

		static std::vector<Polygon> ToPolygons(std::vector<std::vector<ClipperLib::IntPoint>> v)
		{
			std::vector<Polygon> result;
			for (auto item : v)
				result.push_back(ToPolygon(item));
			return result;
		}
	};
}