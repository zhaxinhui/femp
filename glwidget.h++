#ifndef GLWIDGET_HPP
#define GLWIDGET_HPP

#include <QGLWidget>
#include <QWheelEvent>

#include "Document.h++"
#include "ViewportColors.h++"
#include "Camera.h++"
#include "DisplayOptions.h++"


class GLWidget : public QGLWidget
{
	Q_OBJECT

	public:
		GLWidget(QWidget *parent = 0);
		~GLWidget();

		QSize minimumSizeHint() const;
		QSize sizeHint() const;
		void setDocument(Document *);	// sets a reference to a document object
		void setColors(ViewportColors *colors);
		void setNodeRadiusScale(float r)	{ node_scale = r; }

	public Q_SLOTS:
		void togglePerspective();
		void setXRotation(int angle);
		void setYRotation(int angle);
		void setZRotation(int angle);
		void setPosition(int x, int y);

	Q_SIGNALS:
		void xRotationChanged(int angle);
		void yRotationChanged(int angle);
		void zRotationChanged(int angle);

	protected:
		void initializeGL();
		void paintGL();
		inline void paintArrow(const fem::point &p, const fem::point &direction);
		void resizeGL(int width, int height);

		// surface rendering methods
		void renderLine3(const fem::point &p1, const fem::point &p2, const fem::point &p3, int partitions = 10);
		void renderQuad4(const fem::point &p1, const fem::point &p2,const fem::point &p3,const fem::point &p4, int partitions = 4);
		void renderQuad9(const fem::point &p1, const fem::point &p2, const fem::point &p3, const fem::point &p4,const fem::point &p5, const fem::point &p6, const fem::point &p7, const fem::point &p8, const fem::point &p9, int partitions = 4);
		void renderTriangle3(const fem::point &p1, const fem::point &p2,const fem::point &p3, int partitions = 4);
		void renderTriangle6(const fem::point &p1, const fem::point &p2,const fem::point &p3,const fem::point &p4, const fem::point &p5, const fem::point &p6, int partitions = 10);
		
		// display lists methods
		void generateDisplayLists();
		void generateNodesDisplayList();

		void mousePressEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);
		void wheelEvent(QWheelEvent *event);
		void keyPressEvent(QKeyEvent *event);

	public:
		DisplayOptions display_options;	// options list to be used by the render routine

	private:
		void normalizeAngle(int *angle);

		// methods to render FEM elements
		void paintNode(size_t label, const fem::Node);
		void paintElement(const fem::Element &element);

		// select rendered objects that intersect a given line (start point, finish point)
		void selectModelObjects(const fem::point &,const fem::point &);
		void deselectAllModelObjects();

		Camera camera;	// transition to a camera class

		QPoint lastPos;
		QColor qtPurple;

		Document *document;

		float node_scale;	// the scale used by the nodes, reset when a window resizes
		float scale;

		ViewportColors *colors;	// color definitions
		bool perspective;	// true if perspective, false if ortogonal

		// display list indices
		GLuint dl_nodes;
		GLuint dl_faces;
		GLuint dl_wireframe;
};

#endif
