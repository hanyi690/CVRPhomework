// GA.h
#ifndef GA_H
#define GA_H

#include <vector>
#include <string>
#include <random>
#include "Graph.h"

// 前置声明
class  graph;

// 随机数生成函数
double generateRandomNumber();

// 路径交换函数
void randomswap(std::vector<int>& route1, std::vector<int>& route2);

// 个体结构体声明
struct individual {
    static int count; // 静态成员声明

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

// 比较函数声明
bool compareByR(const individual& I1, const individual& I2);
bool compareByE(const individual& I1, const individual& I2);

// 交叉函数声明
std::vector<individual> crossover(std::vector<individual>& parents, graph& G);

// 遗传算法主函数声明
int  GA(int VNum, point* p, int CarNum, int MaxLoad, graph& G,const individual* best_valid_individual);

#endif // GA_H