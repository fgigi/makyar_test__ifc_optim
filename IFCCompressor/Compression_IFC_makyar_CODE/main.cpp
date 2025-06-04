#include "IFCCompressor.h"
#include <iostream>
#include <iomanip>

using namespace std;

void showHelp() {
    cout << "IFCCompressor - Algorithme de compression IFC" << endl;
    cout << "Version 1.0 - Base sur l'algorithme de Yu-Shen Liu" << endl;
    cout << "==============================================" << endl;
    cout << "" << endl;
    cout << "Usage: IFCCompressor <fichier_entree> <fichier_sortie> [options]" << endl;
    cout << "" << endl;
    cout << "Options:" << endl;
    cout << "  -fpr <valeur>  Parametre FPR (Floating Point Reduction, defaut: 0.01)" << endl;
    cout << "  -csv           Generer un rapport CSV des statistiques" << endl;
    cout << "  -h, --help     Afficher cette aide" << endl;
    cout << "" << endl;
    cout << "Exemples:" << endl;
    cout << "  IFCCompressor model.ifc compressed.ifc" << endl;
    cout << "  IFCCompressor model.ifc compressed.ifc -csv" << endl;
    cout << "  IFCCompressor model.ifc compressed.ifc -fpr 0.001 -csv" << endl;
}

int main(int argc, char* argv[]) {
    cout << "IFCCompressor - Algorithme de compression IFC" << endl;
    cout << "Version 1.0 - Base sur l'algorithme de Yu-Shen Liu" << endl;
    cout << "==============================================" << endl;
    cout << "" << endl;
    
    // Verification du nombre d'arguments minimum
    if (argc < 3) {
        cerr << "Erreur: Nombre d'arguments insuffisant." << endl;
        cout << "Utilisez -h ou --help pour l'aide." << endl;
        return 1;
    }
    
    // Verification de l'aide
    string arg1 = argv[1];
    if (arg1 == "-h" || arg1 == "--help") {
        showHelp();
        return 0;
    }
    
    // Recuperation des arguments obligatoires
    string inputFile = argv[1];
    string outputFile = argv[2];
    
    // Parametres optionnels
    double fprValue = 0.01; // Valeur par defaut
    bool generateCSV = false;
    
    // Parsing des options
    for (int i = 3; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-csv") {
            generateCSV = true;
        } else if (arg == "-fpr" && i + 1 < argc) {
            try {
                fprValue = stod(argv[i + 1]);
                i++; // Ignorer le prochain argument car c'est la valeur FPR
            } catch (const exception& e) {
                cerr << "Erreur: Valeur FPR invalide: " << argv[i + 1] << endl;
                return 1;
            }
        } else if (arg == "-h" || arg == "--help") {
            showHelp();
            return 0;
        } else {
            cerr << "Erreur: Option inconnue: " << arg << endl;
            cout << "Utilisez -h ou --help pour l'aide." << endl;
            return 1;
        }
    }
    
    // Validation des fichiers
    if (inputFile.empty() || outputFile.empty()) {
        cerr << "Erreur: Fichiers d'entree et de sortie requis." << endl;
        return 1;
    }
    
    // Affichage des parametres
    cout << "Initialisation de la compression..." << endl;
    cout << "Parametres:" << endl;
    cout << "  - Fichier d'entree: " << inputFile << endl;
    cout << "  - Fichier de sortie: " << outputFile << endl;
    cout << "  - Parametre FPR: " << fprValue << endl;
    cout << "  - Generation CSV: " << (generateCSV ? "Oui" : "Non") << endl;
    cout << "" << endl;
    
    // Demarrage de la compression
    cout << "Demarrage de la compression du fichier IFC..." << endl;
    cout << "Fichier d'entree: " << inputFile << endl;
    cout << "Fichier de sortie: " << outputFile << endl;
    cout << "Parametre FPR: " << fixed << setprecision(4) << fprValue << endl;
    cout << "" << endl;
    
    try {
        // Appel de la fonction de compression
        bool success = IFCCompressor::ProcessFile(inputFile, outputFile, fprValue, generateCSV);
        
        if (success) {
            cout << "" << endl;
            cout << "Compression terminee avec succes!" << endl;
            cout << "Fichier compresse sauvegarde: " << outputFile << endl;
            if (generateCSV) {
                cout << "Rapport CSV genere: " << outputFile << ".csv" << endl;
            }
            return 0;
        } else {
            cerr << "Erreur: La compression a echoue." << endl;
            return 1;
        }
    } catch (const exception& e) {
        cerr << "Erreur lors de la compression: " << e.what() << endl;
        return 1;
    } catch (...) {
        cerr << "Erreur inconnue lors de la compression." << endl;
        return 1;
    }
}