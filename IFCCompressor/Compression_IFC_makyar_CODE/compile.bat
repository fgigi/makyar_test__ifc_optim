@echo off
echo ============================================
echo Script de compilation automatique IFCCompressor
echo Algorithme de compression IFC - Yu-Shen Liu
echo ============================================
echo.

echo Verification des fichiers source...
if not exist "IFCCompressor.h" (
    echo ERREUR: IFCCompressor.h introuvable
    pause
    exit /b 1
)
if not exist "IFCCompressor.cpp" (
    echo ERREUR: IFCCompressor.cpp introuvable
    pause
    exit /b 1
)
if not exist "main.cpp" (
    echo ERREUR: main.cpp introuvable
    pause
    exit /b 1
)
echo Tous les fichiers source sont presents.
echo.

echo ============================================
echo METHODE 1 : Compilation avec MSBuild
echo ============================================
echo.

echo Recherche de Visual Studio 2022...
if exist "%ProgramFiles%\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD_PATH=%ProgramFiles%\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
    goto :found_msbuild
)
if exist "%ProgramFiles%\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD_PATH=%ProgramFiles%\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe"
    goto :found_msbuild
)
if exist "%ProgramFiles%\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD_PATH=%ProgramFiles%\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe"
    goto :found_msbuild
)

echo Visual Studio 2022 non trouve avec MSBuild.
echo Passage a la methode alternative...
goto :method2

:found_msbuild
echo Visual Studio 2022 trouve : %MSBUILD_PATH%
echo.
echo Compilation en cours (Release x64)...
"%MSBUILD_PATH%" IFCCompressor.sln /p:Configuration=Release /p:Platform=x64 /verbosity:quiet
if errorlevel 1 (
    echo ERREUR: Compilation MSBuild echouee
    goto :method2
) else (
    echo Compilation MSBuild reussie !
    goto :success
)

:method2
echo.
echo ============================================
echo METHODE 2 : Compilation avec cl.exe
echo ============================================
echo.

echo Recherche de l'environnement de developpement...
if exist "%ProgramFiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
    call "%ProgramFiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
    goto :compile_cl
)
if exist "%ProgramFiles%\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" (
    call "%ProgramFiles%\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
    goto :compile_cl
)
if exist "%ProgramFiles%\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat" (
    call "%ProgramFiles%\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
    goto :compile_cl
)

echo Visual Studio 2022 non trouve.
echo Tentative avec les outils SDK Windows...
goto :method3

:compile_cl
echo Environnement de developpement configure.
echo Compilation en cours...
if not exist "x64" mkdir x64
if not exist "x64\Release" mkdir x64\Release

cl /EHsc /std:c++17 /O2 /MD /Fe:x64\Release\IFCCompressor.exe main.cpp IFCCompressor.cpp 2>nul
if errorlevel 1 (
    echo ERREUR: Compilation cl.exe echouee
    goto :method3
) else (
    echo Compilation cl.exe reussie !
    goto :success
)

:method3
echo.
echo ============================================
echo METHODE 3 : Compilation avec g++
echo ============================================
echo.

where g++ >nul 2>&1
if errorlevel 1 (
    echo g++ non trouve. Installation de MinGW ou MSYS2 requise.
    goto :error
)

echo g++ trouve. Compilation en cours...
if not exist "x64" mkdir x64
if not exist "x64\Release" mkdir x64\Release

g++ -std=c++17 -O2 -o x64\Release\IFCCompressor.exe main.cpp IFCCompressor.cpp
if errorlevel 1 (
    echo ERREUR: Compilation g++ echouee
    goto :error
) else (
    echo Compilation g++ reussie !
    goto :success
)

:success
echo.
echo ============================================
echo COMPILATION REUSSIE !
echo ============================================
echo.
echo Executable cree : x64\Release\IFCCompressor.exe
echo.
echo Utilisation :
echo   x64\Release\IFCCompressor.exe fichier.ifc fichier_compresse.ifc
echo   x64\Release\IFCCompressor.exe fichier.ifc fichier_compresse.ifc -csv
echo   x64\Release\IFCCompressor.exe fichier.ifc fichier_compresse.ifc -fpr 0.001 -csv
echo.
echo Pour l'aide complete :
echo   x64\Release\IFCCompressor.exe -h
echo.
echo Appuyez sur une touche pour continuer...
pause >nul
exit /b 0

:error
echo.
echo ============================================
echo ECHEC DE LA COMPILATION
echo ============================================
echo.
echo Aucune methode de compilation n'a fonctionne.
echo.
echo Solutions possibles :
echo 1. Installer Visual Studio 2022 Community (gratuit)
echo 2. Installer MinGW ou MSYS2 pour g++
echo 3. Utiliser Visual Studio Code avec les extensions C++
echo.
echo Pour Visual Studio 2022 :
echo https://visualstudio.microsoft.com/downloads/
echo.
echo Appuyez sur une touche pour quitter...
pause >nul
exit /b 1