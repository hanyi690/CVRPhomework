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
		//���ļ���ɳ�ʼ��ֵ
		ReadFile(V, K, Q, p, Optimalvalue);
		//��ÿ��ʵ������25����ƽ��ֵ
		for (int i = 0; i < 25; i++)
		{
			int newresult;
			newresult = GA(V, p, K, Q);
			bestresult = (newresult < bestresult) ? newresult : bestresult;
			AverageError +=  4.0 * (newresult - Optimalvalue) / Optimalvalue;
		}
		cout << "���ƽ��ֵ��" << AverageError << "%" << endl << "�㷨�������Žⳤ�ȣ�" << bestresult << endl;
	}

	return 0;
}