TEMPLATE = subdirs
CONFIG += ordered
CONFIG += autobuild

include(./Builds/QtCreator/defaults.pri)

SUBDIRS += Builds/QtCreator/Ember Builds/QtCreator/EmberCL Builds/QtCreator/EmberAnimate Builds/QtCreator/EmberGenome Builds/QtCreator/EmberRender Builds/QtCreator/Fractorium

QMAKE_EXTRA_TARGETS += sub-Builds-QtCreator-Ember-make_first-ordered
