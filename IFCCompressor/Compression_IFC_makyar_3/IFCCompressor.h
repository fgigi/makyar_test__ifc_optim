#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <sstream>
#include <regex>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <iomanip>

using namespace std;

// Structure pour une instance IFC
struct IfcInstance {
    int id;
    string type;
    string content;
    bool flag;  // true si toutes les références peuvent être résolues
    
    IfcInstance() : id(0), flag(false) {}
};

// Structure MatchSet - instances déjà traitées et compressées
struct MatchSet {
    vector<string> baseInstance;
    vector<int> index;
    unordered_map<int, int> id_baseID;  // mapping ancien ID -> nouveau ID
    map<string, map<string, int>> type_content;  // type -> content -> ID
};

// Structure UnmatchSet - instances en attente de traitement
struct UnmatchSet {
    vector<IfcInstance> instance;
    vector<bool> flag;  // false = instance déjà traitée
    int totalCount;
    
    UnmatchSet() : totalCount(0) {}
};

class IFCCompressor {
private:
    static const int MAX_PRECISION;
    static const double DEFAULT_FPR;
    static bool FPR;
    static int MAX_PRECISION_VAL;
    
    MatchSet mSet;
    UnmatchSet umSet;
    
    // Fonctions utilitaires
    static void trim(string& str);
    static int IfcInstanceAna(const string& instance, string& type, string& content, vector<int>& cite);
    static string StandardContent(const string& type, const string& content, vector<int>& cite, bool islossy);
    static bool UpdateReference(string& content, const unordered_map<int, int>& rel);
    
public:
    IFCCompressor();
    ~IFCCompressor();
    
    // Fonction principale de compression
    bool CompressFile(const string& inputFile, const string& outputFile, 
                     double fprValue = 0.01, bool generateCSV = false);
    
    // Interface statique pour compatibilité
    static bool ProcessFile(const string& inputFile, const string& outputFile, 
                           double fpr = 0.01, bool csv = false);
};