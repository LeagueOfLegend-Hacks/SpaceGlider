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
			Direction = (end - start).normalize();
			Width = widthStart;
			Perpendicular = Direction.Perpendicular();
			RStart = start;
			REnd = end;
		}

		Polygon ToPolygon(int offset = 0, float overrideWidth = -1)
		{
			Polygon result = Polygon();

			result.Add((RStart + Perpendicular * Width) + Perpendicular * (overrideWidth > 0 ? overrideWidth : Width + offset) + Direction * offset);
			result.Add(RStart - Perpendicular * (overrideWidth > 0 ? overrideWidth : Width + offset) - Direction * offset);
			result.Add(REnd - Perpendicular * (overrideWidth > 0 ? overrideWidth : Width + offset) - Direction * offset);
			result.Add((REnd + Perpendicular * Width) + Perpendicular * (overrideWidth > 0 ? overrideWidth : Width + offset) + Direction * offset);

			return result;
		}
	};

	class Geometry
	{
	public:
		static Polygon ToPolygon(std::vector<ClipperLib::IntPoint> v)
		{
			Polygon polygon = Polygon();
			for (ClipperLib::IntPoint& point : v)
				polygon.Add(Vector3(point.X, point.Y, 0));
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