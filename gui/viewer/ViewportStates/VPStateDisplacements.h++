#ifndef VIEWPORT_STATE_DISPLACEMENTS_HPP
#define VIEWPORT_STATE_DISPLACEMENTS_HPP


#include <QMouseEvent>

#include "../../fem/Node.h++"
#include "ViewportState.h++"
#include "../DisplacementsViewport.h++"
#include "../ViewportData.h++"
#include "../ViewportColors.h++"

#include "../../fem/AnalysisResult.h++"


class DisplacementsViewport;

/*
A pattern for the State pattern which is used to render the model space
*/
class VPStateDisplacements
	:public ViewportState<DisplacementsViewport>
{
	protected:
		std::map<size_t, fem::Node> 	displaced_nodes;
		std::map<size_t, fem::Node> 	displacements;
		std::map<size_t, fem::Node>	*original_nodes;

	public:
		VPStateDisplacements();
		~VPStateDisplacements();

		void initialize(DisplacementsViewport *mv);
		void populateScenegraph(DisplacementsViewport *mv);
		void paintGL(DisplacementsViewport *mv);
		void mousePressEvent(DisplacementsViewport *mv, QMouseEvent *event);
		// void mouseMoveEvent(QMouseEvent *event);
		void keyPressEvent ( DisplacementsViewport *mv, QKeyEvent * event );


		/**
		Sets 
		**/
		template<typename scalar>
		void setDisplacements(fem::AnalysisResult<scalar> &);

		/**
		Sets the new displacement scale to be used when rendering stuff
		@param	new_scale	the new displacement's scale
		**/
		void setDisplacementScale(float new_scale);

	protected:
		/*
		Marks all objects which intersect with a given ray as selected
		*/
		void selectModelObjects(const fem::point &near,const fem::point &far);
};



template<typename scalar>
void VPStateDisplacements::setDisplacements(fem::AnalysisResult<scalar> &analysis)
{
	displacements.clear();

	fem::point p;
	size_t n;


	for(std::map<size_t, boost::tuple<size_t,size_t,size_t> >::iterator i =  analysis.lm.begin(); i != analysis.lm.end(); i++)
	{
		n = i->second.get<0>();
		if(n != 0)
			p.x(analysis.d(n-1));
		else
			p.x(0);
		n = i->second.get<1>();
		if(n != 0)
			p.y(analysis.d(n-1));
		else
			p.y(0);
		n = i->second.get<2>();
		if(n != 0)
			p.z(analysis.d(n-1));
		else
			p.z(0);

		displacements[i->first] = fem::Node(p);
	}

	//TODO set an appropriate displacement scale
	this->setDisplacementScale(1.0);
}

#endif