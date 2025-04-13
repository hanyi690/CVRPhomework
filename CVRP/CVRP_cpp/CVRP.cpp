#include<iostream>
#include"GetFile.h"
#include"GA.h"
#include <thread>
#include <mutex>
#include <atomic>
#include <future>
using namespace std;

int main()
{
    ios::sync_with_stdio(false), cin.tie(0);
    string folderPath;
    vector<string> FileName;
    folderPath = "D:\\VisualStudio2022\\C++\\CVRP\\Vrp_benchmark\\Vrp-Set-A\\A.vrp";
    FileName = getSortedFileList(folderPath);
    std::cout << "��ʼ��CVRP��" << endl;
    vector<Data> dataset;//�⼯��
    int count = 0;
    //int a = 0;
    for (auto& path : FileName) {
        //a++;
        //if (a !=7  )continue;
        point* p = nullptr;
        int VNum;//�����ֿ����ڵĽڵ���
        int CarNum;//������
        int MaxLoad;//ÿ�����������
        int Optimalvalue;//��Ѵ�
        double AverageError = 0;
        //���ļ���ɳ�ʼ��ֵ
        ReadFile(VNum, CarNum, MaxLoad, p, Optimalvalue, path);
        cout << path << "��֪����" << Optimalvalue << endl;

        //������ʼͼ
        graph G(p, VNum);
        individual best_case(VNum - 1, CarNum, MaxLoad);
        best_case.sumod = INFINITY;
        int invalid_e_count = 0;
        //��ÿ��ʵ������25����ƽ��ֵ
        for (int i = 1; i <= 25; i++)
        {

            individual::count = 0;//������������
            int answer = GA(VNum, p, CarNum, MaxLoad, G, best_case);
            double Error_rate = static_cast<double>(answer - Optimalvalue) / Optimalvalue;
            AverageError += Error_rate;

            std::cout << "��������" << individual::count;
            if (individual::count > 50000) invalid_e_count++;
            std::cout << "ʵ��" << i << " " << answer << " Error_rate" << Error_rate << " ";
            std::cout << i * 4 << "%......" << endl; ;
        }
        std::cout << endl;
        Data* data = new Data(best_case.sumod, path, AverageError/25, invalid_e_count);
        dataset.push_back(*data);
        delete data;
        best_case.printIndividual(G);
        if (AverageError/25 > 0.3)count++;
        std::cout << " ��֪����:" << Optimalvalue << "���ƽ��ֵ��" << AverageError/25 << endl << "�㷨�������Žⳤ�ȣ�" << best_case.sumod << endl << endl;
    }
    std::cout << "���ƽ��ֵ����0.3����" << count << "��" << endl;
    generateCSVReport("output.csv", dataset);


    return 0;
}