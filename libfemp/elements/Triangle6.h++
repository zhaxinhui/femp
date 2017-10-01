#ifndef FEMP_TRIANGLE6_HPP
#define FEMP_TRIANGLE6_HPP

#include <vector>

#include "TriangleFamily.h++"
#include "../Point.h++"



namespace fem
{

struct Triangle6
	: public TriangleFamily
{
	public:
		Triangle6();
		~Triangle6()	{};

		std::vector<fem::Point> & setCoordinates();

		std::vector<double> & setN(const Point & p);

		std::vector<double> & setdNdcsi(const Point &p);
		std::vector<double> & setdNdcsi(const double &csi, const double &eta, const double &zeta = 0);

		std::vector<double> & setdNdeta(const Point &p);
		std::vector<double> & setdNdeta(const double &csi, const double &eta, const double &zeta = 0);

		std::vector<double> & setdNdzeta(const Point &p);
		std::vector<double> & setdNdzeta(const double &csi, const double &eta, const double &zeta = 0);

public: // merging with fem::Element
	void set(std::vector<size_t> &nodes);

	/** 
	return the number of nodes that an element of this particular type has
	@return the number of nodes
	**/
	int node_number() const	{ return 6; };
};


}

#endif
