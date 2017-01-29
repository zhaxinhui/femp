#include "VPStateTensorFields.h++"

#include <map>
#include <list>
#include <algorithm>
#include <iostream>

#include <assert.h>

#include <GL/gl.h>
#include <GL/glu.h>	// for gluQuadric()

#include <libfemp/Model.h++>
#include <libfemp/Surface.h++>

#include "../BaseViewport.h++"

#include "ui/DialogScale.h++"

#include "../SceneGraph.h++"
#include "../SceneGraphComponents/SGCNode.h++"
#include "../SceneGraphComponents/SGCNodeRestrictions.h++"



VPStateTensorFields::VPStateTensorFields()
	: ViewportState()
{ 
	this->m_factory.setElementRepresentationPolicy(&m_stress_field_representation);
	this->m_factory.setDisplacementsPolicy(&m_displacements);
}


VPStateTensorFields::~VPStateTensorFields()
{
}


void
VPStateTensorFields::initialize(BaseViewport *mv)
{
	// build the displaced_nodes from the analysis
	assert(mv != NULL);

	//this->setDisplacementsScale(1.0);	//TODO tweak this value 

	this->m_stress_field_representation.setModel(mv->project->getModel());
	this->m_displacements.setModel(mv->project->getModel());
}


void
VPStateTensorFields::populateScenegraph(BaseViewport *viewport)
{
	assert(viewport != NULL);

	scenegraph.clear();

	SceneGraphComponent * component;

	// add the nodes to the scenegraph
	fem::Model &femp_model = viewport->project->getModel();
	for(std::map<size_t, fem::Node>::iterator i = femp_model.node_list.begin(); i != femp_model.node_list.end(); i++)
	{
		component =  new SGC::Node(i->first, i->second, &this->m_displacements);
		if(component)
			this->scenegraph.addPrimitiveComponent(SceneGraph::RG_NODES, component);
	}

	for( std::map<fem::node_restriction_ref_t, fem::NodeRestrictions>::iterator i = femp_model.node_restrictions_list.begin(); i != femp_model.node_restrictions_list.end(); i++)
	{
		component = new SGC::NodeRestrictions(i->first, i->first, i->second, &this->m_displacements);
		if(component) 
			this->scenegraph.addPrimitiveComponent(SceneGraph::RG_NODE_RESTRICTIONS, component);
	}

	// add the elements to the scenegraph
	for( std::vector<fem::Element>::size_type n = 0; n < femp_model.element_list.size(); n++)
	{
		component = this->m_factory(n, femp_model.element_list[n]);
		if(component) 
			this->scenegraph.addPrimitiveComponent(SceneGraph::RG_SURFACES, component);
	}

	// generate the scene graph
	this->scenegraph.generateSceneGraph();
}


void
VPStateTensorFields::setAnalysisResult(fem::AnalysisResult<double> &new_result)
{
	this->m_stress_field_representation.setAnalysisResult(new_result);
}


void
VPStateTensorFields::setResultsRanges(fem::ResultsRanges<double> &)
{
	std::cerr << "VPStateTensorFields::setResultsRanges(fem::ResultsRanges<double> &): to be implemented" << std::endl;
}


/*
void
VPStateTensorFields::setDisplacementsScale(float new_scale)
{
	this->m_displacements.setDisplacementsScale(new_scale);
}
*/


void
VPStateTensorFields::keyPressEvent ( BaseViewport * /*viewport*/, QKeyEvent * event )
{
	switch( event->key() )
	{
		case Qt::Key_S:	
			break;

		default:
			break;
	}
}


void 
VPStateTensorFields::showNegativePrincipalStressesVisibility(bool state)
{
	m_stress_field_representation.showNegativePrincipalStressesVisibility(state);
}


void 
VPStateTensorFields::showPositivePrincipalStressesVisibility(bool state)
{
	m_stress_field_representation.showPositivePrincipalStressesVisibility(state);
}


