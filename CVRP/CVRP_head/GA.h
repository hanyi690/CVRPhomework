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

// 前置声明
class  graph;
struct RouteInfo {
    std::vector<int> customers;
    int total_load;
    int start;
    int end;

    RouteInfo() : total_load(0), start(-1), end(-1) {}
};
// 随机数生成函数
double generateRandomNumber();

// 路径交换函数
void randomswap(std::vector<int>& route1, std::vector<int>& route2);

// 个体结构体声明
struct individual {
    static int count; // 静态成员声明

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
    // 拷贝构造函数
    individual(const individual& other)
        : V(other.V),
        Q(other.Q),
        K(other.K),
        ifuse(other.ifuse),          // vector 深拷贝
        routes(other.routes),        // 嵌套vector 深拷贝
        current_load(other.current_load), // vector 深拷贝
        sumod(other.sumod),
        e(other.e)
    {
        // 静态成员不需要拷贝（所有对象共享）
    }

    // 拷贝赋值运算符
    individual& operator=(const individual& other) {
        if (this != &other) {  // 防止自赋值
            V = other.V;
            Q = other.Q;
            K = other.K;
            ifuse = other.ifuse;       // vector 深拷贝
            routes = other.routes;     // 嵌套vector 深拷贝
            current_load = other.current_load; // vector 深拷贝
            sumod = other.sumod;
            e = other.e;
        }
        return *this;  // 支持链式赋值
    }
};
// 比较函数声明
bool compareByR(const individual& I1, const individual& I2);
bool compareByE(const individual& I1, const individual& I2);

// 交叉函数声明
std::vector<individual> crossover(std::vector<individual>& parents,int need, graph& G);

// 遗传算法主函数声明
int  GA(int VNum, point* p, int CarNum, int MaxLoad, graph& G, individual& out_best);

#endif // GA_H