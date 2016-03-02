#ifndef FRACTORIUM_PCH_H
#define FRACTORIUM_PCH_H//GCC doesn't like #pragma once

#define XFORM_COLOR_COUNT 14

#ifdef _WIN32
	#pragma warning(disable : 4251; disable : 4661; disable : 4100)
#endif

//Has to come first on non-Windows platforms due to some weird naming collisions on *nix.
#ifndef _WIN32
	#include <QtWidgets>
#endif

#include "Renderer.h"
#include "RendererCL.h"
#include "VariationList.h"
#include "OpenCLWrapper.h"
#include "XmlToEmber.h"
#include "EmberToXml.h"
#include "SheepTools.h"
#include "JpegUtils.h"
#include "EmberCommon.h"

#ifdef _WIN32
	#include <QtWidgets>
#endif

#include <deque>
#include "qfunctions.h"
#include <QApplication>
#include <QBrush>
#include <QColor>
#include <QColorDialog>
#include <QComboBox>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QFileInfo>
#include <QFont>
#include <QFontDialog>
#include <QFontMetrics>
#include <QFuture>
#include <QGraphicsView>
#include <QIcon>
#include <QImageReader>
#include <QItemDelegate>
#include <QLineEdit>
#include <QMenu>
#include <QModelIndex>
#include <qopenglfunctions_2_0.h>
#include <QOpenGLWidget>
#include <QPainterPath>
#include <QPushButton>
#include <QSettings>
#include <QSignalMapper>
#include <QSpinBox>
#include <QStandardPaths>
#include <QTextEdit>
#include <QTimer>
#include <QToolBar>
#include <QTreeWidget>
#include <QWheelEvent>
#include <QtConcurrentRun>
#include <QtCore/QMultiHash>
#include <QtCore/QPair>
#include <QtCore/QSharedData>
#include <QtCore/QSize>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtCore/QVector>
#include <QtCore/qglobal.h>
#include <QtGui/QFont>
#include <QtGui/QPalette>
#include <QtGui/QSyntaxHighlighter>
#include <QtWidgets/QMainWindow>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#ifndef _WIN32
	#undef Bool
#endif

using namespace std;
using namespace EmberNs;
using namespace EmberCLns;
#endif
