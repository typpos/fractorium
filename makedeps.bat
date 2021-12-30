REM Make final output folder
mkdir Deps

REM uncomment if cl message is "Cannot open include file: 'stddef.h'"
REM C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat

REM Move to parent of deps folders
cd ..
git clone https://github.com/madler/zlib.git
git clone https://github.com/glennrp/libpng.git
git clone https://github.com/GNOME/libxml2.git
git clone https://github.com/g-truc/glm.git
git clone -b v3.1.3 https://github.com/AcademySoftwareFoundation/openexr.git

REM libjpeg
copy fractorium\Builds\MSVC\WIN32.MAK libjpeg
cd libjpeg
nmake /f makefile.vc setup-v16 CPU=i386
nmake nodebug=1 /f makefile.vc libjpeg.lib CPU=i386
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

REM openexr
cd openexr
SET current=%cd%

if not exist ".\output" mkdir .\output

REM cd ..\OpenEXR

cmake -G "Visual Studio 16 2019"^
      -A x64^
      -DCMAKE_PREFIX_PATH="%current%\output"^
      -DCMAKE_INSTALL_PREFIX="%current%\output"^
      -DILMBASE_PACKAGE_PREFIX="%current%\output" ^
      -DZLIB_ROOT="..\zlib"^
	  -DOPENEXR_BUILD_SHARED_LIBS="ON"^
	  -DOPENEXR_BUILD_VIEWERS="OFF"^
	  -DOPENEXR_BUILD_STATIC_LIBS="OFF"^
	  -DOPENEXR_BUILD_PYTHON_LIBS="OFF"^
	  -DOPENEXR_ENABLE_TESTS="OFF"^
      .\

cmake --build . --target install --config Release

cd %current%

xcopy %current%\output\Include %current%\..\fractorium\Deps\Include\ /S /Y
xcopy %current%\output\bin\Iex-3_1.dll %current%\..\fractorium\Deps\ /Y
xcopy %current%\output\bin\IlmThread-3_1.dll %current%\..\fractorium\Deps\ /Y
xcopy %current%\output\bin\Imath-3_1.dll %current%\..\fractorium\Deps\ /Y
xcopy %current%\output\bin\OpenEXR-3_1.dll %current%\..\fractorium\Deps\ /Y
xcopy %current%\output\lib\*.lib %current%\..\fractorium\Deps\ /Y

cd ..\fractorium
