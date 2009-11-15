#ifndef FEM_HPP
#define FEM_HPP

#include <map>
#include <vector>

#include "Node.h++"
#include "Element.h++"
#include "Material.h++"

namespace fem
{

class Model {
	std::map<size_t, Node> 	node_list;
	std::vector<Element> 	element_list;
	std::map<short, Material> 	material_list;

	public:
		Model();
		~Model();


		void setNode(size_t pos, double x, double y, double z);
};


}

#endif
