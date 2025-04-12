// GA.h
#ifndef GA_H
#define GA_H

#include <vector>
#include <string>
#include <random>
#include "Graph.h"

// ǰ������
class  graph;

// ��������ɺ���
double generateRandomNumber();

// ·����������
void randomswap(std::vector<int>& route1, std::vector<int>& route2);

// ����ṹ������
struct individual {
    static int count; // ��̬��Ա����

    int V, Q, K;
    std::vector<int> ifuse;
    std::vector<std::vector<int>> dmatrix;
    std::vector<std::vector<int>> routes;
    std::vector<int>current_load;
    double sumod, e;

    individual(int num_customers, int car_number, int vehicle_capacity);
    void printIndividual( const graph& G)const;
    bool is_valid(const graph& G)const ;
    void initialize(graph& G);
    void evaluate(graph& G);
    individual mutate(graph& G);
    void repair_overload_routes(graph& G);

};

// �ȽϺ�������
bool compareByR(const individual& I1, const individual& I2);
bool compareByE(const individual& I1, const individual& I2);

// ���溯������
std::vector<individual> crossover(std::vector<individual>& parents, graph& G);

// �Ŵ��㷨����������
int  GA(int VNum, point* p, int CarNum, int MaxLoad, graph& G,const individual* best_valid_individual);

#endif // GA_H