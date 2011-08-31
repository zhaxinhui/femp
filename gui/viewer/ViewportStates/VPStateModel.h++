#ifndef VIEWPORT_STATE_MODEL_HPP
#define VIEWPORT_STATE_MODEL_HPP


#include <sigc++/sigc++.h> 	// to side step a compiler error caused by a conflict with Qt and libsigc++
#include <QMouseEvent>

#include "ViewportState.h++"
#include "../ViewportData.h++"
#include "../ViewportColors.h++"

#include "../SceneGraphComponents/ElementRepresentationPolicy/ElementRepresentationFlyweightFactory.h++"
#include "../SceneGraphComponents/DisplacementsRepresentationPolicy/NoDisplacementsPolicy.h++"
#include "../SceneGraphComponents/ElementFactory.h++"


class BaseViewport;

/*
A pattern for the State pattern which is used to render the model space
*/
class VPStateModel
:public ViewportState<BaseViewport>
{
protected:
	ElementRepresentationFlyweightFactory		m_element_representation_factory;
	NoDisplacementsPolicy 	m_no_displacements;	// the model view doesn't render displacements
	SGC::ElementFactory m_factory;

public:
	VPStateModel();
	~VPStateModel();

	void initialize(BaseViewport *mv);
	void populateScenegraph(BaseViewport *mv);
	void paintGL(BaseViewport *mv);
	void mousePressEvent(BaseViewport *mv, QMouseEvent *event);
	// void mouseMoveEvent(QMouseEvent *event);

protected:
	/*
	Marks all objects which intersect with a given ray as selected
	*/
	void selectModelObjects(const fem::point &near,const fem::point &far);
};


#endif
