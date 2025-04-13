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
    std::cout << "开始求CVRP解" << endl;
    vector<Data> dataset;//解集合
    int count = 0;
    //int a = 0;
    for (auto& path : FileName) {
        //a++;
        //if (a !=7  )continue;
        point* p = nullptr;
        int VNum;//包括仓库在内的节点数
        int CarNum;//车辆数
        int MaxLoad;//每辆车最大负载量
        int Optimalvalue;//最佳答案
        double AverageError = 0;
        //读文件完成初始数值
        ReadFile(VNum, CarNum, MaxLoad, p, Optimalvalue, path);
        cout << path << "已知最优" << Optimalvalue << endl;

        //构建初始图
        graph G(p, VNum);
        individual best_case(VNum - 1, CarNum, MaxLoad);
        best_case.sumod = INFINITY;
        int invalid_e_count = 0;
        //对每个实例计算25次求平均值
        for (int i = 1; i <= 25; i++)
        {

            individual::count = 0;//评估次数置零
            int answer = GA(VNum, p, CarNum, MaxLoad, G, best_case);
            double Error_rate = static_cast<double>(answer - Optimalvalue) / Optimalvalue;
            AverageError += Error_rate;

            std::cout << "评估次数" << individual::count;
            if (individual::count > 50000) invalid_e_count++;
            std::cout << "实例" << i << " " << answer << " Error_rate" << Error_rate << " ";
            std::cout << i * 4 << "%......" << endl; ;
        }
        std::cout << endl;
        Data* data = new Data(best_case.sumod, path, AverageError/25, invalid_e_count);
        dataset.push_back(*data);
        delete data;
        best_case.printIndividual(G);
        if (AverageError/25 > 0.3)count++;
        std::cout << " 已知最优:" << Optimalvalue << "误差平均值：" << AverageError/25 << endl << "算法所得最优解长度：" << best_case.sumod << endl << endl;
    }
    std::cout << "误差平均值大于0.3的有" << count << "个" << endl;
    generateCSVReport("output.csv", dataset);


    return 0;
}