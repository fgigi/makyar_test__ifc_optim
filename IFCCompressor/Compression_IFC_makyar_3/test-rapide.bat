@echo off
echo ============================================
echo Test Rapide - IFCCompressor Version Corrigee
echo ============================================
echo.

:: Verification de l'executable
if not exist "IFCCompressor.exe" (
    if exist "x64\Release\IFCCompressor.exe" (
        echo Executable trouve dans x64\Release\
        set "EXE_PATH=x64\Release\IFCCompressor.exe"
    ) else (
        echo ERREUR: Executable IFCCompressor.exe non trouve
        echo Veuillez d'abord compiler le projet avec compile.bat
        pause
        exit /b 1
    )
) else (
    echo Executable trouve dans le repertoire courant
    set "EXE_PATH=IFCCompressor.exe"
)

echo.
echo ============================================
echo TEST 1 : Affichage de l'aide
echo ============================================
echo.
"%EXE_PATH%" -h

if errorlevel 1 (
    echo ERREUR: Test d'aide echoue
    pause
    exit /b 1
)

echo.
echo ============================================
echo TEST 2 : Validation des arguments
echo ============================================
echo.
echo Test avec arguments insuffisants (doit echouer)...
"%EXE_PATH%"

echo.
echo ============================================
echo TEST 3 : Creation d'un fichier IFC minimal pour test
echo ============================================
echo.

:: Creation d'un fichier IFC minimal pour tester
echo Creation du fichier test_minimal.ifc...
(
echo ISO-10303-21;
echo HEADER;
echo FILE_DESCRIPTION^(^('ViewDefinition [CoordinationView]'^),'2;1'^);
echo FILE_NAME^('test_minimal.ifc','2024-06-03T18:00:00',^('Test'^),^('Test'^),'IFCCompressor Test','Test',''^);
echo FILE_SCHEMA^(^('IFC4'^)^);
echo ENDSEC;
echo DATA;
echo #1=IFCCARTESIANPOINT^(^(0.,0.,0.^)^);
echo #2=IFCCARTESIANPOINT^(^(0.,0.,0.^)^);
echo #3=IFCCARTESIANPOINT^(^(1.,0.,0.^)^);
echo #4=IFCCARTESIANPOINT^(^(0.,0.,0.^)^);
echo #5=IFCDIRECTION^(^(1.,0.,0.^)^);
echo #6=IFCDIRECTION^(^(1.,0.,0.^)^);
echo #7=IFCAXIS2PLACEMENT3D^(#1,#5,$^);
echo #8=IFCAXIS2PLACEMENT3D^(#2,#6,$^);
echo ENDSEC;
echo END-ISO-10303-21;
) > test_minimal.ifc

echo Fichier test_minimal.ifc cree avec succes.
echo.

echo ============================================
echo TEST 4 : Compression du fichier de test
echo ============================================
echo.
echo Compression en cours...
"%EXE_PATH%" test_minimal.ifc test_compressed.ifc -csv

if errorlevel 1 (
    echo ERREUR: Compression echouee
    pause
    exit /b 1
)

echo.
echo ============================================
echo TEST 5 : Verification des resultats
echo ============================================
echo.

if exist "test_compressed.ifc" (
    echo ✅ Fichier compresse cree avec succes
    
    :: Affichage des tailles
    for %%F in (test_minimal.ifc) do set "SIZE_ORIGINAL=%%~zF"
    for %%F in (test_compressed.ifc) do set "SIZE_COMPRESSED=%%~zF"
    
    echo Taille originale: %SIZE_ORIGINAL% octets
    echo Taille compressee: %SIZE_COMPRESSED% octets
    
    if exist "test_compressed.ifc.csv" (
        echo ✅ Rapport CSV genere avec succes
        echo.
        echo Contenu du rapport CSV:
        type test_compressed.ifc.csv
    ) else (
        echo ⚠️ Rapport CSV non trouve
    )
) else (
    echo ❌ ERREUR: Fichier compresse non cree
    pause
    exit /b 1
)

echo.
echo ============================================
echo TESTS COMPLETES AVEC SUCCES !
echo ============================================
echo.
echo Votre IFCCompressor fonctionne correctement.
echo.
echo Pour tester avec votre fichier IFC reel:
echo "%EXE_PATH%" votre_fichier.ifc fichier_compresse.ifc -csv
echo.
echo Nettoyage des fichiers de test...
if exist "test_minimal.ifc" del "test_minimal.ifc"
if exist "test_compressed.ifc" del "test_compressed.ifc"
if exist "test_compressed.ifc.csv" del "test_compressed.ifc.csv"

echo.
echo Les fichiers de test ont ete supprimes.
echo Votre IFCCompressor est pret a l'utilisation !
echo.
pause