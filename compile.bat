SET CODE_DIR=%~dp0%

cd %CODE_DIR%
git fetch
git checkout next
git reset --hard origin/next

cd ..
rmdir build /s /q
mkdir build
cd build

cmake -DCMAKE_PREFIX_PATH=C:\Qt\5.6\msvc2015 -DALLOW_NIGHTLY_BUILDS -DVERSION_SUFFIX=%1 -DPCB=9X %CODE_DIR%
"C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe" radio\src\targets\simu\opentx-simulator.vcxproj /t:Rebuild /p:Configuration=Release

cmake -DCMAKE_PREFIX_PATH=C:\Qt\5.6\msvc2015 -DALLOW_NIGHTLY_BUILDS -DVERSION_SUFFIX=%1 -DPCB=GRUVIN9X %CODE_DIR%
"C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe" radio\src\targets\simu\opentx-simulator.vcxproj /t:Rebuild /p:Configuration=Release

cmake -DCMAKE_PREFIX_PATH=C:\Qt\5.6\msvc2015 -DALLOW_NIGHTLY_BUILDS -DVERSION_SUFFIX=%1 -DPCB=MEGA2560 %CODE_DIR%
"C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe" radio\src\targets\simu\opentx-simulator.vcxproj /t:Rebuild /p:Configuration=Release

cmake -DCMAKE_PREFIX_PATH=C:\Qt\5.6\msvc2015 -DALLOW_NIGHTLY_BUILDS -DVERSION_SUFFIX=%1 -DPCB=SKY9X %CODE_DIR%
"C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe" radio\src\targets\simu\opentx-simulator.vcxproj /t:Rebuild /p:Configuration=Release

cmake -DCMAKE_PREFIX_PATH=C:\Qt\5.6\msvc2015 -DALLOW_NIGHTLY_BUILDS -DVERSION_SUFFIX=%1 -DPCB=9XRPRO %CODE_DIR%
"C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe" radio\src\targets\simu\opentx-simulator.vcxproj /t:Rebuild /p:Configuration=Release

cmake -DCMAKE_PREFIX_PATH=C:\Qt\5.6\msvc2015 -DALLOW_NIGHTLY_BUILDS -DVERSION_SUFFIX=%1 -DPCB=TARANIS %CODE_DIR%
"C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe" radio\src\targets\simu\opentx-simulator.vcxproj /t:Rebuild /p:Configuration=Release

cmake -DCMAKE_PREFIX_PATH=C:\Qt\5.6\msvc2015 -DALLOW_NIGHTLY_BUILDS -DVERSION_SUFFIX=%1 -DPCB=TARANIS -DPCBREV=REVPLUS %CODE_DIR%
"C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe" radio\src\targets\simu\opentx-simulator.vcxproj /t:Rebuild /p:Configuration=Release

cmake -DCMAKE_PREFIX_PATH=C:\Qt\5.6\msvc2015 -DALLOW_NIGHTLY_BUILDS -DVERSION_SUFFIX=%1 -DPCB=TARANIS -DPCBREV=REV9E %CODE_DIR%
"C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe" radio\src\targets\simu\opentx-simulator.vcxproj /t:Rebuild /p:Configuration=Release

cmake -DCMAKE_PREFIX_PATH=C:\Qt\5.6\msvc2015 -DALLOW_NIGHTLY_BUILDS -DVERSION_SUFFIX=%1 -DPCB=HORUS %CODE_DIR%
"C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe" radio\src\targets\simu\opentx-simulator.vcxproj /t:Rebuild /p:Configuration=Release
"C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe" companion\src\installer.vcxproj /t:Rebuild /p:Configuration=Release
