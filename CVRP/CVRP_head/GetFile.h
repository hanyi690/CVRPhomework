#pragma once
#include <fstream>
#include <filesystem>
#include <sstream>
#include <regex>
#include"Graph.h"

using namespace std;
namespace fs = std::filesystem;
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
void ReadFile(int& V, int& K, int& Q, point*& p, int& Optimalvalue, std::string& FileName) {
    FileName = removeQuotes(FileName);
    std::ifstream inputFile(FileName);
    if (!inputFile.is_open()) {
        throw std::runtime_error("无法打开文件: " + FileName);
    }

    // 初始化变量
    V = 0; K = 0; Q = 0; Optimalvalue = 0;
    bool hasDimension = false, hasCapacity = false, hasTruckInfo = false; bool hasName = false;

    string line;
    regex rgxName(R"(NAME\s*:\s*(.*))");
    regex rgx1(R"(No of trucks:\s*(\d+).*Optimal value:\s*(\d+))");
    regex rgx2(R"(DIMENSION\s*:\s*(\d+))");
   regex rgx3(R"(CAPACITY\s*:\s*(\d+))");
    smatch match;

    while (std::getline(inputFile, line)) {
        if (std::regex_search(line, match, rgxName) ) 
        {
            FileName = match[1].str();
            hasName = true;
        }
        else if (std::regex_search(line, match, rgx1)) 
        {
            K = std::stoi(match[1]);
            Optimalvalue = std::stoi(match[2]);
            hasTruckInfo = true;
        }
        else if (std::regex_search(line, match, rgx2)) 
        {
            V = std::stoi(match[1]);
            hasDimension = true;
        }
        else if (std::regex_search(line, match, rgx3)) 
{
            Q = std::stoi(match[1]);
            hasCapacity = true;
        }
        else if (line.find("NODE_COORD_SECTION") != std::string::npos) {
            // 检查关键参数是否已读取
            if (!hasDimension || !hasCapacity || !hasTruckInfo) {
                throw std::runtime_error("文件缺少必要字段（DIMENSION/CAPACITY/No of trucks）");
            }

            // 释放旧内存并分配新数组
            delete[] p;
            p = new point[V];

            // 读取节点坐标
            int order;
            do {
                if (!std::getline(inputFile, line)) {
                    throw std::runtime_error("文件意外结束于 NODE_COORD_SECTION");
                }
                if (line.find("DEMAND_SECTION") != std::string::npos) break;

                std::istringstream ss(line);
                if (!(ss >> order)) continue;
                int v=order-1;
                if (v  < 0 || v >= V) {
                    cout << v << endl;
                    throw std::runtime_error("节点编号 " + std::to_string(v) + " 越界");
                }
                ss >> p[v].x >> p[v].y;
            } while (true);

            // 读取需求
            do {
                if (!std::getline(inputFile, line)) {
                    throw std::runtime_error("文件意外结束于 DEMAND_SECTION");
                }
                if (line.find("DEPOT_SECTION") != std::string::npos) break;

                std::istringstream ss(line);
                if (!(ss >> order)) continue;
                int v = order - 1;
                if (v < 0 || v>= V) {
                    throw std::runtime_error("节点编号 " + std::to_string(v) + " 越界");
                }
                ss >> p[v].q;
            } while (true);
        }
    }

    // 最终检查必要字段
    if (V == 0 || K == 0 || Q == 0) {
        throw std::runtime_error("文件缺少必要字段（DIMENSION/CAPACITY/No of trucks）");
    }
}