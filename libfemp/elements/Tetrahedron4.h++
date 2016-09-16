#ifndef FEMP_TETRAHEDRON4_HPP
#define FEMP_TETRAHEDRON4_HPP

#include <vector>

#include "TetrahedronFamily.h++"
#include "../Point.h++"




namespace fem
{

template <typename T>
struct Tetrahedron4
	: public TetrahedronFamily<T>
{
	public:
		Tetrahedron4();
		~Tetrahedron4()	{};

		std::vector<fem::Point> & setCoordinates();

		std::vector<T> & setN(const Point & p);
		std::vector<T> & setN(const T &csi, const T &eta, const T &zeta = 0);

		std::vector<T> & setdNdcsi(const Point &p);
		std::vector<T> & setdNdcsi(const T &csi, const T &eta, const T &zeta = 0);

		std::vector<T> & setdNdeta(const Point &p);
		std::vector<T> & setdNdeta(const T &csi, const T &eta, const T &zeta = 0);

		std::vector<T> & setdNdzeta(const Point &p);
		std::vector<T> & setdNdzeta(const T &csi, const T &eta, const T &zeta = 0);

public: // merging with fem::Element
        void set(std::vector<size_t> &nodes);

        /** 
        return the number of nodes that an element of this particular type has
        @return the number of nodes
        **/
        int node_number() const	{ return 4; };
};


template<typename T>
Tetrahedron4<T>::Tetrahedron4()
{
	this->type = BaseElement<T>::FE_TETRAHEDRON4;
	this->stiffness_degree = 1;
	this->domain_degree = 1;

	this->coordinates.resize(4);
	this->N.resize(4);
	this->dNdcsi.resize(4);
	this->dNdeta.resize(4);
	this->dNdzeta.resize(4);
}


template<typename T>
std::vector<T> & Tetrahedron4<T>::setN(const Point &p)
{
	return this->setN(p.data[0], p.data[1], p.data[2]);
}


template<typename T>
std::vector<T> & Tetrahedron4<T>::setN(const T &csi, const T &eta, const T &zeta)
{
	//TODO finish this
	this->N[0] = 1.0-zeta-eta-csi;
	this->N[1] = csi;
	this->N[2] = eta;
	this->N[3] = zeta;
	
	return this->N;
}


template<typename T>
std::vector<T> & Tetrahedron4<T>::setdNdcsi(const Point &p)
{
	return this->setdNdcsi(p.data[0], p.data[1], p.data[2]);
}


template<typename T>
std::vector<T> & Tetrahedron4<T>::setdNdcsi(const T &, const T &, const T &)
{
	//TODO finish this
	// this->dNdcsi
	this->dNdcsi[0] = -1.0;
	this->dNdcsi[1] = 1;
	this->dNdcsi[2] = 0;
	this->dNdcsi[3] = 0;

	return this->dNdcsi;
}


template<typename T>
std::vector<T> & Tetrahedron4<T>::setdNdeta(const Point &p)
{
	return this->setdNdeta(p.data[0], p.data[1], p.data[2]);
}


template<typename T>
std::vector<T> & Tetrahedron4<T>::setdNdeta(const T &, const T &, const T &)
{
	//TODO finish this
	// this->dNdeta
	this->dNdeta[0] = -1.0;
	this->dNdeta[1] = 0;
	this->dNdeta[2] = 1;
	this->dNdeta[3] = 0;
	
	return this->dNdeta;
}


template<typename T>
std::vector<T> & Tetrahedron4<T>::setdNdzeta(const Point &p)
{
	return this->setdNdzeta(p.data[0], p.data[1], p.data[2]);
}


template<typename T>
std::vector<T> & Tetrahedron4<T>::setdNdzeta(const T &, const T &, const T &)
{
	//TODO finish this
	// this->dNdzeta
	this->dNdzeta[0] = -1.0;
	this->dNdzeta[1] = 0;
	this->dNdzeta[2] = 0;
	this->dNdzeta[3] = 1;

	return this->dNdzeta;
}


template<typename T>
std::vector<fem::Point> & Tetrahedron4<T>::setCoordinates()
{
	this->coordinates[0] = Point(	0,	0,	0	);
	this->coordinates[1] = Point(	1,	0,	0	);
	this->coordinates[2] = Point(	0,	1,	0	);
	this->coordinates[3] = Point(	0,	0,	1	);

	return this->coordinates;
}


template<typename T>
void
Tetrahedron4<T>::set(std::vector<size_t> &nodes)
{
	if(nodes.size() == 4)
	{
		throw FemException("wrong number of nodes");
	}
	this->nodes = nodes;
}


}

#endif
