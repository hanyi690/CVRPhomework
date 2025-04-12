// GA.cpp
#include "GA.h"
#include "Graph.h"
#include <iostream>
#include <algorithm>
#include <numeric>
#include <unordered_set>
#include <iomanip>
#include <cmath>
// ������������棬ʹ�õ�ǰʱ����Ϊ����

//-----------------------------------------------------------
// ���ߺ���ʵ��
//-----------------------------------------------------------
using namespace std;
random_device rd;
mt19937 gen(rd()); // ����һ��÷ɭ��ת�㷨�����������
double generateRandomNumber() {
  
    uniform_real_distribution<> dis(0.0, 1.0); // �������ɵ��������ΧΪ 0 �� 1

    // �������ɵ������
    return dis(gen);
}

// ��̬��Ա����
int individual::count = 0;



void randomswap(vector<int>& route1, vector<int>& route2) {
    // �����һ·��Ϊ�գ�ֱ�ӷ���
    if (route1.empty() || route2.empty()) return;
;
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

//-----------------------------------------------------------
// individual ��Ա����ʵ��
//-----------------------------------------------------------
individual::individual(int num_customers, int car_number, int vehicle_capacity): sumod(0.0), K(car_number), Q(vehicle_capacity), V(num_customers + 1) {
    e = INFINITY;
    current_load.resize(K, 0);
    // ��ʼ��ifuse�����пͻ����ʼΪδʹ��
    ifuse.resize(num_customers + 1, false); // ����0�ǲֿ⣬1-n�ǿͻ�
    ifuse[0] = true; // �ֿ�ʼ��ʹ��
}

void individual::printIndividual(const graph& G)const {
    // ��ӡ��������
    std::cout << "V: " << V << "\n";
    std::cout << "Q: " << Q << "\n";
    std::cout << "K: " << K << "\n";

    // ��ӡ ifuse ����
    std::cout << "ifuse: [";
    for (size_t i = 0; i <ifuse.size(); ++i) {
        std::cout << ifuse[i];
        if (i != ifuse.size() - 1) std::cout << ", ";
    }
    std::cout << "]\n";

    std::cout << "routes:\n";
   
    for (size_t i = 0; i < routes.size(); ++i) {

        std::cout << "  Route " << i << ": [";
        for (size_t j = 0; j < routes[i].size(); ++j) {
            std::cout << routes[i][j];
            if (j != routes[i].size() - 1) std::cout << ", ";
        }
       std::cout << "]"<< " demand:" << current_load[i] <<"\n";
    }
}
    bool individual::is_valid(const graph & G) const {
        // ������пͻ��Ƿ񶼱�����
        for (int i = 1; i < G.points.size(); i++)
        {
            if (!ifuse[i]) return false;
            if (ifuse[i] > 1) return false;
        }
        if (routes.size() > K) return false;

       // cout << e << " " << sumod << endl;
        if (e != sumod) {
            return false;
            std::cout << "is not valid" << endl;
        }

        return true;
    }

    void individual::initialize(graph& G) {
        routes.resize(K);
        vector<int> customers(V - 1);
        iota(customers.begin(), customers.end(), 1);

        // ������ҿͻ�˳��
        shuffle(customers.begin(), customers.end(),gen);

        int current_vehicle = 0;
        int min_loadv = 0;              // ��ʼʱ���г����ػ���Ϊ0��Ĭ��ѡ��0��

        for (int customer : customers) {
            int count = 0;
            // Ѱ�ҿ����ɸÿͻ��ĳ�������ೢ��K�Σ�
            while (count < K && current_load[current_vehicle] + G.points[customer]->q > Q) {
                current_vehicle = (current_vehicle + 1) % K;  // ���α�������
                count++;
            }

            // ������г��������أ�ǿ��ѡ���ػ�����С�ĳ���
            if (count == K) {
                current_vehicle = min_loadv;
            }

            // ����ͻ�������
            routes[current_vehicle].push_back(customer);
            ifuse[customer] = true;
            current_load[current_vehicle] += G.points[customer]->q;

            // ������С�ػ�������ţ��������г�����
            min_loadv = 0;
            for (int i = 1; i < K; ++i) {
                if (current_load[i] < current_load[min_loadv]) {
                    min_loadv = i;
                }
            }
        }

        evaluate(G);
    }

void individual::evaluate(graph& G) {
    count++;
    if (G.avg_q == 0.0) {
        std::cerr << "Error: G.avg_q is zero!" << std::endl;
        return;
    }

    sumod = 0.0;
    e = 0;
    // ������о������
    dmatrix.clear();
    dmatrix.resize(ifuse.size(), vector<int>(ifuse.size(), 0));
    int count = 0;
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
        dmatrix[route.back()][0] = G.dist_matrix[route.back()][0];
        current_load[count] = demand;
        sumod += dist;// ·�����ȼӵ���ֵ
        e += dist + ((demand > Q) ? G.p * (demand - Q) : 0);//·�����ȼӵ�������ֵ
        count++;
    }
}

individual individual::mutate(graph& G) {
    double r_num = generateRandomNumber();
    double pm = (e < G.avg_e) ? (G.pm1 - (G.pm1 - G.pm2) * (G.avg_e - e) / (G.avg_e - G.min_e)) : G.pm1;
    if (r_num > pm) return *this;

    if (routes.empty())  return *this;

    // ���ѡ��һ�ֱ��췽ʽ
    int mutation_type = rand() % 2;

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

    }

    // ��������
    evaluate(G);
    return *this;
}


    // ����ʽ�Ż���������·���Ŀͻ�ת�Ƶ���������
void individual::repair_overload_routes(graph& G)
{   
    
    // ������С�ػ�������ţ��������г�����
    int min_loadv = 0,max_loadv=0;
    for (int i = 1; i < K; ++i) {
        if (current_load[i] < current_load[min_loadv]) {
            min_loadv = i;
        }
        if (current_load[i] > current_load[max_loadv]) {
            max_loadv = i;
        }
    }
    if (current_load[max_loadv]< Q || current_load[min_loadv] > Q)   return;

    vector<int> nodes;
    current_load[max_loadv] -= G.points[routes[max_loadv].back()]->q;
    nodes.push_back(routes[max_loadv].back());
    routes[max_loadv].pop_back();
    // ������С�ػ�������ţ��������г�����
     min_loadv = 0, max_loadv = 0;
    for (int i = 1; i < K; ++i) {
        if (current_load[i] < current_load[min_loadv]) {
            min_loadv = i;
        }
        if (current_load[i] > current_load[max_loadv]) {
            max_loadv = i;
        }
    }

   while ( !nodes.empty()) {
        

        if ((current_load[max_loadv]< Q || current_load[min_loadv] > Q)&& nodes.empty())break;
        routes[min_loadv].push_back(nodes.back());
        current_load[min_loadv] += G.points[routes[min_loadv].back()]->q;
        nodes.pop_back();
        if (nodes.empty())break;
         min_loadv = 0, max_loadv = 0;
        for (int i = 1; i < K; ++i) {
            if (current_load[i] < current_load[min_loadv]) {
                min_loadv = i;
            }
            if (current_load[i] > current_load[max_loadv]) {
                max_loadv = i;
            }
        }
        while (current_load[max_loadv] > Q && !routes[max_loadv].empty()) {
            current_load[max_loadv] -= G.points[routes[max_loadv].back()]->q;
            nodes.push_back(routes[max_loadv].back());
            routes[max_loadv].pop_back();
        }

         min_loadv = 0, max_loadv = 0;
        for (int i = 1; i < K; ++i) {
            if (current_load[i] < current_load[min_loadv]) {
                min_loadv = i;
            }
            if (current_load[i] > current_load[max_loadv]) {
                max_loadv = i;
            }
        }       
            
    }
    
    this->evaluate(G);
};




//-----------------------------------------------------------
// ��������ʵ��
//-----------------------------------------------------------
bool compareByR(const individual& I1, const individual& I2) {
    return I1.sumod < I2.sumod;
}

bool compareByE(const individual& I1, const individual& I2) {
    return I1.e < I2.e;
}

std::vector<individual> crossover(std::vector<individual>& parents, graph& G) {
    vector<individual> crossresult;
    int i = 0;

    for (auto it = parents.begin(); it != parents.end(); ) {

        double r_num = generateRandomNumber();
        double pc = (it->e < G.avg_e) ? (G.pc1 - (G.pc1 - G.pc2) * (G.avg_e - it->e) / (G.avg_e - G.min_e)) : G.pc1;
        if (r_num > pc) {

            crossresult.push_back(*it);
            it = parents.erase(it);         // ��ȷ�Ƴ������µ�����
        }
        else {
            ++it;
        }
    }

    while (!parents.empty())
    {
        if (parents.size() >= 2)
        { // �Ȼ�ȡ��ǰһ��Ԫ��
            individual parent1 = parents.front();
            parents.erase(parents.begin());  // Ȼ���Ƴ�

            individual parent2 = parents.front();
            parents.erase(parents.begin());

            /* ��һ�����������·�� */
            if (!parent1.routes.empty() && !parent2.routes.empty()) {
                // ���ѡ������·�����ֱ���������������
                random_device rd;
                mt19937 gen(rd());
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

                crossresult.push_back(*parent);
                crossresult.back().evaluate(G);

            }
        }
        else if (parents.size() == 1) {
            crossresult.push_back(parents.back());
            parents.pop_back();
        }
    }
    return crossresult;

}

int GA(int VNum, point* p, int CarNum, int MaxLoad, graph& G, const individual* best_valid_individual) {
    vector<individual>generation_1;
    vector<individual>generation_2;
    int bestresult = 2147483647;
    for (int i = 0; i < 50; i++)
    {
        individual I(VNum - 1, CarNum, MaxLoad);
        I.initialize(G);
        I.repair_overload_routes(G);
        //if (I.is_valid(G))
        generation_1.push_back(I);
        //else i--;

    }
    G.p=50 * G.avg_dist / G.avg_q;
    double min_output = G.avg_dist / G.avg_q;
    PIDController pid_p(7*min_output, 100 * min_output , 0, -0.1*min_output, 0.1*min_output);
    G.self_adaptation(generation_1,pid_p);
    sort(generation_1.begin(), generation_1.end(), compareByE);
    bestresult = generation_1[0].sumod;
    cout << "��" << 1 << "������ѽ��" << bestresult << "�Ƿ�Ϸ�" << generation_1[0].is_valid(G) <<"Υ������"<<G.invalid_rate<<
        endl;
    for (int n = 0; n < 1000; n++)
    {
        //������������
        for (int i = 0; i < 10; i++)
        {
            generation_1[i].mutate(G);
           
            if (!generation_1[i].is_valid(G)  )
            {   
                double r = generateRandomNumber();
                if(r <= G.invalid_rate)
                generation_1[i].repair_overload_routes(G);
            }
            generation_2.push_back(generation_1[i]);
        }

        //���ӽ��深ֳ�ͱ��������

        vector <individual>crosresult = move(crossover(generation_1, G));
        for (individual I : crosresult)
        {

            generation_2.push_back(I.mutate(G));
            if (generation_2.size() >= 50)break;
        }

    
        generation_1 = move(generation_2);
        G.self_adaptation(generation_1,pid_p);

        sort(generation_1.begin(), generation_1.end(), compareByE);

        bestresult = (generation_1[0].sumod < bestresult) ? generation_1[0].sumod : bestresult;

       
    }
    // ��ԭ��ѭ��������쳣����
    bool found_valid = false;
    int answer;
    cout << "��1001������ѽ��" << bestresult << "�Ƿ�Ϸ�" << generation_1[0].is_valid(G) << "Υ������" << G.invalid_rate << endl;
    for (const individual& I : generation_1)  // ʹ��const���ñ����޸�
    {
        if (I.is_valid(G))
        {   
        
            if (best_valid_individual==nullptr||best_valid_individual->sumod > I.sumod)
                best_valid_individual = &I;
            answer = I.sumod;// �����������÷�ʽ
            found_valid = true;
            break;  // �ҵ���һ���Ϸ����������˳�
        }
    }

    if (!found_valid)
    {
        // �����޸�ֱ���ҵ���Ч�����ﵽĳ������
        while(  individual::count < 50000  ) {
            generation_1[0].repair_overload_routes(G);
            if (generation_1[0].is_valid(G)) {
                answer = generation_1[0].sumod;
                if (best_valid_individual->sumod > generation_1[0].sumod)
                    best_valid_individual = &generation_1[0];
                break;
            }
        }
        
    }
    //if (!found_valid) {
    //    // �����Ȼû���ҵ���Ч���壬�׳��쳣�򷵻�һ��Ĭ�Ϲ���ĸ���
    //    throw std::runtime_error("No valid individual found");
    //    // ����
    //    // return individual(); // ����individual����Ĭ�Ϲ��캯��
    //}
    return answer;

}