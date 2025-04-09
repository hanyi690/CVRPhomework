#pragma once


struct point
{
	//ÿ���ڵ�������������ֵ
	int x{};
	int y{};
	int q{};
	point() {};
	point(int X, int Y, int Q)
	{
		x = X; y = Y; q = Q;
	}
};
//������
double caculateDistance(point* p, int i, int j)
{
	return sqrt((p[i].x - p[j].x) * (p[i].x - p[j].x) + (p[i].y - p[j].y) * (p[i].y - p[j].y));
}

struct graph
{
	vector<point*> points;
	vector<vector<double>> dist_matrix;// �������
	// ���캯��
	graph(point* p, int num_points) {
		// �洢���е�ָ��
		for (int i = 0; i < num_points; ++i) {
			points.push_back(&p[i]);
		}

		// ��ʼ���������
		dist_matrix.resize(num_points, vector<double>(num_points, 0));

		// ���㲢���������
		for (int i = 0; i < num_points; ++i) {
			for (int j = 0; j < num_points; ++j) {
				if (i == j) {
					dist_matrix[i][j] = 0; // ͬһ�����Ϊ0
				}
				else {
					// ������벢��������Ϊ����
					dist_matrix[i][j] = static_cast<int>(round(caculateDistance(p, i, j)));
				}
			}
		}
	}

	// ��ȡ�������루���߽��飩
	double get_distance(int from, int to) const {
		if (from < 0 || from >= points.size() || to < 0 || to >= points.size()) {
			throw out_of_range("Point index out of range");
		}
		return dist_matrix[from][to];
	}

};