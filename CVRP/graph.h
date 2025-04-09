#pragma once


struct point
{
	//每个节点包括坐标和需求值
	int x{};
	int y{};
	int q{};
	point() {};
	point(int X, int Y, int Q)
	{
		x = X; y = Y; q = Q;
	}
};
//两点间距
double caculateDistance(point* p, int i, int j)
{
	return sqrt((p[i].x - p[j].x) * (p[i].x - p[j].x) + (p[i].y - p[j].y) * (p[i].y - p[j].y));
}

struct graph
{
	vector<point*> points;
	vector<vector<double>> dist_matrix;// 距离矩阵
	// 构造函数
	graph(point* p, int num_points) {
		// 存储所有点指针
		for (int i = 0; i < num_points; ++i) {
			points.push_back(&p[i]);
		}

		// 初始化距离矩阵
		dist_matrix.resize(num_points, vector<double>(num_points, 0));

		// 计算并填充距离矩阵
		for (int i = 0; i < num_points; ++i) {
			for (int j = 0; j < num_points; ++j) {
				if (i == j) {
					dist_matrix[i][j] = 0; // 同一点距离为0
				}
				else {
					// 计算距离并四舍五入为整数
					dist_matrix[i][j] = static_cast<int>(round(caculateDistance(p, i, j)));
				}
			}
		}
	}

	// 获取两点间距离（带边界检查）
	double get_distance(int from, int to) const {
		if (from < 0 || from >= points.size() || to < 0 || to >= points.size()) {
			throw out_of_range("Point index out of range");
		}
		return dist_matrix[from][to];
	}

};