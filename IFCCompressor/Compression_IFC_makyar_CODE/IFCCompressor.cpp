#include "IFCCompressor.h"

// Definition des variables statiques
bool IFCCompressor::FPR = false;
int IFCCompressor::MAX_PRECISION_VAL = 6;

void IFCCompressor::trim(string& str) {
    if (str.empty()) return;
    
    // Supprimer les espaces au debut
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == string::npos) {
        str.clear();
        return;
    }
    
    // Supprimer les espaces a la fin
    size_t end = str.find_last_not_of(" \t\r\n");
    str = str.substr(start, end - start + 1);
}

int IFCCompressor::IfcInstanceAna(const string& instance, string& type, string& content, vector<int>& cite) {
    cite.clear();
    string instanceCopy = instance;
    trim(instanceCopy);
    
    string index = "";
    int s = instanceCopy.find_first_of('=');
    index = instanceCopy.substr(1, s - 1);
    int id = atoi(index.c_str());
    
    content = instanceCopy.substr(s + 1, instanceCopy.length() - s - 1);
    s = content.find_first_of('(');
    type = content.substr(0, s);
    trim(type);
    
    content = content.substr(s + 1, content.find_last_of(')') - s - 1);
    content = StandardContent(type, content, cite, FPR);
    
    return id;
}

string IFCCompressor::StandardContent(const string& type, const string& content, vector<int>& cite, bool islossy) {
    cite.clear();
    string pp, sptr, str = "";
    char* ptr;
    double dat;
    stringstream ss(content);
    stringstream sss;
    sss.setf(ios::fixed);
    sss.precision(MAX_PRECISION_VAL);
    
    while (getline(ss, pp, ',')) {
        trim(pp);
        if (pp[0] == '(') {
            str += '(';
            bool flag = false;
            if (pp[pp.size() - 1] == ')') {
                pp = pp.substr(1, pp.size() - 2);
                flag = true;
            } else {
                pp = pp.substr(1, pp.size() - 1);
            }
            
            if (pp[0] == '#') {
                cite.push_back(atoi(pp.substr(1, pp.size() - 1).c_str()));
                str += pp;
            } else {
                std::string string_to_parse_block1 = pp; // pp is already substr'd here
                double parsed_double_val_block1;
                std::stringstream parser_ss_block1(string_to_parse_block1);
                parser_ss_block1.imbue(std::locale::classic());

                if (parser_ss_block1 >> parsed_double_val_block1) {
                    dat = parsed_double_val_block1;
                    char first_remaining_char_block1 = 0;
                    if (parser_ss_block1.get(first_remaining_char_block1)) {
                        sptr = std::string(1, first_remaining_char_block1);
                        // For full remainder: std::string temp_rem; parser_ss_block1 >> temp_rem; sptr += temp_rem;
                    } else {
                        sptr = "";
                    }
                } else {
                    dat = 0.0; // Conversion failed
                    sptr = string_to_parse_block1; // Remainder is original string
                }
                if (type == "IFCCARTESIANPOINT" || (sptr != ")" && sptr != "") || !islossy) {
                    str += pp;
                } else {
                    sss.str("");
                    sss << dat;
                    str += sss.str();
                }
            }
            if (flag) str += ')';
        } else if (pp[pp.size() - 1] == ')') {
            pp = pp.substr(0, pp.size() - 1);
            if (pp[0] == '#') {
                cite.push_back(atoi(pp.substr(1, pp.size() - 1).c_str()));
                str += pp;
            } else {
                // pp was already updated: pp = pp.substr(0, pp.size() - 1);
                std::string string_to_parse_block2 = pp;
                double parsed_double_val_block2;
                std::stringstream parser_ss_block2(string_to_parse_block2);
                parser_ss_block2.imbue(std::locale::classic());

                if (parser_ss_block2 >> parsed_double_val_block2) {
                    dat = parsed_double_val_block2;
                    char first_remaining_char_block2 = 0;
                    if (parser_ss_block2.get(first_remaining_char_block2)) {
                        sptr = std::string(1, first_remaining_char_block2);
                    } else {
                        sptr = "";
                    }
                } else {
                    dat = 0.0; // Conversion failed
                    sptr = string_to_parse_block2; // Remainder is original string
                }
                if (type == "IFCCARTESIANPOINT" || (sptr != ")" && sptr != "") || !islossy) {
                    str += pp;
                } else {
                    sss.str("");
                    sss << dat;
                    str += sss.str();
                }
            }
            str += ')';
        } else {
            if (pp[0] == '#') {
                cite.push_back(atoi(pp.substr(1, pp.size() - 1).c_str()));
                str += pp;
            } else {
                std::string string_to_parse_block3 = pp; // pp is used directly
                double parsed_double_val_block3;
                std::stringstream parser_ss_block3(string_to_parse_block3);
                parser_ss_block3.imbue(std::locale::classic());

                if (parser_ss_block3 >> parsed_double_val_block3) {
                    dat = parsed_double_val_block3;
                    char first_remaining_char_block3 = 0;
                    if (parser_ss_block3.get(first_remaining_char_block3)) {
                        sptr = std::string(1, first_remaining_char_block3);
                    } else {
                        sptr = "";
                    }
                } else {
                    dat = 0.0; // Conversion failed
                    sptr = string_to_parse_block3; // Remainder is original string
                }
                if (type == "IFCCARTESIANPOINT" || (sptr != ")" && sptr != "") || !islossy) {
                    str += pp;
                } else {
                    sss.str("");
                    sss << dat;
                    str += sss.str();
                }
            }
        }
        str += ',';
    }
    if (!str.empty()) {
        str = str.substr(0, str.size() - 1);
    }
    return str;
}

bool IFCCompressor::UpdateReference(string& content, const unordered_map<int, int>& rel) {
    string pp;
    int id;
    bool allFind = true;
    unordered_map<int, int>::const_iterator iter;
    stringstream ss(content);
    string newContent = "";
    stringstream sss;
    
    while (getline(ss, pp, ',')) {
        trim(pp);
        if (pp[0] == '(') {
            newContent += '(';
            bool flag = false;
            if (pp[pp.size() - 1] == ')') {
                pp = pp.substr(1, pp.size() - 2);
                flag = true;
            } else {
                pp = pp.substr(1, pp.size() - 1);
            }
            
            if (pp[0] == '#') {
                id = atoi(pp.substr(1, pp.size() - 1).c_str());
                iter = rel.find(id);
                if (iter == rel.end()) {
                    allFind = false;
                } else {
                    sss.str("");
                    sss << "#" << iter->second;
                    pp = sss.str();
                }
            }
            newContent += pp;
            if (flag) newContent += ')';
        } else if (pp[pp.size() - 1] == ')') {
            pp = pp.substr(0, pp.size() - 1);
            if (pp[0] == '#') {
                id = atoi(pp.substr(1, pp.size() - 1).c_str());
                iter = rel.find(id);
                if (iter == rel.end()) {
                    allFind = false;
                } else {
                    sss.str("");
                    sss << "#" << iter->second;
                    pp = sss.str();
                }
            }
            newContent += pp + ')';
        } else {
            if (pp[0] == '#') {
                id = atoi(pp.substr(1, pp.size() - 1).c_str());
                iter = rel.find(id);
                if (iter == rel.end()) {
                    allFind = false;
                } else {
                    sss.str("");
                    sss << "#" << iter->second;
                    pp = sss.str();
                }
            }
            newContent += pp;
        }
        newContent += ',';
    }
    if (!newContent.empty()) {
        newContent = newContent.substr(0, newContent.size() - 1);
    }
    content = allFind ? newContent : content;
    return allFind;
}

bool IFCCompressor::CompressFile(const string& inputFile, const string& outputFile,
                                double fprValue, bool generateCSV) {
    FPR = (fprValue != 0.01); // Active FPR si different de la valeur par defaut
    MAX_PRECISION_VAL = static_cast<int>(-log10(fprValue));
    
    cout << "Debut de la compression..." << endl;
    cout << "Chargement du fichier IFC..." << endl;
    
    ifstream ifile(inputFile);
    if (!ifile.is_open()) {
        cerr << "Erreur: Impossible d'ouvrir le fichier d'entree: " << inputFile << endl;
        return false;
    }
    
    // Lecture de l'en-tete
    string ifcHead = "", ifcEnd = "", ifcInstance;
    for (getline(ifile, ifcInstance); ifcInstance != "DATA;"; getline(ifile, ifcInstance)) {
        ifcHead += ifcInstance + "\n";
    }
    ifcHead += "DATA;\n";
    
    int newInc = 0; // Nouveau numero d'instance
    int iterCount = 0; // Compteur d'instances traitees par iteration
    int deleteCount = 0; // Compteur d'instances supprimees
    int totalCount = 0; // Compteur total d'instances
    int lineCount = 0; // Compteur de lignes pour affichage
    
    // Variables pour parsing multi-lignes
    bool isPart = false;
    string tmp_partial = "";
    
    time_t startT = clock();
    
    // **ETAPE 1: Traitement des noeuds terminaux**
    vector<int> IfcCite;
    IfcInstance* ifcIst;
    
    for (getline(ifile, ifcInstance); ifcInstance != "ENDSEC;"; getline(ifile, ifcInstance)) {
        trim(ifcInstance);
        
        // Gestion des lignes multi-parties
        if (ifcInstance == "") continue;
        else if (ifcInstance[ifcInstance.size() - 1] != ';') {
            if (!isPart) {
                isPart = true;
                tmp_partial = ifcInstance;
            } else {
                tmp_partial += ifcInstance;
            }
            continue;
        } else if (isPart) {
            ifcInstance = tmp_partial + ifcInstance;
            tmp_partial = "";
            isPart = false;
        }
        
        ++totalCount;
        
        // Affichage du progres
        if (totalCount % 50000 == 0) {
            cout << "Lignes traitees: " << totalCount << endl;
        }
        
        IfcCite.clear();
        ifcIst = new IfcInstance();
        ifcIst->id = IfcInstanceAna(ifcInstance, ifcIst->type, ifcIst->content, IfcCite);
        ifcIst->flag = IfcCite.empty(); // Noeud terminal si pas de citations
        
        if (ifcIst->flag) {
            // Instance terminale - traitement immediat
            ++iterCount;
            auto type_iter = mSet.type_content.find(ifcIst->type);
            if (type_iter == mSet.type_content.end()) {
                // Nouveau type d'entite
                map<string, int>* content_id = new map<string, int>();
                content_id->insert(make_pair(ifcIst->content, ++newInc));
                mSet.type_content[ifcIst->type] = *content_id;
                mSet.id_baseID[ifcIst->id] = newInc;
                ifcIst->id = newInc;
            } else {
                // Type d'entite existant
                auto content_iter = type_iter->second.find(ifcIst->content);
                if (content_iter == type_iter->second.end()) {
                    // Nouveau contenu
                    type_iter->second[ifcIst->content] = ++newInc;
                    mSet.id_baseID[ifcIst->id] = newInc;
                    ifcIst->id = newInc;
                } else {
                    // Contenu duplique - suppression
                    ++deleteCount;
                    mSet.id_baseID[ifcIst->id] = content_iter->second;
                }
            }
        } else {
            // Instance avec references - ajout a UnmatchSet
            umSet.instance.push_back(*ifcIst);
            umSet.flag.push_back(true);
        }
        delete ifcIst;
    }
    
    umSet.totalCount = umSet.instance.size();
    
    cout << "Fichier charge avec succes. " << totalCount << " instances trouvees." << endl;
    cout << "Noeuds terminaux identifies: " << iterCount << endl;
    cout << "Step 1:" << iterCount << " delete:" << deleteCount << endl;
    
    // **ETAPE 2: Traitement iteratif des instances avec references**
    int iterationTime = 0;
    while (iterCount > 0) {
        ++iterationTime;
        cout << "Iteration " << iterationTime << "..." << endl;
        
        int predelete = deleteCount;
        iterCount = 0;
        
        // Traiter toutes les instances dans UnmatchSet
        for (size_t i = 0; i < umSet.instance.size(); ++i) {
            if (!umSet.flag[i]) continue; // Instance deja traitee
            
            // Verifier si toutes les references peuvent etre resolues
            string tempContent = umSet.instance[i].content;
            if (UpdateReference(tempContent, mSet.id_baseID)) {
                // Toutes les references sont resolues
                ++iterCount;
                umSet.instance[i].content = tempContent;
                
                auto type_iter = mSet.type_content.find(umSet.instance[i].type);
                if (type_iter == mSet.type_content.end()) {
                    // Nouveau type
                    map<string, int>* content_id = new map<string, int>();
                    content_id->insert(make_pair(umSet.instance[i].content, ++newInc));
                    mSet.type_content[umSet.instance[i].type] = *content_id;
                    mSet.id_baseID[umSet.instance[i].id] = newInc;
                    umSet.instance[i].id = newInc;
                } else {
                    // Type existant
                    auto content_iter = type_iter->second.find(umSet.instance[i].content);
                    if (content_iter == type_iter->second.end()) {
                        // Nouveau contenu
                        type_iter->second[umSet.instance[i].content] = ++newInc;
                        mSet.id_baseID[umSet.instance[i].id] = newInc;
                        umSet.instance[i].id = newInc;
                    } else {
                        // Contenu duplique
                        ++deleteCount;
                        mSet.id_baseID[umSet.instance[i].id] = content_iter->second;
                    }
                }
                --umSet.totalCount;
                umSet.flag[i] = false; // Marquer comme traite
            }
        }
        
        cout << "Step 2(iter :" << iterationTime << "):" << iterCount << " delete:" << (deleteCount - predelete) << endl;
        
        // Protection contre boucle infinie
        if (iterationTime > 20) {
            cout << "Arret: Nombre maximum d'iterations atteint." << endl;
            break;
        }
    }
    
    time_t endT = clock();
    cout << "Instance Compression Ratio:" << static_cast<double>(deleteCount) / totalCount << endl;
    cout << "reserved instances|total instances:" << newInc << "|" << totalCount << endl;
    cout << "Time costs:" << static_cast<double>(endT - startT) / CLOCKS_PER_SEC << endl;
    cout << "finished" << endl;
    
    // **ETAPE 3: Ecriture du fichier compresse**
    ofstream ofile(outputFile);
    if (!ofile.is_open()) {
        cerr << "Erreur: Impossible de creer le fichier de sortie: " << outputFile << endl;
        return false;
    }
    
    ofile << ifcHead;
    
    // Ecrire les instances compressees
    for (const auto& type_pair : mSet.type_content) {
        for (const auto& content_pair : type_pair.second) {
            ofile << "#" << content_pair.second << "=" << type_pair.first
                  << "(" << content_pair.first << ");" << endl;
        }
    }
    
    ofile << "ENDSEC;" << endl;
    ofile << "END-ISO-10303-21;" << endl;
    ofile.close();
    
    // Generation du rapport CSV si demande
    if (generateCSV) {
        ofstream csvFile(outputFile + ".csv");
        csvFile << "Step NO.,Total Num.,Delete Num.,Compression Ratio" << endl;
        csvFile << "Total," << totalCount << "," << deleteCount << ","
                << static_cast<double>(deleteCount) / totalCount << endl;
        csvFile.close();
    }
    
    return true;
}

bool IFCCompressor::ProcessFile(const string& inputFile, const string& outputFile,
                               double fpr, bool csv) {
    IFCCompressor compressor;
    return compressor.CompressFile(inputFile, outputFile, fpr, csv);
}