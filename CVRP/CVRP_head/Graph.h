// Graph.h
#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <stdexcept> // for std::out_of_range

// ǰ������
struct individual; // ������������ͷ�ļ��ж���

class PIDController {
public:
    PIDController(double Kp, double Ki, double Kd, double min_output, double max_output);
    double compute(double error);
private:
    double Kp, Ki, Kd;
    double min_output, max_output;
    double integral;     // �����ۻ�
    double prev_error;   // ��һ�����
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
    double p;//�ͷ�ϵ��
    int avg_q;
    double avg_dist, avg_e, min_e;
    double pc1, pc2, pm1, pm2;
    double invalid_rate;
    int invalid_count;
    double target_invalid_rate;
    // ���캯��
    graph(point* p, int num_points);

    // ��Ա����
    double get_distance(int from, int to) const;
    void self_adaptation(std::vector<individual> generation, PIDController& pid_p);
};

// ���ߺ�������
double calculateDistance(point* p, int i, int j);

#endif // GRAPH_H