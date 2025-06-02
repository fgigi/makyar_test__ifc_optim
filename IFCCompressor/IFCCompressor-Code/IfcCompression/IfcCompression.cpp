#pragma once
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <time.h>
#include <hash_map>
#include "CIfcCompression.h"
#include "..\ifcMultiProc\IFCAnalyzer.h"
#include "..\ifcMultiProc\IFCAnalyzer.cpp"
#include <io.h>  
#include <fstream>  
#include <string>  
#include <vector>  
#include <iostream>
using namespace CIfcCompression;
using namespace std;

const string IFCOBJECT[]={"IFCBUILDINGELEMENTPROXY","IFCWALL","IFCWALLSTANDARDCASE","IFCCOVERING","IFCFOOTING","IFCSITE",
	"IFCBEAM","IFCCOLUMN","IFCDOOR","IFCWINDOW","IFCROOF","IFCSLAB","IFCSTAIR","IFCPLATE","IFCSTAIRFLIGHT",
	"IFCRAMP","IFCRAMPFLIGHT","IFCRAILING","IFCCURTAINWALL","IFCFURNISHINGELEMENT"};
const int IFCOBJNUM=20;

int main(int argc, char** argv) {
	if ( strcmp(argv[1], "-h" ) ==0 || strcmp(argv[1], "-help" ) == 0 )
	{
		cout << "Usage: IfcCompression.exe <input filename> <needOutPutCSV> <output filename>"<<endl;
		cout << "Parameter: needOutPutCSV should be Y or N "<<endl;
		exit(0);
	}
	else
	{
		cout <<argv[1] <<endl;
	}
	if ( argc != 4 )
	{
		cout << "Usage: IfcCompression.exe <input filename> <needOutPutCSV> <output filename>"<<endl;
		cout <<argc<<endl;
		exit(0) ;
	}
	if( strlen(argv[2])  != 1)
	{
		cout <<"Invalid parameters"<<endl;
		exit(0);
	}
	string infilePath = argv[1];
	char needOutPutCSV = argv[2][0];
	string outfilePath = argv[3];
	if ( needOutPutCSV == 'Y' )
	{
		//cout << "Output \t"<< needOutPutCSV <<endl;
		IfcCompressor(infilePath, outfilePath, 2);
	}
	else
	{
		IfcCompressor(infilePath, outfilePath, needOutPutCSV);
	}

	//cout << needOutPutCSV<<endl;
	

	
	//char needOutPutCSV = 'Y';
	//char pathOrFile = 'F';
	//string filePath;
	//cout << "Do you need to output the csv file, please input Y or N¡£" << endl;
	//cin >> needOutPutCSV;
	//cout <<"Do you want to select a path or input the file list by hand, please input P or F  (P:path, F:file)" <<endl;
	//cin >> pathOrFile;
	//if( pathOrFile == 'P' )
	//{
	//	//get all the ifc file

	//}
	//else
	//{
	//	cout << "Please input IFC file name¡£" << endl;
	//	//cin >> filePath;
	//	vector<string> file_list;
	//	while( cin >> filePath )
	//	{
	//		if (filePath == "end")
	//		{
	//			break;
	//		}
	//		file_list.push_back(filePath);
	//		//cout << "vector size = " << file_list.size() << endl;
	//	}
	//	if (needOutPutCSV == 'N')
	//	{
	//		for( int i = 0; i < file_list.size(); i++)
	//		{
	//			cout << "Compression of " << file_list[i] << endl;
	//			filePath = file_list[i] ;
	//			IfcCompressor(filePath, filePath + ".ifc", needOutPutCSV);
	//			cout <<endl;
	//		}
	//		// 
	//	}
	//	else
	//	{
	//		for( int i = 0; i < file_list.size(); i++)
	//		{
	//			cout << "Compression of " << file_list[i] << endl;
	//			filePath = file_list[i] ;
	//			IfcCompressor(filePath, filePath + ".ifc", 2);
	//			cout <<endl;
	//		}
	//	}
	//}

	//system("pause");
	//if ( argc < 2 ) {
	//	cout << "usage: IfcObj <filename.ifc>" << std::endl;
	//	return 1;
	//}else if (argc == 2)
	//{
	//	string fileName(argv[1]);
	//	IfcCompressor(fileName,fileName+".ifc",2);
	//}
	//else if (argc == 3)
	//{
	//	string fileName(argv[1]),wholeLine;
	//	ifstream ifile(fileName);
	//	stringstream ifcText;
	//	while (getline(ifile, wholeLine))
	//		ifcText << wholeLine << endl;
	//	ifile.close();
	//	IfcCompressor(ifcText, fileName + ".ifc");
	//}
	//else
	//{
	//	string fileName(argv[1]),wholeLine;
	//	stringstream fileData,anaResult;
	//	int beforeComp,afterComp;
	//	ifstream ifile(fileName);
	//	while (getline(ifile,wholeLine))
	//		fileData<<wholeLine<<endl;
	//	ifile.close();
	//	anaResult<<"File Name,"<<fileName<<endl;
	//	IfcCompressorAna(fileData,anaResult,beforeComp,afterComp);
	//	anaResult<<"Whole File,"<<beforeComp<<","<<beforeComp-afterComp<<endl;

	//	CIFCAnalyzer ifc;
	//	ifc.CIFCRead(fileName);

	//	set<string> ifcObjects;
	//	for (int i=0;i<IFCOBJNUM;++i)
	//		ifcObjects.insert(IFCOBJECT[i]);

	//	vector<string> entities;
	//	ifc.CIFCGetAllInstanceName(entities);
	//	for (int i=0;i<entities.size();++i)
	//	{
	//		if (ifcObjects.find(entities[i])==ifcObjects.end())
	//			continue;
	//		fileData.clear();
	//		fileData.str("");
	//		ifc.CIFCWrite(fileData,entities[i],false);
	//		stringstream tmp;
	//		IfcCompressorAna(fileData,tmp,beforeComp,afterComp);
	//		anaResult<<entities[i]<<","<<beforeComp<<","<<beforeComp-afterComp<<endl;
	//	}

	//	ofstream ofile(fileName+".csv");
	//	ofile<<anaResult.str();
	//	ofile.close();
	//}

}