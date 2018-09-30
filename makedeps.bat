REM Make final output folder
mkdir Deps

REM Move to parent of deps folders
cd ..
git clone https://github.com/madler/zlib.git
git clone https://github.com/glennrp/libpng.git
git clone https://github.com/GNOME/libxml2.git
git clone https://github.com/g-truc/glm.git
git clone -b tbb_2019 https://github.com/01org/tbb.git
git clone -b v2.3.0 https://github.com/openexr/openexr.git

REM libjpeg
copy fractorium\Builds\MSVC\WIN32.MAK libjpeg
cd libjpeg
nmake /f makefile.vc setup-v15 CPU=i386
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

REM tbb
cd tbb\build\vs2013
set "curdir=%cd%"
devenv.exe makefile.sln /upgrade
cd %curdir%
REM Change PlatformToolset and WindowsTargetPlatformVersion to match whatever your version of Visual Studio supports. You can find this by opening makefile.sln in tbb\build\vs2013
msbuild tbb.vcxproj /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v141 /p:WindowsTargetPlatformVersion=10.0.17134.0
copy X64\Release\tbb.dll ..\..\..\fractorium\Deps
copy X64\Release\tbb.lib ..\..\..\fractorium\Deps
cd ..\..\..

REM openexr
cd openexr
SET current=%cd%

if not exist ".\output" mkdir .\output

cd ..\OpenEXR

cmake -G "Visual Studio 15 2017 Win64"^
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

copy %current%\output\lib\Half-2_3.lib %current%\..\fractorium\Deps\Half-2_3.lib
copy %current%\output\lib\Iex-2_3.lib %current%\..\fractorium\Deps\Iex-2_3.lib
copy %current%\output\lib\IexMath-2_3.lib %current%\..\fractorium\Deps\IexMath-2_3.lib
copy %current%\output\lib\IlmImf-2_3.lib %current%\..\fractorium\Deps\IlmImf-2_3.lib
copy %current%\output\lib\IlmImfUtil-2_3.lib %current%\..\fractorium\Deps\IlmImfUtil-2_3.lib
copy %current%\output\lib\IlmThread-2_3.lib %current%\..\fractorium\Deps\IlmThread-2_3.lib
copy %current%\output\lib\Imath-2_3.lib %current%\..\fractorium\Deps\Imath-2_3.lib
copy %current%\OpenEXR\IlmImf\Release\IlmImf-2_3.dll %current%\..\fractorium\Deps\IlmImf-2_3.dll

xcopy %current%\output\Include %current%\..\fractorium\Deps\Include\ /S /Y
xcopy %current%\output\bin\*.dll %current%\..\fractorium\Deps\ /Y

REM IlmImfUtil is not needed.
del ..\fractorium\Deps\IlmImfUtil-2_3.dll

cd ..\fractorium
