#!/bin/bash

echo "Creating the AppImage."

BUILD_PATH=$PWD
FRACTORIUM_RELEASE_ROOT=$PWD/../Bin/release
DATA_PATH=$BUILD_PATH/../Data
ICON_PATH=$BUILD_PATH/../Source/Fractorium/Icons
APP_DIR=$PWD/../Bin/Fractorium.AppDir
FRACTORIUM_PACKAGE=$BUILD_PATH/../Bin/Fractorium
FRACTORIUM_RPM_PACKAGE=$BUILD_PATH/../Bin/rpmbuild
EXTRA_LIBS=/usr/lib/x86_64-linux-gnu

# replace 5.12.2 by your QT version, and check if the bin path is the same
QT_PATH=/home/$USER/Dev/Qt/5.11.2/gcc_64/bin

LINUX_DEPLOY_QT=/home/$USER/Dev/linuxdeployqt-6-x86_64.AppImage
APP_IMAGE_TOOL=/home/$USER/Dev/appimagetool-x86_64.AppImage

#######################
#simple check

if [ ! -d "$FRACTORIUM_RELEASE_ROOT" ]; then
   echo "release foulder not found. Please, build the project."
   exit 2
fi


if [ -d "$APP_DIR" ]; then
   echo "Fractorium.AppDir folder already exists in Bin: $APP_DIR"
   echo "Move this folder aside or remove it."
   exit 2
fi

if [ -d "$FRACTORIUM_PACKAGE" ]; then
   echo "Fractorium folder already exists in Bin: $FRACTORIUM_PACKAGE"
   echo "Move this folder aside or remove it."
   exit 2
fi

if [ -d "$FRACTORIUM_RPM_PACKAGE" ]; then
   echo "rpmbuild folder already exists in Bin: $FRACTORIUM_RPM_PACKAGE"
   echo "Move this folder aside or remove it."
   exit 2
fi

if [ ! -d "$QT_PATH" ]; then
   echo "QT folder not found. Please, change QT_PATH."
   exit 2
fi

check_apps()
{
  FILE=$1
  echo $FILE
  if [ ! -e "$FILE" ] ; then
    echo "Application not found: " $FILE
    exit 0
  fi
}

check_apps "$LINUX_DEPLOY_QT"
check_apps "$APP_IMAGE_TOOL"

#######################

export PATH=$QT_PATH:$PATH

mkdir -p $APP_DIR

cd $APP_DIR

FRACTORIUM_BIN=usr/bin
FRACTORIUM_LIB=usr/lib
FRACTORIUM_SHR=usr/share/applications
FRACTORIUM_ICO=usr/share/icons/hicolor/256x256/apps

mkdir -p $FRACTORIUM_BIN
mkdir -p $FRACTORIUM_LIB
mkdir -p $FRACTORIUM_SHR
mkdir -p $FRACTORIUM_ICO

cp $FRACTORIUM_RELEASE_ROOT/ember* $FRACTORIUM_BIN
cp $FRACTORIUM_RELEASE_ROOT/fractorium $FRACTORIUM_BIN
cp $FRACTORIUM_RELEASE_ROOT/lib* $FRACTORIUM_LIB

cp $EXTRA_LIBS/libHalf.so.12 $FRACTORIUM_LIB
cp $EXTRA_LIBS/libIex-2_2.so.12 $FRACTORIUM_LIB
cp $EXTRA_LIBS/libIexMath-2_2.so.12 $FRACTORIUM_LIB
cp $EXTRA_LIBS/libIlmImf-2_2.so.22 $FRACTORIUM_LIB
cp $EXTRA_LIBS/libIlmThread-2_2.so.12 $FRACTORIUM_LIB
cp $EXTRA_LIBS/libImath-2_2.so.12 $FRACTORIUM_LIB
cp $EXTRA_LIBS/libjpeg.so.8 $FRACTORIUM_LIB
cp $EXTRA_LIBS/libpng16.so.16 $FRACTORIUM_LIB
cp $EXTRA_LIBS/libOpenCL.so.1 $FRACTORIUM_LIB
cp $EXTRA_LIBS/libtbb.so.2 $FRACTORIUM_LIB

cp $DATA_PATH/dark_linux.qss $FRACTORIUM_BIN
cp $DATA_PATH/lightdark.qss $FRACTORIUM_BIN
cp $DATA_PATH/flam3-palettes.xml $FRACTORIUM_BIN
cp $DATA_PATH/*.gradient $FRACTORIUM_BIN
cp $DATA_PATH/*.ugr $FRACTORIUM_BIN
cp $ICON_PATH/Fractorium.png $FRACTORIUM_ICO/fractorium.png
cp $DATA_PATH/fractorium.appimage.desktop $FRACTORIUM_SHR/fractorium.desktop

cd ../

# LINUX_DEPLOY_QT OPTIONS

# -unsupported-bundle-everything:    Bundles ALL dependency libraries, down to and including the ld-linux.so loader and glibc. This will allow applications built on newer systems to run on older target systems, but it is not recommended since it leads to bundles that are larger than necessary

# -unsupported-allow-new-glibc: Allows linuxdeployqt to run on distributions newer than the oldest still-supported Ubuntu LTS release. This will result in AppImages that will not run on all still-supported distributions, and is neither recommended nor tested or supported

$LINUX_DEPLOY_QT $APP_DIR/usr/share/applications/fractorium.desktop -executable=Fractorium.AppDir/usr/bin/emberrender -executable=Fractorium.AppDir/usr/bin/embergenome -executable=Fractorium.AppDir/usr/bin/emberanimate -always-overwrite #-unsupported-allow-new-glibc

rm $APP_DIR/AppRun

cp $DATA_PATH/AppRun $APP_DIR

$APP_IMAGE_TOOL $APP_DIR

echo ""
echo "Creating the DEB package."
echo ""

mkdir    Fractorium
mkdir -p Fractorium/DEBIAN
mkdir -p Fractorium/usr/bin
mkdir -p Fractorium/usr/share/applications
mkdir -p Fractorium/usr/share/fractorium

cp Fractorium-x86_64.AppImage Fractorium/usr/bin

cp $DATA_PATH/fractorium.package.desktop Fractorium/usr/share/applications/fractorium.desktop

cp $ICON_PATH/Fractorium.png Fractorium/usr/share/fractorium/fractorium.png

cp $DATA_PATH/control.package Fractorium/DEBIAN/control

#creating symbolic links
cd ./Fractorium/usr/bin

create_symlinks()
{
   ln -s Fractorium-x86_64.AppImage fractorium
   ln -s Fractorium-x86_64.AppImage emberrender
   ln -s Fractorium-x86_64.AppImage embergenome
   ln -s Fractorium-x86_64.AppImage emberanimate
}

create_symlinks

cd ../../../

dpkg --build Fractorium

echo ""
echo "Creating RPM package"
echo ""

mkdir    rpmbuild
mkdir -p rpmbuild/BUILD
mkdir -p rpmbuild/BUILDROOT
mkdir -p rpmbuild/RPMS
mkdir -p rpmbuild/SOURCES
mkdir -p rpmbuild/SPECS
mkdir -p rpmbuild/SRPMS
mkdir -p rpmbuild/tmp

cp $DATA_PATH/Fractorium.spec rpmbuild/SPECS

cd Fractorium
cp -r usr ../rpmbuild/BUILDROOT
cd ../rpmbuild

rpmbuild -v -bb SPECS/Fractorium.spec

cd ../

mv rpmbuild/RPMS/x86_64/* ./

emberVersion=$(grep '#define EMBER_VERSION' ../Source/Ember/EmberDefines.h | sed 's/^.*EMBER_VERSION "\([^"]\+\)".*/\1/')

mv Fractorium.deb Fractorium-$emberVersion-.x86_64.deb
mv Fractorium-$emberVersion-1.x86_64.rpm Fractorium-$emberVersion.x86_64.rpm

echo ""
echo "Finishing AppImage"
echo ""

rm -rf Fractorium

mkdir Fractorium

cp Fractorium-x86_64.AppImage Fractorium
cd Fractorium

create_symlinks

cd ../

tar -czvf Fractorium-$emberVersion.x86_64.AppImage.tar.gz Fractorium

#cleaning
rm -rf Fractorium
rm -rf Fractorium.AppDir
rm -rf rpmbuild
rm -rf Fractorium-x86_64.AppImage

