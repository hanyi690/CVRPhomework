#pragma once
#include<random>
#include<cmath>
#include <numeric>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include "graph.h"
// �����������·���е�Ƭ��
void randomswap(vector<int>& route1, vector<int>& route2) {
    // �����һ·��Ϊ�գ�ֱ�ӷ���
    if (route1.empty() || route2.empty()) return;

    static random_device rd;
    static mt19937 gen(rd());

    // ���ѡ��route1�е�Ƭ�� [start1, end1]
    uniform_int_distribution<> dis1(0, route1.size() - 1);
    int start1 = dis1(gen);
    int end1 = dis1(gen);
    if (start1 > end1) swap(start1, end1);
    int len1 = end1 - start1 + 1;

    // ���ѡ��route2�е�Ƭ�� [start2, end2]
    uniform_int_distribution<> dis2(0, route2.size() - 1);
    int start2 = dis2(gen);
    int end2 = dis2(gen);
    if (start2 > end2) swap(start2, end2);
    int len2 = end2 - start2 + 1;


    // ִ�н���
    vector<int> temp1(route1.begin() + start1, route1.begin() + end1 + 1);
    vector<int> temp2(route2.begin() + start2, route2.begin() + end2 + 1);

    // �滻route1�е�Ƭ��
    route1.erase(route1.begin() + start1, route1.begin() + end1 + 1);
    route1.insert(route1.begin() + start1, temp2.begin(), temp2.end());

    // �滻route2�е�Ƭ��
    route2.erase(route2.begin() + start2, route2.begin() + end2 + 1);
    route2.insert(route2.begin() + start2, temp1.begin(), temp1.end());
}

//ĳ�ַ��䷽��
struct individual
{
	int V;
	int Q;//���������
	int K;
	vector<int> ifuse;//���Ƿ�ʹ��
	vector<vector<int>> dmatrix;// ����ɴ����
	vector<vector<int>> routes;//��������·��
	double sumod;//�÷�������
    double e;//��Ӧ��
	// ���캯��
	individual( int num_customers,int car_number, int vehicle_capacity)
		: sumod(0.0),K(car_number),Q(vehicle_capacity),V(num_customers+1)
	{
		// ��ʼ��ifuse�����пͻ����ʼΪδʹ��
		ifuse.resize(num_customers + 1, false); // ����0�ǲֿ⣬1-n�ǿͻ�
		ifuse[0] = true; // �ֿ�ʼ��ʹ��
	}

	// �����Ƿ���Ч�����пͻ�����������ֻ����һ�Σ�
	bool is_valid(graph G) const
	{
		// ������пͻ��Ƿ񶼱�����
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

	// �����ʼ������
	void initialize( graph G)
	{ 
		
		
		routes.resize(K);
		vector<int> customers(V-1);
		iota(customers.begin(), customers.end(), 1); // ����1��num_customers������

		// ������ҿͻ�˳��
		random_shuffle(customers.begin(), customers.end());

		int current_vehicle = 0;
		int current_load = 0;

		for (int customer : customers)
		{
			// �����ǰ�����޷����ɸÿͻ�������һ����
			if (current_load + G.points[customer]->q > Q)
			{
				current_vehicle++;
				current_load = 0;

				// ��������þ����׳��쳣����
				if (current_vehicle >= K)
				{
					// ����򵥴���Ϊ���¿�ʼ
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
        double p = 1.1;//�ͷ�
		sumod = 0.0;
        e = 0;
		// ������о������
		dmatrix.clear();
		dmatrix.resize(ifuse.size(), vector<int>(ifuse.size(), 0));

		for (const auto& route : routes)
        {
            double dist = 0;
            int demand = 0;
			if (route.empty()) continue;

			// �Ӳֿ��������һ���ͻ�
			dist += G.get_distance(0, route[0]);
          
			dmatrix[0][route[0]] = G.get_distance(0, route[0]);
            demand += G.points[route[0]]->q;
			// �ͻ�֮����ƶ�
			for (size_t i = 1; i < route.size(); ++i)
			{
				dist += G.get_distance(route[i - 1], route[i]);
                demand += G.points[route[i]]->q;
				dmatrix[route[i - 1]][route[i]] = G.get_distance(route[i - 1], route[i]);
			}

			// �����һ���ͻ����زֿ�
			dist += G.get_distance(route.back(), 0);
			dmatrix[route.back()][0] = G.get_distance(route.back(), 0);
         
            sumod += dist;// ·�����ȼӵ���ֵ
            e += dist * ((demand > Q) ? p : 1);//·�����ȼӵ�������ֵ
       
		}
	}

    // ֱ�������������ϲ����Ľ��溯��
    void crossover(individual& parent1, individual& parent2,graph G) {
        random_device rd;
        mt19937 gen(rd());

        /* ��һ�����������·�� */
        if (!parent1.routes.empty() && !parent2.routes.empty()) {
            // ���ѡ������·�����ֱ���������������
            uniform_int_distribution<> dis1(0, parent1.routes.size() - 1);
            uniform_int_distribution<> dis2(0, parent2.routes.size() - 1);
            int route_idx1 = dis1(gen);
            int route_idx2 = dis2(gen);

            // �����������·������
            randomswap(parent1.routes[route_idx1], parent2.routes[route_idx2]);
        }

        /* �ڶ����������������ֱ�ȥ�غͲ�ȱ */
        for (auto* parent : { &parent1, &parent2 }) {
            unordered_set<int> existing_customers;
            vector<vector<int>> new_routes;

            // ȥ�ش���
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

            // ��ȱ����
            for (int cust = 1; cust < parent->ifuse.size(); ++cust) {
                if (!existing_customers.count(cust)) {
                    // Ѱ����Ѳ���λ��
                    double min_increase = numeric_limits<double>::max();
                    size_t best_route = 0;
                    size_t best_pos = 0;

                    for (size_t r = 0; r < parent->routes.size(); ++r) {
                        const auto& route = parent->routes[r];

                        // ������п��ܲ���λ��
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

                        // ���·����βλ��
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

                    // ִ�в���
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

            /* �����������¸���״̬ */
            parent->ifuse.assign(parent->ifuse.size(), false);
            parent->ifuse[0] = true;  // �ֿ�

            for (auto& route : parent->routes) {
                for (int cust : route) {
                    parent->ifuse[cust] = true;
                }
            }

            // ���¼�����Ӧ��
            parent->evaluate(G);
        }
    }
    // ������������ֱ��췽ʽ���ѡ��
    void mutate(double mutation_rate,graph G)
    {
        if ((rand() / (double)RAND_MAX) > mutation_rate) return;

        if (routes.empty()) return;

        // ���ѡ��һ�ֱ��췽ʽ
        int mutation_type = rand() % 3;

        switch (mutation_type)
        {
        case 0: // ����·���е������ͻ�
        {
            int route_idx = rand() % routes.size();
            if (routes[route_idx].size() < 2) break;

            int pos1 = rand() % routes[route_idx].size();
            int pos2 = rand() % routes[route_idx].size();
            swap(routes[route_idx][pos1], routes[route_idx][pos2]);
            break;
        }
        case 1: // ��һ���ͻ��Ƶ���һ��·��
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
        case 2: // ��תһ��·��
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

        // ��������
        evaluate(G);
    }
    // 2-opt�ֲ��Ż�
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
                        // ���㵱ǰ�ߺͿ��ܵ��±ߵľ����
                        double delta = -dmatrix[route[i - 1]][route[i]] - dmatrix[route[j]][route[j + 1]]
                            + dmatrix[route[i - 1]][route[j]] + dmatrix[route[i]][route[j + 1]];

                        if (delta < -1e-6) // �иĽ�
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

    // С�����������������
    bool operator<(const individual& other) const
    {
        return sumod < other.sumod;
    }
};