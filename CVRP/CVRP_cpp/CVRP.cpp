#include<iostream>
#include"GetFile.h"
#include"GA.h"
using namespace std;
struct Data {
    int bestresult;
    int out_evaluate_count;
    string name;
    double AverageError_rate;

    // 构造函数
    Data(int br,  int oec, string n, double aer)
        : bestresult(br), out_evaluate_count(oec), name(n), AverageError_rate(aer) {
    }
};
int main()
{

    string folderPath;
    vector<string> FileName;
  /*while (1)
   {*/
       /* cout << "输入文件夹地址:";*/
   folderPath = "D:\\VisualStudio2022\\C++\\CVRP\\Vrp_benchmark\\Vrp-Set-A\\A.vrp";
        folderPath = removeQuotes(folderPath);
      
        vector<fs::path> filePaths;
        try {
            // 遍历文件夹
            for (const auto& entry : fs::directory_iterator(folderPath)) {
                if (entry.is_regular_file()) { 
                    filePaths.push_back(entry.path());
                }
            }

            // 按文件名排序
            sort(filePaths.begin(), filePaths.end());

            // 输出结果
            for (const auto& path : filePaths) {
                FileName.push_back(path.string());
             //   cout << path.string() << endl;
           
            }
            std::cout << "读取完成" << endl;
       /*   break;*/
        }
        catch (const fs::filesystem_error& e) {
            cerr << "错误: " << e.what() << std::endl;
        }
  /*  }*/
        std::cout << "开始求CVRP解" << endl;
    vector<Data> dataset;//解集合
    int count = 0;
    for ( auto& path : FileName) {
        count++;
        if (count != 12)continue;
        point* p = nullptr;
        int VNum;//包括仓库在内的节点数
        int CarNum;//车辆数
        int MaxLoad;//每辆车最大负载量
        int Optimalvalue;//最佳答案
        int bestresult = 2147483647;
        double AverageError = 0;
        //读文件完成初始数值
        ReadFile(VNum, CarNum, MaxLoad, p, Optimalvalue,path);
        cout << path << endl;
        int answer;
        
        //构建初始距离矩阵
        graph G(p, VNum);
        //cout << G.avg_dist << " " << G.avg_q << endl;
        const individual* best_valid_individual = nullptr;
        int o_e_c = 0;
        //对每个实例计算25次求平均值
        for (int i = 1; i <= 25; i++)
        {   
            
            individual::count = 0;//评估次数置零
            answer=GA(VNum, p, CarNum, MaxLoad, G, best_valid_individual);
            AverageError += 4.0 * (answer  - Optimalvalue) / Optimalvalue;
            bestresult = (answer < bestresult) ? answer : bestresult;

            std::cout << "评估次数" << individual::count;
            if (individual::count > 50000) o_e_c++;
            std::cout << "实例" << i << " " << answer << " ";
            std::cout << i * 4 << "%......" << endl; ;
        }
        std::cout << endl;
        Data* data = new Data(bestresult,  o_e_c, path, AverageError / 100);
        dataset.push_back(*data);
        delete data; 
        best_valid_individual->printIndividual(G);
        std::cout << " 已知最优:" << Optimalvalue << "误差平均值：" << AverageError << "%" << endl << "算法所得最优解长度：" << bestresult << endl<<endl;
    }

    ofstream csvFile("D:\\VisualStudio2022\\C++\\CVRP\\Vrp_benchmark\\Vrp-Set-A\\A_output\\output.csv");
    if (!csvFile.is_open()) {
        std::cerr << "无法创建文件！" << std::endl;
        return 1;
    }

    // 写入 CSV 表头
    csvFile << "Name,bestresult,valid_count,out_evaluate_count,AverageError_rate\n";

    // 写入数据行
    for (const auto& data : dataset) {
        // 处理包含逗号或引号的内容（用双引号包裹）
        std::string escapedName = "\"" + data.name + "\"";
        // 替换内部引号为双引号（Excel 的转义规则）
        size_t pos = 0;
        while ((pos = escapedName.find('\"', pos))!= std::string::npos) {
            escapedName.insert(pos, "\"");
            pos += 2;
        }

        // 写入一行数据
        csvFile << data.bestresult << ","
            <<data. AverageError_rate << ","
            << data.out_evaluate_count << "\n";
    }

    csvFile.close();
    std::cout << "CSV 文件已生成！" << endl;

    // 创建一个ofstream对象，用于写入文件
    std::ofstream outFile("D:\\VisualStudio2022\\C++\\CVRP\\Vrp_benchmark\\Vrp-Set-A\\A_output\\output.txt");

    // 检查文件是否成功打开
    if (!outFile.is_open()) {
        std::cerr << "无法打开文件output.txt" << std::endl;
        return 1;
    }

    // 将标准输出流重定向到文件
    std::streambuf* originalCoutBuffer = std::cout.rdbuf(outFile.rdbuf());

    // 现在所有对std::cout的输出都会写入到output.txt文件中
    std::cout << "这是要保存到文件的内容。" << std::endl;

    // 恢复标准输出流
    std::cout.rdbuf(originalCoutBuffer);

    // 关闭文件
    outFile.close();

 
    return 0;
}