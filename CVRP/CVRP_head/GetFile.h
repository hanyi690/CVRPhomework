
// FileIO.h
#ifndef GETFILE_H
#define GETFILE_H
#include <fstream>
#include <filesystem>
#include <sstream>
#include <regex>
#include<iostream>
#include"Graph.h"
#include<string>
using namespace std;
struct Data {
    int bestresult;
    string name;
    double AverageError_rate;
    int invalid_e_count;
    // 构造函数
  Data(int br, string n, double aer,int iec) : bestresult(br), name(n), AverageError_rate(aer), invalid_e_count(iec){

    }
};


using namespace std;
namespace fs = std::filesystem;
// 去除路径引号的工具函数
std::string removeQuotes(const std::string& path);
 
void ReadFile(int& V, int& K, int& Q, point*& p, int& Optimalvalue, std::string& FileName);
vector<string> getSortedFileList(string& folderPath);
void generateCSVReport(
    const std::string& filename,
    const std::vector<Data>& dataset
);
#endif 