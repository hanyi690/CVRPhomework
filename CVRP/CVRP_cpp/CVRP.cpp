#include<iostream>
#include"GetFile.h"
#include"GA.h"
using namespace std;
struct Data {
    int bestresult;
    int out_evaluate_count;
    string name;
    double AverageError_rate;

    // ���캯��
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
       /* cout << "�����ļ��е�ַ:";*/
   folderPath = "D:\\VisualStudio2022\\C++\\CVRP\\Vrp_benchmark\\Vrp-Set-A\\A.vrp";
        folderPath = removeQuotes(folderPath);
      
        vector<fs::path> filePaths;
        try {
            // �����ļ���
            for (const auto& entry : fs::directory_iterator(folderPath)) {
                if (entry.is_regular_file()) { 
                    filePaths.push_back(entry.path());
                }
            }

            // ���ļ�������
            sort(filePaths.begin(), filePaths.end());

            // ������
            for (const auto& path : filePaths) {
                FileName.push_back(path.string());
             //   cout << path.string() << endl;
           
            }
            std::cout << "��ȡ���" << endl;
       /*   break;*/
        }
        catch (const fs::filesystem_error& e) {
            cerr << "����: " << e.what() << std::endl;
        }
  /*  }*/
        std::cout << "��ʼ��CVRP��" << endl;
    vector<Data> dataset;//�⼯��
    int count = 0;
    for ( auto& path : FileName) {
        count++;
        if (count != 12)continue;
        point* p = nullptr;
        int VNum;//�����ֿ����ڵĽڵ���
        int CarNum;//������
        int MaxLoad;//ÿ�����������
        int Optimalvalue;//��Ѵ�
        int bestresult = 2147483647;
        double AverageError = 0;
        //���ļ���ɳ�ʼ��ֵ
        ReadFile(VNum, CarNum, MaxLoad, p, Optimalvalue,path);
        cout << path << endl;
        int answer;
        
        //������ʼ�������
        graph G(p, VNum);
        //cout << G.avg_dist << " " << G.avg_q << endl;
        const individual* best_valid_individual = nullptr;
        int o_e_c = 0;
        //��ÿ��ʵ������25����ƽ��ֵ
        for (int i = 1; i <= 25; i++)
        {   
            
            individual::count = 0;//������������
            answer=GA(VNum, p, CarNum, MaxLoad, G, best_valid_individual);
            AverageError += 4.0 * (answer  - Optimalvalue) / Optimalvalue;
            bestresult = (answer < bestresult) ? answer : bestresult;

            std::cout << "��������" << individual::count;
            if (individual::count > 50000) o_e_c++;
            std::cout << "ʵ��" << i << " " << answer << " ";
            std::cout << i * 4 << "%......" << endl; ;
        }
        std::cout << endl;
        Data* data = new Data(bestresult,  o_e_c, path, AverageError / 100);
        dataset.push_back(*data);
        delete data; 
        best_valid_individual->printIndividual(G);
        std::cout << " ��֪����:" << Optimalvalue << "���ƽ��ֵ��" << AverageError << "%" << endl << "�㷨�������Žⳤ�ȣ�" << bestresult << endl<<endl;
    }

    ofstream csvFile("D:\\VisualStudio2022\\C++\\CVRP\\Vrp_benchmark\\Vrp-Set-A\\A_output\\output.csv");
    if (!csvFile.is_open()) {
        std::cerr << "�޷������ļ���" << std::endl;
        return 1;
    }

    // д�� CSV ��ͷ
    csvFile << "Name,bestresult,valid_count,out_evaluate_count,AverageError_rate\n";

    // д��������
    for (const auto& data : dataset) {
        // ����������Ż����ŵ����ݣ���˫���Ű�����
        std::string escapedName = "\"" + data.name + "\"";
        // �滻�ڲ�����Ϊ˫���ţ�Excel ��ת�����
        size_t pos = 0;
        while ((pos = escapedName.find('\"', pos))!= std::string::npos) {
            escapedName.insert(pos, "\"");
            pos += 2;
        }

        // д��һ������
        csvFile << data.bestresult << ","
            <<data. AverageError_rate << ","
            << data.out_evaluate_count << "\n";
    }

    csvFile.close();
    std::cout << "CSV �ļ������ɣ�" << endl;

    // ����һ��ofstream��������д���ļ�
    std::ofstream outFile("D:\\VisualStudio2022\\C++\\CVRP\\Vrp_benchmark\\Vrp-Set-A\\A_output\\output.txt");

    // ����ļ��Ƿ�ɹ���
    if (!outFile.is_open()) {
        std::cerr << "�޷����ļ�output.txt" << std::endl;
        return 1;
    }

    // ����׼������ض����ļ�
    std::streambuf* originalCoutBuffer = std::cout.rdbuf(outFile.rdbuf());

    // �������ж�std::cout���������д�뵽output.txt�ļ���
    std::cout << "����Ҫ���浽�ļ������ݡ�" << std::endl;

    // �ָ���׼�����
    std::cout.rdbuf(originalCoutBuffer);

    // �ر��ļ�
    outFile.close();

 
    return 0;
}