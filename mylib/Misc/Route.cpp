#include "stdafx.h"
#include "Route.h"
#include "fstream"

RouteMgr::RouteMgr()
{

}

bool RouteMgr::load(const char* filename)
{
	std::ifstream inf(filename);
	if (!inf.is_open())
		return false;

	unsigned long magic;
	inf >> magic;
	if (magic != ROUTE_MAGIC)
		return false;

	size_t routeCount;
	inf >> routeCount;

	for (size_t k = 0; k < routeCount && !inf.eof(); k ++) {

		Route route;
		inf >> route.id;
		inf >> route.name;

		unsigned short count;
		inf >> count;
		for (unsigned short i = 0; i < count; i ++ ) {
			RoutePoint3D pt;
			inf >> pt.x;
			inf >> pt.y;
			inf >> pt.z;
			inf >> pt.flag;
			route.points.push_back(pt);
		}

		_routes[route.id] = route;
	}

	return true;
}

bool RouteMgr::save(const char* filename)
{
	std::ofstream outf(filename);

	if (!outf.is_open())
		return false;

	outf << ROUTE_MAGIC;
	outf << _routes.size();
	for (Routes::iterator it = _routes.begin(); it != _routes.end(); it ++) {

		Route& route = it->second;

		outf << route.id;
		outf << route.name;

		RoutePoints& points = route.points;
		outf << (unsigned short )points.size();

		for (size_t i = 0; i < points.sie(); i ++ ) {

			RoutePoint3D& pt = points[i];

			outf << pt.x;
			outf << pt.y;
			outf << pt.z;
			outf << pt.flag;
		}
	}

	return true;
}

Route* RouteMgr::getWay(const Point3D& beginPt, const Point3D& endPt)
{
	return NULL;
}
