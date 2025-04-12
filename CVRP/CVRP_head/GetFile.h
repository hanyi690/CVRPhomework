#pragma once
#include <fstream>
#include <filesystem>
#include <sstream>
#include <regex>
#include"Graph.h"

using namespace std;
namespace fs = std::filesystem;
//ȥ��˫����
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

//���ļ�
void ReadFile(int& V, int& K, int& Q, point*& p, int& Optimalvalue, std::string& FileName) {
    FileName = removeQuotes(FileName);
    std::ifstream inputFile(FileName);
    if (!inputFile.is_open()) {
        throw std::runtime_error("�޷����ļ�: " + FileName);
    }

    // ��ʼ������
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
            // ���ؼ������Ƿ��Ѷ�ȡ
            if (!hasDimension || !hasCapacity || !hasTruckInfo) {
                throw std::runtime_error("�ļ�ȱ�ٱ�Ҫ�ֶΣ�DIMENSION/CAPACITY/No of trucks��");
            }

            // �ͷž��ڴ沢����������
            delete[] p;
            p = new point[V];

            // ��ȡ�ڵ�����
            int order;
            do {
                if (!std::getline(inputFile, line)) {
                    throw std::runtime_error("�ļ���������� NODE_COORD_SECTION");
                }
                if (line.find("DEMAND_SECTION") != std::string::npos) break;

                std::istringstream ss(line);
                if (!(ss >> order)) continue;
                int v=order-1;
                if (v  < 0 || v >= V) {
                    cout << v << endl;
                    throw std::runtime_error("�ڵ��� " + std::to_string(v) + " Խ��");
                }
                ss >> p[v].x >> p[v].y;
            } while (true);

            // ��ȡ����
            do {
                if (!std::getline(inputFile, line)) {
                    throw std::runtime_error("�ļ���������� DEMAND_SECTION");
                }
                if (line.find("DEPOT_SECTION") != std::string::npos) break;

                std::istringstream ss(line);
                if (!(ss >> order)) continue;
                int v = order - 1;
                if (v < 0 || v>= V) {
                    throw std::runtime_error("�ڵ��� " + std::to_string(v) + " Խ��");
                }
                ss >> p[v].q;
            } while (true);
        }
    }

    // ���ռ���Ҫ�ֶ�
    if (V == 0 || K == 0 || Q == 0) {
        throw std::runtime_error("�ļ�ȱ�ٱ�Ҫ�ֶΣ�DIMENSION/CAPACITY/No of trucks��");
    }
}