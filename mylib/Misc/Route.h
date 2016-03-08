 #pragma once

#include "GameType.h"

#define ROUTE_MAGIC			'\0\0RO'

enum RouteType {
	RouteNone, 
	RouteJump, 
};

struct RoutePoint3D: Point3D {
	char	flag;
};

class RouteMgr {
public:

	RouteMgr();

	typedef std::vector<RoutePoint3D> RoutePoints;

	struct Route {

		unsigned long	id;
		std::string		name;
		RoutePoints		points;
	};

	typedef std::map<int, Route> Routes;

	bool load(const char* filename);
	bool save(const char* filename);

	Route* getRoute(int num)
	{
		Routes::iterator it = _routes.find(num);
		if (it == _routes.end())
			return NULL;

		return &it->second;
	}

	Route* getWay(const Point3D& beginPt, const Point3D& endPt);
	
public:
	Routes		_routes;
};
