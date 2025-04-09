#pragma once
#include<random>
#include<cmath>
#include <numeric>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include "graph.h"
// 随机交换两个路径中的片段
void randomswap(vector<int>& route1, vector<int>& route2) {
    // 如果任一路径为空，直接返回
    if (route1.empty() || route2.empty()) return;

    static random_device rd;
    static mt19937 gen(rd());

    // 随机选择route1中的片段 [start1, end1]
    uniform_int_distribution<> dis1(0, route1.size() - 1);
    int start1 = dis1(gen);
    int end1 = dis1(gen);
    if (start1 > end1) swap(start1, end1);
    int len1 = end1 - start1 + 1;

    // 随机选择route2中的片段 [start2, end2]
    uniform_int_distribution<> dis2(0, route2.size() - 1);
    int start2 = dis2(gen);
    int end2 = dis2(gen);
    if (start2 > end2) swap(start2, end2);
    int len2 = end2 - start2 + 1;


    // 执行交换
    vector<int> temp1(route1.begin() + start1, route1.begin() + end1 + 1);
    vector<int> temp2(route2.begin() + start2, route2.begin() + end2 + 1);

    // 替换route1中的片段
    route1.erase(route1.begin() + start1, route1.begin() + end1 + 1);
    route1.insert(route1.begin() + start1, temp2.begin(), temp2.end());

    // 替换route2中的片段
    route2.erase(route2.begin() + start2, route2.begin() + end2 + 1);
    route2.insert(route2.begin() + start2, temp1.begin(), temp1.end());
}

//某种分配方法
struct individual
{
	int V;
	int Q;//车最大容量
	int K;
	vector<int> ifuse;//点是否使用
	vector<vector<int>> dmatrix;// 距离可达矩阵
	vector<vector<int>> routes;//各个车的路线
	double sumod;//该方案距离
    double e;//适应度
	// 构造函数
	individual( int num_customers,int car_number, int vehicle_capacity)
		: sumod(0.0),K(car_number),Q(vehicle_capacity),V(num_customers+1)
	{
		// 初始化ifuse，所有客户点初始为未使用
		ifuse.resize(num_customers + 1, false); // 假设0是仓库，1-n是客户
		ifuse[0] = true; // 仓库始终使用
	}

	// 检查解是否有效（所有客户都被服务且只服务一次）
	bool is_valid(graph G) const
	{
		// 检查所有客户是否都被服务
		for (int i = 1; i <= G.points.size(); ++i)
		{
			if (!ifuse[i]) return false;
			if (ifuse[i] > 1) return false;
		}
		
		for (const auto& route : routes)
		{
			int demand=0;
			for (int customer : route)
			{
				demand+=G.points[customer]->q;
				if (demand > Q) return false;
				
			}
		}

		return true;
	}

	// 随机初始化个体
	void initialize( graph G)
	{ 
		
		
		routes.resize(K);
		vector<int> customers(V-1);
		iota(customers.begin(), customers.end(), 1); // 生成1到num_customers的序列

		// 随机打乱客户顺序
		random_shuffle(customers.begin(), customers.end());

		int current_vehicle = 0;
		int current_load = 0;

		for (int customer : customers)
		{
			// 如果当前车辆无法容纳该客户，换下一辆车
			if (current_load + G.points[customer]->q > Q)
			{
				current_vehicle++;
				current_load = 0;

				// 如果车辆用尽，抛出异常或处理
				if (current_vehicle >= K)
				{
					// 这里简单处理为重新开始
					current_vehicle = 0;
					current_load = 0;
				}
			}

			routes[current_vehicle].push_back(customer);
			ifuse[customer] = true;
			current_load += G.points[customer]->q;
		}
	}

	void evaluate(graph G)
	{   
        double p = 1.1;//惩罚
		sumod = 0.0;
        e = 0;
		// 清空现有距离矩阵
		dmatrix.clear();
		dmatrix.resize(ifuse.size(), vector<int>(ifuse.size(), 0));

		for (const auto& route : routes)
        {
            double dist = 0;
            int demand = 0;
			if (route.empty()) continue;

			// 从仓库出发到第一个客户
			dist += G.get_distance(0, route[0]);
          
			dmatrix[0][route[0]] = G.get_distance(0, route[0]);
            demand += G.points[route[0]]->q;
			// 客户之间的移动
			for (size_t i = 1; i < route.size(); ++i)
			{
				dist += G.get_distance(route[i - 1], route[i]);
                demand += G.points[route[i]]->q;
				dmatrix[route[i - 1]][route[i]] = G.get_distance(route[i - 1], route[i]);
			}

			// 从最后一个客户返回仓库
			dist += G.get_distance(route.back(), 0);
			dmatrix[route.back()][0] = G.get_distance(route.back(), 0);
         
            sumod += dist;// 路径长度加到总值
            e += dist * ((demand > Q) ? p : 1);//路径长度加到总评估值
       
		}
	}

    // 直接在两个父本上操作的交叉函数
    void crossover(individual& parent1, individual& parent2,graph G) {
        random_device rd;
        mt19937 gen(rd());

        /* 第一步：随机交换路径 */
        if (!parent1.routes.empty() && !parent2.routes.empty()) {
            // 随机选择两条路径（分别来自两个父代）
            uniform_int_distribution<> dis1(0, parent1.routes.size() - 1);
            uniform_int_distribution<> dis2(0, parent2.routes.size() - 1);
            int route_idx1 = dis1(gen);
            int route_idx2 = dis2(gen);

            // 随机交换两条路径部分
            randomswap(parent1.routes[route_idx1], parent2.routes[route_idx2]);
        }

        /* 第二步：对两个父本分别去重和补缺 */
        for (auto* parent : { &parent1, &parent2 }) {
            unordered_set<int> existing_customers;
            vector<vector<int>> new_routes;

            // 去重处理
            for (auto& route : parent->routes) {
                vector<int> cleaned_route;
                for (int cust : route) {
                    if (existing_customers.insert(cust).second) {
                        cleaned_route.push_back(cust);
                    }
                }
                if (!cleaned_route.empty()) {
                    new_routes.push_back(cleaned_route);
                }
            }
            parent->routes = new_routes;

            // 补缺处理
            for (int cust = 1; cust < parent->ifuse.size(); ++cust) {
                if (!existing_customers.count(cust)) {
                    // 寻找最佳插入位置
                    double min_increase = numeric_limits<double>::max();
                    size_t best_route = 0;
                    size_t best_pos = 0;

                    for (size_t r = 0; r < parent->routes.size(); ++r) {
                        const auto& route = parent->routes[r];

                        // 检查所有可能插入位置
                        for (size_t i = 1; i < route.size(); ++i) {
                            double increase =
                                G.get_distance(route[i - 1], cust) +
                                G.get_distance(cust, route[i]) -
                                G.get_distance(route[i - 1], route[i]);

                            if (increase < min_increase) {
                                min_increase = increase;
                                best_route = r;
                                best_pos = i;
                            }
                        }

                        // 检查路径首尾位置
                        double start_increase =
                            G.get_distance(0, cust) +
                            G.get_distance(cust, route[0]) -
                            G.get_distance(0, route[0]);

                        double end_increase =
                            G.get_distance(route.back(), cust) +
                            G.get_distance(cust, 0) -
                            G.get_distance(route.back(), 0);

                        if (start_increase < min_increase) {
                            min_increase = start_increase;
                            best_route = r;
                            best_pos = 0;
                        }
                        if (end_increase < min_increase) {
                            min_increase = end_increase;
                            best_route = r;
                            best_pos = route.size();
                        }
                    }

                    // 执行插入
                    if (parent->routes.empty()) {
                        parent->routes.push_back({ cust });
                    }
                    else {
                        parent->routes[best_route].insert(
                            parent->routes[best_route].begin() + best_pos, cust);
                    }
                    existing_customers.insert(cust);
                }
            }

            /* 第三步：更新父本状态 */
            parent->ifuse.assign(parent->ifuse.size(), false);
            parent->ifuse[0] = true;  // 仓库

            for (auto& route : parent->routes) {
                for (int cust : route) {
                    parent->ifuse[cust] = true;
                }
            }

            // 重新计算适应度
            parent->evaluate(G);
        }
    }
    // 变异操作（三种变异方式随机选择）
    void mutate(double mutation_rate,graph G)
    {
        if ((rand() / (double)RAND_MAX) > mutation_rate) return;

        if (routes.empty()) return;

        // 随机选择一种变异方式
        int mutation_type = rand() % 3;

        switch (mutation_type)
        {
        case 0: // 交换路径中的两个客户
        {
            int route_idx = rand() % routes.size();
            if (routes[route_idx].size() < 2) break;

            int pos1 = rand() % routes[route_idx].size();
            int pos2 = rand() % routes[route_idx].size();
            swap(routes[route_idx][pos1], routes[route_idx][pos2]);
            break;
        }
        case 1: // 将一个客户移到另一条路径
        {
            if (routes.size() < 2) break;

            int from_route = rand() % routes.size();
            if (routes[from_route].empty()) break;

            int to_route = rand() % routes.size();
            while (to_route == from_route) to_route = rand() % routes.size();

            int customer_pos = rand() % routes[from_route].size();
            int customer = routes[from_route][customer_pos];

            routes[from_route].erase(routes[from_route].begin() + customer_pos);
            routes[to_route].push_back(customer);
            break;
        }
        case 2: // 反转一段路径
        {
            int route_idx = rand() % routes.size();
            if (routes[route_idx].size() < 2) break;

            int start = rand() % routes[route_idx].size();
            int end = rand() % routes[route_idx].size();
            if (start > end) swap(start, end);

            reverse(routes[route_idx].begin() + start, routes[route_idx].begin() + end + 1);
            break;
        }
        }

        // 重新评估
        evaluate(G);
    }
    // 2-opt局部优化
    void two_opt_optimize()
    {
        for (auto& route : routes)
        {
            if (route.size() < 4) continue;

            bool improved = true;
            while (improved)
            {
                improved = false;
                for (size_t i = 1; i < route.size() - 2; ++i)
                {
                    for (size_t j = i + 1; j < route.size(); ++j)
                    {
                        // 计算当前边和可能的新边的距离差
                        double delta = -dmatrix[route[i - 1]][route[i]] - dmatrix[route[j]][route[j + 1]]
                            + dmatrix[route[i - 1]][route[j]] + dmatrix[route[i]][route[j + 1]];

                        if (delta < -1e-6) // 有改进
                        {
                            reverse(route.begin() + i, route.begin() + j + 1);
                            improved = true;
                            sumod += delta;
                        }
                    }
                }
            }
        }
    }

    // 小于运算符，用于排序
    bool operator<(const individual& other) const
    {
        return sumod < other.sumod;
    }
};