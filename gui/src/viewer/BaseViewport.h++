#ifndef BASE_VIEWPORT_HPP
#define BASE_VIEWPORT_HPP

#include <sigc++/sigc++.h> 	// to side step a compiler error caused by a conflict with Qt and libsigc++
#include <assert.h>

#include <QOpenGLWidget>

#include <array>

#include <ui/MdiWindowProperties.h++>
#include <options/ViewportColors.h++>
#include <Selection.h++>

#include "../Project.h++"

#include <libfemp/Model.h++>
#include <libfemp/AnalysisResult.h++>

#include "SceneGraph.h++"
#include <options/Options.h++>

#include "Selection.h++"

#include "ViewportStates/ViewportState.h++"	// for the vieport's state pattern base class
#include "InputStates/Input.h++"		// for the state pattern that handles user input
#include "InputStates/ISStart.h++"		// for the state pattern that handles user input
#include "InputStates/ISLeftClick.h++"		// for the state pattern that handles user input
#include "InputStates/ISLeftDrag.h++"		// for the state pattern that handles user input
#include "InputStates/ISRightClick.h++"		// for the state pattern that handles user input
#include "InputStates/ISRightDrag.h++"		// for the state pattern that handles user input


class ViewportState;
class Input;

/*
QGLidget subclass designed for a Base class for all Qt widgets that provide an opengl viewports to render the model
*/
class BaseViewport
  : public QOpenGLWidget,
  virtual public sigc::trackable
{

Q_OBJECT


public:
  BaseViewport(fem::Project &project, QWidget *parent);
  virtual ~BaseViewport();


  void setColors(ViewportColors &new_colors);

  QSize minimumSizeHint() const;

  QSize sizeHint() const;

  /*
  Generic method to implement a state pattern to render variants of the same model (i.e., XX tension, YY tension...)
  This method:
	- allocates memory for a new state object
	- initializes the object
	- generates the scenegraph
  */
  void setState(ViewportState *new_state);

  /**
  Method to refresh the scene
  **/
  void refresh(void);

  /*
  sets if the element nodes are visible
  @param	state	true if nodes should be visible, false if they shouldn't be rendered
  */
  void setNodeVisibility(bool const state);
  void setNodeRestrictionsVisibility(bool const state);
  void setSurfaceVisibility(bool const state);

  void setTrianglesVisible(bool const state);
  void setShading(bool const state);


public Q_SLOTS:
  void setXRotation(int angle);
  void setYRotation(int angle);
  void setZRotation(int angle);
  void setPosition(double x, double y, double z);

  /**
  Configures the current viewport to render new_result
  **/
  void setAnalysisResult(fem::AnalysisResult &new_result);

  void setDisplacementsScale(float scale);

  /**
  Sets the visibility state of any scenegraph group of the current viewport state
  **/
  void setRenderGroupVisibility(SceneGraph::Groups group, bool state);

  /**
  Renders only the current selection of elements
  **/
  void setViewSelection(Selection);

  /**
  Extract the current pick ray and select all objects in the scene graph that are intersected
  **/
  void selectObjectsFromRay(fem::Point3D const &origin, fem::Point3D const &destination);
  void selectObjectsFromFrustum(std::array<fem::Point3D,4> const &near, std::array<fem::Point3D,4> const &far);

Q_SIGNALS:
  void xRotationChanged(int angle);
  void yRotationChanged(int angle);
  void zRotationChanged(int angle);
  void positionChanged( double x, double y, double z);

public:
  // libsigc++ slots
  void setSelection(Selection);	// sets the selection
  void clearSelection();		// clears the selection list representation
  void showSelection(const Selection);
  void showAll();

protected:
  void initializeGL();
  void resizeGL(int width, int height);
  virtual void paintGL();

  // routines to handle input
  void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event);
  void keyPressEvent ( QKeyEvent * event );

  void normalizeAngle(int *angle);

public:
  // libsigc++ signals
  sigc::signal<void, Selection>	selection_changed;	// signals that this window originated a change of item selection

  InputStates::Start 	m_is_start;
  InputStates::LeftClick	m_is_left_click;
  InputStates::LeftDrag	m_is_left_drag;
  InputStates::RightClick	m_is_right_click;
  InputStates::RightDrag	m_is_right_drag;

protected:
  Input *m_input;	// state pattern that handles user input

public:
  ViewportData viewport_data;
  ViewportColors colors;	// color definitions

  fem::Project *project;

  ViewportState	*state;	// pointer to object used for the State pattern

};


#endif
