// GA.cpp
#include "GA.h"
#include <cmath>
#include <algorithm> // for std::min


 PIDController::PIDController(double Kp, double Ki, double Kd, double min_output, double max_output)
        : Kp(Kp), Ki(Ki), Kd(Kd), min_output(min_output), max_output(max_output),
        integral(0.0), prev_error(0.0) {}



    double PIDController::compute(double error) {
        integral += error ;  // �������ۻ�
        double derivative = (error - prev_error) ;  // ΢����
        prev_error = error;

        // ����PID���
        double output = Kp * error + Ki * integral + Kd * derivative;

        // ����޷��Ϳ����ʹ���
        if (output > max_output) {
            output = max_output;
            integral -= error ; // �����ͣ����˻���
        }
        else if (output < min_output) {
            output = min_output;
            integral -= error ;
        }
        return output;
    }



// point ���캯��
point::point() : x(0), y(0), q(0) {}
point::point(int X, int Y, int Q) : x(X), y(Y), q(Q) {}

// �����������
double calculateDistance(point* p, int i, int j) {
    return sqrt((p[i].x - p[j].x) * (p[i].x - p[j].x) +
        (p[i].y - p[j].y) * (p[i].y - p[j].y));
}

// graph ���캯��
graph::graph(point* p, int num_points) {
    const double target_invalid_rate = 0.3; // Ŀ�겻�Ϸ�����30%
    avg_dist = 0;
    avg_q = 0;
    for (int i = 0; i < num_points; ++i) {
        points.push_back(&p[i]);
        avg_q += p[i].q;
    }
    avg_q /= (num_points - 1);

    dist_matrix.resize(num_points, std::vector<int>(num_points, 0));
    for (int i = 0; i < num_points; ++i) {
        for (int j = 0; j < num_points; ++j) {
            if (i == j) {
                dist_matrix[i][j] = 0;
            }
            else {
                dist_matrix[i][j] = calculateDistance(p, i, j);
                avg_dist += dist_matrix[i][j];
            }
        }
    }
    avg_dist /= (num_points - 1) * num_points;
}

// ��ȡ������루���߽��飩
double graph::get_distance(int from, int to) const {
    if (from < 0 || from >= points.size() || to < 0 || to >= points.size()) {
        throw std::out_of_range("Point index out of range");
    }
    return dist_matrix[from][to];
}


// ����Ӧ��������
void graph::self_adaptation(std::vector<individual> generation,PIDController& pid_p) {

    min_e = INFINITY;
    avg_e = 0;
    pc1 = 0.9, pc2 = 0.4, pm1 = 0.1, pm2 = 0.001;
    invalid_count = 0;
    for (const individual& I : generation) {
        min_e = std::min(I.e, min_e);
        avg_e += I.e;
        if (!I.is_valid(*this)) invalid_count++;
    }

    avg_e /= generation.size();
    invalid_rate = static_cast<double>(invalid_count) / generation.size();
    // �������
    double error = invalid_rate - target_invalid_rate;
    double new_p = pid_p.compute(error);
    p += new_p;
    double min_output = avg_dist / avg_q;
    if (p < min_output)p = min_output;
    else if (p > 100 * min_output) p = 100 * min_output;



}

