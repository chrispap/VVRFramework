@echo off

:: Set install location
set install_location=C:\Users\Christos\workspace_vs\GeoLab

:: Copy libs
copy build\Release\VVRScene.lib %install_location%\VVRFramework\lib\Release
copy build\Debug\VVRScene.lib %install_location%\VVRFramework\lib\Debug
copy build\Release\GeoLib.lib %install_location%\VVRFramework\lib\Release
copy build\Debug\GeoLib.lib %install_location%\VVRFramework\lib\Debug

:: Copy dlls
copy build\Release\VVRScene.dll %install_location%\Release
copy build\Debug\VVRScene.dll %install_location%\Debug
copy build\Release\GeoLib.dll %install_location%\Release
copy build\Debug\GeoLib.dll %install_location%\Debug

:: Copy headers
copy src\VVRScene\canvas.h %install_location%\VVRFramework\include
copy src\VVRScene\geom.h %install_location%\VVRFramework\include
copy src\VVRScene\mesh.h %install_location%\VVRFramework\include
copy src\VVRScene\scene.h %install_location%\VVRFramework\include
copy src\VVRScene\settings.h %install_location%\VVRFramework\include
copy src\VVRScene\utils.h %install_location%\VVRFramework\include
copy src\VVRScene\vvrscenedll.h %install_location%\VVRFramework\include
