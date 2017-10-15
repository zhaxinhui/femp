#ifndef FEMP_HEXAHEDRON_FAMILY_HPP
#define FEMP_HEXAHEDRON_FAMILY_HPP


#include "BaseElement.h++"


namespace fem
{

class HexahedronFamily
	: public BaseElement
{
public:
	HexahedronFamily();

	enum BaseElement::ElementFamily family() const;

	/**
	 * Returns the total number of degrees of freedom
	 **/
	virtual unsigned int getDofAmount() const	{ return 3*this->getNodeAmount(); }

protected:
	void generateQuadratureData();
};


}

#endif
