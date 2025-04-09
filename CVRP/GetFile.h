#pragma once
#include <fstream>
#include <sstream>
#include <regex>
#include"graph.h"
using namespace std;

//去除双引号
string removeQuotes(const string& path) 
{
    string result = path;
    if (!result.empty() && result.front() == '"') 
    {
        result.erase(0, 1);
    }
    if (!result.empty() && result.back() == '"')
    {
        result.erase(result.size() - 1);
    }
    return result;
}

//读文件
void ReadFile(int& V,int& K,int& Q,point*& p,int& Optimalvalue)
{
    string FileName;
    
    while (1)
    {       
        cout << "输入文件地址：";
        cin >> FileName;  
        FileName = removeQuotes(FileName);
        ifstream testFile(FileName);
        if (testFile)
        {
            testFile.close();
            break;
        }
        else cout << "无法打开文件！" << endl;
    }

    ifstream inputFile(FileName);
    string line;
    regex rgx1("No of trucks:\\s*(\\d+).*Optimal value:\\s*(\\d+)");
    regex rgx2("DIMENSION :\\s*(\\d+)");
    regex rgx3("CAPACITY :\\s*(\\d+)");
    smatch match;

    while (getline(inputFile, line)) 
    {
        if (regex_search(line, match, rgx1)) 
        {
            K = stoi(match[1]);
            Optimalvalue = stoi(match[2]);
            continue;
        }
        if (regex_search(line, match, rgx2))
        {
            V = stoi(match[1]);
            continue;
        }
        if (regex_search(line, match, rgx3))
        {
            Q = stoi(match[1]);
            continue;
        }
        if (line.find("NODE_COORD_SECTION") != string::npos) 
        {
            p = new point[V];
            int order = 0;
           do
           { 
               getline(inputFile, line);
               istringstream ss(line);
               ss >> order >> p[order].x >> p[order].y;
           } while (line.find("DEMAND_SECTION") == string::npos);
           order = 0;
           do
           {
               getline(inputFile, line);
               istringstream ss(line);
               ss >> order >> p[order].q;
           } while (line.find("DEPOT_SECTION") == string::npos);
        }   
    }

    inputFile.close();
}