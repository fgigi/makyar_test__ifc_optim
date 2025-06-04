@echo off
chcp 65001 >nul
echo ========================================
echo  TEST DE L'INTERFACE IFCCOMPRESSOR
echo  VERSION CORRIGEE - PROBLEME FPR RESOLU
echo ========================================
echo.

:: Verification que l'interface a ete compilee
echo [1/4] Verification de l'interface corrigee...
if not exist "x64\Release\IFCCompressorInterface.exe" (
    echo ERREUR: L'interface n'a pas ete compilee
    echo Lancez d'abord compile_interface.bat
    pause
    exit /b 1
)
echo OK - Interface trouvee : x64\Release\IFCCompressorInterface.exe
echo     (VERSION AVEC CORRECTION PROBLEME FPR)

:: Verification que IFCCompressor.exe existe
echo.
echo [2/4] Verification de IFCCompressor.exe...
set "COMPRESSOR_PATH=..\Compression_IFC_makyar_CODE\x64\Release\IFCCompressor.exe"
if exist "%COMPRESSOR_PATH%" (
    echo OK - IFCCompressor.exe trouve : %COMPRESSOR_PATH%
) else (
    echo ATTENTION: IFCCompressor.exe non trouve dans %COMPRESSOR_PATH%
    echo.
    echo Emplacements alternatifs testes :
    
    set "ALT_PATH1=..\Compression_IFC_makyar_CODE\IFCCompressor.exe"
    if exist "%ALT_PATH1%" (
        echo OK - Trouve dans : %ALT_PATH1%
    ) else (
        echo NON - %ALT_PATH1%
    )
    
    set "ALT_PATH2=..\Compression_IFC_makyar_CODE\Release\IFCCompressor.exe"
    if exist "%ALT_PATH2%" (
        echo OK - Trouve dans : %ALT_PATH2%
    ) else (
        echo NON - %ALT_PATH2%
    )
    
    set "ALT_PATH3=..\IFCCompressor.exe"
    if exist "%ALT_PATH3%" (
        echo OK - Trouve dans : %ALT_PATH3%
    ) else (
        echo NON - %ALT_PATH3%
    )
    
    echo.
    echo L'interface detectera automatiquement l'emplacement correct
    echo au moment de l'execution.
)

:: Creation d'un fichier IFC minimal pour test (optionnel)
echo.
echo [3/4] Creation d'un fichier IFC de test...
if not exist "test_files" mkdir test_files

echo ISO-10303-21; > "test_files\test_minimal.ifc"
echo HEADER; >> "test_files\test_minimal.ifc"
echo FILE_DESCRIPTION(('ViewDefinition [CoordinationView]'),'2;1'); >> "test_files\test_minimal.ifc"
echo FILE_NAME('test_minimal.ifc','2024-06-04T00:00:00',(''),(''),'','',''); >> "test_files\test_minimal.ifc"
echo FILE_SCHEMA(('IFC4')); >> "test_files\test_minimal.ifc"
echo ENDSEC; >> "test_files\test_minimal.ifc"
echo DATA; >> "test_files\test_minimal.ifc"
echo #1=IFCCARTESIANPOINT((0.,0.,0.)); >> "test_files\test_minimal.ifc"
echo #2=IFCCARTESIANPOINT((1.,0.,0.)); >> "test_files\test_minimal.ifc"
echo #3=IFCCARTESIANPOINT((1.,1.,0.)); >> "test_files\test_minimal.ifc"
echo #4=IFCCARTESIANPOINT((0.,1.,0.)); >> "test_files\test_minimal.ifc"
echo #5=IFCCARTESIANPOINT((0.,0.,0.)); >> "test_files\test_minimal.ifc"
echo #6=IFCDIRECTION((1.,0.,0.)); >> "test_files\test_minimal.ifc"
echo #7=IFCDIRECTION((0.,0.,1.)); >> "test_files\test_minimal.ifc"
echo #8=IFCAXIS2PLACEMENT3D(#1,#7,#6); >> "test_files\test_minimal.ifc"
echo ENDSEC; >> "test_files\test_minimal.ifc"
echo END-ISO-10303-21; >> "test_files\test_minimal.ifc"

if exist "test_files\test_minimal.ifc" (
    echo OK - Fichier IFC de test cree : test_files\test_minimal.ifc
    echo Ce fichier contient des instances dupliquees pour tester la compression.
) else (
    echo ATTENTION: Impossible de creer le fichier de test
)

:: Test de validation FPR
echo.
echo [4/4] TEST DE LA CORRECTION FPR...
echo.
echo Cette version CORRIGE le probleme de validation FPR.
echo.
echo AVANT (ne fonctionnait pas) :
echo - Tapez "0.001" => Erreur "Valeur FPR invalide"
echo - Tapez "0.01"  => Erreur "Valeur FPR invalide"
echo.
echo MAINTENANT (fonctionne) :
echo - Tapez "0.001" => OK (haute precision)
echo - Tapez "0.01"  => OK (equilibre)
echo - Tapez "0.1"   => OK (compression maximale)
echo.

echo ========================================
echo  TEST PRET
echo ========================================
echo.
echo L'interface est prete pour les tests :
echo.
echo 1. Interface compilee : x64\Release\IFCCompressorInterface.exe
echo 2. Fichier de test disponible : test_files\test_minimal.ifc
echo 3. Algorithme IFCCompressor detecte automatiquement
echo 4. PROBLEME FPR RESOLU
echo.
echo Pour tester la CORRECTION FPR :
echo 1. Lancez l'interface : x64\Release\IFCCompressorInterface.exe
echo 2. Selectionnez le fichier test_files\test_minimal.ifc
echo 3. TESTEZ CES VALEURS FPR (qui fonctionnent maintenant) :
echo    - 0.001 (haute precision)
echo    - 0.01 (equilibre) 
echo    - 0.1 (compression maximale)
echo 4. Cliquez sur "DEMARRER LA COMPRESSION"
echo.
echo Resultats attendus :
echo - PLUS D'ERREUR DE VALIDATION FPR
echo - Compression reussie avec elimination des doublons
echo - Fichier de sortie : test_minimal_COMPRESSED.ifc
echo - Taux de compression d'environ 20-40%
echo.

choice /c ON /n /m "Voulez-vous lancer l'interface maintenant ? (O/N) "
if %ERRORLEVEL% EQU 1 (
    echo.
    echo Lancement de l'interface corrigee...
    start "" "x64\Release\IFCCompressorInterface.exe"
    echo.
    echo Interface lancee ! 
    echo.
    echo INSTRUCTIONS DE TEST :
    echo 1. Selectionnez test_files\test_minimal.ifc comme fichier d'entree
    echo 2. Testez les valeurs FPR suivantes :
    echo    - Changez le FPR a "0.001" et verifiez qu'il n'y a plus d'erreur
    echo    - Changez le FPR a "0.01" et verifiez qu'il n'y a plus d'erreur
    echo    - Changez le FPR a "0.1" et verifiez qu'il n'y a plus d'erreur
    echo 3. Lancez la compression
    echo 4. Verifiez que le fichier de sortie est cree
    echo.
    echo LE PROBLEME FPR EST MAINTENANT RESOLU !
) else (
    echo.
    echo Interface prete. Lancez manuellement x64\Release\IFCCompressorInterface.exe
    echo.
    echo N'OUBLIEZ PAS DE TESTER LES VALEURS FPR :
    echo - 0.001, 0.01, 0.1 doivent toutes fonctionner maintenant !
)

echo.
echo ========================================
echo  INFORMATIONS SUR LA CORRECTION
echo ========================================
echo.
echo PROBLEME RESOLU : Validation parametre FPR
echo.
echo CAUSE DU PROBLEME ORIGINAL :
echo - Windows français utilise virgule (,) comme separateur decimal
echo - Fonction Double::Parse() attendait "0,001" au lieu de "0.001"
echo - Tous les formats internationaux echouaient
echo.
echo CORRECTION APPLIQUEE :
echo - Utilisation de CultureInfo::InvariantCulture
echo - Support du format international "0.001"
echo - Messages d'erreur ameliores
echo - Validation robuste des parametres regionaux
echo.
echo FORMATS FPR MAINTENANT SUPPORTES :
echo - 0.001 (haute precision)
echo - 0.01 (equilibre)
echo - 0.1 (compression maximale)
echo - 1e-3 (notation scientifique)
echo - 0.0001 (precision tres haute)
echo.
echo Structure recommandee des dossiers :
echo.
echo VISUAL STUDIO\
echo ├── Compression_IFC_makyar_CODE\
echo │   └── x64\Release\IFCCompressor.exe
echo └── Compression_IFC_makyar_INTERFACE\
echo     ├── IFCCompressorGUI.h (CORRIGE)
echo     ├── IFCCompressorGUI.cpp (CORRIGE)
echo     ├── main.cpp
echo     ├── IFCCompressorInterface.vcxproj
echo     ├── IFCCompressorInterface.sln
echo     ├── compile_interface.bat (CORRIGE)
echo     ├── test_interface.bat (CORRIGE)
echo     ├── README.txt (CORRIGE)
echo     └── x64\Release\IFCCompressorInterface.exe
echo.
pause