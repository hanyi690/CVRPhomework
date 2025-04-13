// Graph.h
#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <stdexcept> // for std::out_of_range

// 前置声明
struct individual; // 假设已在其他头文件中定义

class PIDController {
public:
    PIDController(double Kp, double Ki, double Kd, double min_output, double max_output);
    double compute(double error);
private:
    double Kp, Ki, Kd;
    double min_output, max_output;
    double integral;     // 积分累积
    double prev_error;   // 上一次误差
};
struct point {
    int x, y, q;
    point();
    point(int X, int Y, int Q);
};

class graph {
public:
    std::vector<point*> points;
    std::vector<std::vector<double>> dist_matrix;
    double p;//惩罚系数
    int avg_q;
    double avg_dist, avg_e, min_e;
    double pc1, pc2, pm1, pm2;
    double invalid_rate;
    int invalid_count;
    double target_invalid_rate;
    // 构造函数
    graph(point* p, int num_points);

    // 成员函数
    double get_distance(int from, int to) const;
    void self_adaptation(std::vector<individual> generation, PIDController& pid_p);
};

// 工具函数声明
double calculateDistance(point* p, int i, int j);

#endif // GRAPH_H