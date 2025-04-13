// GA.cpp
#include "GA.h"

// 创建随机数引擎，使用当前时间作为种子

//-----------------------------------------------------------
// 工具函数实现
//-----------------------------------------------------------
using namespace std;
random_device rd;
mt19937 gen(rd()); // 生成一个梅森旋转算法的随机数引擎
double generateRandomNumber() {
  
    uniform_real_distribution<> dis(0.0, 1.0); // 设置生成的随机数范围为 0 到 1

    // 返回生成的随机数
    return dis(gen);
}

// 静态成员定义
int individual::count = 0;



void randomswap(vector<int>& route1, vector<int>& route2) {
    // 如果任一路径为空，直接返回
    if (route1.empty() || route2.empty()) return;
;
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

//-----------------------------------------------------------
// individual 成员函数实现
//-----------------------------------------------------------
individual::individual(int num_customers, int car_number, int vehicle_capacity): sumod(0.0), K(car_number), Q(vehicle_capacity), V(num_customers + 1) {
    e = INFINITY;
    sumod = INFINITY;
    current_load.resize(K, 0);
    // 初始化ifuse，所有客户点初始为未使用
    ifuse.resize(num_customers + 1, false); // 假设0是仓库，1-n是客户
    ifuse[0] = true; // 仓库始终使用
}



void individual::printIndividual(const graph& G)const {

    // 打印基本属性
    std::cout << "V: " << V << "\n";
    std::cout << "Q: " << Q << "\n";
    std::cout << "K: " << K << "\n";

    // 打印 ifuse 数组
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
        // 检查所有客户是否都被服务
        for (int i = 1; i < G.points.size(); i++)
        {
            if (!ifuse[i]) return false;
            if (ifuse[i] > 1) return false;
        }
        if (routes.size() > K) return false;

        if (e != sumod) {
            return false;
        }

        return true;
    }

    void individual::initialize(graph& G) {
        routes.resize(K);
        const int num_customers = V - 1;
        std::vector<int> customers(num_customers);
        std::iota(customers.begin(), customers.end(), 1);
        // 随机打乱客户顺序
        shuffle(customers.begin(), customers.end(), gen);
        // 使用list保存路径以便安全删除/添加操作
        std::list<RouteInfo> temp_routes;
        std::unordered_map<int, std::list<RouteInfo>::iterator> start_map;
        std::unordered_map<int, std::list<RouteInfo>::iterator> end_map;

        // 初始化每个客户为独立路径
        for (int c : customers) {
            RouteInfo new_route;
            new_route.customers.push_back(c);
            new_route.total_load = G.points[c]->q;
            new_route.start = c;
            new_route.end = c;
            temp_routes.push_back(new_route);
            auto it = --temp_routes.end();
            start_map[c] = it;
            end_map[c] = it;
        }
      
        // 计算所有节约值
        std::vector<std::pair<double, std::pair<int, int>>> savings;
        for (int i = 1; i <= num_customers; ++i) {
            for (int j = i + 1; j <= num_customers; ++j) {
                double save_ij = G.dist_matrix[i][0] + G.dist_matrix[0][j] - G.dist_matrix[i][j];
                double save_ji = G.dist_matrix[j][0] + G.dist_matrix[0][i] - G.dist_matrix[j][i];
                savings.emplace_back(save_ij, std::make_pair(i, j));
                savings.emplace_back(save_ji, std::make_pair(j, i));
            }
        }

        std::sort(savings.begin(), savings.end(), std::greater<>());
        // 合并路径直到达到K辆或无法合并
        constexpr double merge_prob = 0.9; // 85%概率接受最优合并
        for (const auto& [saving_val, nodes] : savings) {
           /*  添加随机跳过机制*/
            if (std::uniform_real_distribution<double>(0, 1)(gen) > merge_prob) {
                continue;
            }
            if (temp_routes.size() <= K) break;
            
            int i = nodes.first;  // 路径A的末端
            int j = nodes.second; // 路径B的起点

            if (!end_map.count(i) || !start_map.count(j)) continue;

            auto routeA_it = end_map[i];
            auto routeB_it = start_map[j];
            if (routeA_it == routeB_it) continue;
            
            // 提前保存路径信息
            const auto& routeA = *routeA_it;
            const auto& routeB = *routeB_it;
            const int routeA_start = routeA.start;
            const int routeA_end = routeA.end;
            const int routeB_start = routeB.start;
            const int routeB_end = routeB.end;

            // 检查容量约束
            if (routeA.total_load + routeB.total_load > Q) continue;
           
            // 创建合并后的路径
            RouteInfo merged;
            merged.customers = routeA.customers;
            merged.customers.insert(merged.customers.end(),
                routeB.customers.begin(),
                routeB.customers.end());
            merged.total_load = routeA.total_load + routeB.total_load;
            merged.start = routeA_start;
            merged.end = routeB_end;

            // 删除原路径并更新哈希表
            temp_routes.erase(routeA_it);
            temp_routes.erase(routeB_it);
            start_map.erase(routeA_start);
            end_map.erase(routeA_end);
            start_map.erase(routeB_start);
            end_map.erase(routeB_end);

            // 插入新路径
            temp_routes.push_back(merged);
            auto merged_it = --temp_routes.end();
            start_map[merged.start] = merged_it;
            end_map[merged.end] = merged_it;
            
        }
       
        // 将结果分配到车辆路径
        int route_idx = 0;
        for (auto& route : temp_routes) {
            if (route_idx >= K) break;
            current_load[route_idx] = route.total_load;
            routes[route_idx++] = std::move(route.customers);          
        }
  
        // 处理剩余车辆（若有）
        while (route_idx < K) {
            routes[route_idx++].clear();
        }

        // 标记已使用的客户
        std::fill(ifuse.begin(), ifuse.end(), false);
        for (auto& route : routes) {
            for (int c : route) {
                ifuse[c] = true;
            }
        }
       
        int min_loadv = 0;              
        for (int i = 1; i < K; ++i) {
            if (current_load[i] < current_load[min_loadv]) {
                min_loadv = i;
            }
        }
        int current_vehicle = min_loadv;
        for (int customer : customers) {
            if (ifuse[customer] == false)
            {
                // 分配客户到车辆
                routes[current_vehicle].push_back(customer);
                ifuse[customer] = true;
                current_load[current_vehicle] += G.points[customer]->q;

                // 更新最小载货车辆序号（遍历所有车辆）
                min_loadv = 0;
                for (int i = 1; i < K; ++i) {
                    if (current_load[i] < current_load[min_loadv]) {
                        min_loadv = i;
                    }
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
    int count = 0;
    
    for (const auto& route : routes)
    {
  
        double dist = 0;
        int demand = 0;
        if (route.empty()) continue;

        // 从仓库出发到第一个客户
        dist += G.dist_matrix[0][route[0]];
        demand += G.points[route[0]]->q;
        // 客户之间的移动
        for (size_t i = 1; i < route.size(); ++i)
        {
            dist += G.dist_matrix[route[i - 1]][ route[i]];
            demand += G.points[route[i]]->q;
            
        }

        // 从最后一个客户返回仓库
        dist += G.dist_matrix[route.back()][0];
        current_load[count] = demand;
        sumod += dist;// 路径长度加到总值
        e += dist + ((demand > Q) ? G.p * (demand - Q) : 0);//路径长度加到总评估值
        count++;
    }
    for (int c = 0; c < V; c++)
    {
        if (!ifuse[c])
            e = INFINITY;
    }
}

individual individual::mutate(graph& G) {
    double r_num = generateRandomNumber();
    double pm = (e < G.avg_e) ? (G.pm1 - (G.pm1 - G.pm2) * (G.avg_e - e) / (G.avg_e - G.min_e)) : G.pm1;
    if (r_num > pm) return *this;

    if (routes.empty())  return *this;

    // 随机选择一种变异方式
    int mutation_type = rand() % 2;

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

    }

    // 重新评估
    evaluate(G);
    return *this;
}


    // 启发式优化：将超载路径的客户转移到其他车辆
void individual::repair_overload_routes(graph& G)
{   
    
    // 更新最小载货车辆序号（遍历所有车辆）
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
    // 更新最小载货车辆序号（遍历所有车辆）
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
        std::sort(nodes.begin(), nodes.end(),
            [&](const int& a, const int& b) {
                // 降序排列：更大的 q 值排在前面
                return G.points[a]->q > G.points[b]->q;
            }
        );

        routes[min_loadv].push_back(nodes.back());
        current_load[min_loadv] += G.points[nodes.back()]->q;
        nodes.pop_back();

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
// 其他函数实现
//-----------------------------------------------------------
bool compareByR(const individual& I1, const individual& I2) {
    return I1.sumod < I2.sumod;
}

bool compareByE(const individual& I1, const individual& I2) {
    return I1.e < I2.e;
}

std::vector<individual> crossover(std::vector<individual>& parents,int need, graph& G) {
    vector<individual> crossresult;
    int i = 0;

    for (auto it = parents.begin(); it != parents.end(); ) {

        double r_num = generateRandomNumber();
        double pc = (it->e < G.avg_e) ? (G.pc1 - (G.pc1 - G.pc2) * (G.avg_e - it->e) / (G.avg_e - G.min_e)) : G.pc1;
        if (r_num > pc) {

            crossresult.push_back(*it);
            it = parents.erase(it);         // 正确移除并更新迭代器
            i++;
            if (i >= need)return crossresult;
        }
        else {
            ++it;
        }
    }

    while (!parents.empty())
    {
        if (parents.size() >= 2)
        { // 先获取最前一个元素
            individual parent1 = parents.front();
            parents.erase(parents.begin());  // 然后移除

            individual parent2 = parents.front();
            parents.erase(parents.begin());

            /* 第一步：随机交换路径 */
            if (!parent1.routes.empty() && !parent2.routes.empty()) {
                // 随机选择两条路径（分别来自两个父代）
                random_device rd;
                mt19937 gen(rd());
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
                                    G.dist_matrix[route[i - 1]][cust] +
                                    G.dist_matrix[cust][route[i]] -
                                    G.dist_matrix[route[i - 1]] [route[i]];

                                if (increase < min_increase) {
                                    min_increase = increase;
                                    best_route = r;
                                    best_pos = i;
                                }
                            }

                            // 检查路径首尾位置
                            double start_increase =
                                G.dist_matrix[0][cust] +
                                G.dist_matrix[cust][route[0]] -
                                G.dist_matrix[0][ route[0]];

                            double end_increase =
                                G.dist_matrix[route.back()][cust] +
                                G.dist_matrix[cust][0] -
                                G.dist_matrix[route.back()][0];

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

                crossresult.push_back(*parent);
                crossresult.back().evaluate(G);
                i++;
                if (i >= need)return crossresult;

            }
        }
        else if (parents.size() == 1) {
            crossresult.push_back(parents.back());
            parents.pop_back();
            i++;
            if (i >= need)return crossresult;
        }
    }
    return crossresult;

}

int GA(int VNum, point* p, int CarNum, int MaxLoad, graph& G,  individual& out_best ) {
    vector<individual>generation_1;
    vector<individual>generation_2;
    // 最终结果处理部分修改：
    individual local_best(VNum - 1, CarNum, MaxLoad);
    int answer = INFINITY;
    for (int i = 0; i < 50; i++)
    {
        individual I(VNum - 1, CarNum, MaxLoad);
        I.initialize(G);
        I.repair_overload_routes(G);
        generation_1.push_back(I);

    }
    //自适应调参
    G.p=500 * G.avg_dist / G.avg_q;
    double min_output = G.avg_dist / G.avg_q;
    PIDController pid_p(7*min_output, 10 * min_output , 0, -8*min_output, 8*min_output);
    G.self_adaptation(generation_1,pid_p);
    //初始化答案
    std::sort(generation_1.begin(), generation_1.end(), compareByE);
    answer = generation_1[0].sumod;
    local_best = generation_1[0];
    if(out_best.routes.empty())
        out_best = local_best;
    std::cout << answer << endl;
    //开始迭代
 
    for (int n = 0; n < 1000; n++)
    {
        /*保留优秀样本*/
        for (int i = 0; i < 5; i++)
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

        //增加交叉繁殖和变异后样本
        int need = 45;
        vector <individual>crosresult = move(crossover(generation_1, need, G));
        for (individual I : crosresult)
        {
            generation_2.push_back(I.mutate(G));
            if (generation_2.size() >= 50)break;
        }

        //准备下一代
        generation_1 = move(generation_2);
        G.self_adaptation(generation_1,pid_p);
        std::sort(generation_1.begin(), generation_1.end(), compareByE);
 
        for (auto& I : generation_1) {
            if (I.is_valid(G)) {
                if (I.sumod < local_best.sumod) {
                    local_best = I;
                    answer = I.sumod;
                    if (answer < out_best.sumod)
                        out_best = local_best;
                }
            }
        }
    }
    // 在原有循环后添加异常处理
    bool found_valid = false;
   //寻找合法的最好结果
    for ( auto& I : generation_1) {
        if (I.is_valid(G)) {
            found_valid = true;
            if ( I.sumod < local_best.sumod) {
                local_best = I;
                answer = I.sumod;
                if (answer < out_best.sumod)
                    out_best = local_best;
            }   
            std::cout << "最佳结果" << answer << "有效" << " 无效比例" << G.invalid_rate << endl;
            return answer;
        }
    }

    if (!found_valid)
    {
        // 尝试修复直到找到有效个体或达到某个条件
        while(  individual::count < 50000  ) {
            for (int i = 0; i < 10; i++)
            {
                generation_1[i].repair_overload_routes(G);

                if (generation_1[i].is_valid(G)) {
                    found_valid = true;
                    if ( generation_1[i].sumod < local_best.sumod) {
                        local_best = generation_1[i];
                        answer = generation_1[i].sumod;
                        if (answer < out_best.sumod)
                            out_best = local_best;
                    }

                    std::cout << "最佳结果" << answer << "有效"  << " 无效比例" << G.invalid_rate << endl;
                    return answer;
                }
                std::sort(generation_1.begin(), generation_1.end(), compareByE);
            }
        }
        
    }

    return answer;

}