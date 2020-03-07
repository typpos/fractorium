TEMPLATE = app
QT += core gui widgets opengl concurrent

TARGET = fractorium

include(../defaults.pri)

#Point to visual studio resource file to embed file information and icon.
win32 {
    RC = $$RCPATH/Fractorium.rc
    win32:RC_FILE = $$RC
	#message(RC_FILE: $$RC)
}

#message(QTDIR: $$(QTDIR))

#Go up one folder because the paths defined in defaults were relative to it, which is up one folder.
PRJ_SRC_DIR = $$absolute_path($$EMBER_ROOT/../Source/Fractorium)
#message(PRJ_SRC_DIR: $$PRJ_SRC_DIR)

#Qt will be installed on a system wide level on *nix. It should be the same for Windows, but copy it local just to be safe.
win32 {
CONFIG(release, debug|release) {
	qtfiles.path = $$BIN_INSTALL_DIR
	qtfiles.files = $$(QTDIR)\bin\Qt5Core.dll $$(QTDIR)\bin\Qt5Gui.dll $$(QTDIR)\bin\Qt5Widgets.dll
	INSTALLS += qtfiles

	qtplatforms.path = $$BIN_INSTALL_DIR\platforms
	qtplatforms.files = $$(QTDIR)\plugins\platforms\qwindows.dll
	INSTALLS += qtplatforms
	
	qimageformats.path = $$BIN_INSTALL_DIR\imageformats
	qimageformats.files = $$(QTDIR)\plugins\imageformats\qjpeg.dll
	INSTALLS += qimageformats
}

CONFIG(debug, debug|release) {
	qtfiles.path = $$BIN_INSTALL_DIR
	qtfiles.files = $$(QTDIR)\bin\Qt5Cored.dll $$(QTDIR)\bin\Qt5Guid.dll $$(QTDIR)\bin\Qt5Widgetsd.dll
	INSTALLS += qtfiles

	qtplatforms.path = $$BIN_INSTALL_DIR\platforms
	qtplatforms.files = $$(QTDIR)\plugins\platforms\qwindowsd.dll
	INSTALLS += qtplatforms
	
	qimageformats.path = $$BIN_INSTALL_DIR\imageformats
	qimageformats.files = $$(QTDIR)\plugins\imageformats\qjpeg.dll
	INSTALLS += qimageformats
}
}

#For some reason, a Qt project needs to be told to look at itself.
INCLUDEPATH += $$PRJ_SRC_DIR
INCLUDEPATH += $$PRJ_SRC_DIR/PaletteEditor

# Uncomment this if you only want to build a binary instead of an app bundle.
#macx:CONFIG -= app_bundle

target.path = $$BIN_INSTALL_DIR
#message(TARGET INSTALL: $$target.path)
INSTALLS += target

palettes.path = $$SHARE_INSTALL_DIR
palettes.files = $$ASSETS_DIR/flam3-palettes.xml \
$$ASSETS_DIR/boxtail_pack_02.gradient \
$$ASSETS_DIR/boxtail_pack_03_triangle.gradient \
$$ASSETS_DIR/boxtail_pack_04_mineshack.gradient \
$$ASSETS_DIR/fardareismai_pack_01_variety_number_128.gradient \
$$ASSETS_DIR/fardareismai_pack_02_b_sides.gradient \
$$ASSETS_DIR/fardareismai_pack_03_old_and_new.gradient \
$$ASSETS_DIR/fardareismai_pack_04_hoard.gradient \
$$ASSETS_DIR/fractaldesire_pack_01.gradient \
$$ASSETS_DIR/rce_ordinary_pack_01_colornation.gradient \
$$ASSETS_DIR/tatasz_pack_01.gradient \
$$ASSETS_DIR/tatasz_pack_02_colder.gradient \
$$ASSETS_DIR/tatasz_pack_02_dark.gradient \
$$ASSETS_DIR/tatasz_pack_02_warmer.gradient \
$$ASSETS_DIR/tatasz_pack_03.gradient \
$$ASSETS_DIR/Amphibole_Supergroup.ugr \
$$ASSETS_DIR/Apatite_Supergroup.ugr \
$$ASSETS_DIR/Feldspar_Group.ugr \
$$ASSETS_DIR/Mica_Group.ugr \
$$ASSETS_DIR/Quartz_Varieties.ugr

#message(PALETTE INSTALL SOURCE: $$palettes.files)
INSTALLS += palettes

themes.path = $$SHARE_INSTALL_DIR
win32 {
themes.files = $$ASSETS_DIR/dark_windows.qss
}

macx {
themes.files = $$ASSETS_DIR/dark_mac.qss
}

unix:!macx {
themes.files = $$ASSETS_DIR/dark_linux.qss
}

themes.files += $$ASSETS_DIR/lightdark.qss
themes.files += $$ASSETS_DIR/uranium.qss

#message(THEMES INSTALL SOURCE: $$themes.files)
INSTALLS += themes

!win32 {
	icon.path = $$SHARE_INSTALL_DIR
	icon.files = $$absolute_path($$PRJ_SRC_DIR/Icons/Fractorium.png)
	#message(ICON INSTALL SOURCE: $$icon.files)
	INSTALLS += icon

	launcher.path = $$LAUNCHER_INSTALL_DIR
	launcher.files = $$ASSETS_DIR/Fractorium.desktop
	#message(LAUNCHER INSTALL SOURCE: $$launcher.files)
	INSTALLS += launcher
}

macx:ICON = $$ASSETS_DIR/Fractorium.icns
!macx:PRECOMPILED_HEADER = $$PRJ_SRC_DIR/FractoriumPch.h

LIBS += -L$$absolute_path($$DESTDIR) -lember
LIBS += -L$$absolute_path($$DESTDIR) -lembercl

SOURCES += \
    $$PRJ_SRC_DIR/AboutDialog.cpp \
    $$PRJ_SRC_DIR/csshighlighter.cpp \
    $$PRJ_SRC_DIR/CurvesGraphicsView.cpp \
    $$PRJ_SRC_DIR/DoubleSpinBox.cpp \
    $$PRJ_SRC_DIR/FinalRenderDialog.cpp \
    $$PRJ_SRC_DIR/FinalRenderEmberController.cpp \
    $$PRJ_SRC_DIR/Fractorium.cpp \
    $$PRJ_SRC_DIR/FractoriumEmberController.cpp \
    $$PRJ_SRC_DIR/FractoriumInfo.cpp \
    $$PRJ_SRC_DIR/FractoriumLibrary.cpp \
    $$PRJ_SRC_DIR/FractoriumMenus.cpp \
    $$PRJ_SRC_DIR/FractoriumPalette.cpp \
    $$PRJ_SRC_DIR/FractoriumParams.cpp \
    $$PRJ_SRC_DIR/FractoriumPch.cpp \
    $$PRJ_SRC_DIR/FractoriumRender.cpp \
    $$PRJ_SRC_DIR/FractoriumSettings.cpp \
    $$PRJ_SRC_DIR/FractoriumToolbar.cpp \
    $$PRJ_SRC_DIR/FractoriumXaos.cpp \
    $$PRJ_SRC_DIR/FractoriumXformsAffine.cpp \
    $$PRJ_SRC_DIR/FractoriumXformsColor.cpp \
    $$PRJ_SRC_DIR/FractoriumXforms.cpp \
    $$PRJ_SRC_DIR/FractoriumXformsSelect.cpp \
    $$PRJ_SRC_DIR/FractoriumXformsVariations.cpp \
    $$PRJ_SRC_DIR/GLEmberController.cpp \
    $$PRJ_SRC_DIR/GLWidget.cpp \
    $$PRJ_SRC_DIR/Main.cpp \
    $$PRJ_SRC_DIR/OptionsDialog.cpp \
    $$PRJ_SRC_DIR/qcssparser.cpp \
    $$PRJ_SRC_DIR/qcssscanner.cpp \
    $$PRJ_SRC_DIR/QssDialog.cpp \
    $$PRJ_SRC_DIR/QssTextEdit.cpp \
    $$PRJ_SRC_DIR/SpinBox.cpp \
    $$PRJ_SRC_DIR/VariationsDialog.cpp \
    $$PRJ_SRC_DIR/LibraryTreeWidget.cpp \
    $$PRJ_SRC_DIR/PaletteEditor/ColorPanel.cpp \
    $$PRJ_SRC_DIR/PaletteEditor/ColorPickerWidget.cpp \
    $$PRJ_SRC_DIR/PaletteEditor/ColorTriangle.cpp \
    $$PRJ_SRC_DIR/PaletteEditor/GradientColorsView.cpp \
    $$PRJ_SRC_DIR/PaletteEditor/PaletteEditor.cpp

HEADERS += \
    $$SRC_COMMON_DIR/EmberCommon.h \
    $$SRC_COMMON_DIR/EmberCommonPch.h \
    $$SRC_COMMON_DIR/JpegUtils.h \
    $$PRJ_SRC_DIR/AboutDialog.h \
    $$PRJ_SRC_DIR/csshighlighter.h \
    $$PRJ_SRC_DIR/CurvesGraphicsView.h \
    $$PRJ_SRC_DIR/DoubleSpinBox.h \
    $$PRJ_SRC_DIR/DoubleSpinBoxTableItemDelegate.h \
    $$PRJ_SRC_DIR/EmberFile.h \
    $$PRJ_SRC_DIR/EmberTreeWidgetItem.h \
    $$PRJ_SRC_DIR/FinalRenderDialog.h \
    $$PRJ_SRC_DIR/FinalRenderEmberController.h \
    $$PRJ_SRC_DIR/FractoriumCommon.h \
    $$PRJ_SRC_DIR/FractoriumEmberController.h \
    $$PRJ_SRC_DIR/Fractorium.h \
    $$PRJ_SRC_DIR/FractoriumPch.h \
    $$PRJ_SRC_DIR/FractoriumSettings.h \
    $$PRJ_SRC_DIR/GLEmberController.h \
    $$PRJ_SRC_DIR/GLWidget.h \
    $$PRJ_SRC_DIR/OptionsDialog.h \
    $$PRJ_SRC_DIR/PaletteTableWidgetItem.h \
    $$PRJ_SRC_DIR/qcssparser.h \
    $$PRJ_SRC_DIR/qcssscanner.h \
    $$PRJ_SRC_DIR/qfunctions.h \
    $$PRJ_SRC_DIR/QssDialog.h \
    $$PRJ_SRC_DIR/QssTextEdit.h \
    $$PRJ_SRC_DIR/resource.h \
    $$PRJ_SRC_DIR/SpinBox.h \
    $$PRJ_SRC_DIR/StealthComboBox.h \
    $$PRJ_SRC_DIR/TableWidget.h \
    $$PRJ_SRC_DIR/TwoButtonComboWidget.h \
    $$PRJ_SRC_DIR/VariationsDialog.h \
    $$PRJ_SRC_DIR/VariationTreeWidgetItem.h \
    $$PRJ_SRC_DIR/LibraryTreeWidget.h \
    $$PRJ_SRC_DIR/PaletteEditor/ColorPanel.h \
    $$PRJ_SRC_DIR/PaletteEditor/ColorPickerWidget.h \
    $$PRJ_SRC_DIR/PaletteEditor/ColorTriangle.h \
    $$PRJ_SRC_DIR/PaletteEditor/GradientArrow.h \
    $$PRJ_SRC_DIR/PaletteEditor/GradientColorsView.h \
    $$PRJ_SRC_DIR/PaletteEditor/PaletteEditor.h

FORMS += \
    $$PRJ_SRC_DIR/AboutDialog.ui \
    $$PRJ_SRC_DIR/FinalRenderDialog.ui \
    $$PRJ_SRC_DIR/Fractorium.ui \
    $$PRJ_SRC_DIR/OptionsDialog.ui \
    $$PRJ_SRC_DIR/QssDialog.ui \
    $$PRJ_SRC_DIR/VariationsDialog.ui \
    $$PRJ_SRC_DIR/PaletteEditor.ui

OTHER_FILES += \
    $$PRJ_SRC_DIR/Fractorium.aps \
    $$PRJ_SRC_DIR/Fractorium.rc

RESOURCES += \
    $$PRJ_SRC_DIR/Fractorium.qrc

DISTFILES += \
    ../../../Data/FlameExamples/b33rheart_examples.flame \
    ../../../Data/FlameExamples/b33rheart_sierpinski.flame \
    ../../../Data/FlameExamples/plangkye_examples.flame \
    ../../../Data/FlameExamples/tatasz_examples.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/-x^3+x.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/-x^3+x/[a.b-d.d-c.d-a.c] -x^3+x.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/-x^3+x/[a.b2.b2-c.a-b.a] -x^3+x.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/-x^3+x/[a.b2.b2-c.c-a.c] -x^3+x.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/-x^3+x/[a2.b-a.b2.b2.a2] -x^3+x.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/-x^3+x/[a2.b-b.a.a2] -x^3+x.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/-x^3+x/[b.b-a.a3.b3.a2] -x^3+x.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/-x^3+x/[b.b.a-b.a2] -x^3+x.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/-x^3+x/[b2.b-a.a.b2] -x^3+x.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/-x^3+x/[b3.a-a.a.a.a.b] -x^3+x.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/-x^3+x/[b3.b3.a-a.a.b] -x^3+x.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/-x^3+x^2+x.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/-x^3+x^2+x/[b.b.b-a.a3.b4]C12[-x^3+x^2+x].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/-x^3+x^2+x/[b.b.b-a.b2] -x^3+x^2+x.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/-x^3+x^2+x/[b2.b-a.a.b2] -x^3+x^2+x.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^2-x+4.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^2-x+4/[3a.b-2a.2b] x^2-x+4.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^2-x+4/[a.a.a.b.b-a.b.b] x^2-x+4.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^2-x+4/[a.a.b.b-a.a.b.b] x^2-x+4.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^2-x+4/[b.b-a0.b.b]B2[x^2-x+4].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^2-x+4/[b0.b0-a.b.c-a.a]B2[x^2-x+4].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^2-x+4/[b0.c0-a.b.c-a.a]B2[x^2-x+4].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3+2x-1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3+2x-1/[a.a2.b-a2.b.a2] x^3+2x-1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3+2x-1/[a.b-a.b.c-c.b2]B2[x^3+2x-1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3+2x-1/[a.b.a2-a.a2.a3] x^3+2x-1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3+2x-1/[a.b.a2-b.a2.a2]B2[x^3+2x-1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3+2x-1/[a.b2-a.a.b.b2]B2[x^3+2x-1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3+2x-1/[a.c-a.a.b-c.c2.a2.a2]B2[x^3+2x-1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3+2x-1/[a.c2.c2-a.b.c-a.b] x^3+2x-1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3+2x-1/[a3.b.b-a3.a.b] x^3+2x-1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3+2x-1/[b.a-a.a.a3.a2] x^3+2x-1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3+2x-1/[b.a.b2-a.b2.a2]B2[x^3+2x-1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3+2x-1/[b.a.c2-a.b-a.c] x^3+2x-1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3+2x-2.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3+2x-2/[b.b-b.b.a2.a2] x^3+2x-2.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3+2x-2/[b.b.a.a-a2.a2] x^3+2x-2.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3+x-1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3+x-1/[a.b2-a3.b4.a4]B2[x^3+x-1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3+x-1/[a.b3.a4-b3.a4]B2[x^3+x-1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3+x-1/[a.b3.b4-a2.b4]B2[x^3+x-1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3+x-1/[a.b3.b4-a3.b2]B2[x^3+x-1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3+x-1/[a2.a2.a7]B2[x^3+x-1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3+x-1/[a2.a3.a4] x^3+x-1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3+x-1/[a2.a4.a4.a6]B2[x^3+x-1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3+x-1/[a3.a3.a4.a5]B2[x^3+x-1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3+x-1/[a3.b3-a.a.a3.a2]B2[x^3+x-1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3+x-1/[a3.b3-a.a3.b3.a2]B2[x^3+x-1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3+x-1/[b.b.b3-a3.b2]B2[x^3+x-1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3+x-1/[b.b2-a2.b4.a4]B2[x^3+x-1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3+x-1/[b.b2.a2-b.a5]B2[x^3+x-1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3+x-1/[b.b3.a4-a3.a2]B2[x^3+x-1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-2x^2+2x+1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-2x^2+2x+1/[b.a.a-a.a.a2] x^3-2x^2+2x+1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-2x^2+2x+1/[b.b2-a.b.b.b2]B2[x^3-2x^2+2x+1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-2x^2+3x-1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-2x^2+3x-1/[a.a.b2-a.b2.a2]B2[x^3-2x^2+3x-1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-2x^2+3x-1/[a.b-a.b.c-b.a2]B2[x^3-2x^2+3x-1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-2x^2+3x-1/[a.b2-b.b.b3.a2]B2[x^3-2x^2+3x-1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-2x^2+3x-1/[b.a.a2-a.a.a4]B2[x^3-2x^2+3x-1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-2x^2+3x-1/[b2.a2-a.b.b.a2] x^3-2x^2+3x-1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-2x^2+x+1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-2x^2+x+1/[a.a.b-b.b2.a4]B2[x^3-2x^2+x+1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-2x^2+x+1/[a.a.b2-a3.a2]B2[x^3-2x^2+x+1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-2x^2+x+1/[a.a2.b-a2.b.b2] x^3-2x^2+x+1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-2x^2+x+1/[a.b-a.a.c-c.b2]B2[x^3-2x^2+x+1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-2x^2+x+1/[a.b-a.b.c-b2.c2]B2[x^3-2x^2+x+1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-2x^2+x+1/[a.b.a2-b.b2.a2]B2[x^3-2x^2+x+1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-2x^2+x+1/[a.d-b.c-a.b-a.b.c] x^3-2x^2+x+1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-2x^2+x+1/[b.a-a.a2.a2.b2] x^3-2x^2+x+1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-2x^2+x+1/[b.a.a2-b.b.a4]B2[x^3-2x^2+x+1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-2x^2+x+1/[b.a.b2-a.a3.a2]B2[x^3-2x^2+x+1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-2x^2+x+1/[b.a3-a.a.b.a3] x^3-2x^2+x+1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-2x^2+x+1/[b.b.b2-a.b2.a2]B2[x^3-2x^2+x+1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-2x^2+x+1/[b0.b-a.b3.b2]B2[x^3-2x^2+x+1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+1/[a2.a3] x^3-x^2+1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+1/[a2.a6.a5]B2[x^3-x^2+1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+1/[a3.a4.a5] x^3-x^2+1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+1/[a3.a4.a5]B2[x^3-x^2+1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+2x+1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+2x+1/[a.b.b3-a2.b2.b4]B2[x^3-x^2+2x-1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+2x+1/[b.a-a.a.b.a2] x^3-x^2+2x+1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+2x+1/[b.a.a-a.a2] x^3-x^2+2x+1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+2x-1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+2x-1/[a.a2.b2-b.b4.a4]B2[x^3-x^2+2x-1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+2x-1/[a.a3.b4-a.b.b3]B2[x^3-x^2+2x-1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+2x-1/[a.b2-a.b2.a2]B2[x^3-x^2+2x-1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+2x-1/[a.b2-b3.b2.a2.a2]B2[x^3-x^2+2x-1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+2x-1/[a.b3-a.b.b3] x^3-x^2+2x-1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+2x-1/[a2.b2-b.b3.a3.a2]B2[x^3-x^2+2x-1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+2x-1/[a4.a2.a] x^3-x^2+2x-1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+2x-1/[a5.a3.a3.a] x^3-x^2+2x-1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+2x-1/[b.a-a3.a2.b2] x^3-x^2+2x-1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+2x-1/[b.a-a3.a3.b3.b2] x^3-x^2+2x-1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+2x-1/[b.b.b3-b.a3.b3]B2[x^3-x^2+2x-1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+2x-1/[b.b3-b.b3.a2] x^3-x^2+2x-1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+2x-1/[b3.b-a.a2.b2] x^3-x^2+2x-1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+3x-1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+3x-1/[a.a.b2.a2-a.a.b] x^3-x^2+3x-1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+3x-1/[b.b.a-b.b.b2.a2] x^3-x^2+3x-1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+3x-1/[b.b2.a2-a.b.b.a2]B2[x^3-x^2+3x-1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+3x-1/[b0.a-b.a.a3]B2[x^3-x^2+3x-1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+3x-2.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+3x-2/[b.b.a-b.b.a2.a2] x^3-x^2+3x-2.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+3x-2/[b.b.a.a-b.a2.a2] x^3-x^2+3x-2.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+x+1 1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+x+1 2.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+x+1/[a.b.b2-b.a3.a4]B2[x^3-x^2+x+1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+x+1/[a.b.b2-b.a3.b3]B2[x^3-x^2+x+1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+x+1/[a.b.b2-b.b3.a3]B2[x^3-x^2+x+1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+x+1/[a.b.b2-b2.a2.a4]B2[x^3-x^2+x+1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+x+1/[a.b.b3-a3.b2.a2]B2[x^3-x^2+x+1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+x+1/[a.b2-a.b.a3]B2[x^3-x^2+x+1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+x+1/[a.b2-b.b3.a3.a2]B2[x^3-x^2+x+1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+x+1/[a.b2.b2-b3.b3.b2.a2]B2[x^3-x^2+x+1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+x+1/[a.b3-a.b.b3.a2] x^3-x^2+x+1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+x+1/[b.a-b3.a3.b2.a2] x^3-x^2+x+1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+x+1/[b.a.a3-b.a3.b3]B2[x^3-x^2+x+1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+x+1/[b.a3-a.a.a3.a2] x^3-x^2+x+1.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+x+1/[b.b2.b2-a.b2.b4]B2[x^3-x^2+x+1].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+x+2.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+x+2/[a3.a3.b-a.b.b] x^3-x^2+x+2.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+x+2/[b.b-a.b.a2.a2] x^3-x^2+x+2.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+x+2/[b.b-a.b.c-a.a]B2[x^3-x^2+x+2].flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+x+2/[b.b.a-a.a2.a2] x^3-x^2+x+2.flame \
    ../../../Data/FlameExamples/tatasz_tiled_swirls_param_pack/x^3-x^2+x+2/[c0.b-a.a2.a2-a.b]B2[x^3-x^2+x+2].flame \
    ../../../Data/tatasz_pack_04.gradient \
    ../../../Data/tatasz_pack_05.gradient \
    ../../../Data/tatasz_pack_06.gradient \
    ../../../Data/tatasz_pack_07.gradient
