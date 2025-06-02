#include "CIfcCompression.h"
#include <sys/stat.h>

//flag:0:不统计删除实例信息 1:统计每个步骤中删除的实例及实例数 2：统计每个实例的删除数
//defult parameters int flag = 0,int precision = 2, bool fpr = false
void CIfcCompression::IfcCompressor(string fileName, string fileName4Write, int flag, int precision, bool fpr)
{
	FPR = fpr;
	MAX_PRECISION = precision;
	time_t startT,endT;
	startT=clock();
	ifstream ifile(fileName);
	ofstream ofile2(fileName4Write+".csv");

	//int a;
	//cout<<"=================="<<fileName<<endl;
	//cin>>a;

	string ifcHead="",ifcEnd="",ifcInstance;
	for(getline(ifile,ifcInstance);ifcInstance!="DATA;";getline(ifile,ifcInstance))
	{
		ifcHead+=ifcInstance+"\n";
	}
	ifcHead+="DATA;\n";

	int newInc=0;	//新的实例编号
	int iterCount=0;	//每轮迭代的实例分析数统计
	int deleteCount=0;	//用于统计删除的实例

	//第一遍处理，将文件中不存在引用的实例进行简化
	IfcInstance *ifcIst;
	MatchSet mSet;
	UnmatchSet umSet;
	
	map<string,int> *content_id;
	map<string,map<string,int>>::iterator type_content_iter;
	map<string,int>::iterator content_id_iter;
	map<string,int> entity_num; 
	map<string,int>::iterator entity_num_iter; 

	vector<int>  IfcCite;
	stringstream sss;
	bool isPart = false;
	string tmp_partial = "";
	int compCount=0,totalCount=0,predelete=0;
	umSet.totalCount=0;

	for(getline(ifile,ifcInstance);ifcInstance!="ENDSEC;";getline(ifile,ifcInstance))
	{
		//std::cout<<ifcInstance <<endl;
		trim(ifcInstance);
		//std::cout<< "after trim\t"<<ifcInstance <<endl;
		//combines multi-lines
		if (ifcInstance=="")
			continue;
		else if (ifcInstance[ifcInstance.size() - 1] != ';')
		{
			if (!isPart)
			{
				isPart = true;
				tmp_partial = ifcInstance;
			}
			else
				tmp_partial += ifcInstance;
			continue;
		}
		else if (isPart)
		{
			ifcInstance = tmp_partial + ifcInstance;
			tmp_partial = "";
			isPart = false;
		}

		++totalCount;
		IfcCite.clear();
		ifcIst=new IfcInstance();
		ifcIst->id=IfcInstanceAna(ifcInstance,ifcIst->type,ifcIst->content,IfcCite);
		ifcIst->flag=IfcCite.empty();

		if (ifcIst->flag)
		{
			++iterCount;
			type_content_iter=mSet.type_content.find(ifcIst->type);
			if (type_content_iter==mSet.type_content.end())
			{
				content_id=new map<string,int>();
				//保留原始id编号
				//content_id->insert(map<string,int>::value_type(ifcIst->content,ifcIst->id));
				//mSet.type_content.insert(map<string,map<string,int>>::value_type(ifcIst->type,*content_id));
				//mSet.id_baseID.insert(map<int,int>::value_type(ifcIst->id,ifcIst->id));
				////sss.str("");
				////sss<<'#'<<ifcIst->id<<'='<<ifcIst->type<<'('<<ifcIst->content<<");"<<endl;

				//将id重新编号
				content_id->insert(map<string,int>::value_type(ifcIst->content,++newInc));
				mSet.type_content.insert(map<string,map<string,int>>::value_type(ifcIst->type,*content_id));
				mSet.id_baseID.insert(map<int,int>::value_type(ifcIst->id,newInc));
				ifcIst->id=newInc;
				///*sss.str("");
				//sss<<'#'<<newInc<<'='<<ifcIst->type<<'('<<ifcIst->content<<");"<<endl; */

				mSet.index.push_back(umSet.instance.size());
				
				umSet.instance.push_back(*ifcIst);
				umSet.flag.push_back(false);
				--umSet.totalCount;

				//mSet.baseInstance.push_back(sss.str());
			}else
			{
				content_id_iter=type_content_iter->second.find(ifcIst->content);
				if (content_id_iter==type_content_iter->second.end())
				{
					//保留原始id编号
					//type_content_iter->second.insert(map<string,int>::value_type(ifcIst->content,ifcIst->id));
					//mSet.id_baseID.insert(map<int,int>::value_type(ifcIst->id,ifcIst->id));
					////sss.str("");
					////sss<<'#'<<ifcIst->id<<'='<<ifcIst->type<<'('<<ifcIst->content<<");"<<endl;

					//将id重新编号
					type_content_iter->second.insert(map<string,int>::value_type(ifcIst->content,++newInc));
					mSet.id_baseID.insert(map<int,int>::value_type(ifcIst->id,newInc));
					ifcIst->id=newInc;
					///*sss.str("");
					//sss<<'#'<<newInc<<'='<<ifcIst->type<<'('<<ifcIst->content<<");"<<endl;*/

					mSet.index.push_back(umSet.instance.size());

					umSet.instance.push_back(*ifcIst);
					umSet.flag.push_back(false);
					--umSet.totalCount;

					//mSet.baseInstance.push_back(sss.str());
				}else
				{
					//记录删除信息
					++deleteCount;
					if (flag > 0)
					{
						entity_num_iter = entity_num.find(ifcIst->type);
						if (entity_num_iter == entity_num.end())
							entity_num.insert(map<string, int>::value_type(ifcIst->type, 1));
						else
							++(entity_num_iter->second);	
					}
					mSet.id_baseID.insert(map<int, int>::value_type(ifcIst->id, content_id_iter->second));
				}
			}
			delete ifcIst;
		}else
		{
			umSet.instance.push_back(*ifcIst);


			//umSet.instance.push_back(*ifcIst);
			umSet.flag.push_back(true);
		}
	}
	umSet.totalCount+=umSet.instance.size();
	std::cout<<(double)deleteCount/totalCount<<endl;
	std::cout << "Step 1:" << iterCount << " delete:" << deleteCount << endl;
	for (entity_num_iter = entity_num.begin(); entity_num_iter != entity_num.end(); ++entity_num_iter)
		std::cout << entity_num_iter->first << ':' << entity_num_iter->second << endl;
	if (flag == 1)
	{
		ofile2 << "Step NO.,Total Num.,Delete Num.,Memo" << endl;
		ofile2 << "Step 1:," << iterCount << "," << deleteCount << ",";
		for (entity_num_iter = entity_num.begin(); entity_num_iter != entity_num.end(); ++entity_num_iter)
			ofile2 << entity_num_iter->first << ':' << entity_num_iter->second << ' ';
		ofile2 << endl;
		entity_num.clear();
	}

	//规范化未匹配集合中的引用
	for (int i=0;i<umSet.instance.size();++i)
		umSet.instance[i].flag=StandardContent(umSet.instance[i].content,mSet.id_baseID);
	ifcEnd="ENDSEC;\n";
	while(getline(ifile,ifcInstance))
	{
		ifcEnd+=ifcInstance+"\n";
	}
	ifile.close();

	int iterationTime=0;//统计迭代次数
	bool isSt2=true;	//是否进行迭代压缩

	while (umSet.totalCount!=0)
	{
		++iterationTime;
		iterCount=0;
		predelete=deleteCount;
		for (int i=0;i<umSet.instance.size();++i)
		{
			//不迭代处理，只单纯将集合中的所有实例加入到输出队列中
			if (!isSt2)
			{
				sss.str("");
				sss<<'#'<<umSet.instance[i].id<<'='<<umSet.instance[i].type<<'('<<umSet.instance[i].content<<");"<<endl;
				mSet.baseInstance.push_back(sss.str());
				--umSet.totalCount;
				umSet.flag[i]=false;
				continue;
			}
			//迭代处理部分
			if (umSet.flag[i]&&umSet.instance[i].flag)
			{
				++iterCount;
				type_content_iter=mSet.type_content.find(umSet.instance[i].type);
				if (type_content_iter==mSet.type_content.end())
				{
					content_id=new map<string,int>();
					////保留原始id编号
					//content_id->insert(map<string,int>::value_type(umSet.instance[i].content,umSet.instance[i].id));
					//mSet.type_content.insert(map<string,map<string,int>>::value_type(umSet.instance[i].type,*content_id));
					//mSet.id_baseID.insert(map<int,int>::value_type(umSet.instance[i].id,umSet.instance[i].id));
					//sss.str("");
					//sss<<'#'<<umSet.instance[i].id<<'='<<umSet.instance[i].type<<'('<<umSet.instance[i].content<<");"<<endl;

					//将id重新编号
					content_id->insert(map<string,int>::value_type(umSet.instance[i].content,++newInc));
					mSet.type_content.insert(map<string,map<string,int>>::value_type(umSet.instance[i].type,*content_id));
					mSet.id_baseID.insert(map<int,int>::value_type(umSet.instance[i].id,newInc));
					umSet.instance[i].id=newInc;
					///*sss.str("");
					//sss<<'#'<<newInc<<'='<<umSet.instance[i].type<<'('<<umSet.instance[i].content<<");"<<endl;*/

					mSet.index.push_back(i);
					//mSet.baseInstance.push_back(sss.str());
				}else
				{
					content_id_iter=type_content_iter->second.find(umSet.instance[i].content);
					if (content_id_iter==type_content_iter->second.end())
					{
						////保留原始id编号
						//type_content_iter->second.insert(map<string,int>::value_type(umSet.instance[i].content,umSet.instance[i].id));
						//mSet.id_baseID.insert(map<int,int>::value_type(umSet.instance[i].id,umSet.instance[i].id));
						////sss.str("");
						////sss<<'#'<<umSet.instance[i].id<<'='<<umSet.instance[i].type<<'('<<umSet.instance[i].content<<");"<<endl;

						//将id重新编号
						type_content_iter->second.insert(map<string,int>::value_type(umSet.instance[i].content,++newInc));
						mSet.id_baseID.insert(map<int,int>::value_type(umSet.instance[i].id,newInc));
						umSet.instance[i].id=newInc;
						///*sss.str("");
						//sss<<'#'<<newInc<<'='<<umSet.instance[i].type<<'('<<umSet.instance[i].content<<");"<<endl;*/

						mSet.index.push_back(i);
						//mSet.baseInstance.push_back(sss.str());
					}else
					{
						//记录删除信息
						++deleteCount;
						if (flag > 0)
						{
							entity_num_iter = entity_num.find(umSet.instance[i].type);
							if (entity_num_iter == entity_num.end())
								entity_num.insert(map<string, int>::value_type(umSet.instance[i].type, 1));
							else
								++(entity_num_iter->second);
						}
						mSet.id_baseID.insert(map<int,int>::value_type(umSet.instance[i].id,content_id_iter->second));
						//cout<<iterationTime<<':'<<umSet.instance[i].id<<umSet.instance[i].type<<endl;
					}
				}
				--umSet.totalCount;
				umSet.flag[i]=false;
			}
		}

		//规范化未匹配集合中的引用
		for (int i=0;i<umSet.instance.size();++i)
		{
			if (umSet.flag[i])
			{
				umSet.instance[i].flag=StandardContent(umSet.instance[i].content,mSet.id_baseID);
			}
		}

		//输出每一步的删除实例数
		std::cout<<"Step 2(iter :"<<iterationTime<<"):"<<iterCount<<" delete:"<<deleteCount-predelete<<endl;
		for (entity_num_iter = entity_num.begin(); entity_num_iter != entity_num.end(); ++entity_num_iter)
			std::cout << entity_num_iter->first << ':' << entity_num_iter->second << endl;
		if (flag == 1)
		{
			ofile2 << "Step 2(iter :" << iterationTime << "):," << iterCount << "," << deleteCount - predelete << ",";
			for (entity_num_iter = entity_num.begin(); entity_num_iter != entity_num.end(); ++entity_num_iter)
				ofile2 << entity_num_iter->first << ':' << entity_num_iter->second << ' ';
			ofile2 << endl;
			entity_num.clear();
		}
	}

	if (flag > 1)
	{
		ofile2 << "Entity Name,Removed Number"<<endl;
		for (entity_num_iter = entity_num.begin(); entity_num_iter != entity_num.end(); ++entity_num_iter)
			ofile2 << entity_num_iter->first << ',' << entity_num_iter->second <<endl;
	}

	ofile2.close();

	endT=clock();

	std::cout<<"Instance Compression Ratio:"<<(double)deleteCount/totalCount<<endl;

	std::cout<<"reserved instances|total instances:"<<newInc<<"|"<<totalCount<<endl;

	std::cout<<"Time costs:"<<(double)(endT-startT)/CLOCKS_PER_SEC<<endl;

	std::cout<<"Sa->b:"<<1.0<<" Sb->a"<<1.0<<endl;

	ofstream ofile(fileName4Write);
	ofile<<ifcHead;
	//将
	for (int i=0;i<mSet.index.size();++i)
		ofile<<'#'<<umSet.instance[mSet.index[i]].id<<'='<<umSet.instance[mSet.index[i]].type<<'('<<umSet.instance[mSet.index[i]].content<<");"<<endl;
	for (int i=0;i<umSet.instance.size();++i)
	{
		if(umSet.flag[i])
			ofile<<'#'<<umSet.instance[i].id<<'='<<umSet.instance[i].type<<'('<<umSet.instance[i].content<<");"<<endl;
	}
	ofile<<ifcEnd;
	ofile.close();
}


void CIfcCompression::IfcCompressor(string fileName, string fileName4Write, char noCSV, int flag, int precision, bool fpr)
{
    FPR = fpr;
    MAX_PRECISION = precision;
    time_t startT, endT;
    startT = clock();
    ifstream ifile(fileName);
    string ifcHead = "", ifcEnd = "", ifcInstance;
    for (getline(ifile, ifcInstance); ifcInstance != "DATA;"; getline(ifile, ifcInstance))
    {
        ifcHead += ifcInstance + "\n";
    }
    ifcHead += "DATA;\n";

    int newInc = 0;	//新的实例编号
    int iterCount = 0;	//每轮迭代的实例分析数统计
    int deleteCount = 0;	//用于统计删除的实例

    //第一遍处理，将文件中不存在引用的实例进行简化
    IfcInstance *ifcIst;
    MatchSet mSet;
    UnmatchSet umSet;

    map<string, int> *content_id;
    map<string, map<string, int>>::iterator type_content_iter;
    map<string, int>::iterator content_id_iter;
    map<string, int> entity_num;
    map<string, int>::iterator entity_num_iter;

    vector<int>  IfcCite;
    stringstream sss;
    bool isPart = false;
    string tmp_partial = "";
    int compCount = 0, totalCount = 0, predelete = 0;
    umSet.totalCount = 0;

    for (getline(ifile, ifcInstance); ifcInstance != "ENDSEC;"; getline(ifile, ifcInstance))
    {
        trim(ifcInstance);
        //combines multi-lines
        if (ifcInstance == "")
            continue;
        else if (ifcInstance[ifcInstance.size() - 1] != ';')
        {
            if (!isPart)
            {
                isPart = true;
                tmp_partial = ifcInstance;
            }
            else
                tmp_partial += ifcInstance;
            continue;
        }
        else if (isPart)
        {
            ifcInstance = tmp_partial + ifcInstance;
            tmp_partial = "";
            isPart = false;
        }

        ++totalCount;
        IfcCite.clear();
        ifcIst = new IfcInstance();
        ifcIst->id = IfcInstanceAna(ifcInstance, ifcIst->type, ifcIst->content, IfcCite);
        ifcIst->flag = IfcCite.empty();

        if (ifcIst->flag)
        {
            ++iterCount;
            type_content_iter = mSet.type_content.find(ifcIst->type);
            if (type_content_iter == mSet.type_content.end())
            {
                content_id = new map<string, int>();
                //保留原始id编号
                //content_id->insert(map<string,int>::value_type(ifcIst->content,ifcIst->id));
                //mSet.type_content.insert(map<string,map<string,int>>::value_type(ifcIst->type,*content_id));
                //mSet.id_baseID.insert(map<int,int>::value_type(ifcIst->id,ifcIst->id));
                ////sss.str("");
                ////sss<<'#'<<ifcIst->id<<'='<<ifcIst->type<<'('<<ifcIst->content<<");"<<endl;

                //将id重新编号
                content_id->insert(map<string, int>::value_type(ifcIst->content, ++newInc));
                mSet.type_content.insert(map<string, map<string, int>>::value_type(ifcIst->type, *content_id));
                mSet.id_baseID.insert(map<int, int>::value_type(ifcIst->id, newInc));
                ifcIst->id = newInc;
                ///*sss.str("");
                //sss<<'#'<<newInc<<'='<<ifcIst->type<<'('<<ifcIst->content<<");"<<endl; */

                mSet.index.push_back(umSet.instance.size());

                umSet.instance.push_back(*ifcIst);
                umSet.flag.push_back(false);
                --umSet.totalCount;

                //mSet.baseInstance.push_back(sss.str());
            }
            else
            {
                content_id_iter = type_content_iter->second.find(ifcIst->content);
                if (content_id_iter == type_content_iter->second.end())
                {
                    //保留原始id编号
                    //type_content_iter->second.insert(map<string,int>::value_type(ifcIst->content,ifcIst->id));
                    //mSet.id_baseID.insert(map<int,int>::value_type(ifcIst->id,ifcIst->id));
                    ////sss.str("");
                    ////sss<<'#'<<ifcIst->id<<'='<<ifcIst->type<<'('<<ifcIst->content<<");"<<endl;

                    //将id重新编号
                    type_content_iter->second.insert(map<string, int>::value_type(ifcIst->content, ++newInc));
                    mSet.id_baseID.insert(map<int, int>::value_type(ifcIst->id, newInc));
                    ifcIst->id = newInc;
                    ///*sss.str("");
                    //sss<<'#'<<newInc<<'='<<ifcIst->type<<'('<<ifcIst->content<<");"<<endl;*/

                    mSet.index.push_back(umSet.instance.size());

                    umSet.instance.push_back(*ifcIst);
                    umSet.flag.push_back(false);
                    --umSet.totalCount;

                    //mSet.baseInstance.push_back(sss.str());
                }
                else
                {
                    //记录删除信息
                    ++deleteCount;
                    if (flag > 0)
                    {
                        entity_num_iter = entity_num.find(ifcIst->type);
                        if (entity_num_iter == entity_num.end())
                            entity_num.insert(map<string, int>::value_type(ifcIst->type, 1));
                        else
                            ++(entity_num_iter->second);
                    }
                    mSet.id_baseID.insert(map<int, int>::value_type(ifcIst->id, content_id_iter->second));
                }
            }
            delete ifcIst;
        }
        else
        {
            umSet.instance.push_back(*ifcIst);


            //umSet.instance.push_back(*ifcIst);
            umSet.flag.push_back(true);
        }
    }
    umSet.totalCount += umSet.instance.size();
    std::cout << (double)deleteCount / totalCount << endl;
    std::cout << "Step 1:" << iterCount << " delete:" << deleteCount << endl;
    for (entity_num_iter = entity_num.begin(); entity_num_iter != entity_num.end(); ++entity_num_iter)
        std::cout << entity_num_iter->first << ':' << entity_num_iter->second << endl;
    if (flag == 1)
    {
        entity_num.clear();
    }

    //规范化未匹配集合中的引用
    for (int i = 0; i<umSet.instance.size(); ++i)
        umSet.instance[i].flag = StandardContent(umSet.instance[i].content, mSet.id_baseID);
    ifcEnd = "ENDSEC;\n";
    while (getline(ifile, ifcInstance))
    {
        ifcEnd += ifcInstance + "\n";
    }
    ifile.close();

    int iterationTime = 0;//统计迭代次数
    bool isSt2 = true;	//是否进行迭代压缩

    while (umSet.totalCount != 0)
    {
        ++iterationTime;
        iterCount = 0;
        predelete = deleteCount;
        for (int i = 0; i<umSet.instance.size(); ++i)
        {
            //不迭代处理，只单纯将集合中的所有实例加入到输出队列中
            if (!isSt2)
            {
                sss.str("");
                sss << '#' << umSet.instance[i].id << '=' << umSet.instance[i].type << '(' << umSet.instance[i].content << ");" << endl;
                mSet.baseInstance.push_back(sss.str());
                --umSet.totalCount;
                umSet.flag[i] = false;
                continue;
            }
            //迭代处理部分
            if (umSet.flag[i] && umSet.instance[i].flag)
            {
                ++iterCount;
                type_content_iter = mSet.type_content.find(umSet.instance[i].type);
                if (type_content_iter == mSet.type_content.end())
                {
                    content_id = new map<string, int>();
                    ////保留原始id编号
                    //content_id->insert(map<string,int>::value_type(umSet.instance[i].content,umSet.instance[i].id));
                    //mSet.type_content.insert(map<string,map<string,int>>::value_type(umSet.instance[i].type,*content_id));
                    //mSet.id_baseID.insert(map<int,int>::value_type(umSet.instance[i].id,umSet.instance[i].id));
                    //sss.str("");
                    //sss<<'#'<<umSet.instance[i].id<<'='<<umSet.instance[i].type<<'('<<umSet.instance[i].content<<");"<<endl;

                    //将id重新编号
                    content_id->insert(map<string, int>::value_type(umSet.instance[i].content, ++newInc));
                    mSet.type_content.insert(map<string, map<string, int>>::value_type(umSet.instance[i].type, *content_id));
                    mSet.id_baseID.insert(map<int, int>::value_type(umSet.instance[i].id, newInc));
                    umSet.instance[i].id = newInc;
                    ///*sss.str("");
                    //sss<<'#'<<newInc<<'='<<umSet.instance[i].type<<'('<<umSet.instance[i].content<<");"<<endl;*/

                    mSet.index.push_back(i);
                    //mSet.baseInstance.push_back(sss.str());
                }
                else
                {
                    content_id_iter = type_content_iter->second.find(umSet.instance[i].content);
                    if (content_id_iter == type_content_iter->second.end())
                    {
                        ////保留原始id编号
                        //type_content_iter->second.insert(map<string,int>::value_type(umSet.instance[i].content,umSet.instance[i].id));
                        //mSet.id_baseID.insert(map<int,int>::value_type(umSet.instance[i].id,umSet.instance[i].id));
                        ////sss.str("");
                        ////sss<<'#'<<umSet.instance[i].id<<'='<<umSet.instance[i].type<<'('<<umSet.instance[i].content<<");"<<endl;

                        //将id重新编号
                        type_content_iter->second.insert(map<string, int>::value_type(umSet.instance[i].content, ++newInc));
                        mSet.id_baseID.insert(map<int, int>::value_type(umSet.instance[i].id, newInc));
                        umSet.instance[i].id = newInc;
                        ///*sss.str("");
                        //sss<<'#'<<newInc<<'='<<umSet.instance[i].type<<'('<<umSet.instance[i].content<<");"<<endl;*/

                        mSet.index.push_back(i);
                        //mSet.baseInstance.push_back(sss.str());
                    }
                    else
                    {
                        //记录删除信息
                        ++deleteCount;
                        if (flag > 0)
                        {
                            entity_num_iter = entity_num.find(umSet.instance[i].type);
                            if (entity_num_iter == entity_num.end())
                                entity_num.insert(map<string, int>::value_type(umSet.instance[i].type, 1));
                            else
                                ++(entity_num_iter->second);
                        }
                        mSet.id_baseID.insert(map<int, int>::value_type(umSet.instance[i].id, content_id_iter->second));
                        //cout<<iterationTime<<':'<<umSet.instance[i].id<<umSet.instance[i].type<<endl;
                    }
                }
                --umSet.totalCount;
                umSet.flag[i] = false;
            }
        }

        //规范化未匹配集合中的引用
        for (int i = 0; i<umSet.instance.size(); ++i)
        {
            if (umSet.flag[i])
            {
                umSet.instance[i].flag = StandardContent(umSet.instance[i].content, mSet.id_baseID);
            }
        }

        //输出每一步的删除实例数
        std::cout << "Step 2(iter :" << iterationTime << "):" << iterCount << " delete:" << deleteCount - predelete << endl;
        for (entity_num_iter = entity_num.begin(); entity_num_iter != entity_num.end(); ++entity_num_iter)
            std::cout << entity_num_iter->first << ':' << entity_num_iter->second << endl;
        if (flag == 1)
        {

            entity_num.clear();
        }
    }



    endT = clock();

    std::cout << "Instance Compression Ratio:" << (double)deleteCount / totalCount << endl;

    std::cout << "reserved instances|total instances:" << newInc << "|" << totalCount << endl;

    std::cout << "Time costs:" << (double)(endT - startT) / CLOCKS_PER_SEC << endl;

    std::cout << "Sa->b:" << 1.0 << " Sb->a" << 1.0 << endl;

    ofstream ofile(fileName4Write);
    ofile << ifcHead;
    //将
    for (int i = 0; i<mSet.index.size(); ++i)
        ofile << '#' << umSet.instance[mSet.index[i]].id << '=' << umSet.instance[mSet.index[i]].type << '(' << umSet.instance[mSet.index[i]].content << ");" << endl;
    for (int i = 0; i<umSet.instance.size(); ++i)
    {
        if (umSet.flag[i])
            ofile << '#' << umSet.instance[i].id << '=' << umSet.instance[i].type << '(' << umSet.instance[i].content << ");" << endl;
    }
    ofile << ifcEnd;
    ofile.close();
}



void CIfcCompression::IfcCompressor(string fileName, string fileName4Write, HANDLE hSemaphore, int &process, string &pInfo, int flag, int precision, bool fpr)
{
	stringstream ss;
	FPR = fpr;
	MAX_PRECISION = precision;
	time_t startT, endT;
	startT = clock();
	ifstream ifile(fileName);
	ofstream ofile2(fileName + ".csv");
	string ifcHead = "", ifcEnd = "", ifcInstance;
	for (getline(ifile, ifcInstance); ifcInstance != "DATA;"; getline(ifile, ifcInstance))
	{
		ifcHead += ifcInstance + "\n";
	}
	ifcHead += "DATA;\n";

	process = 10;
	ReleaseSemaphore(hSemaphore, 1, NULL);

	int newInc = 0;	//新的实例编号
	int iterCount = 0;	//每轮迭代的实例分析数统计
	int deleteCount = 0;	//用于统计删除的实例

	//第一遍处理，将文件中不存在引用的实例进行简化
	IfcInstance *ifcIst;
	MatchSet mSet;
	UnmatchSet umSet;

	map<string, int> *content_id;
	map<string, map<string, int>>::iterator type_content_iter;
	map<string, int>::iterator content_id_iter;
	map<string, int> entity_num;
 	map<string, int>::iterator entity_num_iter;

	vector<int>  IfcCite;
	stringstream sss;
	bool isPart = false;
	string tmp_partial = "";
	int compCount = 0, totalCount = 0, predelete = 0;
	umSet.totalCount = 0;

	for (getline(ifile, ifcInstance); ifcInstance != "ENDSEC;"; getline(ifile, ifcInstance))
	{
		trim(ifcInstance);
		//combines multi-lines
		if (ifcInstance == "")
			continue;
		else if (ifcInstance[ifcInstance.size() - 1] != ';')
		{
			if (!isPart)
			{
				isPart = true;
				tmp_partial = ifcInstance;
			}
			else
				tmp_partial += ifcInstance;
			continue;
		}
		else if (isPart)
		{
			ifcInstance = tmp_partial + ifcInstance;
			tmp_partial = "";
			isPart = false;
		}

		++totalCount;
		IfcCite.clear();
		ifcIst = new IfcInstance();
		ifcIst->id = IfcInstanceAna(ifcInstance, ifcIst->type, ifcIst->content, IfcCite);
		ifcIst->flag = IfcCite.empty();

		if (ifcIst->flag)
		{
			++iterCount;
			type_content_iter = mSet.type_content.find(ifcIst->type);
			if (type_content_iter == mSet.type_content.end())
			{
				content_id = new map<string, int>();
				//保留原始id编号
				//content_id->insert(map<string,int>::value_type(ifcIst->content,ifcIst->id));
				//mSet.type_content.insert(map<string,map<string,int>>::value_type(ifcIst->type,*content_id));
				//mSet.id_baseID.insert(map<int,int>::value_type(ifcIst->id,ifcIst->id));
				////sss.str("");
				////sss<<'#'<<ifcIst->id<<'='<<ifcIst->type<<'('<<ifcIst->content<<");"<<endl;

				//将id重新编号
				content_id->insert(map<string, int>::value_type(ifcIst->content, ++newInc));
				mSet.type_content.insert(map<string, map<string, int>>::value_type(ifcIst->type, *content_id));
				mSet.id_baseID.insert(map<int, int>::value_type(ifcIst->id, newInc));
				ifcIst->id = newInc;
				///*sss.str("");
				//sss<<'#'<<newInc<<'='<<ifcIst->type<<'('<<ifcIst->content<<");"<<endl; */

				mSet.index.push_back(umSet.instance.size());

				umSet.instance.push_back(*ifcIst);
				umSet.flag.push_back(false);
				--umSet.totalCount;

				//mSet.baseInstance.push_back(sss.str());
			}
			else
			{
				content_id_iter = type_content_iter->second.find(ifcIst->content);
				if (content_id_iter == type_content_iter->second.end())
				{
					//保留原始id编号
					//type_content_iter->second.insert(map<string,int>::value_type(ifcIst->content,ifcIst->id));
					//mSet.id_baseID.insert(map<int,int>::value_type(ifcIst->id,ifcIst->id));
					////sss.str("");
					////sss<<'#'<<ifcIst->id<<'='<<ifcIst->type<<'('<<ifcIst->content<<");"<<endl;

					//将id重新编号
					type_content_iter->second.insert(map<string, int>::value_type(ifcIst->content, ++newInc));
					mSet.id_baseID.insert(map<int, int>::value_type(ifcIst->id, newInc));
					ifcIst->id = newInc;
					///*sss.str("");
					//sss<<'#'<<newInc<<'='<<ifcIst->type<<'('<<ifcIst->content<<");"<<endl;*/

					mSet.index.push_back(umSet.instance.size());

					umSet.instance.push_back(*ifcIst);
					umSet.flag.push_back(false);
					--umSet.totalCount;

					//mSet.baseInstance.push_back(sss.str());
				}
				else
				{
					//记录删除信息
					++deleteCount;
					if (flag > 0)
					{
						entity_num_iter = entity_num.find(ifcIst->type);
						if (entity_num_iter == entity_num.end())
							entity_num.insert(map<string, int>::value_type(ifcIst->type, 1));
						else
							++(entity_num_iter->second);
					}
					mSet.id_baseID.insert(map<int, int>::value_type(ifcIst->id, content_id_iter->second));
				}
			}
			delete ifcIst;
		}
		else
		{
			umSet.instance.push_back(*ifcIst);


			//umSet.instance.push_back(*ifcIst);
			umSet.flag.push_back(true);
		}
	}
	umSet.totalCount += umSet.instance.size();

	//ss << (double)deleteCount / totalCount << "\r\n";
	ss << "Step 1:" << iterCount << " delete:" << deleteCount << "\r\n";
	for (entity_num_iter = entity_num.begin(); entity_num_iter != entity_num.end(); ++entity_num_iter)
		ss << entity_num_iter->first << ':' << entity_num_iter->second << "\r\n";
	if (flag == 1)
	{
		ofile2 << "Step NO.,Total Num.,Delete Num.,Memo" << "\r\n";
		ofile2 << "Step 1:," << iterCount << "," << deleteCount << ",";
		for (entity_num_iter = entity_num.begin(); entity_num_iter != entity_num.end(); ++entity_num_iter)
			ofile2 << entity_num_iter->first << ':' << entity_num_iter->second << ' ';
		ofile2 << "\r\n";
		entity_num.clear();
	}

	process = 50;
	pInfo = ss.str();
	ReleaseSemaphore(hSemaphore, 1, NULL);

	//规范化未匹配集合中的引用
	for (int i = 0; i<umSet.instance.size(); ++i)
		umSet.instance[i].flag = StandardContent(umSet.instance[i].content, mSet.id_baseID);
	ifcEnd = "ENDSEC;\n";
	while (getline(ifile, ifcInstance))
	{
		ifcEnd += ifcInstance + "\n";
	}
	ifile.close();

	int iterationTime = 0;//统计迭代次数
	bool isSt2 = true;	//是否进行迭代压缩

	while (umSet.totalCount != 0)
	{
		++iterationTime;
		iterCount = 0;
		predelete = deleteCount;
		for (int i = 0; i<umSet.instance.size(); ++i)
		{
			//不迭代处理，只单纯将集合中的所有实例加入到输出队列中
			if (!isSt2)
			{
				sss.str("");
				sss << '#' << umSet.instance[i].id << '=' << umSet.instance[i].type << '(' << umSet.instance[i].content << ");" << "\r\n";
				mSet.baseInstance.push_back(sss.str());
				--umSet.totalCount;
				umSet.flag[i] = false;
				continue;
			}
			//迭代处理部分
			if (umSet.flag[i] && umSet.instance[i].flag)
			{
				++iterCount;
				type_content_iter = mSet.type_content.find(umSet.instance[i].type);
				if (type_content_iter == mSet.type_content.end())
				{
					content_id = new map<string, int>();
					////保留原始id编号
					//content_id->insert(map<string,int>::value_type(umSet.instance[i].content,umSet.instance[i].id));
					//mSet.type_content.insert(map<string,map<string,int>>::value_type(umSet.instance[i].type,*content_id));
					//mSet.id_baseID.insert(map<int,int>::value_type(umSet.instance[i].id,umSet.instance[i].id));
					//sss.str("");
					//sss<<'#'<<umSet.instance[i].id<<'='<<umSet.instance[i].type<<'('<<umSet.instance[i].content<<");"<<endl;

					//将id重新编号
					content_id->insert(map<string, int>::value_type(umSet.instance[i].content, ++newInc));
					mSet.type_content.insert(map<string, map<string, int>>::value_type(umSet.instance[i].type, *content_id));
					mSet.id_baseID.insert(map<int, int>::value_type(umSet.instance[i].id, newInc));
					umSet.instance[i].id = newInc;
					///*sss.str("");
					//sss<<'#'<<newInc<<'='<<umSet.instance[i].type<<'('<<umSet.instance[i].content<<");"<<endl;*/

					mSet.index.push_back(i);
					//mSet.baseInstance.push_back(sss.str());
				}
				else
				{
					content_id_iter = type_content_iter->second.find(umSet.instance[i].content);
					if (content_id_iter == type_content_iter->second.end())
					{
						////保留原始id编号
						//type_content_iter->second.insert(map<string,int>::value_type(umSet.instance[i].content,umSet.instance[i].id));
						//mSet.id_baseID.insert(map<int,int>::value_type(umSet.instance[i].id,umSet.instance[i].id));
						////sss.str("");
						////sss<<'#'<<umSet.instance[i].id<<'='<<umSet.instance[i].type<<'('<<umSet.instance[i].content<<");"<<endl;

						//将id重新编号
						type_content_iter->second.insert(map<string, int>::value_type(umSet.instance[i].content, ++newInc));
						mSet.id_baseID.insert(map<int, int>::value_type(umSet.instance[i].id, newInc));
						umSet.instance[i].id = newInc;
						///*sss.str("");
						//sss<<'#'<<newInc<<'='<<umSet.instance[i].type<<'('<<umSet.instance[i].content<<");"<<endl;*/

						mSet.index.push_back(i);
						//mSet.baseInstance.push_back(sss.str());
					}
					else
					{
						//记录删除信息
						++deleteCount;
                        if (umSet.instance[i].type == "IFCSHAPEREPRESENTATION")
                        {
                            int i = 1;
                        }

						if (flag > 0)
						{
							entity_num_iter = entity_num.find(umSet.instance[i].type);
							if (entity_num_iter == entity_num.end())
								entity_num.insert(map<string, int>::value_type(umSet.instance[i].type, 1));
							else
								++(entity_num_iter->second);
						}
						mSet.id_baseID.insert(map<int, int>::value_type(umSet.instance[i].id, content_id_iter->second));
						//cout<<iterationTime<<':'<<umSet.instance[i].id<<umSet.instance[i].type<<endl;
					}
				}
				--umSet.totalCount;
				umSet.flag[i] = false;
			}
		}

		//规范化未匹配集合中的引用
		for (int i = 0; i<umSet.instance.size(); ++i)
		{
			if (umSet.flag[i])
			{
				umSet.instance[i].flag = StandardContent(umSet.instance[i].content, mSet.id_baseID);
			}
		}

		//输出每一步的删除实例数
		ss << "Step 2(iter :" << iterationTime << "):" << iterCount << " delete:" << deleteCount - predelete << "\r\n";
		for (entity_num_iter = entity_num.begin(); entity_num_iter != entity_num.end(); ++entity_num_iter)
			ss << entity_num_iter->first << ':' << entity_num_iter->second << "\r\n";
		if (flag == 1)
		{
			ofile2 << "Step 2(iter :" << iterationTime << "):," << iterCount << "," << deleteCount - predelete << ",";
			for (entity_num_iter = entity_num.begin(); entity_num_iter != entity_num.end(); ++entity_num_iter)
				ofile2 << entity_num_iter->first << ':' << entity_num_iter->second << ' ';
			ofile2 << "\r\n";
			entity_num.clear();
		}
		pInfo = ss.str();
		ReleaseSemaphore(hSemaphore, 1, NULL);
	}

	if (flag > 1)
	{
		ofile2 << "Entity Name,Removed Number" << endl;
		for (entity_num_iter = entity_num.begin(); entity_num_iter != entity_num.end(); ++entity_num_iter)
			ofile2 << entity_num_iter->first << ',' << entity_num_iter->second << endl;
	}

	ofile2.close();

	endT = clock();

	//std::cout << "Sa->b:" << 1.0 << " Sb->a" << 1.0 << endl;

	process = 90;
	pInfo = ss.str();
	ReleaseSemaphore(hSemaphore, 1, NULL);

	ofstream ofile(fileName4Write);
	ofile << ifcHead;
	for (int i = 0; i<mSet.index.size(); ++i)
		ofile << '#' << umSet.instance[mSet.index[i]].id << '=' << umSet.instance[mSet.index[i]].type << '(' << umSet.instance[mSet.index[i]].content << ");" << endl;
	for (int i = 0; i<umSet.instance.size(); ++i)
	{
		if (umSet.flag[i])
			ofile << '#' << umSet.instance[i].id << '=' << umSet.instance[i].type << '(' << umSet.instance[i].content << ");" << endl;
	}
	ofile << ifcEnd;
	ofile.close();

	struct stat info;
	stat(fileName.c_str(), &info);
	int oriSize = info.st_size;
	stat(fileName4Write.c_str(), &info);
	int comSize = info.st_size;

	ss << "Compression Ratio: " << ((double)deleteCount / totalCount) * 100 << " %\r\n";
	//ss << "Compression Ratio: " << (double)comSize / oriSize << "\r\n";

	ss << "Original File Size: " << (double)oriSize / (1024 * 1024) << " MB\r\n";

	ss << "Compressed File Size: " << (double)comSize / (1024 * 1024) << " MB\r\n";

	ss << "Time cost: " << (double)(endT - startT) / CLOCKS_PER_SEC << " s\r\n";

	process = 100;
	pInfo = ss.str();
	ReleaseSemaphore(hSemaphore, 1, NULL);
}


void CIfcCompression::IfcCompressor(stringstream &ifile, string fileName4Write, int flag)
{
	time_t startT, endT;
	startT = clock();
	ofstream ofile2(fileName4Write+".csv");
	string ifcHead = "", ifcEnd = "", ifcInstance;
	for (getline(ifile, ifcInstance); ifcInstance != "DATA;"; getline(ifile, ifcInstance))
	{
		//std::cout<< ifile<<"  " << ifcInstance <<endl;
		ifcHead += ifcInstance + "\n";

		//std::cout<< ifile<<"  " << ifcHead <<endl;
	}
	ifcHead += "DATA;\n";
	//std::cout<< ifcHead<<endl;
	int newInc = 0;	//新的实例编号
	int iterCount = 0;	//每轮迭代的实例分析数统计
	int deleteCount = 0;	//用于统计删除的实例

	//第一遍处理，将文件中不存在引用的实例进行简化
	IfcInstance *ifcIst;
	MatchSet mSet;
	UnmatchSet umSet;

	map<string, int> *content_id;
	map<string, map<string, int>>::iterator type_content_iter;
	map<string, int>::iterator content_id_iter;
	map<string, int> entity_num;
	map<string, int>::iterator entity_num_iter;

	vector<int>  IfcCite;
	stringstream sss;
	bool isPart = false;
	string tmp_partial = "";
	int compCount = 0, totalCount = 0, predelete = 0;
	umSet.totalCount = 0;

	for (getline(ifile, ifcInstance); ifcInstance != "ENDSEC;"; getline(ifile, ifcInstance))
	{
		trim(ifcInstance);
		//combines multi-lines
		if (ifcInstance == "")
			continue;
		else if (ifcInstance[ifcInstance.size() - 1] != ';')
		{
			if (!isPart)
			{
				isPart = true;
				tmp_partial = ifcInstance;
			}
			else
				tmp_partial += ifcInstance;
			continue;
		}
		else if (isPart)
		{
			ifcInstance = tmp_partial + ifcInstance;
			tmp_partial = "";
			isPart = false;
		}
		++totalCount;
		IfcCite.clear();
		ifcIst = new IfcInstance();
		ifcIst->id = IfcInstanceAna(ifcInstance, ifcIst->type, ifcIst->content, IfcCite);
		ifcIst->flag = IfcCite.empty();

		if (ifcIst->flag)
		{
			++iterCount;
			type_content_iter = mSet.type_content.find(ifcIst->type);
			if (type_content_iter == mSet.type_content.end())
			{
				content_id = new map<string, int>();
				//保留原始id编号
				//content_id->insert(map<string,int>::value_type(ifcIst->content,ifcIst->id));
				//mSet.type_content.insert(map<string,map<string,int>>::value_type(ifcIst->type,*content_id));
				//mSet.id_baseID.insert(map<int,int>::value_type(ifcIst->id,ifcIst->id));
				////sss.str("");
				////sss<<'#'<<ifcIst->id<<'='<<ifcIst->type<<'('<<ifcIst->content<<");"<<endl;

				//将id重新编号
				content_id->insert(map<string, int>::value_type(ifcIst->content, ++newInc));
				mSet.type_content.insert(map<string, map<string, int>>::value_type(ifcIst->type, *content_id));
				mSet.id_baseID.insert(map<int, int>::value_type(ifcIst->id, newInc));
				ifcIst->id = newInc;
				///*sss.str("");
				//sss<<'#'<<newInc<<'='<<ifcIst->type<<'('<<ifcIst->content<<");"<<endl; */

				mSet.index.push_back(umSet.instance.size());

				umSet.instance.push_back(*ifcIst);
				umSet.flag.push_back(false);
				--umSet.totalCount;

				//mSet.baseInstance.push_back(sss.str());
			}
			else
			{
				content_id_iter = type_content_iter->second.find(ifcIst->content);
				if (content_id_iter == type_content_iter->second.end())
				{
					//保留原始id编号
					//type_content_iter->second.insert(map<string,int>::value_type(ifcIst->content,ifcIst->id));
					//mSet.id_baseID.insert(map<int,int>::value_type(ifcIst->id,ifcIst->id));
					////sss.str("");
					////sss<<'#'<<ifcIst->id<<'='<<ifcIst->type<<'('<<ifcIst->content<<");"<<endl;

					//将id重新编号
					type_content_iter->second.insert(map<string, int>::value_type(ifcIst->content, ++newInc));
					mSet.id_baseID.insert(map<int, int>::value_type(ifcIst->id, newInc));
					ifcIst->id = newInc;
					///*sss.str("");
					//sss<<'#'<<newInc<<'='<<ifcIst->type<<'('<<ifcIst->content<<");"<<endl;*/

					mSet.index.push_back(umSet.instance.size());

					umSet.instance.push_back(*ifcIst);
					umSet.flag.push_back(false);
					--umSet.totalCount;

					//mSet.baseInstance.push_back(sss.str());
				}
				else
				{
					//记录删除信息
					++deleteCount;
					if (flag > 0)
					{
						entity_num_iter = entity_num.find(ifcIst->type);
						if (entity_num_iter == entity_num.end())
							entity_num.insert(map<string, int>::value_type(ifcIst->type, 1));
						else
							++(entity_num_iter->second);
					}
					mSet.id_baseID.insert(map<int, int>::value_type(ifcIst->id, content_id_iter->second));
				}
			}
			delete ifcIst;
		}
		else
		{
			umSet.instance.push_back(*ifcIst);


			//umSet.instance.push_back(*ifcIst);
			umSet.flag.push_back(true);
		}
	}
	umSet.totalCount += umSet.instance.size();
	std::cout << (double)deleteCount / totalCount << endl;
	std::cout << "Step 1:" << iterCount << " delete:" << deleteCount << endl;
	for (entity_num_iter = entity_num.begin(); entity_num_iter != entity_num.end(); ++entity_num_iter)
		std::cout << entity_num_iter->first << ':' << entity_num_iter->second << endl;
	if (flag == 1)
	{
		ofile2 << "Step NO.,Total Num.,Delete Num.,Memo" << endl;
		ofile2 << "Step 1:," << iterCount << "," << deleteCount << ",";
		for (entity_num_iter = entity_num.begin(); entity_num_iter != entity_num.end(); ++entity_num_iter)
			ofile2 << entity_num_iter->first << ':' << entity_num_iter->second << ' ';
		ofile2 << endl;
		entity_num.clear();
	}

	//规范化未匹配集合中的引用
	for (int i = 0; i<umSet.instance.size(); ++i)
		umSet.instance[i].flag = StandardContent(umSet.instance[i].content, mSet.id_baseID);
	ifcEnd = "ENDSEC;\n";
	while (getline(ifile, ifcInstance))
	{
		ifcEnd += ifcInstance + "\n";
	}

	int iterationTime = 0;//统计迭代次数
	bool isSt2 = true;	//是否进行迭代压缩

	while (umSet.totalCount != 0)
	{
		++iterationTime;
		iterCount = 0;
		predelete = deleteCount;
		for (int i = 0; i<umSet.instance.size(); ++i)
		{
			//不迭代处理，只单纯将集合中的所有实例加入到输出队列中
			if (!isSt2)
			{
				sss.str("");
				sss << '#' << umSet.instance[i].id << '=' << umSet.instance[i].type << '(' << umSet.instance[i].content << ");" << endl;
				mSet.baseInstance.push_back(sss.str());
				--umSet.totalCount;
				umSet.flag[i] = false;
				continue;
			}
			//迭代处理部分
			if (umSet.flag[i] && umSet.instance[i].flag)
			{
				++iterCount;
				type_content_iter = mSet.type_content.find(umSet.instance[i].type);
				if (type_content_iter == mSet.type_content.end())
				{
					content_id = new map<string, int>();
					////保留原始id编号
					//content_id->insert(map<string,int>::value_type(umSet.instance[i].content,umSet.instance[i].id));
					//mSet.type_content.insert(map<string,map<string,int>>::value_type(umSet.instance[i].type,*content_id));
					//mSet.id_baseID.insert(map<int,int>::value_type(umSet.instance[i].id,umSet.instance[i].id));
					//sss.str("");
					//sss<<'#'<<umSet.instance[i].id<<'='<<umSet.instance[i].type<<'('<<umSet.instance[i].content<<");"<<endl;

					//将id重新编号
					content_id->insert(map<string, int>::value_type(umSet.instance[i].content, ++newInc));
					mSet.type_content.insert(map<string, map<string, int>>::value_type(umSet.instance[i].type, *content_id));
					mSet.id_baseID.insert(map<int, int>::value_type(umSet.instance[i].id, newInc));
					umSet.instance[i].id = newInc;
					///*sss.str("");
					//sss<<'#'<<newInc<<'='<<umSet.instance[i].type<<'('<<umSet.instance[i].content<<");"<<endl;*/

					mSet.index.push_back(i);
					//mSet.baseInstance.push_back(sss.str());
				}
				else
				{
					content_id_iter = type_content_iter->second.find(umSet.instance[i].content);
					if (content_id_iter == type_content_iter->second.end())
					{
						////保留原始id编号
						//type_content_iter->second.insert(map<string,int>::value_type(umSet.instance[i].content,umSet.instance[i].id));
						//mSet.id_baseID.insert(map<int,int>::value_type(umSet.instance[i].id,umSet.instance[i].id));
						////sss.str("");
						////sss<<'#'<<umSet.instance[i].id<<'='<<umSet.instance[i].type<<'('<<umSet.instance[i].content<<");"<<endl;

						//将id重新编号
						type_content_iter->second.insert(map<string, int>::value_type(umSet.instance[i].content, ++newInc));
						mSet.id_baseID.insert(map<int, int>::value_type(umSet.instance[i].id, newInc));
						umSet.instance[i].id = newInc;
						///*sss.str("");
						//sss<<'#'<<newInc<<'='<<umSet.instance[i].type<<'('<<umSet.instance[i].content<<");"<<endl;*/

						mSet.index.push_back(i);
						//mSet.baseInstance.push_back(sss.str());
					}
					else
					{
						//记录删除信息
						++deleteCount;
						if (flag > 0)
						{
							entity_num_iter = entity_num.find(umSet.instance[i].type);
							if (entity_num_iter == entity_num.end())
								entity_num.insert(map<string, int>::value_type(umSet.instance[i].type, 1));
							else
								++(entity_num_iter->second);
						}
						mSet.id_baseID.insert(map<int, int>::value_type(umSet.instance[i].id, content_id_iter->second));
						//cout<<iterationTime<<':'<<umSet.instance[i].id<<umSet.instance[i].type<<endl;
					}
				}
				--umSet.totalCount;
				umSet.flag[i] = false;
			}
		}

		//规范化未匹配集合中的引用
		for (int i = 0; i<umSet.instance.size(); ++i)
		{
			if (umSet.flag[i])
			{
				umSet.instance[i].flag = StandardContent(umSet.instance[i].content, mSet.id_baseID);
			}
		}

		//输出每一步的删除实例数
		std::cout << "Step 2(iter :" << iterationTime << "):" << iterCount << " delete:" << deleteCount - predelete << endl;
		for (entity_num_iter = entity_num.begin(); entity_num_iter != entity_num.end(); ++entity_num_iter)
			std::cout << entity_num_iter->first << ':' << entity_num_iter->second << endl;
		if (flag == 1)
		{
			ofile2 << "Step 2(iter :" << iterationTime << "):," << iterCount << "," << deleteCount - predelete << ",";
			for (entity_num_iter = entity_num.begin(); entity_num_iter != entity_num.end(); ++entity_num_iter)
				ofile2 << entity_num_iter->first << ':' << entity_num_iter->second << ' ';
			ofile2 << endl;
			entity_num.clear();
		}
	}

	if (flag > 1)
	{
		ofile2 << "Entity Name,Removed Number" << endl;
		for (entity_num_iter = entity_num.begin(); entity_num_iter != entity_num.end(); ++entity_num_iter)
			ofile2 << entity_num_iter->first << ',' << entity_num_iter->second << endl;
	}

	ofile2.close();

	endT = clock();

	std::cout << "Instance Compression Ratio:" << (double)deleteCount / totalCount << endl;

	std::cout << "reserved instances|total instances:" << newInc << "|" << totalCount << endl;

	std::cout << "Time costs:" << (double)(endT - startT) / CLOCKS_PER_SEC << endl;

	std::cout << "Sa->b:" << 1.0 << " Sb->a" << 1.0 << endl;

	ofstream ofile(fileName4Write);
	ofile << ifcHead;
	//将
	for (int i = 0; i<mSet.index.size(); ++i)
		ofile << '#' << umSet.instance[mSet.index[i]].id << '=' << umSet.instance[mSet.index[i]].type << '(' << umSet.instance[mSet.index[i]].content << ");" << endl;
	for (int i = 0; i<umSet.instance.size(); ++i)
	{
		if (umSet.flag[i])
			ofile << '#' << umSet.instance[i].id << '=' << umSet.instance[i].type << '(' << umSet.instance[i].content << ");" << endl;
	}
	ofile << ifcEnd;
	ofile.close();
}


void CIfcCompression::IfcCompressorAna(stringstream &ifile,stringstream &result,int &totalCount,int &deleteCount)
{
	time_t startT,endT;
	startT=clock();
	string ifcHead="",ifcEnd="",ifcInstance;
	for(getline(ifile,ifcInstance);ifcInstance!="DATA;";getline(ifile,ifcInstance))
	{
		ifcHead+=ifcInstance+"\n";
	}
	ifcHead+="DATA;\n";

	int newInc=0;

	//第一遍处理，将文件中不存在引用的实例进行简化
	IfcInstance *ifcIst;
	MatchSet mSet;
	UnmatchSet umSet;
	map<string,int> *content_id;
	map<string,map<string,int>>::iterator type_content_iter;
	map<string,int>::iterator content_id_iter;
	//统计个实体的压缩比
	map<string,int> type_total,type_delete;
	map<string,int>::iterator type_total_iter,type_delete_iter;


	vector<int>  IfcCite;
	stringstream sss;
	bool isPart = false;
	string tmp_partial = "";
	deleteCount=0,totalCount=0;
	for(getline(ifile,ifcInstance);ifcInstance!="ENDSEC;";getline(ifile,ifcInstance))
	{
		trim(ifcInstance);
		//combines multi-lines
		if (ifcInstance == "")
			continue;
		else if (ifcInstance[ifcInstance.size() - 1] != ';')
		{
			if (!isPart)
			{
				isPart = true;
				tmp_partial = ifcInstance;
			}
			else
				tmp_partial += ifcInstance;
			continue;
		}
		else if (isPart)
		{
			ifcInstance = tmp_partial + ifcInstance;
			tmp_partial = "";
			isPart = false;
		}

		++totalCount;
		IfcCite.clear();
		ifcIst=new IfcInstance();
		ifcIst->id=IfcInstanceAna(ifcInstance,ifcIst->type,ifcIst->content,IfcCite);
		ifcIst->flag=IfcCite.empty();
		//统计不同类总数
		type_total_iter=type_total.find(ifcIst->type);
		if (type_total_iter!=type_total.end())
			++type_total_iter->second;
		else
			type_total.insert(map<string,int>::value_type(ifcIst->type,1));

		if (ifcIst->flag)
		{
			type_content_iter=mSet.type_content.find(ifcIst->type);
			if (type_content_iter==mSet.type_content.end())
			{
				content_id=new map<string,int>();
				//content_id->insert(map<string,int>::value_type(ifcIst->content,ifcIst->id));
				content_id->insert(map<string,int>::value_type(ifcIst->content,++newInc));
				mSet.type_content.insert(map<string,map<string,int>>::value_type(ifcIst->type,*content_id));
				//mSet.id_baseID.insert(map<int,int>::value_type(ifcIst->id,ifcIst->id));
				mSet.id_baseID.insert(map<int,int>::value_type(ifcIst->id,newInc));
				//sss.clear();
				sss.str("");
				//sss<<'#'<<ifcIst->id<<'='<<ifcIst->type<<'('<<ifcIst->content<<");"<<endl;
				sss<<'#'<<newInc<<'='<<ifcIst->type<<'('<<ifcIst->content<<");"<<endl;
				mSet.baseInstance.push_back(sss.str());
			}else
			{
				content_id_iter=type_content_iter->second.find(ifcIst->content);
				if (content_id_iter==type_content_iter->second.end())
				{
					//type_content_iter->second.insert(map<string,int>::value_type(ifcIst->content,ifcIst->id));
					type_content_iter->second.insert(map<string,int>::value_type(ifcIst->content,++newInc));
					//mSet.id_baseID.insert(map<int,int>::value_type(ifcIst->id,ifcIst->id));
					mSet.id_baseID.insert(map<int,int>::value_type(ifcIst->id,newInc));
					//sss.clear();
					sss.str("");
					//sss<<'#'<<ifcIst->id<<'='<<ifcIst->type<<'('<<ifcIst->content<<");"<<endl;
					sss<<'#'<<newInc<<'='<<ifcIst->type<<'('<<ifcIst->content<<");"<<endl;
					mSet.baseInstance.push_back(sss.str());
				}else
				{
					//统计删减数
					type_delete_iter=type_delete.find(ifcIst->type);
					if (type_delete_iter!=type_delete.end())
						++type_delete_iter->second;
					else
						type_delete.insert(map<string,int>::value_type(ifcIst->type,1));

					++deleteCount;
					mSet.id_baseID.insert(map<int,int>::value_type(ifcIst->id,content_id_iter->second));
				}
			}
			delete ifcIst;
		}else
		{
			umSet.instance.push_back(*ifcIst);
			umSet.flag.push_back(true);
		}
	}
	umSet.totalCount=umSet.instance.size();

	//规范化未匹配集合中的引用
	for (int i=0;i<umSet.instance.size();++i)
	{
		umSet.instance[i].flag=StandardContent(umSet.instance[i].content,mSet.id_baseID);
	}
	ifcEnd="ENDSEC;\n";
	while(getline(ifile,ifcInstance))
	{
		ifcEnd+=ifcInstance+"\n";
	}

	while (umSet.totalCount!=0)
	{
		for (int i=0;i<umSet.instance.size();++i)
		{
			if (umSet.flag[i]&&umSet.instance[i].flag)
			{
				type_content_iter=mSet.type_content.find(umSet.instance[i].type);
				if (type_content_iter==mSet.type_content.end())
				{
					content_id=new map<string,int>();
					//content_id->insert(map<string,int>::value_type(umSet.instance[i].content,umSet.instance[i].id));
					content_id->insert(map<string,int>::value_type(umSet.instance[i].content,++newInc));
					mSet.type_content.insert(map<string,map<string,int>>::value_type(umSet.instance[i].type,*content_id));
					//mSet.id_baseID.insert(map<int,int>::value_type(umSet.instance[i].id,umSet.instance[i].id));
					mSet.id_baseID.insert(map<int,int>::value_type(umSet.instance[i].id,newInc));
					//sss.clear();
					sss.str("");
					//sss<<'#'<<umSet.instance[i].id<<'='<<umSet.instance[i].type<<'('<<umSet.instance[i].content<<");"<<endl;
					sss<<'#'<<newInc<<'='<<umSet.instance[i].type<<'('<<umSet.instance[i].content<<");"<<endl;
					mSet.baseInstance.push_back(sss.str());
				}else
				{
					content_id_iter=type_content_iter->second.find(umSet.instance[i].content);
					if (content_id_iter==type_content_iter->second.end())
					{
						//type_content_iter->second.insert(map<string,int>::value_type(umSet.instance[i].content,umSet.instance[i].id));
						//mSet.id_baseID.insert(map<int,int>::value_type(umSet.instance[i].id,umSet.instance[i].id));
						type_content_iter->second.insert(map<string,int>::value_type(umSet.instance[i].content,++newInc));
						mSet.id_baseID.insert(map<int,int>::value_type(umSet.instance[i].id,newInc));
						//sss.clear();
						sss.str("");
						//sss<<'#'<<umSet.instance[i].id<<'='<<umSet.instance[i].type<<'('<<umSet.instance[i].content<<");"<<endl;
						sss<<'#'<<newInc<<'='<<umSet.instance[i].type<<'('<<umSet.instance[i].content<<");"<<endl;
						mSet.baseInstance.push_back(sss.str());
					}else
					{
						//统计删减数
						type_delete_iter=type_delete.find(umSet.instance[i].type);
						if (type_delete_iter!=type_delete.end())
							++type_delete_iter->second;
						else
							type_delete.insert(map<string,int>::value_type(umSet.instance[i].type,1));

						++deleteCount;
						mSet.id_baseID.insert(map<int,int>::value_type(umSet.instance[i].id,content_id_iter->second));
					}
				}
				--umSet.totalCount;
				umSet.flag[i]=false;
			}
		}

		//规范化未匹配集合中的引用
		for (int i=0;i<umSet.instance.size();++i)
		{
			if (umSet.flag[i])
				umSet.instance[i].flag=StandardContent(umSet.instance[i].content,mSet.id_baseID);
		}
	}
	endT=clock();

	result<<"Cost Time,"<<(double)(endT-startT)/CLOCKS_PER_SEC<<endl;
	for (type_delete_iter=type_delete.begin();type_delete_iter!=type_delete.end();++type_delete_iter)
	{
		result<<type_delete_iter->first<<","<<type_delete_iter->second<<endl;
	}

}

void CIfcCompression::IfcCompare(string fileA,string fileB,float &sA2B,float &sB2A,float &machR,float &missR,float &guidPR, float &AR)
{

}

void CIfcCompression::IfcInstanceAna(string instance,int &id,string &type,string &content,vector<int> &cite)
{
	cite.clear();
	trim(instance);
	int s=instance.find_first_of('=');
	id=atoi(instance.substr(1,s-1).c_str());
	content=instance.substr(s+1,instance.length()-s-1);
	s=content.find_first_of('(');
	type=content.substr(0,s);
	trim(type);
	content=content.substr(s+1,content.find_last_of(')')-s-1);
	//处理内容
	string pp,str_static="";
	stringstream ss(content);
	while (getline(ss,pp,','))
	{
		trim(pp);
		if (pp[0]=='(')
		{
			str_static+='(';
			bool flag=false; 
			if (pp[pp.size()-1]==')')
			{
				pp=pp.substr(1,pp.size()-2);
				flag=true;
			}
			else
				pp=pp.substr(1,pp.size()-1);
			if (pp[0]=='#')
				cite.push_back(atoi(pp.substr(1,pp.size()-1).c_str()));
			else
				str_static+=pp;
			if (flag)
				str_static+=')';
		}else if (pp[pp.size()-1]==')')	//(#12,#14)情况
		{
			pp=pp.substr(0,pp.size()-1);
			if (pp[0]=='#')		
				cite.push_back(atoi(pp.substr(1,pp.size()-1).c_str()));
			else
				str_static+=pp;
			str_static+=')';
		}else
		{
			if (pp[0]=='#')
				cite.push_back(atoi(pp.substr(1,pp.size()-1).c_str()));
			else
				str_static+=pp;
		}
		str_static+=',';
	}
	content=str_static.substr(0,str_static.size()-1);
}

int CIfcCompression::IfcInstanceAna(string instance,string &type,string &content,vector<int> &cite)
{
	trim(instance);
	string index="";
	int s=instance.find_first_of('='),id;
	index=instance.substr(1,s-1);
	id=atoi(index.c_str());
    if (id == 995)
    {
        int k = 1;
    }
	content=instance.substr(s+1,instance.length()-s-1);
	s=content.find_first_of('(');
	type=content.substr(0,s);
	trim(type);
	content=StandardContent(type,content.substr(s+1,content.find_last_of(')')-s-1),cite,FPR);
	//char *ptr;
	//double s1=strtod("-1234f",&ptr);
	

	return id;
}

//string CIfcCompression::StandardContent(string &content,vector<int> &cite)
//{
//	cite.clear();
//	string pp,sptr,str="";
//	char *ptr;
//	double dat;
//	stringstream ss(content);
//	stringstream sss;
//	sss.setf(ios::fixed);
//	sss.precision(MAX_PRECISION);
//	while (getline(ss,pp,','))
//	{
//		trim(pp);
//		if (pp[0]=='(')
//		{
//			str+='(';
//			bool flag=false; 
//			if (pp[pp.size()-1]==')')
//			{
//				pp=pp.substr(1,pp.size()-2);
//				flag=true;
//			}
//			else
//				pp=pp.substr(1,pp.size()-1);
//			if (pp[0]=='#')
//			{
//				cite.push_back(atoi(pp.substr(1,pp.size()-1).c_str()));
//				str+=pp;
//			}else
//			{
//				dat=strtod(pp.c_str(),&ptr);
//				sptr=string(ptr);
//				if (sptr!=")"&&sptr!="")
//					str+=pp;
//				else
//				{
//					//sss.clear();
//					sss.str("");
//					sss<<dat;
//					str+=sss.str();
//				}
//			}
//			if (flag)
//				str+=')';
//		}else if (pp[pp.size()-1]==')')
//		{
//			pp=pp.substr(0,pp.size()-1);
//			if (pp[0]=='#')
//			{
//				cite.push_back(atoi(pp.substr(1,pp.size()-1).c_str()));
//				str+=pp;
//			}else
//			{
//				dat=strtod(pp.c_str(),&ptr);
//				sptr=string(ptr);
//				if (sptr!="")
//					str+=pp;
//				else
//				{
//					//sss.clear();
//					sss.str("");
//					sss<<dat;
//					str+=sss.str();
//				}
//			}
//			str+=')';
//		}else
//		{
//			if (pp[0]=='#')
//			{
//				cite.push_back(atoi(pp.substr(1,pp.size()-1).c_str()));
//				str+=pp;
//			}else
//			{
//				dat=strtod(pp.c_str(),&ptr);
//				sptr=string(ptr);
//				if (sptr!="")
//					str+=pp;
//				else
//				{
//					//sss.clear();
//					sss.str("");
//					sss<<dat;
//					str+=sss.str();
//				}
//			}
//		}
//		str+=',';
//	}
//	str=str.substr(0,str.size()-1);
//	return str;
//}

string CIfcCompression::StandardContent(string type,string &content,vector<int> &cite,bool islossy)
{
	cite.clear();
	string pp,sptr,str="";
	char *ptr;
	double dat;
	stringstream ss(content);
	stringstream sss;
	sss.setf(ios::fixed);
	sss.precision(MAX_PRECISION);
	while (getline(ss,pp,','))
	{
		trim(pp);
		if (pp[0]=='(')
		{
			str+='(';
			bool flag=false; 
			if (pp[pp.size()-1]==')')
			{
				pp=pp.substr(1,pp.size()-2);
				flag=true;
			}
			else
				pp=pp.substr(1,pp.size()-1);
			if (pp[0]=='#')
			{
				cite.push_back(atoi(pp.substr(1,pp.size()-1).c_str()));
				str+=pp;
			}else
			{
				dat=strtod(pp.c_str(),&ptr);
				sptr=string(ptr);
				if ((sptr!=")"&&sptr!="")||type!="IFCCARTESIANPOINT"||!islossy)
					str+=pp;
				else
				{
					//sss.clear();
					sss.str("");
					sss<<dat;
					str+=sss.str();
				}
			}
			if (flag)
				str+=')';
		}else if (pp[pp.size()-1]==')')	//(#12,#14)情况
		{
			pp=pp.substr(0,pp.size()-1);
			if (pp[0]=='#')
			{
				cite.push_back(atoi(pp.substr(1,pp.size()-1).c_str()));
				str+=pp;
			}else
			{
				dat=strtod(pp.c_str(),&ptr);
				sptr=string(ptr);
				if (sptr!=""||type!="IFCCARTESIANPOINT"||!islossy)
					str+=pp;
				else
				{
					//sss.clear();
					sss.str("");
					sss<<dat;
					str+=sss.str();
				}
			}
			str+=')';
		}else
		{
			if (pp[0]=='#')
			{
				cite.push_back(atoi(pp.substr(1,pp.size()-1).c_str()));
				str+=pp;
			}else
			{
				dat=strtod(pp.c_str(),&ptr);
				sptr=string(ptr);
				if (sptr!=""||type!="IFCCARTESIANPOINT"||!islossy)
					str+=pp;
				else
				{
					//sss.clear();
					sss.str("");
					sss<<dat;
					str+=sss.str();
				}
			}
		}
		str+=',';
	}
	str=str.substr(0,str.size()-1);
	return str;
}

bool CIfcCompression::StandardContent(string &content,hash_map<int,int> &rel)
{
	string pp;
	int id;
	bool allFind=true;
	hash_map<int,int>::iterator iter;
	stringstream ss(content);
	string newContent="";
	stringstream sss;
	while (getline(ss,pp,','))
	{
		
		trim(pp);
		if (pp[0]=='(')
		{
			newContent+='(';
			bool flag=false; 
			if (pp[pp.size()-1]==')')
			{
				pp=pp.substr(1,pp.size()-2);
				flag=true;
			}
			else
				pp=pp.substr(1,pp.size()-1);
			if (pp[0]=='#')
			{
				id=atoi(pp.substr(1,pp.size()-1).c_str());
				iter=rel.find(id);
				if (iter==rel.end())
					allFind=false;
				else
				{
					//sss.clear();
					sss.str("");
					sss<<"#"<<iter->second;
					pp=sss.str();
				}
			}
			newContent+=pp;
			if (flag)
				newContent+=')';
		}else if (pp[pp.size()-1]==')')
		{
			pp=pp.substr(0,pp.size()-1);
			if (pp[0]=='#')
			{
				id=atoi(pp.substr(1,pp.size()-1).c_str());
				iter=rel.find(id);
				if (iter==rel.end())
					allFind=false;
				else
				{
					//sss.clear();
					sss.str("");
					sss<<"#"<<iter->second;
					pp=sss.str();
				}
			}
			newContent+=pp+')';
		}else
		{
			if (pp[0]=='#')
			{
				id=atoi(pp.substr(1,pp.size()-1).c_str());
				iter=rel.find(id);
				if (iter==rel.end())
					allFind=false;
				else
				{
					//sss.clear();
					sss.str("");
					sss<<"#"<<iter->second;
					pp=sss.str();
				}
			}
			newContent+=pp;
		}
		newContent+=',';
	}
	newContent=newContent.substr(0,newContent.size()-1);
	content=allFind?newContent:content;
	return allFind;
}

void CIfcCompression::trim(string &str)
{
	if (str=="")
		return;
	else if (str==" ")
		str="";
	else
	{
		int s=str.find_first_not_of(' ');
		int e=str.find_last_not_of(' ');
		str=str.substr(s,e-s+1);
	}
}