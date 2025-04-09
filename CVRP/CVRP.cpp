#include<iostream>
#include"GetFile.h"
#include"GA.h"
using namespace std;
int main()
{
	point* p=nullptr;
	int V;
	int K;
	int Q;
	int bestresult = 2147483647;
	int Optimalvalue;
	double AverageError = 0;

	while (1)
	{
		//读文件完成初始数值
		ReadFile(V, K, Q, p, Optimalvalue);
		//对每个实例计算25次求平均值
		for (int i = 0; i < 25; i++)
		{
			int newresult;
			newresult = GA(V, p, K, Q);
			bestresult = (newresult < bestresult) ? newresult : bestresult;
			AverageError +=  4.0 * (newresult - Optimalvalue) / Optimalvalue;
		}
		cout << "误差平均值：" << AverageError << "%" << endl << "算法所得最优解长度：" << bestresult << endl;
	}

	return 0;
}