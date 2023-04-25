TEMPLATE = lib
CONFIG += plugin
CONFIG += shared
CONFIG -= app_bundle
CONFIG -= qt

TARGET = embercl

include(../defaults.pri)

#Point to visual studio resource file to embed file information.
win32 {
    RC = $$RCPATH/EmberCL.rc
    win32:RC_FILE = $$RC
	#message(RC_FILE: $$RC)
}

#Go up one folder because the paths defined in defaults were relative to it, which is up one folder.
PRJ_SRC_DIR = $$absolute_path($$EMBER_ROOT/../Source/EmberCL)
#message(PRJ_SRC_DIR: $$PRJ_SRC_DIR)

#Project specific compiler flags.
QMAKE_CXXFLAGS += -BUILDING_EMBERCL

win32 {
    DEFINES += BUILDING_EMBERCL
}

!win32 {
    target.path = $$LIB_INSTALL_DIR
    INSTALLS += target
}

!macx:PRECOMPILED_HEADER = $$PRJ_SRC_DIR/EmberCLPch.h

LIBS += -L$$absolute_path($$DESTDIR) -lember

SOURCES += \
    $$PRJ_SRC_DIR/DEOpenCLKernelCreator.cpp \
    $$PRJ_SRC_DIR/DllMain.cpp \
    $$PRJ_SRC_DIR/FinalAccumOpenCLKernelCreator.cpp \
    $$PRJ_SRC_DIR/FunctionMapper.cpp \
    $$PRJ_SRC_DIR/IterOpenCLKernelCreator.cpp \
    $$PRJ_SRC_DIR/OpenCLInfo.cpp \
    $$PRJ_SRC_DIR/OpenCLWrapper.cpp \
    $$PRJ_SRC_DIR/RendererCL.cpp \
    $$PRJ_SRC_DIR/RendererClDevice.cpp

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    $$PRJ_SRC_DIR/DEOpenCLKernelCreator.h \
    $$PRJ_SRC_DIR/EmberCLFunctions.h \
    $$PRJ_SRC_DIR/EmberCLPch.h \
    $$PRJ_SRC_DIR/EmberCLStructs.h \
    $$PRJ_SRC_DIR/FinalAccumOpenCLKernelCreator.h \
    $$PRJ_SRC_DIR/FunctionMapper.h \
    $$PRJ_SRC_DIR/IterOpenCLKernelCreator.h \
    $$PRJ_SRC_DIR/OpenCLInfo.h \
    $$PRJ_SRC_DIR/OpenCLWrapper.h \
    $$PRJ_SRC_DIR/RendererClDevice.h \
    $$PRJ_SRC_DIR/RendererCL.h