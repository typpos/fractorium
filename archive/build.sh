#!/bin/bash

OSX_BUILD_PATH=$PWD
FRACTORIUM_RELEASE_ROOT=$PWD/../Bin/release
# replace 5.9 by your QT version, and check if the instation path is the same
QT_MACDEPLOY=~/Qt/5.9.3/clang_64/bin/macdeployqt

cd $FRACTORIUM_RELEASE_ROOT

EMBERANIMATE_FINAL_ROOT=$PWD/emberanimate.app/Contents/MacOS
EMBERGENOME_FINAL_ROOT=$PWD/embergenome.app/Contents/MacOS
EMBERRENDER_FINAL_ROOT=$PWD/emberrender.app/Contents/MacOS
FRACTORIUM_FINAL_ROOT=$PWD/fractorium.app/Contents/MacOS
FRACTORIUM_FINAL_FRAMEWORKS=$PWD/fractorium.app/Contents/Frameworks

install_name_tool -id $PWD/libember.dylib libember.dylib
install_name_tool -id $PWD/libembercl.dylib libembercl.dylib
install_name_tool -change libember.dylib $PWD/libember.dylib libembercl.dylib

install_name_tool -change libember.dylib $PWD/libember.dylib $EMBERANIMATE_FINAL_ROOT/emberanimate
install_name_tool -change libembercl.dylib $PWD/libembercl.dylib $EMBERANIMATE_FINAL_ROOT/emberanimate

install_name_tool -change libember.dylib $PWD/libember.dylib $EMBERGENOME_FINAL_ROOT/embergenome
install_name_tool -change libembercl.dylib $PWD/libembercl.dylib $EMBERGENOME_FINAL_ROOT/embergenome

install_name_tool -change libember.dylib $PWD/libember.dylib $EMBERRENDER_FINAL_ROOT/emberrender
install_name_tool -change libembercl.dylib $PWD/libembercl.dylib $EMBERRENDER_FINAL_ROOT/emberrender

install_name_tool -change libember.dylib $PWD/libember.dylib $FRACTORIUM_FINAL_ROOT/fractorium
install_name_tool -change libembercl.dylib $PWD/libembercl.dylib $FRACTORIUM_FINAL_ROOT/fractorium

$QT_MACDEPLOY emberanimate.app
$QT_MACDEPLOY embergenome.app
$QT_MACDEPLOY emberrender.app
$QT_MACDEPLOY fractorium.app

cp ./emberanimate.app/Contents/MacOS/emberanimate $FRACTORIUM_FINAL_ROOT
cp ./embergenome.app/Contents/MacOS/embergenome $FRACTORIUM_FINAL_ROOT
cp ./emberrender.app/Contents/MacOS/emberrender $FRACTORIUM_FINAL_ROOT

#solving macdeployqt 5.9.3 bug (wasn't necessary when using 5.4.2)
cd $FRACTORIUM_FINAL_FRAMEWORKS
install_name_tool -change /usr/local/Cellar/ilmbase/2.2.0/lib/libIex-2_2.12.dylib @executable_path/../Frameworks/libIex-2_2.12.dylib libIexMath-2_2.12.dylib
install_name_tool -change /usr/local/Cellar/ilmbase/2.2.0/lib/libIex-2_2.12.dylib @executable_path/../Frameworks/libIex-2_2.12.dylib libIlmThread-2_2.12.dylib
install_name_tool -change /usr/local/Cellar/ilmbase/2.2.0/lib/libIex-2_2.12.dylib @executable_path/../Frameworks/libIex-2_2.12.dylib libImath-2_2.12.dylib

cd $OSX_BUILD_PATH

cd ../Data

cp dark_mac.qss $FRACTORIUM_FINAL_ROOT
cp flam3-palettes.xml $FRACTORIUM_FINAL_ROOT
cp *.gradient $FRACTORIUM_FINAL_ROOT
cp Info.plist $FRACTORIUM_FINAL_ROOT/../

cd $FRACTORIUM_RELEASE_ROOT

mv fractorium.app Fractorium.app

hdiutil create -format UDBZ -quiet -srcfolder Fractorium.app Fractorium.dmg
