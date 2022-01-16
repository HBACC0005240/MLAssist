for /f "usebackq tokens=*" %%i in (`vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
  set InstallDir=%%i
)

if exist "%InstallDir%\Common7\Tools\vsdevcmd.bat" (
    
    "%InstallDir%\Common7\Tools\vsdevcmd.bat" -arch=x86

    cd qhttp

    D:\Environment\Qt\Qt5.15.2\5.15.2\msvc2019\bin\qmake qhttp.pro -spec win32-msvc "CONFIG+=qtquickcompiler"

   D:\Environment\Qt\Qt5.15.2\Tools\QtCreator\bin\jom\jom.exe -f MakeFile qmake_all

   D:\Environment\Qt\Qt5.15.2\Tools\QtCreator\bin\jom\jom.exe

   D:\Environment\Qt\Qt5.15.2\Tools\QtCreator\bin\jom\jom.exe clean

    copy "xbin\qhttp.dll" "..\bin\"
)