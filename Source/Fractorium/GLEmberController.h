#pragma once

#include "FractoriumPch.h"

/// <summary>
/// GLEmberControllerBase and GLEmberController<T> classes.
/// </summary>

/// <summary>
/// Use/draw pre or post affine transform.
/// </summary>
enum class eAffineType : et { AffinePre, AffinePost };

/// <summary>
/// Hovering over nothing, the x axis, the y axis or the center.
/// </summary>
enum class eHoverType : et { HoverNone, HoverXAxis, HoverYAxis, HoverTranslation };

/// <summary>
/// Dragging an affine transform or panning, rotating or scaling the image.
/// </summary>
enum class eDragState : et { DragNone, DragSelect, DragPanning, DragDragging, DragRotateScale, DragPitchYaw };

/// <summary>
/// Dragging with no keys pressed, shift, control or alt.
/// </summary>
enum class eDragModifier : et { DragModNone = 0x00, DragModShift = 0x01, DragModControl = 0x02, DragModAlt = 0x04 };

/// <summary>
/// GLController, FractoriumEmberController, GLWidget and Fractorium need each other, but each can't all include the other.
/// So GLWidget includes this file, and GLWidget, FractoriumEmberController and Fractorium are declared as forward declarations here.
/// </summary>
class GLWidget;
class Fractorium;
template <typename T> class FractoriumEmberController;

#define USE_GLSL 1

/// <summary>
/// GLEmberControllerBase serves as a non-templated base class with virtual
/// functions which will be overridden in a derived class that takes a template parameter.
/// The controller serves as a way to access both the GLWidget and the underlying ember
/// objects through an interface that doesn't require template argument, but does allow
/// templated objects to be used underneath.
/// The functions not implemented in this file can be found in GLWidget.cpp near the area of code which uses them.
/// </summary>
class GLEmberControllerBase
{
public:
	GLEmberControllerBase(Fractorium* fractorium, GLWidget* glWidget);
	virtual ~GLEmberControllerBase();

	void ClearDrag();
	bool Allocate(bool force = false);
	bool GetAlt();
	bool GetShift();
	bool GetControl();
	void SetAlt();
	void SetShift();
	void SetControl();
	void ClearAlt();
	void ClearShift();
	void ClearControl();
	eDragState DragState() { return m_DragState; }
	eAffineType AffineType() { return m_AffineType; }
	virtual void DrawImage() { }
	virtual void DrawAffines(bool pre, bool post) { }
	virtual void ClearWindow() { }
	virtual bool KeyPress_(QKeyEvent* e);
	virtual bool KeyRelease_(QKeyEvent* e);
	virtual void MousePress(QMouseEvent* e) { }
	virtual void MouseRelease(QMouseEvent* e) { }
	virtual void MouseMove(QMouseEvent* e) { }
	virtual void Wheel(QWheelEvent* e) { }
	virtual bool SizesMatch() { return false; }
	virtual bool CheckForSizeMismatch(int w, int h) { return true; }
	virtual void QueryMatrices(bool print) { }
	virtual void ResetMouseState() { }

protected:
	uint m_DragModifier;
	glm::ivec2 m_MousePos;
	glm::ivec2 m_MouseDownPos;
	glm::ivec4 m_Viewport;
	eDragState m_DragState;
	eHoverType m_HoverType;
	eAffineType m_AffineType;
	GLWidget* m_GL;
	Fractorium* m_Fractorium;
};

/// <summary>
/// Templated derived class which implements all interaction functionality between the embers
/// of a specific template type and the GLWidget;
/// </summary>
template<typename T>
class GLEmberController : public GLEmberControllerBase
{
public:
	GLEmberController(Fractorium* fractorium, GLWidget* glWidget, FractoriumEmberController<T>* controller);
	virtual ~GLEmberController();
	virtual void DrawImage() override;
	virtual void DrawAffines(bool pre, bool post) override;
	virtual void ClearWindow() override;
	virtual void MousePress(QMouseEvent* e) override;
	virtual void MouseRelease(QMouseEvent* e) override;
	virtual void MouseMove(QMouseEvent* e) override;
	virtual void Wheel(QWheelEvent* e) override;
	virtual void QueryMatrices(bool print) override;
	virtual bool SizesMatch() override;
	virtual bool CheckForSizeMismatch(int w, int h) override;
	virtual void ResetMouseState() override;

	T CalcScale();
	T CalcRotation();
	v3T ScrolledCenter(bool toWorld = false);
	void CalcDragXAxis();
	void CalcDragYAxis();
	void CalcDragTranslation();
	void SetSelectedXform(Xform<T>* xform);
	void DrawGrid();
	void DrawAffine(const Xform<T>* xform, bool pre, bool selected, bool hovered);
	int UpdateHover(const v3T& glCoords);
	bool CheckXformHover(const Xform<T>* xform, const v3T& glCoords, T& bestDist, bool pre, bool post);

private:
	v2T SnapToGrid(v2T& vec);
	v3T SnapToGrid(v3T& vec);
	v3T SnapToNormalizedAngle(v3T& vec, uint divisions);
	v3T WindowToWorld(v3T& v, bool flip);
	void QueryVMP();
#ifndef USE_GLSL
	void MultMatrix(m4T& mat);
#endif

	T m_CenterDownX;
	T m_CenterDownY;
	T m_RotationDown;
	T m_ScaleDown;
    T m_PitchDown;
    T m_YawDown;
	v4T m_BoundsDown;

	v3T m_MouseWorldPos;
	v3T m_MouseDownWorldPos;
	v3T m_DragHandlePos;
	v3T m_HoverHandlePos;

	m4T m_Modelview;
	m4T m_Projection;

	Affine2D<T> m_DragSrcTransform;
	std::map<size_t, Affine2D<T>> m_DragSrcPreTransforms;
	std::map<size_t, Affine2D<T>> m_DragSrcPostTransforms;

	Xform<T>* m_HoverXform;
	Xform<T>* m_SelectedXform;
	FractoriumEmberController<T>* m_FractoriumEmberController;
	T GridStep;
	vector<float> m_Verts;//Can't make this T because GLSL only works with floats.
};

