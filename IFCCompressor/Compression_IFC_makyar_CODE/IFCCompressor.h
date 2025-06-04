#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <cctype>

using namespace std;

struct IfcInstance {
    int id;
    string type;
    string content;
    bool flag; // true si noeud terminal, false sinon
};

struct MatchSet {
    unordered_map<int, int> id_baseID; // Mapping ancien ID vers nouvel ID
    map<string, map<string, int>> type_content; // type -> (contenu -> ID)
};

struct UnmatchSet {
    vector<IfcInstance> instance;
    vector<bool> flag;
    int totalCount;
    
    UnmatchSet() : totalCount(0) {}
};

class IFCCompressor {
private:
    static bool FPR; // Flag pour Floating Point Reduction
    static int MAX_PRECISION_VAL; // Precision pour FPR
    
    MatchSet mSet;
    UnmatchSet umSet;

    // Fonction utilitaire pour nettoyer les espaces
    static void trim(string& str);
    
    // Analyse d'une instance IFC
    static int IfcInstanceAna(const string& instance, string& type, string& content, vector<int>& cite);
    
    // Standardisation du contenu avec support FPR
    static string StandardContent(const string& type, const string& content, vector<int>& cite, bool islossy);
    
    // Mise a jour des references
    static bool UpdateReference(string& content, const unordered_map<int, int>& rel);

public:
    IFCCompressor() {}
    ~IFCCompressor() {}
    
    // Fonction principale de compression
    bool CompressFile(const string& inputFile, const string& outputFile, 
                     double fprValue = 0.01, bool generateCSV = false);
    
    // Fonction statique pour interface simplifiee
    static bool ProcessFile(const string& inputFile, const string& outputFile, 
                           double fpr = 0.01, bool csv = false);
};