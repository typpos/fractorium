#Build Guide For Visual Studio 2017 or Qt Creator
##Tools

###git

Install [git](https://git-scm.com/downloads).

###Visual Studio

Install [Microsoft Visual Studio 2017 or later](https://www.visualstudio.com/downloads/), then install the latest updates.

###Qt

Install Qt for Windows 64-bit (VS 2017) 5.8 or later (http://www.qt.io/download/). 

Add system environment variable named `QTPATH` and point it to the location of the Qt folder. On a default install, this will be something like:

`C:\Qt\5.8\msvc2017_64`

###Wix

To build the installer, you must have Wix installed. If you are unconcerned with it, you can skip this step and just dismiss the warning that shows when opening the solution later. It's recommended you ignore the installer since official builds are provided on this page.

###CMake

Install [CMake](https://cmake.org/download/) and be sure to check the option to add it to the system path.

##Obtaining source

###This project

Open up the Visual Studio x64 Native Tools Command Prompt.

Create a new folder in your development area named fractorium:

`mkdir fractorium`

`cd fractorium`

`git clone https://mfeemster@bitbucket.org/mfeemster/fractorium.git`

###Prerequisites

There are six prerequisite dependencies. One of them must be downloaded manually:

Download the zip file here: [libjpeg](http://www.ijg.org/).

Extract the source contents into the folder you created such that it is arranged like so:

```
[fractorium]
│  
├─libjpeg
```

Go into the fractorium folder and run this script which will get the rest of the prerequisites from git and build them:

`cd fractorium`

`makedeps.bat`

This will download and build `glm libopenexr libpng libxml tbb zlib`. You will have a folder structure like this:

```
[fractorium]
│  
├─glm
├─libjpeg
├─libpng
├─libxml2
├─openexr
├─tbb
└─fractorium
    │
    ├─Deps
        │
        ├─Include
            │
            ├─OpenEXR
                │
                ├─*.h
        │
        ├─libjpeg.lib
        ├─libpng.lib
        ├─libxml2.lib
        ├─libxml2.dll
        ├─tbb.dll
        ├─tbb.lib
        ├─zlib.lib
        ├─Half-2_3.lib
        ├─Iex-2_3.lib
        ├─IexMath-2_3.lib
        ├─IlmImf-2_3.lib
        ├─IlmImfUtil-2_3.lib
        ├─IlmThread-2_3.lib
        ├─Imath-2_3.lib
        ├─Half-2_3.dll
        ├─Iex-2_3.dll
        ├─IexMath-2_3.dll
        ├─IlmImf-2_3.dll
        ├─IlmImfUtil-2_3.dll
        ├─IlmThread-2_3.dll
        ├─Imath-2_3.dll
```

##Building with Qt Creator or Visual Studio

###Begin build with Qt Creator

Open the Qt Project `fractorium/main.pro` using Qt Creator with the default config of *Desktop Qt [version] MSVC2017 64bit*.
Make sure *Shadow build* in *Edit build configuration* for both *Debug* and *Release* is unchecked.

Switch to the *Release* configuration.

Under *Build Steps*, add an additional argument of `install` to the `make` command to force all dependencies to be copied to the output folder. The final make command should look like:

`jom.exe install in /path/to/fractorium`

Ensure all projects are in the *Release* configuration and build main.pro.

The outputs will be placed in `fractorium/Bin/release` several minutes later if no error occurs.

###Begin build with Visual Studio

####Visual Studio Qt Addon

Install the [Visual Studio Qt Addon](http://www.qt.io/download/).

Run Visual Studio and verify there is a menu item named *Qt5*. Click on it and click *Qt Options*.

Add a new Qt version to the list with the exact name of "Qt5", and set its path to the same as `$QTPATH`, which will be something like:

`C:\Qt\5.9.1\msvc2017_64`

The name "Qt5" must match exactly and this step must be completed before the Fractorium solution is opened. If not, the Qt add-in will completely ruin all solution and project files that use Qt.

Set the default version to the newly created Qt version and click *Ok*.

Open the file Fractorium.sln under Builds/MSVC/2017

Set the configuration to release, and build all.

###Outputs

The outputs will be the same whether Visual Studio or Qt Creator was used, however their locations will be different.

Qt Creator will place its outputs in:

`fractorium/Bin/Release`

and Visual Studio will place its outputs in:

`fractorium/Bin/x64/Release`

The output contents will be:

```
Amphibole_Supergroup.ugr
Apatite_Supergroup.ugr
boxtail_pack_02.gradient
boxtail_pack_03_triangle.gradient
boxtail_pack_04_mineshack.gradient
dark_windows.qss
lightdark.qss
ember.dll
ember.exp
ember.lib
emberanimate.exe
embercl.dll
embercl.exp
embercl.lib
embergenome.exe
emberrender.exe
fardareismai_pack_01_variety_number_128.gradient
fardareismai_pack_02_b_sides.gradient
fardareismai_pack_03_old_and_new.gradient
fardareismai_pack_04_hoard.gradient
Feldspar_Group.ugr
flam3-palettes.xml
fractaldesire_pack_01.gradient
fractorium.exe
half-2_3.dll
iex-2_3.dll
iexmath-2_3.dll
ilmimf-2_3.dll
ilmthread-2_3.dll
imath-2_3.dll
libxml2.dll
Mica_Group.ugr
Qt5Core.dll
Qt5Gui.dll
Qt5Widgets.dll
Quartz_Varieties.ugr
rce_ordinary_pack_01_colornation.gradient
tatasz_pack_01.gradient
tatasz_pack_02_colder.gradient
tatasz_pack_02_dark.gradient
tatasz_pack_02_warmer.gradient
tatasz_pack_03.gradient
tbb.dll
imageformats\qjpeg.dll
platforms\qwindows.dll
```

Double click fractorium.exe to run it, and use the command line to run the others.

To run on a computer without Visual Studio 2017, these files also need to be in the folder:

```
MSVC2017\VC\Redist\MSVC\14.11.25325\x64\Microsoft.VC141.CRT\msvcp140.dll
MSVC2017\VC\Redist\MSVC\14.11.25325\x64\Microsoft.VC141.CRT\vcruntime140.dll
MSVC2017\VC\Redist\MSVC\14.11.25325\x64\Microsoft.VC141.CRT\vccorlib140.dll
MSVC2017\VC\Redist\MSVC\14.11.25325\x64\Microsoft.VC141.CRT\concrt140.dll
```

or you can install [Visual C++ Redistributable Packages for Visual Studio 2017 (64 bit)](https://www.visualstudio.com/downloads/)

##Final file structure for distribution

```
[YOUR FOLDER]
 │
 ├─ Amphibole_Supergroup.ugr
 ├─ Apatite_Supergroup.ugr
 ├─ boxtail_pack_02.gradient
 ├─ boxtail_pack_03_triangle.gradient
 ├─ boxtail_pack_04_mineshack.gradient
 ├─ dark_windows.qss
 ├─ lightdark.qss
 ├─ ember.dll
 ├─ emberanimate.exe
 ├─ embercl.dll
 ├─ embergenome.exe
 ├─ emberrender.exe
 ├─ fardareismai_pack_01_variety_number_128.gradient
 ├─ fardareismai_pack_02_b_sides.gradient
 ├─ fardareismai_pack_03_old_and_new.gradient
 ├─ fardareismai_pack_04_hoard.gradient
 ├─ Feldspar_Group.ugr
 ├─ flam3-palettes.xml
 ├─ fractaldesire_pack_01.gradient
 ├─ fractorium.exe
 ├─ half-2_3.dll
 ├─ iex-2_3.dll
 ├─ iexmath-2_3.dll
 ├─ ilmimf-2_3.dll
 ├─ ilmthread-2_3.dll
 ├─ imath-2_3.dll
 ├─ libxml2.dll
 ├─ Mica_Group.ugr
 ├─ Qt5Core.dll
 ├─ Qt5Gui.dll
 ├─ Qt5Widgets.dll
 ├─ Quartz_Varieties.ugr
 ├─ rce_ordinary_pack_01_colornation.gradient
 ├─ tatasz_pack_01.gradient
 ├─ tatasz_pack_02_colder.gradient
 ├─ tatasz_pack_02_dark.gradient
 ├─ tatasz_pack_02_warmer.gradient
 ├─ tatasz_pack_03.gradient
 ├─ tbb.dll
 │
 ├─ msvcp140.dll (optional)
 ├─ vcruntime140.dll (optional)
 ├─ vccorlib140.dll (optional)
 ├─ concrt140.dll (optional)
 │  
 └─imageformats
    │
    ├─qjpeg.dll
 └─platforms
    │
    ├─qwindows.dll
```
