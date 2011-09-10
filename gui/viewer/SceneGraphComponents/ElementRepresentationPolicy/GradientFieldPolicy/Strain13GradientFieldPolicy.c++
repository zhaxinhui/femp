#include "Strain13GradientFieldPolicy.h++"

#include "../../SceneGraphException.h++"

#include "../../../../fem/point.h++"


Strain13GradientFieldPolicy::Strain13GradientFieldPolicy()
{
}


void 
Strain13GradientFieldPolicy::calculateGradientValues (fem::Element &reference_element)
{
	assert(this->m_model != NULL);
	assert(this->m_analysis_result != NULL);

	m_gradient_value.clear();
	fem::BaseElement<double> *element;

	switch(reference_element.type)
	{
		case fem::Element::FE_TETRAHEDRON4:
			element = &m_tetrahedron4;
			m_gradient_value.resize(4);
			break;

		case fem::Element::FE_TETRAHEDRON10:
			element = &m_tetrahedron10;
			m_gradient_value.resize(10);
			break;

		case fem::Element::FE_HEXAHEDRON8:
			element = &m_hexahedron8;
			m_gradient_value.resize(8);
			break;

		case fem::Element::FE_HEXAHEDRON20:
			element = &m_hexahedron20;
			m_gradient_value.resize(20);
			break;

		case fem::Element::FE_HEXAHEDRON27:
			element = &m_hexahedron27;
			m_gradient_value.resize(27);
			break;

		case fem::Element::FE_PRISM6:
			element = &m_prism6;
			m_gradient_value.resize(6);
			break;

		case fem::Element::FE_PRISM15:
			element = &m_prism15;
			m_gradient_value.resize(15);
			break;

		case fem::Element::FE_PRISM18:
			element = &m_prism18;
			m_gradient_value.resize(18);
			break;

		default:
			throw SceneGraphException("unknown element type request");
			return;
			break;
	}

	// calculate
	element->setCoordinates();

	fem::point dxdcsi, dxdeta, dxdzeta;
	
	// cycle to calculate a gradient value for each node
	for(unsigned int coord = 0; coord < element->coordinates.size(); coord++)
	{
		fem::point local = element->coordinates[coord];	// point in element in \xi, the local coordinates
		fem::point global;	// node coordinate in x, the global coordinates
		
		element->setdNdcsi(local);
		element->setdNdeta(local);
		element->setdNdzeta(local);

		dxdcsi.zero(); dxdeta.zero(); dxdzeta.zero();

		// set Dg
		Dg.setZero();
		for(unsigned int node = 0; node < element->coordinates.size(); node++)
		{
			// get the node's x coordinate, the coordinate in the global frame of reference
			global = m_model->getNode(reference_element.nodes[node]);

			dxdcsi += element->dNdcsi[node]*global;
			dxdeta += element->dNdeta[node]*global;
			dxdzeta += element->dNdzeta[node]*global;
		}

		Dg(0,0) = dxdcsi.x();	Dg(0,1) = dxdcsi.y();	Dg(0,2) = dxdcsi.z();
		Dg(1,0) = dxdeta.x();	Dg(1,1) = dxdeta.y();	Dg(1,2) = dxdeta.z();
		Dg(2,0) = dxdzeta.x();	Dg(2,1) = dxdzeta.y();	Dg(2,2) = dxdzeta.z();

		Dg.computeInverse(&invDg);

		m_gradient_value[coord] = 0;

		// set \epsilon_13
		float dNdx = 0;
		//float dNdy = 0;
		float dNdz = 0;
		for(unsigned int node = 0; node < element->coordinates.size(); node++)
		{
			fem::point d; // displacements
			d = this->m_analysis_result->displacements[ reference_element.nodes[node] ];	// displacements calculated in this node

			// calculate \epsilon_{13} = dNdx_1*d1
			dNdx  = invDg(0,0)*element->dNdcsi[node] + invDg(0,1)*element->dNdeta[node] + invDg(0,2)*element->dNdzeta[node];
			//dNdy  = invDg(1,0)*element->dNdcsi[node] + invDg(1,1)*element->dNdeta[node] + invDg(1,2)*element->dNdzeta[node];
			dNdz  = invDg(2,0)*element->dNdcsi[node] + invDg(2,1)*element->dNdeta[node] + invDg(2,2)*element->dNdzeta[node];
			// m_gradient_value[coord] += (invDg(0,0)*element->dNdcsi[node] + invDg(1,0)*element->dNdeta[node] + invDg(2,0)*element->dNdzeta[node])*d.x();
			m_gradient_value[coord] += dNdx*d.z() + dNdz*d.x(); // e_13 = du_1/dx_3 + du_3/dx_1 
		}

		// adjust max and min values
		if(m_gradient_value[coord] > m_max_value)
			m_max_value = m_gradient_value[coord];
		if(m_gradient_value[coord] < m_min_value)
			m_min_value = m_gradient_value[coord];
	}

	/*
	std::cout << "Strain 13: [\t";
	for( std::vector<float>::iterator i = m_gradient_value.begin();	i != m_gradient_value.end(); i++)// gradient value on each node
	{
		std::cout << *i << "\t";
	}
	std::cout << "]" << std::endl;
	*/
}

