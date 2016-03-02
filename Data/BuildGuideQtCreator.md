#Building Guide for Fractorium Using MSVC2013 and Qt Creator (64 bit)
##Requirements

Install [git](https://git-scm.com/downloads).

Install [Microsoft Visual Studio 2013 or later](https://www.visualstudio.com/en-us/downloads/download-visual-studio-vs.aspx), then install the latest updates.

Install Qt for Windows 64-bit (VS 2013) 5.4 or later (http://www.qt.io/download/). 

##Get this project
Open up the Visual Studio x64 Native Tools Command Prompt

Create a new folder in your development area named fractorium:

`mkdir fractorium`

`cd fractorium`

`git clone https://github.com/mfeemster/fractorium.git`

##Prerequisites
There are six prerequisite dependencies. Two of them must be downloaded manually:

[libjpeg](http://www.ijg.org/)

[tbb](https://www.threadingbuildingblocks.org/download) (get the open source download)

Extract them into the folder you created such that they are arranged like so:

```
[fractorium]
│  
├─libjpeg
├─tbb
```

Go into the fractorium folder and run this script which will get the rest of the prerequisites from git and build them:

`cd fractorium`

`makedeps.bat`

This will download and build `glm libpng libxml zlib`. You will have a folder structure like this:

```
[YOUR ROOT FOLDER]
│  
├─glm
├─libjpeg
├─libpng
├─libxml2
├─tbb
└─fractorium
    │
    ├─Deps
        │
        ├─libjpeg.lib
        ├─libpng.lib
        ├─libxml2.lib
        ├─libxml2.dll
        ├─tbb.dll
        ├─tbb.lib
        ├─zlib.lib
```

##Begin build with Qt Creator:
Open the Qt Project `fractorium/main.pro` using Qt Creator with the default config of *Desktop Qt [version] MSVC2013 64bit*  
Select *shadow build* in *Edit build configuration* for both *Debug* and *Release*

Switch to *Release* configuration for all projects and build main.pro.

The outputs will be placed in `fractorium/Bin/release` several minutes later if no error occurs.

##Begin build with Visual Studio:
Open the file Fractorium.sln under Builds/MSVC/2013

Set the configuration to release, and build all.

The outputs will be the same as with Qt Creator and will be placed in:

`fractorium/Bin/x64/Release`

Regardless of the IDE chosen, the output folder will have these contents:

```
Ember.dll
Ember.exp
Ember.lib
emberanimate.exe
EmberCL.dll
EmberCL.exp
EmberCL.lib
embergenome.exe
emberrender.exe
fractorium.exe
```

Double click fractorium.exe to run it, and use the command line to run the others.

To run it on a machine which does not have Qt installed, put the .exe and .dll files above along with these files together in one folder

```
Qt5.5.1\5.5\msvc2013_64\bin\Qt5Core.dll
Qt5.5.1\5.5\msvc2013_64\bin\Qt5Gui.dll
Qt5.5.1\5.5\msvc2013_64\bin\Qt5Widgets.dll
Qt5.5.1\5.5\msvc2013_64\plugins\platforms\qwindows.dll  (put in folder "platforms")
Deps\libxml2.dll
Deps\tbb.dll
fractorium\Data\dark.qss
fractorium\Data\flam3-palettes.xml
```

To run on a computer without Visual Studio 2013, these files also need to be in the folder:

```
MSVC2013\VC\redist\x64\Microsoft.VC120.CRT\msvcp120.dll
MSVC2013\VC\redist\x64\Microsoft.VC120.CRT\msvcr120.dll
MSVC2013\VC\redist\x64\Microsoft.VC120.CRT\vccorlib120.dll
```

or you can install *Visual C++ Redistributable Packages for Visual Studio 2013 (64 bit)*

##Final file structure for distribution

```
[YOUR FOLDER]
│  dark.qss
│  Ember.dll
│  emberanimate.exe
│  EmberCL.dll
│  embergenome.exe
│  emberrender.exe
│  flam3-palettes.xml
│  fractorium.exe
│  libxml2.dll
│  Qt5Core.dll
│  Qt5Gui.dll
│  Qt5OpenGL.dll
│  Qt5Widgets.dll
│  tbb.dll
│ 
│  vccorlib120.dll   (optional)
│  msvcp120.dll      (optional)
│  msvcr120.dll      (optional)
│  
└─platforms
        qwindows.dll
```
