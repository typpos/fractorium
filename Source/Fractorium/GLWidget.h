#pragma once

#include "FractoriumEmberController.h"

/// <summary>
/// GLWidget class.
/// </summary>

class Fractorium;//Forward declaration since Fractorium uses this widget.
class GLEmberControllerBase;
template<typename T> class GLEmberController;
template<typename T> class FractoriumEmberController;

/// <summary>
/// The main drawing area.
/// This uses the Qt wrapper around OpenGL to draw the output of the render to a texture whose
/// size matches the size of the window.
/// On top of that, the circles that represent the pre and post affine transforms for each xform are drawn.
/// Based on values specified on the GUI, it will either draw the presently selected xform, or all of them.
/// It can show/hide pre/post affine as well.
/// The currently selected xform is drawn with a circle around it, with all others only showing their axes.
/// The current xform is set by either clicking on it, or by changing the index of the xforms combo box on the main window.
/// A problem here is that all drawing is done using the legacy OpenGL fixed function pipeline which is deprecated
/// and even completely disabled on Mac OS. This will need to be replaced with shader programs for every draw operation.
/// Since this window has to know about various states of the renderer and the main window, it retains pointers to
/// the main window and several of its members.
/// This class uses a controller-based design similar to the main window.
/// </summary>
class GLWidget : public QOpenGLWidget, protected
#ifdef USE_GLSL
	QOpenGLFunctions
#else
	QOpenGLFunctions_2_0
#endif
{
	Q_OBJECT

	friend Fractorium;
	friend FractoriumEmberController<float>;
	friend GLEmberControllerBase;
	friend GLEmberController<float>;

#ifdef DO_DOUBLE
	friend GLEmberController<double>;
	friend FractoriumEmberController<double>;
#endif

public:
	GLWidget(QWidget* p = nullptr);
	~GLWidget();
	void InitGL();
	void DrawQuad();
	void SetMainWindow(Fractorium* f);
	bool Init();
	bool Drawing();
	GLint MaxTexSize();
	GLuint OutputTexID();
	GLint TexWidth();
	GLint TexHeight();

protected:
	virtual void initializeGL() override;
	virtual void paintGL() override;
	virtual void keyPressEvent(QKeyEvent* e) override;
	virtual void keyReleaseEvent(QKeyEvent* e) override;
	virtual void mousePressEvent(QMouseEvent* e) override;
	virtual void mouseReleaseEvent(QMouseEvent* e) override;
	virtual void mouseMoveEvent(QMouseEvent* e) override;
	virtual void wheelEvent(QWheelEvent* e) override;

	void DrawPointOrLine(const QVector4D& col, const GLfloat* vertices, int size, int drawType, bool dashed = false, GLfloat pointSize = 1.0f);
	void DrawPointOrLine(const QVector4D& col, const std::vector<float>& vertices, int drawType, bool dashed = false, GLfloat pointSize = 1.0f);

private:
	void SetDimensions(int w, int h);
	bool Allocate(bool force = false);
	bool Deallocate();
	void SetViewport();
	void DrawUnitSquare();
	void DrawAffineHelper(int index, bool selected, bool hovered, bool pre, bool final, bool background);
	GLEmberControllerBase* GLController();

	bool m_Init = false;
	bool m_Drawing = false;
	GLint m_MaxTexSize = 16384;
	GLint m_TexWidth = 0;
	GLint m_TexHeight = 0;
	GLint m_ViewWidth = 0;
	GLint m_ViewHeight = 0;
	GLuint m_OutputTexID = 0;
#ifdef USE_GLSL
	GLuint m_PosAttr;
	GLuint m_ColAttr;
	GLuint m_PointSizeUniform;
	GLuint m_MatrixUniform;
	GLuint m_TexturePosAttr;
	GLuint m_TextureUniform;
	GLuint m_TextureMatrixUniform;
	glm::ivec4 m_Viewport;
	QMatrix4x4 m_ProjMatrix;
	QMatrix4x4 m_ModelViewMatrix;
	QMatrix4x4 m_ModelViewProjectionMatrix;
	QMatrix4x4 m_TextureProjMatrix;
	vector<float> m_Verts;
	std::array<GLfloat, 10> m_TexVerts = std::array<GLfloat, 10>
	{
		0, 0,
		0, 1,
		1, 1,
		1, 0,
		0, 0
	};
	QOpenGLShaderProgram* m_Program = nullptr;
	QOpenGLShaderProgram* m_QuadProgram = nullptr;
#endif
	Fractorium* m_Fractorium = nullptr;
};
