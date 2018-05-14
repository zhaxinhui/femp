#ifndef FEMP_TRIANGLE10_HPP
#define FEMP_TRIANGLE10_HPP

#include <vector>

#include "TriangleFamily.h++"
#include "../Point3D.h++"



namespace fem
{

class Triangle10
		: public TriangleFamily
{
public:
	Triangle10();

	std::vector<fem::Point3D> getLocalCoordinates();

	std::vector<double> getN(const Point3D & p);

	std::vector<double> getdNdcsi(const Point3D &p);

	std::vector<double> getdNdeta(const Point3D &p);

	std::vector<double> getdNdzeta(const Point3D &p);

	/**
	return the number of nodes that an element of this particular type has
	@return the number of nodes
	**/
	virtual int getNodeAmount() const override { return 10; }
};


}	// namespace fem

#endif