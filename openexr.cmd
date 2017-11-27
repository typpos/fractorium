ECHO building OpenEXR

cd..
if not exist ".\openexr" ^
	mkdir openexr
	
git clone git://github.com/meshula/openexr.git

cd openexr
git pull

SET current=%cd%

if not exist ".\output" mkdir .\output

cd IlmBase

cmake -G "Visual Studio 15 2017 Win64"^
      -DCMAKE_PREFIX_PATH="%current%\output"^
      -DCMAKE_INSTALL_PREFIX="%current%\output"^
      .\

cmake --build . --target install --config Release -- /maxcpucount:8

cd ..\OpenEXR

cmake -G "Visual Studio 15 2017 Win64"^
      -DCMAKE_PREFIX_PATH="%current%\output"^
      -DCMAKE_INSTALL_PREFIX="%current%\output"^
      -DILMBASE_PACKAGE_PREFIX="%current%\output" ^
      -DZLIB_ROOT="%current%\..\zlib"^
      .\

cmake --build . --target install --config Release

cd %current%

copy %current%\output\lib\Half.lib %current%\..\fractorium\Deps\Half.lib
copy %current%\output\lib\Iex-2_2.lib %current%\..\fractorium\Deps\Iex.lib
copy %current%\output\lib\IexMath-2_2.lib %current%\..\fractorium\Deps\IexMath.lib
copy %current%\output\lib\IlmImf-2_2.lib %current%\..\fractorium\Deps\IlmImf.lib
copy %current%\output\lib\IlmImfUtil-2_2.lib %current%\..\fractorium\Deps\IlmImfUtil.lib
copy %current%\output\lib\IlmThread-2_2.lib %current%\..\fractorium\Deps\IlmThread.lib
copy %current%\output\lib\Imath-2_2.lib %current%\..\fractorium\Deps\Imath.lib
xcopy %current%\output\Include %current%\..\fractorium\Deps\Include\ /S /Y
xcopy %current%\output\lib\*.dll %current%\..\fractorium\Deps\ /Y
