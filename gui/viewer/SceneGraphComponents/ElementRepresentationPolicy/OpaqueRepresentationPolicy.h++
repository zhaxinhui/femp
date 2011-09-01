
#ifndef OPAQUEREPRESENTATIONPOLICY_H
#define OPAQUEREPRESENTATIONPOLICY_H

#include "ElementRepresentationPolicy.h++"
#include "../../../fem/point.h++"
#include "../../../fem/Element.h++"
#include "../../ViewportColors.h++"


class OpaqueRepresentationPolicy 
	: virtual public ElementRepresentationPolicy
{
protected:
	std::vector<fem::point> m_temp_p;

public:

	// Constructors/Destructors
	//  


	/**
	 * Empty Constructor
	 */
	OpaqueRepresentationPolicy ( );

	/**
	 * Empty Destructor
	 */
	virtual ~OpaqueRepresentationPolicy ( );


protected:
	inline void tri3 (fem::point &p1, fem::point &p2, fem::point &p3, ViewportColors &color);
	inline void tri6 (fem::point &p1, fem::point &p2, fem::point &p3, fem::point &p4, fem::point &p5, fem::point &p6, ViewportColors &color);
	inline void quad4(fem::point &p1, fem::point &p2, fem::point &p3, fem::point &p4, ViewportColors &color);
	inline void quad8(fem::point &p1, fem::point &p2, fem::point &p3, fem::point &p4, fem::point &p5, fem::point &p6, fem::point &p7, fem::point &p8, ViewportColors &color);
	inline void quad9(fem::point &p1, fem::point &p2, fem::point &p3, fem::point &p4, fem::point &p5, fem::point &p6, fem::point &p7, fem::point &p8, fem::point &p9, ViewportColors &color);

public:
	/**
	 */
	void tetra4 (fem::Element &element, ViewportColors &color, DisplacementsRepresentationPolicy *displacement);

	/**
	 */
	void tetra10 (fem::Element &element, ViewportColors &color, DisplacementsRepresentationPolicy *displacement); 

	/**
	 */
	void hexa8 (fem::Element &element, ViewportColors &color, DisplacementsRepresentationPolicy *displacement);

	/**
	 */
	void hexa20 (fem::Element &element, ViewportColors &color, DisplacementsRepresentationPolicy *displacement);

	/**
	 */
	void hexa27 (fem::Element &element, ViewportColors &color, DisplacementsRepresentationPolicy *displacement);

	/**
	 */
	void prism6 (fem::Element &element, ViewportColors &color, DisplacementsRepresentationPolicy *displacement);

	/**
	 */
	void prism15 (fem::Element &element, ViewportColors &color, DisplacementsRepresentationPolicy *displacement);

	/**
	 */
	void prism18 (fem::Element &element, ViewportColors &color, DisplacementsRepresentationPolicy *displacement);


};

#endif // OPAQUEREPRESENTATIONPOLICY_H