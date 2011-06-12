#ifndef SCENE_GRAPH_COMPONENT_WIREFRAME_TRIANGLE_6_HPP
#define SCENE_GRAPH_COMPONENT_WIREFRAME_TRIANGLE_6_HPP


#include <map>
#include "../../ViewportData.h++"
#include "../../../fem/Model.h++"
#include "../../../fem/Node.h++"
#include "../../../fem/Surface.h++"

/*
Policy class to implemente the SGCDisplacementSurface through a template policy pattern
*/
class WireframeSurfaceTriangle6
{
	public:
		void paintGL(ViewportData &data, fem::Project &project,fem::AnalysisResult<double> *result,  ViewportColors &colors, std::map<size_t, fem::Node> *node_list, fem::Surface *surface);
};


void WireframeSurfaceTriangle6::paintGL(ViewportData &, fem::Project &,fem::AnalysisResult<double> *result,  ViewportColors &colors, std::map<size_t, fem::Node> *node_list, fem::Surface *surface)
{
	int partitions = 6;	//TODO implement a better code

	// temp code to help with the copy/paste
	fem::point p1 = (*node_list)[surface->nodes[0]];
	fem::point p2 = (*node_list)[surface->nodes[1]];
	fem::point p3 = (*node_list)[surface->nodes[2]];
	fem::point p4 = (*node_list)[surface->nodes[3]];
	fem::point p5 = (*node_list)[surface->nodes[4]];
	fem::point p6 = (*node_list)[surface->nodes[5]];

/*
     v
     ^          
     |           
     2            
     |`, 
     |  `.          
     5    `4         
     |      `.        
     |        `.       
     0-----3----1 --> u 

*/
	glColor3fv(colors.wireframe);
	glBegin(GL_LINE_STRIP); 
	for(int i = 0; i <= partitions; i++) 
	{ 
		float x = i/(float)partitions; 
		glVertex3dv( (2.0*p2*(x-0.5)*x - 4.0*p4*(x-1)*x + 2.0*p1*(x-1)*(x-0.5)).data); 
	} 
	glEnd();
	glBegin(GL_LINE_STRIP); 
	for(int i = 0; i <= partitions; i++) 
	{ 
		float x = i/(float)partitions; 
		glVertex3dv( (2.0*p3*(x-0.5)*x - 4.0*p5*(x-1)*x + 2.0*p2*(x-1)*(x-0.5)).data); 
	} 
	glEnd();
	glBegin(GL_LINE_STRIP); 
	for(int i = 0; i <= partitions; i++) 
	{ 
		float x = i/(float)partitions; 
		glVertex3dv( (2.0*p1*(x-0.5)*x - 4.0*p6*(x-1)*x + 2.0*p3*(x-1)*(x-0.5)).data); 
	} 
	glEnd();
}

#endif
