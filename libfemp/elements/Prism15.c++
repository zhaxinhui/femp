#include "Prism15.h++"


#include <libfemp/FemException.h++>


namespace fem
{


Prism15::Prism15()
	: PrismFamily()
{
	this->type = BaseElement::FE_PRISM15;
	this->stiffness_degree = 5;
	this->domain_degree = 2;

	this->coordinates.resize(15);
}


std::vector<double>
Prism15::getN(const Point &p)
{
	const double csi = p.x();
	const double eta = p.y();
	const double zeta = p.z();

	std::vector<double> N(this->node_number());

	N[0] = -((-1 + csi + eta)*(-1 + zeta)*(2*(csi + eta) + zeta))/2.0;
	N[1] =  (csi*(-1 + zeta)*(2 - 2*csi + zeta))/2.0;
	N[2] =  (eta*(-1 + zeta)*(2 - 2*eta + zeta))/2.0;
	N[3] =  -((-1 + csi + eta)*(1 + zeta)*(-2*(csi + eta) + zeta))/2.0;
	N[4] =  (csi*(1 + zeta)*(-2 + 2*csi + zeta))/2.0;
	N[5] =  (eta*(1 + zeta)*(-2 + 2*eta + zeta))/2.0;
	N[6] =  2*csi*(-1 + csi + eta)*(-1 + zeta);
	N[7] = 2*eta*(-1 + csi + eta)*(-1 + zeta);
	N[8] =  (-1 + csi + eta)*(-1 + zeta*zeta);
	N[9] = -2*csi*eta*(-1 + zeta);
	N[10] =  csi - csi*zeta*zeta;
	N[11] = eta - eta*zeta*zeta;
	N[12] =  -2*csi*(-1 + csi + eta)*(1 + zeta);
	N[13] = -2*eta*(-1 + csi + eta)*(1 + zeta);
	N[14] =  2*csi*eta*(1 + zeta);
	
	return N;
}


std::vector<double>
Prism15::getdNdcsi(const Point &p)
{
	double csi = p.x();
	double eta = p.y();
	double zeta = p.z();

	std::vector<double> dNdcsi(this->node_number());

	dNdcsi[0] = -(zeta-1)*(zeta+2*(eta+csi))/2-(eta+csi-1)*(zeta-1);
	dNdcsi[1] = (zeta-1)*(zeta-2*csi+2)/2-csi*(zeta-1);
	dNdcsi[2] = 0;
	dNdcsi[3] = (eta+csi-1)*(zeta+1)-(zeta+1)*(zeta-2*(eta+csi))/2;
	dNdcsi[4] = (zeta+1)*(zeta+2*csi-2)/2+csi*(zeta+1);
	dNdcsi[5] = 0;
	dNdcsi[6] =  2*(eta+csi-1)*(zeta-1)+2*csi*(zeta-1);
	dNdcsi[7] = 2*eta*(zeta-1);
	dNdcsi[8] = zeta*zeta-1;
	dNdcsi[9] = -2*eta*(zeta-1);
	dNdcsi[10] = 1-zeta*zeta;
	dNdcsi[11] = 0;
	dNdcsi[12] = -2*(eta+csi-1)*(zeta+1)-2*csi*(zeta+1);
	dNdcsi[13] = -2*eta*(zeta+1);
	dNdcsi[14] = 2*eta*(zeta+1);

	return dNdcsi;
}


std::vector<double>
Prism15::getdNdeta(const Point &p)
{
	double csi = p.x();
	double eta = p.y();
	double zeta = p.z();

	std::vector<double> dNdeta(this->node_number());

	dNdeta[0] = -(zeta-1)*(zeta+2*(eta+csi))/2-(eta+csi-1)*(zeta-1);
	dNdeta[1] = 0;
	dNdeta[2] = (zeta-1)*(zeta-2*eta+2)/2-eta*(zeta-1);
	dNdeta[3] = (eta+csi-1)*(zeta+1)-(zeta+1)*(zeta-2*(eta+csi))/2;
	dNdeta[4] = 0;
	dNdeta[5] = (zeta+1)*(zeta+2*eta-2)/2+eta*(zeta+1);
	dNdeta[6] =  2*csi*(zeta-1);
	dNdeta[7] = 2*(eta+csi-1)*(zeta-1)+2*eta*(zeta-1);
	dNdeta[8] = zeta*zeta-1;
	dNdeta[9] = -2*csi*(zeta-1);
	dNdeta[10] = 0;
	dNdeta[11] = 1-zeta*zeta;
	dNdeta[12] = -2*csi*(zeta+1);
	dNdeta[13] = -2*(eta+csi-1)*(zeta+1)-2*eta*(zeta+1);
	dNdeta[14] = 2*csi*(zeta+1);
	
	return dNdeta;
}


std::vector<double>
Prism15::getdNdzeta(const Point &p)
{
	double csi = p.x();
	double eta = p.y();
	double zeta = p.z();

	std::vector<double> dNdzeta(this->node_number());

	dNdzeta[0] = -(eta+csi-1)*(zeta+2*(eta+csi))/2-(eta+csi-1)*(zeta-1)/2;
	dNdzeta[1] = csi*(zeta-2*csi+2)/2+csi*(zeta-1)/2;
	dNdzeta[2] = eta*(zeta-2*eta+2)/2+eta*(zeta-1)/2;
	dNdzeta[3] = -(eta+csi-1)*(zeta-2*(eta+csi))/2-(eta+csi-1)*(zeta+1)/2;
	dNdzeta[4] =  csi*(zeta+2*csi-2)/2+csi*(zeta+1)/2;
	dNdzeta[5] = eta*(zeta+2*eta-2)/2+eta*(zeta+1)/2;
	dNdzeta[6] = 2*csi*(eta+csi-1);
	dNdzeta[7] = 2*eta*(eta+csi-1);
	dNdzeta[8] = 2*(eta+csi-1)*zeta;
	dNdzeta[9] = -2*csi*eta;
	dNdzeta[10] = -2*csi*zeta;
	dNdzeta[11] = -2*eta*zeta;
	dNdzeta[12] = -2*csi*(eta+csi-1);
	dNdzeta[13] =  -2*eta*(eta+csi-1);
	dNdzeta[14] = 2*csi*eta;

	return dNdzeta;
}

std::vector<fem::Point> & Prism15::setCoordinates()
{
	this->coordinates[0] = Point(	0,	0,	-1	);
	this->coordinates[1] = Point(	1,	0,	-1	);
	this->coordinates[2] = Point(	0,	1,	-1	);
	this->coordinates[3] = Point(	0,	0,	1	);
	this->coordinates[4] = Point(	1,	0,	1	);
	this->coordinates[5] = Point(	0,	1,	1	);

	this->coordinates[6] = Point(	0.5,	0,	-1	);
	this->coordinates[7] = Point(	0,	0.5,	-1	);
	this->coordinates[8] = Point(	0,	0,	0	);
	this->coordinates[9] = Point(	0.5,	0.5,	-1	);
	this->coordinates[10] = Point(	1,	0,	0	);
	this->coordinates[11] = Point(	0,	1,	0	);
	this->coordinates[12] = Point(	0.5,	0,	1	);
	this->coordinates[13] = Point(	0,	0.5,	1	);
	this->coordinates[14] = Point(	0.5,	0.5,	1	);

	return this->coordinates;
}


void
Prism15::get(std::vector<size_t> &nodes)
{
	if(nodes.size() == 15)
	{
		throw FemException("wrong number of nodes");
	}
	this->nodes = nodes;
}


}	// namespace fem

