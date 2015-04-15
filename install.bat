@echo off

:: Set install location
set install_location=C:\Users\Christos\workspace_vs\GeoLab

:: Copy libs
copy build-vs\Release\VVRScene.lib %install_location%\VVRFramework\lib\Release
copy build-vs\RelWithDebInfo\VVRScene.lib %install_location%\VVRFramework\lib\Debug

:: Copy dlls
copy build-vs\Release\VVRScene.dll %install_location%\Release
copy build-vs\RelWithDebInfo\VVRScene.dll %install_location%\Debug

:: Copy headers
copy src\VVRScene\canvas.h %install_location%\VVRFramework\include
copy src\VVRScene\geom.h %install_location%\VVRFramework\include
copy src\VVRScene\mesh.h %install_location%\VVRFramework\include
copy src\VVRScene\scene.h %install_location%\VVRFramework\include
copy src\VVRScene\settings.h %install_location%\VVRFramework\include
copy src\VVRScene\utils.h %install_location%\VVRFramework\include
copy src\VVRScene\vvrscenedll.h %install_location%\VVRFramework\include
