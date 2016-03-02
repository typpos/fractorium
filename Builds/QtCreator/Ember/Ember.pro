TEMPLATE = lib
CONFIG += plugin
CONFIG += shared
CONFIG -= app_bundle
CONFIG -= qt

TARGET = ember

include(../defaults.pri)

#Point to visual studio resource file to embed file information.
win32 {
    RC = $$RCPATH/Ember.rc
    win32:RC_FILE = $$RC
	#message(RC_FILE: $$RC)
}

#Go up one folder because the paths defined in defaults were relative to it, which is up one folder.
PRJ_SRC_DIR = $$absolute_path($$EMBER_ROOT/../Source/Ember)
#message(PRJ_SRC_DIR: $$PRJ_SRC_DIR)

#Project specific compiler flags.
QMAKE_CXXFLAGS += -BUILDING_EMBER

win32 {
    DEFINES += BUILDING_EMBER
}

!win32 {
    target.path = $$LIB_INSTALL_DIR
    INSTALLS += target
}

!macx:PRECOMPILED_HEADER = $$PRJ_SRC_DIR/EmberPch.h

SOURCES += \
    $$PRJ_SRC_DIR/Affine2D.cpp \
    $$PRJ_SRC_DIR/DllMain.cpp \
    $$PRJ_SRC_DIR/Ember.cpp \
    $$PRJ_SRC_DIR/EmberPch.cpp \
    $$PRJ_SRC_DIR/RendererBase.cpp \
    $$PRJ_SRC_DIR/Renderer.cpp \
    $$PRJ_SRC_DIR/VariationList.cpp

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    $$PRJ_SRC_DIR/Affine2D.h \
    $$PRJ_SRC_DIR/CarToRas.h \
    $$PRJ_SRC_DIR/Curves.h \
    $$PRJ_SRC_DIR/DensityFilter.h \
    $$PRJ_SRC_DIR/EmberDefines.h \
    $$PRJ_SRC_DIR/Ember.h \
    $$PRJ_SRC_DIR/EmberMotion.h \
    $$PRJ_SRC_DIR/EmberPch.h \
    $$PRJ_SRC_DIR/EmberToXml.h \
    $$PRJ_SRC_DIR/Interpolate.h \
    $$PRJ_SRC_DIR/Isaac.h \
    $$PRJ_SRC_DIR/Iterator.h \
    $$PRJ_SRC_DIR/Palette.h \
    $$PRJ_SRC_DIR/PaletteList.h \
    $$PRJ_SRC_DIR/Point.h \
    $$PRJ_SRC_DIR/RendererBase.h \
    $$PRJ_SRC_DIR/Renderer.h \
    $$PRJ_SRC_DIR/SheepTools.h \
    $$PRJ_SRC_DIR/SpatialFilter.h \
    $$PRJ_SRC_DIR/TemporalFilter.h \
    $$PRJ_SRC_DIR/Timing.h \
    $$PRJ_SRC_DIR/Utils.h \
    $$PRJ_SRC_DIR/Variation.h \
    $$PRJ_SRC_DIR/VariationList.h \
    $$PRJ_SRC_DIR/Variations01.h \
    $$PRJ_SRC_DIR/Variations02.h \
    $$PRJ_SRC_DIR/Variations03.h \
    $$PRJ_SRC_DIR/Variations04.h \
    $$PRJ_SRC_DIR/Variations05.h \
    $$PRJ_SRC_DIR/Variations06.h \
    $$PRJ_SRC_DIR/Variations07.h \
    $$PRJ_SRC_DIR/VariationsDC.h \
    $$PRJ_SRC_DIR/VarFuncs.h \
    $$PRJ_SRC_DIR/Xform.h \
    $$PRJ_SRC_DIR/XmlToEmber.h

#message("")
