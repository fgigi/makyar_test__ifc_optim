@echo off
chcp 65001 >nul
echo ========================================
echo  COMPILATION DE L'INTERFACE IFCCOMPRESSOR
echo  VERSION CORRIGEE - PROBLEME FPR RESOLU
echo ========================================
echo.

:: Verification des fichiers source
echo [1/4] Verification des fichiers source...
if not exist "IFCCompressorGUI.h" (
    echo ERREUR: IFCCompressorGUI.h introuvable
    pause
    exit /b 1
)
if not exist "IFCCompressorGUI.cpp" (
    echo ERREUR: IFCCompressorGUI.cpp introuvable
    pause
    exit /b 1
)
if not exist "main.cpp" (
    echo ERREUR: main.cpp introuvable
    pause
    exit /b 1
)
if not exist "IFCCompressorInterface.vcxproj" (
    echo ERREUR: IFCCompressorInterface.vcxproj introuvable
    pause
    exit /b 1
)
echo OK - Tous les fichiers source sont presents (VERSION CORRIGEE)

:: Verification de Visual Studio 2022
echo.
echo [2/4] Detection de Visual Studio 2022...

:: Essayer plusieurs emplacements possibles pour VS 2022
set "VS_PATH="
if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe" (
    set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe"
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe" (
    set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe"
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" (
    set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe" (
    set "VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe"
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe" (
    set "VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe"
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" (
    set "VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
)

if "%VS_PATH%"=="" (
    echo ERREUR: Visual Studio 2022 n'a pas ete trouve
    echo.
    echo Veuillez installer Visual Studio 2022 avec support C++/CLI
    echo ou utiliser Developer Command Prompt for VS 2022
    echo.
    echo Appuyez sur une touche pour essayer une compilation manuelle...
    pause >nul
    goto MANUAL_COMPILE
)

echo OK - Visual Studio 2022 trouve : %VS_PATH%

:: Creation des dossiers de sortie
echo.
echo [3/4] Preparation des dossiers...
if not exist "x64" mkdir x64
if not exist "x64\Release" mkdir x64\Release
if not exist "x64\Debug" mkdir x64\Debug
echo OK - Dossiers crees

:: Compilation
echo.
echo [4/4] Compilation en cours...
echo.
echo COMPILATION AVEC CORRECTIONS POUR LE PROBLEME FPR
echo - Validation FPR corrigee (Culture Invariante)
echo - Support format international "0.001" au lieu de "0,001"
echo - Messages d'erreur ameliores
echo.

echo Compilation Release x64 (recommandee)...

"%VS_PATH%" IFCCompressorInterface.vcxproj /p:Configuration=Release /p:Platform=x64 /p:CLRSupport=true

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo  COMPILATION REUSSIE !
    echo  PROBLEME FPR RESOLU
    echo ========================================
    echo.
    echo Executable cree : x64\Release\IFCCompressorInterface.exe
    echo.
    echo CORRECTIONS APPORTEES :
    echo - Validation FPR corrigee pour accepter "0.001" format international
    echo - Utilisation de CultureInfo::InvariantCulture
    echo - Messages d'erreur ameliores
    echo - Support complet des parametres regionaux Windows
    echo.
    echo L'interface cherchera automatiquement IFCCompressor.exe dans :
    echo ..\Compression_IFC_makyar_CODE\x64\Release\
    echo.
    
    :: Test de l'executable
    if exist "x64\Release\IFCCompressorInterface.exe" (
        echo Voulez-vous lancer l'interface maintenant ? (O/N)
        choice /c ON /n
        if !ERRORLEVEL! EQU 1 (
            echo.
            echo Lancement de l'interface corrigee...
            start "" "x64\Release\IFCCompressorInterface.exe"
            echo.
            echo Interface lancee ! Testez avec vos valeurs FPR :
            echo - 0.001 (haute precision)
            echo - 0.01 (equilibre)
            echo - 0.1 (compression maximale)
        )
    )
) else (
    echo.
    echo ========================================
    echo  ECHEC DE LA COMPILATION
    echo ========================================
    echo.
    echo La compilation a echoue. Verifiez :
    echo 1. Que .NET Framework 4.7.2 est installe
    echo 2. Que le support C++/CLI est installe dans VS 2022
    echo 3. Que tous les composants Windows Forms sont installes
    echo.
    goto MANUAL_COMPILE
)

echo.
pause
exit /b 0

:MANUAL_COMPILE
echo.
echo ========================================
echo  COMPILATION MANUELLE
echo ========================================
echo.
echo Instructions pour compiler manuellement :
echo.
echo 1. Ouvrez Visual Studio 2022
echo 2. Ouvrez le fichier IFCCompressorInterface.sln
echo 3. Selectionnez Release x64 dans la barre d'outils
echo 4. Allez dans Generer ^> Generer la solution (Ctrl+Maj+B)
echo.
echo Si vous avez des erreurs :
echo - Verifiez que le support C++/CLI est installe
echo - Installez .NET Framework 4.7.2 SDK
echo - Activez Windows Forms dans les composants VS
echo.
echo REMARQUE IMPORTANTE :
echo Cette version CORRIGE le probleme de validation FPR
echo que vous aviez rencontre precedemment.
echo.
pause
exit /b 1