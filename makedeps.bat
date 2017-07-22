REM Make final output folder
mkdir Deps

REM Move to parent of deps folders
cd ..
git clone https://github.com/madler/zlib.git
git clone https://github.com/glennrp/libpng.git
git clone https://github.com/GNOME/libxml2.git
git clone https://github.com/g-truc/glm.git

REM Set VC paths
set INCLUDE=%INCLUDE%;C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Include;C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Include;C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Bin;

REM libjpeg
copy fractorium\Builds\MSVC\WIN32.MAK libjpeg
cd libjpeg
nmake /f makefile.vc setup-v10 CPU=i386
nmake nodebug=1 /f makefile.vc all CPU=i386
copy libjpeg.lib ..\fractorium\Deps
cd ..

REM zlib
cd zlib
nmake -f win32/Makefile.msc all
copy zlib.lib ..\fractorium\Deps
cd ..

REM libxml2
cd libxml2\win32
cscript configure.js compiler=msvc iconv=no zlib=yes include=..\..\zlib lib=..\..\fractorium\Deps
nmake /f Makefile.msvc all
cd bin.msvc
copy libxml2.dll ..\..\..\fractorium\Deps
copy libxml2.lib ..\..\..\fractorium\Deps
cd ..\..\..

REM libpng
cd libpng
mkdir zlib
copy ..\zlib\zlib.lib zlib
copy ..\zlib\zlib.h zlib
copy ..\zlib\zconf.h zlib
nmake -f scripts\makefile.vcwin32 all
copy libpng.lib ..\fractorium\Deps
cd ..

REM tbb
cd tbb\build\vs2012
devenv.exe tbb.vcxproj /upgrade
msbuild tbb.vcxproj /p:Configuration=Release
copy X64\Release\tbb.dll ..\..\..\fractorium\Deps
copy X64\Release\tbb.lib ..\..\..\fractorium\Deps
cd ..\..\..

cd fractorium
openexr.cmd
cd ..\fractorium
