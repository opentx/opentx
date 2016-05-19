SET SRCDIR=%~dp0%\..\..

cd %SRCDIR%
git fetch
git checkout next
git reset --hard origin/next

cd ..
rmdir build /s /q
mkdir build
cd build

SET COMMON_OPTIONS=-DCMAKE_PREFIX_PATH=C:\Qt\5.6\msvc2015 -DALLOW_NIGHTLY_BUILDS=YES -DVERSION_SUFFIX=%1 -DGVARS=YES -DHELI=YES
SET STM32_OPTIONS=%COMMON_OPTIONS% -DLUA=YES

cmake %COMMON_OPTIONS% -DPCB=9X %SRCDIR%
"C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe" radio\src\targets\simu\libsimulator.vcxproj /t:Rebuild /p:Configuration=Release

cmake %COMMON_OPTIONS% -DPCB=GRUVIN9X %SRCDIR%
"C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe" radio\src\targets\simu\libsimulator.vcxproj /t:Rebuild /p:Configuration=Release

cmake %COMMON_OPTIONS% -DPCB=MEGA2560 %SRCDIR%
"C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe" radio\src\targets\simu\libsimulator.vcxproj /t:Rebuild /p:Configuration=Release

cmake %COMMON_OPTIONS% -DPCB=SKY9X %SRCDIR%
"C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe" radio\src\targets\simu\libsimulator.vcxproj /t:Rebuild /p:Configuration=Release

cmake %COMMON_OPTIONS% -DPCB=9XRPRO %SRCDIR%
"C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe" radio\src\targets\simu\libsimulator.vcxproj /t:Rebuild /p:Configuration=Release

cmake %STM32_OPTIONS% -DPCB=TARANIS %SRCDIR%
"C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe" radio\src\targets\simu\libsimulator.vcxproj /t:Rebuild /p:Configuration=Release

cmake %STM32_OPTIONS% -DPCB=TARANIS -DPCBREV=REVPLUS %SRCDIR%
"C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe" radio\src\targets\simu\libsimulator.vcxproj /t:Rebuild /p:Configuration=Release

cmake %STM32_OPTIONS% -DPCB=TARANIS -DPCBREV=REV9E %SRCDIR%
"C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe" radio\src\targets\simu\libsimulator.vcxproj /t:Rebuild /p:Configuration=Release

cmake %STM32_OPTIONS% -DPCB=HORUS %SRCDIR%
"C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe" radio\src\targets\simu\libsimulator.vcxproj /t:Rebuild /p:Configuration=Release
"C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe" companion\src\installer.vcxproj /t:Rebuild /p:Configuration=Release
