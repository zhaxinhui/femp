#include "Model.h++"
#include <algorithm>


namespace fem
{

Model::Model()
{
	default_material = 0;
}


Model::~Model()
{
}


void Model::clear()
{
	node_list.clear();
	element_list.clear();
	material_list.clear();
	node_restrictions_list.clear();
	load_pattern_list.clear();
}


void Model::setNode(size_t pos, double x, double y, double z)
{
	Node n;
	n.set(x,y,z);
	this->node_list[pos] = n;
}


void Model::pushElement(fem::Element e)
{
	e.material = default_material;
	this->element_list.push_back(e);
}


void Model::pushElement(fem::Element::Type type, std::vector<size_t> nodes)
{
	fem::Element e;
	e.set(type,nodes);
	e.material = default_material;
	this->pushElement(e);
}


enum Model::Error Model::pushNodeRestrictions(size_t pos, fem::NodeRestrictions nr)
{
	// check if node is set
	if(node_list.find(pos) == node_list.end())
		return ERR_INVALID_NODE_REFERENCE;

	//TODO perform aditional error checking

	// push node restrictions object
	node_restrictions_list[pos] = nr;

	// everything went smoothly
	return ERR_OK;
}


enum Model::Error Model::pushLoadPattern(fem::LoadPattern lp)
{
	//TODO perform error checks

	load_pattern_list.push_back(lp);

	return ERR_OK;
}


enum Model::Error Model::build_fem_equation(struct FemEquation &f, const LoadPattern &lp)
{
	using namespace std;
	using namespace boost::numeric::ublas;

	// perform sanity checks on the model
	if(element_list.empty() )
		return ERR_NO_ELEMENTS;

	// at this point the model is considered to be OK.
	//TODO resize the global stiffness matrix and nodal forces vector
	f.f.clear();
	f.f.resize(node_list.size()*3);	// remove this after implementing a decent resize code
				
		// declare variables
	std::vector<boost::tuple<fem::point, double> > ipwpl;	// integration points/weights pair list
	double detJ = 0;
	matrix<double>	J(3,3), invJ(3,3);
	std::vector< symmetric_matrix<double, upper> > D_list;
	// set up the temporary variables for the elementary matrix and vector
	symmetric_matrix<double, upper> k_elem;
	mapped_vector<double> f_elem;
	matrix<double> B;
	matrix<double> Bt;
	boost::tuple< std::vector<double>, std::vector<double>, std::vector<double>,std::vector<double> > sf;	// tuple: sf, dNdcsi, dNdeta, dNdzeta
	int nnodes;	// number of nodes


	//build a list of constitutive matrices
	for(std::vector<Material>::iterator material = material_list.begin(); material != material_list.end(); material++)
	{
		D_list.push_back(material->generateD());
	}

	// generate stiffness matrix by cycling through all elements in the model
	for(std::vector<Element>::iterator element = element_list.begin(); element != element_list.end(); element++)
	{
		// get a list for the Gauss quadrature to be used by the current element
		ipwpl = integration_points(element->type);	

		// get the number of expected nodes used by the current element type
		nnodes = element->node_number();

		// resize the elementary matrices to fit the new node size
		k_elem.resize(nnodes*3,nnodes*3,false);
		f_elem.resize(nnodes*3,false);
		B.resize(6,3*nnodes,false);
		Bt.resize(3*nnodes,6,false);

		// initialize variables
		k_elem.clear();
		f_elem.clear();
		B.clear();

		// cycle through the number of integration points
		for (std::vector<boost::tuple<fem::point,double> >::iterator i = ipwpl.begin(); i != ipwpl.end(); i++)
		{
#define X(N) node_list[element->nodes[N]].x()
#define Y(N) node_list[element->nodes[N]].y()
#define Z(N) node_list[element->nodes[N]].z()
#define dNdcsi(N) sf.get<1>()[n]
#define dNdeta(N) sf.get<2>()[n]
#define dNdzeta(N) sf.get<3>()[n]

			// get the shape function and it's partial derivatives for this integration point
			sf = this->sf(element->type, i->get<0>());
			std::vector<double> test = sf.get<0>();

			// generate the jacobian
			J.clear();
			for(int n = 0; n < nnodes; n++)
			{
				J(0,0) += dNdcsi(n)*X(n);	J(0,1) += dNdcsi(n)*Y(n);	J(0,2) += dNdcsi(n)*Z(n);
				J(1,0) += dNdeta(n)*X(n);	J(1,1) += dNdeta(n)*Y(n);	J(1,2) += dNdeta(n)*Z(n);
				J(2,0) += dNdzeta(n)*X(n);	J(2,1) += dNdzeta(n)*Y(n);	J(2,2) += dNdzeta(n)*Z(n);
			}
			cout << J << endl;

			detJ = det3by3(J);
			invJ = invert3by3(J,detJ);

			// Set up the B matrix
			for(int n = 0; n < nnodes; n++)
			{
				// set the variables

				// set the partial derivatives
#undef X
#undef Y
#undef Z
#define dNdx invJ(0,0)*dNdcsi(n) + invJ(0,1)*dNdeta(n) + invJ(0,2)*dNdzeta(n)
#define dNdy invJ(1,0)*dNdcsi(n) + invJ(1,1)*dNdeta(n) + invJ(1,2)*dNdzeta(n)
#define dNdz invJ(2,0)*dNdcsi(n) + invJ(2,1)*dNdeta(n) + invJ(2,2)*dNdzeta(n)

				// set the current node portion of the B matrix
				B(0,3*n)	= dNdx;
				B(1,3*n+1)	= dNdy;
				B(2,3*n+2)	= dNdz;
				B(3,3*n)	= dNdy;	B(3,3*n+1) = dNdx;
				B(4,3*n)	= dNdz;	B(4,3*n+2) = dNdx;
				B(5,3*n+1)	= dNdz;	B(5,3*n+2) = dNdy;
#undef dNdx
#undef dNdy
#undef dNdz

#undef dNdcsi
#undef dNdeta
#undef dNdzeta
			}

			// having done the legwork, let's build up the elementary stiffness matrix and equivalent nodal force vector

			Bt = trans(B);
			symmetric_matrix<double> D = D_list[element->material];

			matrix<double> temp;
			// and now, k_elem = sum(Bt*D*B*detJ*i->second);
			temp = prod(Bt,D);
			temp = prod(temp,B);
			temp *= detJ*i->get<1>();
			k_elem += temp;	// adding up the full result


			// check if there is a domain load associated with this element
			// size_t d = distance(lp.domain_loads.begin(), element);

			// integrate the domain loads
			for(int n = 0; n < nnodes; n++)
			{
#define N(n) sf.get<0>()[n]

#undef N
			}
			std::cout << B << std::endl;
		}
		//output
		std::cout << k_elem << std::endl;
	}


	// now set up the equivalent forces vector

	// set nodal forces
	for(std::map<size_t,fem::NodalLoad>::const_iterator nodal_load = lp.nodal_loads.begin(); nodal_load != lp.nodal_loads.end(); nodal_load++)
	{
		//TODO must implement the scatter operation
		size_t n;
		n = nodal_load->first;

		// set the nodal loads
		f.f[3*n] = nodal_load->second.force.data[0];
		f.f[3*n+1] = nodal_load->second.force.data[1];
		f.f[3*n+2] = nodal_load->second.force.data[2];
	}

	// fem equation is set.
	return ERR_OK;
}


boost::numeric::ublas::matrix<double> Model::invert3by3(const boost::numeric::ublas::matrix<double> &M, double det)
{
	assert(M.size1() == 3);
	assert(M.size2() == 3);
	assert(det > 0);
	/*
(%i1) M: matrix([a,b,c],[d,e,f],[g,h,i]);
	   [ a  b  c ]
	   [         ]
(%o1)      [ d  e  f ]
	   [         ]
	   [ g  h  i ]

(%i2) N: invert(M), detout;
	   [ e i - f h  c h - b i  b f - c e ]
	   [                                 ]
	   [ f g - d i  a i - c g  c d - a f ]
	   [                                 ]
	   [ d h - e g  b g - a h  a e - b d ]
(%o2) ---------------------------------------------
      a (e i - f h) + b (f g - d i) + c (d h - e g)
	 */

	boost::numeric::ublas::matrix<double> invJ(3,3);
	invJ(0,0) = (M(1,1)*M(2,2) - M(1,2)*M(2,1))/det;
	invJ(0,1) = (M(0,2)*M(2,1) - M(0,1)*M(2,2))/det;
	invJ(0,2) = (M(0,1)*M(1,2) - M(0,2)*M(1,1))/det;
	invJ(1,0) = (M(1,2)*M(2,0) - M(1,0)*M(2,2))/det;
	invJ(1,1) = (M(0,0)*M(2,2) - M(0,2)*M(2,0))/det;
	invJ(1,2) = (M(0,2)*M(1,0) - M(0,0)*M(1,2))/det;
	invJ(2,0) = (M(1,0)*M(2,1) - M(1,1)*M(2,0))/det;
	invJ(2,1) = (M(0,1)*M(2,0) - M(0,0)*M(2,1))/det;
	invJ(2,2) = (M(0,0)*M(1,1) - M(0,1)*M(1,0))/det;

	return invJ;
}


double Model::det3by3(const boost::numeric::ublas::matrix<double> &M)
{
	assert(M.size1() == 3);
	assert(M.size2() == 3);
	return	M(0,0)*( M(1,1)*M(2,2) - M(1,2)*M(2,1)) + \
		M(0,1)*( M(1,2)*M(2,0) - M(1,0)*M(2,2)) + \
		M(0,2)*( M(1,0)*M(2,1) - M(1,1)*M(2,0));
}


enum Model::Error Model::run(const LoadPattern &lp)
{
	struct FemEquation f;
	//TODO finish this

	//this is a nasty hack to test the code. To be removed.
	build_fem_equation(f, lp);

	return ERR_OK;
}


void Model::gauleg(double x[], double w[], int n)
{
	int m,j,i;
	double z1,z,pp,p3,p2,p1;
	m=(n+1)/2; /* The roots are symmetric, so we only find half of them. */

	for (i=1;i<=m;i++) 
	{ /* Loop over the desired roots. */
		z=cos(3.141592654*(i-0.25)/(n+0.5));
		/* Starting with the above approximation to the ith root, we enter */
		/* the main loop of refinement by Newton's method.                 */
		do {
			p1=1.0;
			p2=0.0;
			for (j=1;j<=n;j++) { /* Recurrence to get Legendre polynomial. */
				p3=p2;
				p2=p1;
				p1=((2.0*j-1.0)*z*p2-(j-1.0)*p3)/j;
			}
			/* p1 is now the desired Legendre polynomial. We next compute */
			/* pp, its derivative, by a standard relation involving also  */
			/* p2, the polynomial of one lower order.                     */
			pp=n*(z*p1-p2)/(z*z-1.0);
			z1=z;
			z=z1-p1/pp; /* Newton's method. */
		} while (fabs(z-z1) > 3e-11);	// ARBITRATED
		x[i-1]=-z;      /* Scale the root to the desired interval, */
		x[n-i]=z;  /* and put in its symmetric counterpart.   */
		w[i-1]=2.0/((1.0-z*z)*pp*pp); /* Compute the weight             */
		w[n-i]=w[i-1];                 /* and its symmetric counterpart. */
	}
}


boost::tuple<std::vector<double>, std::vector<double>, std::vector<double>, std::vector<double> > Model::sf(const Element::Type type, const fem::point &point)
{
	using namespace boost;
	using namespace std;
	tuple<vector<double>, vector<double>, vector<double>, vector<double> > sfd;	// shape function derivatives tuple
	vector<double> sf, dNdcsi, dNdeta, dNdzeta;	// shape function derivatives weights 

	// declare a set of macros to make the code more readable
#define csi point.data[0]
#define eta point.data[1]
#define zeta point.data[2]

	// let's fill in the vectors
	switch(type)
	{
		case Element::FE_TETRAHEDRON4:
			sf.resize(4);
			sf[0] = 1-csi-eta-zeta;
			sf[1] = csi;
			sf[2] = eta;
			sf[3] = zeta;

			dNdcsi.resize(4);
			dNdcsi[0] = -1;
			dNdcsi[1] = 1;
			dNdcsi[2] = 0;
			dNdcsi[3] = 0;

			dNdeta.resize(4);
			dNdeta[0] = -1;
			dNdeta[1] = 0;
			dNdeta[2] = 1;
			dNdeta[3] = 0;

			dNdzeta.resize(4);
			dNdzeta[0] = -1;
			dNdzeta[1] = 0;
			dNdzeta[2] = 0;
			dNdzeta[3] = 1;
			break;

		case Element::FE_HEXAHEDRON8:
			sf.resize(8);
			sf[0] = (1-csi)*(1-eta)*(1-zeta)/8;
			sf[1] = (csi+1)*(1-eta)*(1-zeta)/8;
			sf[2] = (csi+1)*(eta+1)*(1-zeta)/8;
			sf[3] = (1-csi)*(eta+1)*(1-zeta)/8;
			sf[4] = (1-csi)*(1-eta)*(zeta+1)/8;
			sf[5] = (csi+1)*(1-eta)*(zeta+1)/8;
			sf[6] = (csi+1)*(eta+1)*(zeta+1)/8;
			sf[7] = (1-csi)*(eta+1)*(zeta+1)/8;

			// dNdcsi
			dNdcsi.resize(8);
			dNdcsi[ 0] = -(1-eta)*(1-zeta)/8;
			dNdcsi[ 1] = (1-eta)*(1-zeta)/8;
			dNdcsi[ 2] = (eta+1)*(1-zeta)/8;
			dNdcsi[ 3] = -(eta+1)*(1-zeta)/8;
			dNdcsi[ 4] = -(1-eta)*(zeta+1)/8;
			dNdcsi[ 5] = (1-eta)*(zeta+1)/8;
			dNdcsi[ 6] = (eta+1)*(zeta+1)/8;
			dNdcsi[ 7] = -(eta+1)*(zeta+1)/8;

			// dNdeta
			dNdeta.resize(8);
			dNdeta[ 0] = -(1-csi)*(1-zeta)/8;
			dNdeta[ 1] = -(csi+1)*(1-zeta)/8;
			dNdeta[ 2] = (csi+1)*(1-zeta)/8;
			dNdeta[ 3] = (1-csi)*(1-zeta)/8;
			dNdeta[ 4] = -(1-csi)*(zeta+1)/8;
			dNdeta[ 5] = -(csi+1)*(zeta+1)/8;
			dNdeta[ 6] = (csi+1)*(zeta+1)/8;
			dNdeta[ 7] = (1-csi)*(zeta+1)/8;

			// dNdzeta
			dNdzeta.resize(8);
			dNdzeta[ 0] = -(1-csi)*(1-eta)/8;
			dNdzeta[ 1] = -(csi+1)*(1-eta)/8;
			dNdzeta[ 2] = -(csi+1)*(eta+1)/8;
			dNdzeta[ 3] = -(1-csi)*(eta+1)/8;
			dNdzeta[ 4] = (1-csi)*(1-eta)/8;
			dNdzeta[ 5] = (csi+1)*(1-eta)/8;
			dNdzeta[ 6] = (csi+1)*(eta+1)/8;
			dNdzeta[ 7] = (1-csi)*(eta+1)/8;
			break;

		case Element::FE_HEXAHEDRON27:
			// sf
			sf.resize(27);
			sf[ 0] = (csi-1)*csi*(eta-1)*eta*(zeta-1)*zeta/8;
			sf[ 1] = csi*(csi+1)*(eta-1)*eta*(zeta-1)*zeta/8;
			sf[ 2] = csi*(csi+1)*eta*(eta+1)*(zeta-1)*zeta/8;
			sf[ 3] = (csi-1)*csi*eta*(eta+1)*(zeta-1)*zeta/8;
			sf[ 4] = (csi-1)*csi*(eta-1)*eta*zeta*(zeta+1)/8;
			sf[ 5] = csi*(csi+1)*(eta-1)*eta*zeta*(zeta+1)/8;
			sf[ 6] = csi*(csi+1)*eta*(eta+1)*zeta*(zeta+1)/8;
			sf[ 7] = (csi-1)*csi*eta*(eta+1)*zeta*(zeta+1)/8;
			sf[ 8] = -(csi-1)*(csi+1)*(eta-1)*eta*(zeta-1)*zeta/4;
			sf[ 9] = -(csi-1)*csi*(eta-1)*(eta+1)*(zeta-1)*zeta/4;
			sf[10] = -(csi-1)*csi*(eta-1)*eta*(zeta-1)*(zeta+1)/4;
			sf[11] = -csi*(csi+1)*(eta-1)*(eta+1)*(zeta-1)*zeta/4;
			sf[12] = -csi*(csi+1)*(eta-1)*eta*(zeta-1)*(zeta+1)/4;
			sf[13] = -(csi-1)*(csi+1)*eta*(eta+1)*(zeta-1)*zeta/4;
			sf[14] = -csi*(csi+1)*eta*(eta+1)*(zeta-1)*(zeta+1)/4;
			sf[15] = -(csi-1)*csi*eta*(eta+1)*(zeta-1)*(zeta+1)/4;
			sf[16] = -(csi-1)*(csi+1)*(eta-1)*eta*zeta*(zeta+1)/4;
			sf[17] = -(csi-1)*csi*(eta-1)*(eta+1)*zeta*(zeta+1)/4;
			sf[18] = -csi*(csi+1)*(eta-1)*(eta+1)*zeta*(zeta+1)/4;
			sf[19] = -(csi-1)*(csi+1)*eta*(eta+1)*zeta*(zeta+1)/4;
			sf[20] = (csi-1)*(csi+1)*(eta-1)*(eta+1)*(zeta-1)*zeta/2;
			sf[21] = (csi-1)*(csi+1)*(eta-1)*eta*(zeta-1)*(zeta+1)/2;
			sf[22] = (csi-1)*csi*(eta-1)*(eta+1)*(zeta-1)*(zeta+1)/2;
			sf[23] = csi*(csi+1)*(eta-1)*(eta+1)*(zeta-1)*(zeta+1)/2;
			sf[24] = (csi-1)*(csi+1)*eta*(eta+1)*(zeta-1)*(zeta+1)/2;
			sf[25] = (csi-1)*(csi+1)*(eta-1)*(eta+1)*zeta*(zeta+1)/2;
			sf[26] = -(csi-1)*(csi+1)*(eta-1)*(eta+1)*(zeta-1)*(zeta+1);

			// dNdcsi
			dNdcsi.resize(27);
			dNdcsi[ 0] = csi*(eta-1)*eta*(zeta-1)*zeta/8+(csi-1)*(eta-1)*eta*(zeta-1)*zeta/8;
			dNdcsi[ 1] = (csi+1)*(eta-1)*eta*(zeta-1)*zeta/8+csi*(eta-1)*eta*(zeta-1)*zeta/8;
			dNdcsi[ 2] = (csi+1)*eta*(eta+1)*(zeta-1)*zeta/8+csi*eta*(eta+1)*(zeta-1)*zeta/8;
			dNdcsi[ 3] = csi*eta*(eta+1)*(zeta-1)*zeta/8+(csi-1)*eta*(eta+1)*(zeta-1)*zeta/8;
			dNdcsi[ 4] = csi*(eta-1)*eta*zeta*(zeta+1)/8+(csi-1)*(eta-1)*eta*zeta*(zeta+1)/8;
			dNdcsi[ 5] = (csi+1)*(eta-1)*eta*zeta*(zeta+1)/8+csi*(eta-1)*eta*zeta*(zeta+1)/8;
			dNdcsi[ 6] = (csi+1)*eta*(eta+1)*zeta*(zeta+1)/8+csi*eta*(eta+1)*zeta*(zeta+1)/8;
			dNdcsi[ 7] = csi*eta*(eta+1)*zeta*(zeta+1)/8+(csi-1)*eta*(eta+1)*zeta*(zeta+1)/8;
			dNdcsi[ 8] = -(csi+1)*(eta-1)*eta*(zeta-1)*zeta/4-(csi-1)*(eta-1)*eta*(zeta-1)*zeta/4;
			dNdcsi[ 9] = -csi*(eta-1)*(eta+1)*(zeta-1)*zeta/4-(csi-1)*(eta-1)*(eta+1)*(zeta-1)*zeta/4;
			dNdcsi[10] = -csi*(eta-1)*eta*(zeta-1)*(zeta+1)/4-(csi-1)*(eta-1)*eta*(zeta-1)*(zeta+1)/4;
			dNdcsi[11] = -(csi+1)*(eta-1)*(eta+1)*(zeta-1)*zeta/4-csi*(eta-1)*(eta+1)*(zeta-1)*zeta/4;
			dNdcsi[12] = -(csi+1)*(eta-1)*eta*(zeta-1)*(zeta+1)/4-csi*(eta-1)*eta*(zeta-1)*(zeta+1)/4;
			dNdcsi[13] = -(csi+1)*eta*(eta+1)*(zeta-1)*zeta/4-(csi-1)*eta*(eta+1)*(zeta-1)*zeta/4;
			dNdcsi[14] = -(csi+1)*eta*(eta+1)*(zeta-1)*(zeta+1)/4-csi*eta*(eta+1)*(zeta-1)*(zeta+1)/4;
			dNdcsi[15] = -csi*eta*(eta+1)*(zeta-1)*(zeta+1)/4-(csi-1)*eta*(eta+1)*(zeta-1)*(zeta+1)/4;
			dNdcsi[16] = -(csi+1)*(eta-1)*eta*zeta*(zeta+1)/4-(csi-1)*(eta-1)*eta*zeta*(zeta+1)/4;
			dNdcsi[17] = -csi*(eta-1)*(eta+1)*zeta*(zeta+1)/4-(csi-1)*(eta-1)*(eta+1)*zeta*(zeta+1)/4;
			dNdcsi[18] = -(csi+1)*(eta-1)*(eta+1)*zeta*(zeta+1)/4-csi*(eta-1)*(eta+1)*zeta*(zeta+1)/4;
			dNdcsi[19] = -(csi+1)*eta*(eta+1)*zeta*(zeta+1)/4-(csi-1)*eta*(eta+1)*zeta*(zeta+1)/4;
			dNdcsi[20] = (csi+1)*(eta-1)*(eta+1)*(zeta-1)*zeta/2+(csi-1)*(eta-1)*(eta+1)*(zeta-1)*zeta/2;
			dNdcsi[21] = (csi+1)*(eta-1)*eta*(zeta-1)*(zeta+1)/2+(csi-1)*(eta-1)*eta*(zeta-1)*(zeta+1)/2;
			dNdcsi[22] = csi*(eta-1)*(eta+1)*(zeta-1)*(zeta+1)/2+(csi-1)*(eta-1)*(eta+1)*(zeta-1)*(zeta+1)/2;
			dNdcsi[23] = (csi+1)*(eta-1)*(eta+1)*(zeta-1)*(zeta+1)/2+csi*(eta-1)*(eta+1)*(zeta-1)*(zeta+1)/2;
			dNdcsi[24] = (csi+1)*eta*(eta+1)*(zeta-1)*(zeta+1)/2+(csi-1)*eta*(eta+1)*(zeta-1)*(zeta+1)/2;
			dNdcsi[25] = (csi+1)*(eta-1)*(eta+1)*zeta*(zeta+1)/2+(csi-1)*(eta-1)*(eta+1)*zeta*(zeta+1)/2;
			dNdcsi[26] = -(csi+1)*(eta-1)*(eta+1)*(zeta-1)*(zeta+1)-(csi-1)*(eta-1)*(eta+1)*(zeta-1)*(zeta+1);

			// dNdeta
			dNdeta.resize(27);
			dNdeta[ 0] = (csi-1)*csi*eta*(zeta-1)*zeta/8+(csi-1)*csi*(eta-1)*(zeta-1)*zeta/8;
			dNdeta[ 1] = csi*(csi+1)*eta*(zeta-1)*zeta/8+csi*(csi+1)*(eta-1)*(zeta-1)*zeta/8;
			dNdeta[ 2] =  csi*(csi+1)*(eta+1)*(zeta-1)*zeta/8+csi*(csi+1)*eta*(zeta-1)*zeta/8;
			dNdeta[ 3] = (csi-1)*csi*(eta+1)*(zeta-1)*zeta/8+(csi-1)*csi*eta*(zeta-1)*zeta/8;
			dNdeta[ 4] =  (csi-1)*csi*eta*zeta*(zeta+1)/8+(csi-1)*csi*(eta-1)*zeta*(zeta+1)/8;
			dNdeta[ 5] = csi*(csi+1)*eta*zeta*(zeta+1)/8+csi*(csi+1)*(eta-1)*zeta*(zeta+1)/8;
			dNdeta[ 6] =  csi*(csi+1)*(eta+1)*zeta*(zeta+1)/8+csi*(csi+1)*eta*zeta*(zeta+1)/8;
			dNdeta[ 7] = (csi-1)*csi*(eta+1)*zeta*(zeta+1)/8+(csi-1)*csi*eta*zeta*(zeta+1)/8;
			dNdeta[ 8] =  -(csi-1)*(csi+1)*eta*(zeta-1)*zeta/4-(csi-1)*(csi+1)*(eta-1)*(zeta-1)*zeta/4;
			dNdeta[ 9] = -(csi-1)*csi*(eta+1)*(zeta-1)*zeta/4-(csi-1)*csi*(eta-1)*(zeta-1)*zeta/4;
			dNdeta[10] =  -(csi-1)*csi*eta*(zeta-1)*(zeta+1)/4-(csi-1)*csi*(eta-1)*(zeta-1)*(zeta+1)/4;
			dNdeta[11] = -csi*(csi+1)*(eta+1)*(zeta-1)*zeta/4-csi*(csi+1)*(eta-1)*(zeta-1)*zeta/4;
			dNdeta[12] =  -csi*(csi+1)*eta*(zeta-1)*(zeta+1)/4-csi*(csi+1)*(eta-1)*(zeta-1)*(zeta+1)/4;
			dNdeta[13] = -(csi-1)*(csi+1)*(eta+1)*(zeta-1)*zeta/4-(csi-1)*(csi+1)*eta*(zeta-1)*zeta/4;
			dNdeta[14] =  -csi*(csi+1)*(eta+1)*(zeta-1)*(zeta+1)/4-csi*(csi+1)*eta*(zeta-1)*(zeta+1)/4;
			dNdeta[15] = -(csi-1)*csi*(eta+1)*(zeta-1)*(zeta+1)/4-(csi-1)*csi*eta*(zeta-1)*(zeta+1)/4;
			dNdeta[16] =  -(csi-1)*(csi+1)*eta*zeta*(zeta+1)/4-(csi-1)*(csi+1)*(eta-1)*zeta*(zeta+1)/4;
			dNdeta[17] = -(csi-1)*csi*(eta+1)*zeta*(zeta+1)/4-(csi-1)*csi*(eta-1)*zeta*(zeta+1)/4;
			dNdeta[18] =  -csi*(csi+1)*(eta+1)*zeta*(zeta+1)/4-csi*(csi+1)*(eta-1)*zeta*(zeta+1)/4;
			dNdeta[19] = -(csi-1)*(csi+1)*(eta+1)*zeta*(zeta+1)/4-(csi-1)*(csi+1)*eta*zeta*(zeta+1)/4;
			dNdeta[20] =  (csi-1)*(csi+1)*(eta+1)*(zeta-1)*zeta/2+(csi-1)*(csi+1)*(eta-1)*(zeta-1)*zeta/2;
			dNdeta[21] =  (csi-1)*(csi+1)*eta*(zeta-1)*(zeta+1)/2+(csi-1)*(csi+1)*(eta-1)*(zeta-1)*(zeta+1)/2;
			dNdeta[22] =  (csi-1)*csi*(eta+1)*(zeta-1)*(zeta+1)/2+(csi-1)*csi*(eta-1)*(zeta-1)*(zeta+1)/2;
			dNdeta[23] =  csi*(csi+1)*(eta+1)*(zeta-1)*(zeta+1)/2+csi*(csi+1)*(eta-1)*(zeta-1)*(zeta+1)/2;
			dNdeta[24] =  (csi-1)*(csi+1)*(eta+1)*(zeta-1)*(zeta+1)/2+(csi-1)*(csi+1)*eta*(zeta-1)*(zeta+1)/2;
			dNdeta[25] =  (csi-1)*(csi+1)*(eta+1)*zeta*(zeta+1)/2+(csi-1)*(csi+1)*(eta-1)*zeta*(zeta+1)/2;
			dNdeta[26] =  -(csi-1)*(csi+1)*(eta+1)*(zeta-1)*(zeta+1)-(csi-1)*(csi+1)*(eta-1)*(zeta-1)*(zeta+1);

			// dNdzeta
			dNdzeta.resize(27);
			dNdzeta[ 0] = (csi-1)*csi*(eta-1)*eta*zeta/8+(csi-1)*csi*(eta-1)*eta*(zeta-1)/8;
			dNdzeta[ 1] = csi*(csi+1)*(eta-1)*eta*zeta/8+csi*(csi+1)*(eta-1)*eta*(zeta-1)/8;
			dNdzeta[ 2] =  csi*(csi+1)*eta*(eta+1)*zeta/8+csi*(csi+1)*eta*(eta+1)*(zeta-1)/8;
			dNdzeta[ 3] = (csi-1)*csi*eta*(eta+1)*zeta/8+(csi-1)*csi*eta*(eta+1)*(zeta-1)/8;
			dNdzeta[ 4] =  (csi-1)*csi*(eta-1)*eta*(zeta+1)/8+(csi-1)*csi*(eta-1)*eta*zeta/8;
			dNdzeta[ 5] = csi*(csi+1)*(eta-1)*eta*(zeta+1)/8+csi*(csi+1)*(eta-1)*eta*zeta/8;
			dNdzeta[ 6] =  csi*(csi+1)*eta*(eta+1)*(zeta+1)/8+csi*(csi+1)*eta*(eta+1)*zeta/8;
			dNdzeta[ 7] = (csi-1)*csi*eta*(eta+1)*(zeta+1)/8+(csi-1)*csi*eta*(eta+1)*zeta/8;
			dNdzeta[ 8] =  -(csi-1)*(csi+1)*(eta-1)*eta*zeta/4-(csi-1)*(csi+1)*(eta-1)*eta*(zeta-1)/4;
			dNdzeta[ 9] = -(csi-1)*csi*(eta-1)*(eta+1)*zeta/4-(csi-1)*csi*(eta-1)*(eta+1)*(zeta-1)/4;
			dNdzeta[10] =  -(csi-1)*csi*(eta-1)*eta*(zeta+1)/4-(csi-1)*csi*(eta-1)*eta*(zeta-1)/4;
			dNdzeta[11] = -csi*(csi+1)*(eta-1)*(eta+1)*zeta/4-csi*(csi+1)*(eta-1)*(eta+1)*(zeta-1)/4;
			dNdzeta[12] =  -csi*(csi+1)*(eta-1)*eta*(zeta+1)/4-csi*(csi+1)*(eta-1)*eta*(zeta-1)/4;
			dNdzeta[13] = -(csi-1)*(csi+1)*eta*(eta+1)*zeta/4-(csi-1)*(csi+1)*eta*(eta+1)*(zeta-1)/4;
			dNdzeta[14] =  -csi*(csi+1)*eta*(eta+1)*(zeta+1)/4-csi*(csi+1)*eta*(eta+1)*(zeta-1)/4;
			dNdzeta[15] = -(csi-1)*csi*eta*(eta+1)*(zeta+1)/4-(csi-1)*csi*eta*(eta+1)*(zeta-1)/4;
			dNdzeta[16] =  -(csi-1)*(csi+1)*(eta-1)*eta*(zeta+1)/4-(csi-1)*(csi+1)*(eta-1)*eta*zeta/4;
			dNdzeta[17] = -(csi-1)*csi*(eta-1)*(eta+1)*(zeta+1)/4-(csi-1)*csi*(eta-1)*(eta+1)*zeta/4;
			dNdzeta[18] =  -csi*(csi+1)*(eta-1)*(eta+1)*(zeta+1)/4-csi*(csi+1)*(eta-1)*(eta+1)*zeta/4;
			dNdzeta[19] = -(csi-1)*(csi+1)*eta*(eta+1)*(zeta+1)/4-(csi-1)*(csi+1)*eta*(eta+1)*zeta/4;
			dNdzeta[20] =  (csi-1)*(csi+1)*(eta-1)*(eta+1)*zeta/2+(csi-1)*(csi+1)*(eta-1)*(eta+1)*(zeta-1)/2;
			dNdzeta[21] =  (csi-1)*(csi+1)*(eta-1)*eta*(zeta+1)/2+(csi-1)*(csi+1)*(eta-1)*eta*(zeta-1)/2;
			dNdzeta[22] = (csi-1)*csi*(eta-1)*(eta+1)*(zeta+1)/2+(csi-1)*csi*(eta-1)*(eta+1)*(zeta-1)/2;
			dNdzeta[23] =  csi*(csi+1)*(eta-1)*(eta+1)*(zeta+1)/2+csi*(csi+1)*(eta-1)*(eta+1)*(zeta-1)/2;
			dNdzeta[24] = (csi-1)*(csi+1)*eta*(eta+1)*(zeta+1)/2+(csi-1)*(csi+1)*eta*(eta+1)*(zeta-1)/2;
			dNdzeta[25] =  (csi-1)*(csi+1)*(eta-1)*(eta+1)*(zeta+1)/2+(csi-1)*(csi+1)*(eta-1)*(eta+1)*zeta/2;
			dNdzeta[26] =  -(csi-1)*(csi+1)*(eta-1)*(eta+1)*(zeta+1)-(csi-1)*(csi+1)*(eta-1)*(eta+1)*(zeta-1);
				break;

		default:
			//TODO this part should never be reached
			break;
	}
#undef csi
#undef eta
#undef zeta

	// let's fill in the tuple
	sfd.get<0>() = sf;
	sfd.get<1>() = dNdcsi;
	sfd.get<2>() = dNdeta;
	sfd.get<3>() = dNdzeta;
	return sfd;
}

enum Model::Error Model::sanity_check()
{
	// check if the Element's nodes are right
	for( std::vector<Element>::iterator it = element_list.begin(); it != element_list.end(); it++)
	{
		// check if the referenced material exists
		if(material_list.size() > (size_t)it->material)
			return ERR_INVALID_MATERIAL_REFERENCE;

		// check node's type
		switch(it->type)
		{
			case Element::FE_LINE2:
				if(it->nodes.size() != 2)
				{
					return ERR_NODE_NUMBER;
				}
				break;

			case Element::FE_TETRAHEDRON4:
				if(it->nodes.size() != 4)
				{
					return ERR_NODE_NUMBER;
				}
				break;

			case Element::FE_TETRAHEDRON10:
				if(it->nodes.size() != 10)
				{
					return ERR_NODE_NUMBER;
				}
				break;


			case Element::FE_HEXAHEDRON8:
				if(it->nodes.size() != 8)
				{
					return ERR_NODE_NUMBER;
				}
				break;

			case Element::FE_HEXAHEDRON20:
				if(it->nodes.size() != 20)
				{
					return ERR_NODE_NUMBER;
				}
				break;

			case Element::FE_HEXAHEDRON27:
				if(it->nodes.size() != 27)
				{
					return ERR_NODE_NUMBER;
				}
				break;

			default:
				return ERR_UNSUPPORTED_ELEMENT;
				break;
		}

		// run a sanity check on the node's references
		for(std::vector<size_t>::iterator n = it->nodes.begin(); n != it->nodes.end(); n++)
		{
			if(node_list.find(*n) == node_list.end())
				return ERR_ELEMENT_NODE_REFERENCE;
		}
	}

	//test node_restrictions_list
	//TODO finish this
	return ERR_OK;
}


std::vector<boost::tuple<fem::point, double> > Model::integration_points(const Element::Type &type, const int &degree)
{
	using namespace boost;
	std::vector<tuple<fem::point, double> > ips;
	int d;

	// now build up
	switch(type)
	{
		case Element::FE_TETRAHEDRON4:
			{
				d=2;
				switch(d)
				{
					case 1:
						{
							ips.push_back(tuple<fem::point,double>(fem::point(0.25,0.25,0.25), 1/6.0));
						}
						break;

					case 2:
						{
							ips.push_back(tuple<fem::point,double>(fem::point(0.58541019662496845446,0.13819660112501051518,0.13819660112501051518),0.041666666666666666667));
							ips.push_back(tuple<fem::point,double>(fem::point(0.13819660112501051518,0.58541019662496845446,0.13819660112501051518),0.041666666666666666667));
							ips.push_back(tuple<fem::point,double>(fem::point(0.13819660112501051518,0.13819660112501051518,0.58541019662496845446),0.041666666666666666667));
							ips.push_back(tuple<fem::point,double>(fem::point(0.13819660112501051518,0.13819660112501051518,0.13819660112501051518),0.041666666666666666667));
						}
						break;

					case 3:
						{
						}
						break;

					default:
						//TODO
						break;
				}
		
			}
			break;

		case Element::FE_HEXAHEDRON8:
		d = (degree == 0)?2:degree;
		case Element::FE_HEXAHEDRON27:
		d = (degree == 0)?3:degree;
			{
				double x[d], w[d];	// for the Gauss-Legendre integration points and weights
				// get the Gauss-Legendre integration points and weights
				gauleg(x,w,d);
				// and now generate a list with those points
				for(int i = 0; i < d; i++)
				{
					for(int j = 0; j < d; j++)
					{
						for(int k = 0; k < d; k++)
						{
							ips.push_back(tuple<fem::point,double>(fem::point(x[i],x[j],x[k]), w[i]*w[j]*w[k]));
						}
					}
				}
			}
			break;

		default:
			std::cerr << "Model::integration_points(): unsupported element" << std::endl;
			assert(false);	// this part should never be reached
			break;
	}

	// and now return the integration points
	return ips;
}


}
