#pragma once
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <time.h>
#include <hash_map>
#include <windows.h>

using namespace std;

namespace CIfcCompression
{
	//#define MAX_PRECISION 6
	//#define FPR 0

	static int MAX_PRECISION = 2;
	static bool FPR = false;

	struct IfcInstance
	{
		int id;
		string type;
		string content;
		//vector<int>  cite;
		bool flag;	//是否可以用于匹配,即其引用是否均已存在
	};

	struct MatchSet
	{
		vector<string> baseInstance;
		vector<int> index;	//保存实例在数组中存储的编号
		hash_map<int,int>  id_baseID;
		map<string,map<string,int>> type_content;
	};

	struct UnmatchSet
	{
		vector<IfcInstance>  instance;
		vector<bool> flag; //为false表示该实例不参与匹配，即该实例已经被处理过
		int totalCount;
	};


	void IfcCompressor(string fileName,string fileName4Write,int flag = 0,int precision = 2, bool fpr = false);
    void IfcCompressor(string fileName, string fileName4Write, char noCSV, int flag = 0, int precision = 2, bool fpr = false);
    void IfcCompressor(string fileName, string fileName4Write, HANDLE hSemaphore, int &process, string &pInfo, int flag = 0, int precision = 2, bool fpr = false);
	void IfcCompressor(stringstream &textContent, string fileName4Write, int flag = 0);
	void IfcCompressorAna(stringstream &ifile,stringstream &result,int &totalCount,int &compCount);
	void IfcCompare(string fileA,string fileB,float &sA2B,float &sB2A,float &machR,float &missR,float &guidPR, float &AR);

	void IfcInstanceAna(string instance,int &id,string &type,string &content,vector<int> &cite); //返回的content为所有非引用，引用部分为空
	int IfcInstanceAna(string instance,string &type,string &content,vector<int> &cite);	//返回的content包含引用
	string StandardContent(string &content,vector<int> &cite);
	string StandardContent(string type,string &content,vector<int> &cite,bool islossy);
	bool StandardContent(string &content,hash_map<int,int> &rel);
	void trim(string &str);
}

