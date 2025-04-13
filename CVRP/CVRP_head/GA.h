// GA.h
#ifndef GA_H
#define GA_H

#include "Graph.h"
#include "Graph.h"
#include <iostream>
#include <algorithm>
#include <numeric>
#include <unordered_set>
#include<random>
#include <iomanip>
#include <cmath>
#include <list>
#include <unordered_map>

// ǰ������
class  graph;
struct RouteInfo {
    std::vector<int> customers;
    int total_load;
    int start;
    int end;

    RouteInfo() : total_load(0), start(-1), end(-1) {}
};
// ��������ɺ���
double generateRandomNumber();

// ·����������
void randomswap(std::vector<int>& route1, std::vector<int>& route2);

// ����ṹ������
struct individual {
    static int count; // ��̬��Ա����

    int V, Q, K;
    std::vector<int> ifuse;
    std::vector<std::vector<int>> routes;
    std::vector<int>current_load;
    double sumod, e;

    individual(int num_customers, int car_number, int vehicle_capacity);
    void printIndividual(const graph& G)const;
    bool is_valid(const graph& G)const;
    void initialize(graph& G);
    void evaluate(graph& G);
    individual mutate(graph& G);
    void repair_overload_routes(graph& G);
    // �������캯��
    individual(const individual& other)
        : V(other.V),
        Q(other.Q),
        K(other.K),
        ifuse(other.ifuse),          // vector ���
        routes(other.routes),        // Ƕ��vector ���
        current_load(other.current_load), // vector ���
        sumod(other.sumod),
        e(other.e)
    {
        // ��̬��Ա����Ҫ���������ж�����
    }

    // ������ֵ�����
    individual& operator=(const individual& other) {
        if (this != &other) {  // ��ֹ�Ը�ֵ
            V = other.V;
            Q = other.Q;
            K = other.K;
            ifuse = other.ifuse;       // vector ���
            routes = other.routes;     // Ƕ��vector ���
            current_load = other.current_load; // vector ���
            sumod = other.sumod;
            e = other.e;
        }
        return *this;  // ֧����ʽ��ֵ
    }
};
// �ȽϺ�������
bool compareByR(const individual& I1, const individual& I2);
bool compareByE(const individual& I1, const individual& I2);

// ���溯������
std::vector<individual> crossover(std::vector<individual>& parents,int need, graph& G);

// �Ŵ��㷨����������
int  GA(int VNum, point* p, int CarNum, int MaxLoad, graph& G, individual& out_best);

#endif // GA_H