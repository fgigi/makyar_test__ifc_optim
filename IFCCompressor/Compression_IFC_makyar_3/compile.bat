@echo off
echo ============================================
echo Script de compilation automatique IFCCompressor
echo Version corrigee - Base sur l'algorithme de Yu-Shen Liu
echo ============================================
echo.

:: Verification de la presence des fichiers source
if not exist "IFCCompressor.h" (
    echo ERREUR: Fichier IFCCompressor.h manquant
    echo Assurez-vous que tous les fichiers source sont presents
    pause
    exit /b 1
)

if not exist "IFCCompressor.cpp" (
    echo ERREUR: Fichier IFCCompressor.cpp manquant
    echo Assurez-vous que tous les fichiers source sont presents
    pause
    exit /b 1
)

if not exist "main.cpp" (
    echo ERREUR: Fichier main.cpp manquant
    echo Assurez-vous que tous les fichiers source sont presents
    pause
    exit /b 1
)

echo Tous les fichiers source sont presents.
echo.

:: Detection de Visual Studio 2022
echo Detection de Visual Studio 2022...
set "VS2022_PATH="
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
    set "VS2022_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
    echo Visual Studio 2022 Community trouve
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" (
    set "VS2022_PATH=C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
    echo Visual Studio 2022 Professional trouve
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat" (
    set "VS2022_PATH=C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
    echo Visual Studio 2022 Enterprise trouve
)

if "%VS2022_PATH%"=="" (
    echo ERREUR: Visual Studio 2022 non trouve
    echo.
    echo Tentative de compilation directe avec cl.exe...
    goto DIRECT_COMPILE
)

echo.
echo Configuration de l'environnement Visual Studio 2022...
call "%VS2022_PATH%"

if errorlevel 1 (
    echo ERREUR: Echec de la configuration de l'environnement
    echo Tentative de compilation directe...
    goto DIRECT_COMPILE
)

echo.
echo Compilation avec MSBuild (methode recommandee)...
msbuild IFCCompressor.sln /p:Configuration=Release /p:Platform=x64 /m

if errorlevel 1 (
    echo.
    echo Echec de MSBuild, tentative avec cl.exe...
    goto DIRECT_COMPILE
) else (
    echo.
    echo === COMPILATION REUSSIE ===
    echo Executable genere: x64\Release\IFCCompressor.exe
    goto SUCCESS
)

:DIRECT_COMPILE
echo.
echo Compilation directe avec cl.exe...
cl /EHsc /std:c++17 /O2 /MD main.cpp IFCCompressor.cpp /Fe:IFCCompressor.exe

if errorlevel 1 (
    echo.
    echo ERREUR: Compilation echouee
    echo.
    echo Solutions possibles:
    echo 1. Installez Visual Studio 2022 avec les outils C++
    echo 2. Verifiez que tous les fichiers source sont presents
    echo 3. Ouvrez "Developer Command Prompt for VS 2022" manuellement
    echo.
    pause
    exit /b 1
) else (
    echo.
    echo === COMPILATION REUSSIE ===
    echo Executable genere: IFCCompressor.exe
    goto SUCCESS
)

:SUCCESS
echo.
echo ============================================
echo Compilation terminee avec succes!
echo ============================================
echo.
echo Pour tester le programme:
echo   IFCCompressor.exe -h
echo.
echo Pour compresser un fichier IFC:
echo   IFCCompressor.exe votre_fichier.ifc fichier_compresse.ifc
echo.
echo Options avancees:
echo   IFCCompressor.exe votre_fichier.ifc fichier_compresse.ifc -csv
echo   IFCCompressor.exe votre_fichier.ifc fichier_compresse.ifc -fpr 0.001 -csv
echo.
pause