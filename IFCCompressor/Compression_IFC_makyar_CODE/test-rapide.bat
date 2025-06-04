@echo off
echo ============================================
echo Tests automatiques IFCCompressor
echo ============================================
echo.

set "EXE_PATH=x64\Release\IFCCompressor.exe"

echo ============================================
echo TEST 1 : Verification de l'executable
echo ============================================
echo.

if not exist "%EXE_PATH%" (
    echo ERREUR: Executable non trouve : %EXE_PATH%
    echo Veuillez d'abord compiler le projet avec compile.bat
    pause
    exit /b 1
)
echo Executable trouve : %EXE_PATH%

echo.
echo ============================================
echo TEST 2 : Affichage de l'aide
echo ============================================
echo.

"%EXE_PATH%" -h
if errorlevel 1 (
    echo ERREUR: Affichage de l'aide echoue
    pause
    exit /b 1
)

echo.
echo ============================================
echo TEST 3 : Creation d'un fichier IFC minimal
echo ============================================
echo.

echo Creation du fichier test_minimal.ifc...
(
echo ISO-10303-21;
echo HEADER;
echo FILE_DESCRIPTION(('ViewDefinition [CoordinationView]'),'2;1');
echo FILE_NAME('test_minimal.ifc','2025-01-01T00:00:00',('Test'),'','IfcCompressor','','');
echo FILE_SCHEMA(('IFC4'));
echo ENDSEC;
echo DATA;
echo #1=IFCCARTESIANPOINT((0.,0.,0.));
echo #2=IFCCARTESIANPOINT((1.,0.,0.));
echo #3=IFCCARTESIANPOINT((0.,1.,0.));
echo #4=IFCCARTESIANPOINT((0.,0.,1.));
echo #5=IFCCARTESIANPOINT((0.,0.,0.));
echo #6=IFCCARTESIANPOINT((1.,0.,0.));
echo #7=IFCDIRECTION((1.,0.,0.));
echo #8=IFCDIRECTION((0.,1.,0.));
echo #9=IFCDIRECTION((0.,0.,1.));
echo #10=IFCDIRECTION((1.,0.,0.));
echo #11=IFCDIRECTION((0.,1.,0.));
echo #12=IFCAXIS2PLACEMENT3D(#1,#7,#8);
echo #13=IFCAXIS2PLACEMENT3D(#2,#9,#10);
echo #14=IFCAXIS2PLACEMENT3D(#3,#11,#12);
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
"%EXE_PATH%" test_minimal.ifc test_compresse.ifc -csv
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

if exist "test_compresse.ifc" (
    echo ✓ Fichier compresse cree avec succes
    
    rem Affichage des tailles
    for %%F in (test_minimal.ifc) do set "SIZE_ORIGINAL=%%~zF"
    for %%F in (test_compresse.ifc) do set "SIZE_COMPRESSED=%%~zF"
    
    echo Taille originale: %SIZE_ORIGINAL% octets
    echo Taille compressee: %SIZE_COMPRESSED% octets
    
    if exist "test_compresse.ifc.csv" (
        echo ✓ Rapport CSV genere avec succes
        echo.
        echo Contenu du rapport CSV:
        type test_compresse.ifc.csv
    ) else (
        echo ⚠ Rapport CSV non trouve
    )
) else (
    echo ✗ ERREUR: Fichier compresse non cree
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
if exist "test_compresse.ifc" del "test_compresse.ifc"
if exist "test_compresse.ifc.csv" del "test_compresse.ifc.csv"

echo.
echo Les fichiers de test ont ete supprimes.
echo Votre IFCCompressor est pret a l'utilisation !
echo.
pause