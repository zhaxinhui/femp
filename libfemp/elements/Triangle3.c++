#include "Triangle3.h++"

#include <libfemp/FemException.h++>


namespace fem
{


Triangle3::Triangle3()
{
	this->stiffness_degree = 1;
	this->domain_degree = 1;

	this->coordinates.resize(3);
}


std::vector<double>
Triangle3::getN(const Point &p)
{
	const double csi = p.x();
	const double eta = p.y();

	std::vector<double> N(this->node_number());

	N[0] = 1-csi-eta;
	N[1] = csi;
	N[2] = eta;
	
	return N;
}


std::vector<double>
Triangle3::getdNdcsi(const Point &)
{
	std::vector<double> dNdcsi(this->node_number());

	dNdcsi[0] = -1;
	dNdcsi[1] = 1;
	dNdcsi[2] = 0;

	return dNdcsi;
}


std::vector<double>
Triangle3::getdNdeta(const Point &)
{
	std::vector<double> dNdeta(this->node_number());

	dNdeta[0] = -1;
	dNdeta[1] = 0;
	dNdeta[2] = 1;
	
	return dNdeta;
}


std::vector<double>
Triangle3::getdNdzeta(const Point &)
{
	std::vector<double> dNdzeta(this->node_number());

	dNdzeta[0] = 0;
	dNdzeta[1] = 0;
	dNdzeta[2] = 0;

	return dNdzeta;
}


std::vector<fem::Point> & 
Triangle3::setCoordinates()
{
	this->coordinates[0] = Point(	0,	0,	0	);
	this->coordinates[1] = Point(	1,	0,	0	);
	this->coordinates[2] = Point(	0,	1,	0	);

	return this->coordinates;
}


void
Triangle3::get(std::vector<size_t> &nodes)
{
	if(nodes.size() == 3)
	{
		throw FemException("wrong number of nodes");
	}

	this->nodes = nodes;
}


}	// namespace fem
