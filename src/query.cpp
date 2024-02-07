#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <cmath>
#define extractor_type "./extractors-result/color-layout-aloi.txt"
#define image_number 79
#define relevant_images 72
#define first_image 504
#define final_image 575

class Object
{
public:
    std::string name;             // Nome da imagem
    std::vector<double> features; // Vetor de características Zernike ou Color Layout
    double distance;              // Variável para armazenar a distância
    int number;

    Object(const std::string &n, const std::vector<double> &f) : name(n), features(f), distance(0.0), number(0) {}
};

double euclideanDistance(const std::vector<double> &v1, const std::vector<double> &v2)
{
    double distance = 0.0;
    for (size_t i = 0; i < v1.size(); ++i)
    {
        double diff = v1[i] - v2[i];
        distance += diff * diff;
    }
    return std::sqrt(distance);
}

std::pair<std::vector<double>, std::vector<double>> calculatePrecisionAndRecall(
    const std::vector<Object> &knn_results)
{
    int image_initial = first_image;
    int image_final = final_image;
    int total_relevant = relevant_images;

    std::vector<double> precision_result;
    std::vector<double> recall_result;

    int total_retrieved = 0;
    int relevant_retrieved = 0;

    for (const Object &obj : knn_results)
    {
        total_retrieved++;

        if (obj.number >= image_initial && obj.number <= image_final)
        {
            relevant_retrieved++;
        }

        double precision = static_cast<double>(relevant_retrieved) / total_retrieved;
        double recall = static_cast<double>(relevant_retrieved) / total_relevant;

        precision_result.push_back(precision);
        recall_result.push_back(recall);
    }

    // ----- Exibe os resultados do Precision x Recall -----
    // std::cout << "" << std::endl;
    // for (const double precision : precision_result)
    // {
    //     std::cout << precision << " ";
    // }
    // std::cout << std::endl;
    // for (const double recall : recall_result)
    // {
    //     std::cout << recall << " ";
    // }

    return std::make_pair(precision_result, recall_result);
}

std::vector<double> calculateInterpolatedPrecision(
    const std::vector<double> &precision_result,
    const std::vector<double> &recall_result)
{
    std::vector<double> recall_axis = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
    std::vector<double> interpolated_precision;

    for (const double recall_point : recall_axis)
    {
        double max_precision = 0.0;

        for (int i = 0; i < precision_result.size(); i++)
        {
            if (precision_result[i] > max_precision && recall_result[i] >= recall_point)
            {
                max_precision = precision_result[i];
            }
        }
        interpolated_precision.push_back(max_precision);
    }

    //----- Exibe os resultados da precisão interpolada -----
    // for (const double precision : interpolated_precision)
    // {
    //     std::cout << precision << " ";
    // }
    // std::cout << "" << std::endl;

    return interpolated_precision;
}

void rangeQuery(std::vector<Object> dataset, std::vector<double> &queryImageFeatures, double range_threshold)
{
    std::vector<Object> range_results;

    auto start = std::chrono::high_resolution_clock::now(); // Inicia a contagem do tempo

    for (Object &obj : dataset)
    {
        obj.distance = euclideanDistance(obj.features, queryImageFeatures); // Atualiza a distância no objeto
        if (obj.distance <= range_threshold)
        {
            range_results.push_back(obj);
        }
    }

    auto stop = std::chrono::high_resolution_clock::now();                                                            // Marca o fim da execução
    std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(stop - start); // Calcula o tempo de execução em milissegundos

    // Ordena os resultados do range
    std::sort(range_results.begin(), range_results.end(), [](const Object &a, const Object &b) { // Ordena os resultados por distância
        return a.distance < b.distance;
    });

    // ----- Exibe os resultados do Range Query -----
    // int numberObj = 0;
    std::cout << "" << std::endl;
    for (const Object &obj : range_results)
    {
        std::cout << obj.name << " " << obj.distance << std::endl;
        //     numberObj++;
        //     if(numberObj == 100){
        //          std::cout << "range------------->100" << std::endl;
        //     }else if(numberObj == 200){
        //         std::cout << "range------------->200" << std::endl;
        //     }
        //     else if(numberObj == 300){
        //         std::cout << "range------------->300" << std::endl;
        //     }
    }
    // std::cout << "\Quantity images: " << numberObj << std::endl;
    // std::cout << "Tempo: " << duration.count() << "ms" << std::endl;
}

std::vector<double> knnQuery(std::vector<Object> dataset, std::vector<double> &queryImageFeatures, int k)
{
    std::vector<Object> knn_results;

    auto start = std::chrono::high_resolution_clock::now(); // Inicia a contagem do tempo
    int numberObject = 0;
    for (Object &obj : dataset)
    {
        obj.distance = euclideanDistance(obj.features, queryImageFeatures); // Atualiza a distância no objeto
        obj.number = numberObject;
        numberObject++;

        if (knn_results.size() < k)
        {
            knn_results.push_back(obj);
        }
        else
        {
            double max_distance = euclideanDistance(knn_results[k - 1].features, queryImageFeatures);
            if (obj.distance < max_distance)
            {
                knn_results.pop_back();
                knn_results.push_back(obj);
                std::sort(knn_results.begin(), knn_results.end(), [](const Object &a, const Object &b) { // Ordena os resultados por distância
                    return a.distance < b.distance;
                });
            }
        }
    }

    auto stop = std::chrono::high_resolution_clock::now();                                                            // Marca o fim da execução
    std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(stop - start); // Calcula o tempo de execução em milissegundos

    // ----- Exibe os resultados do k-NN -----
    std::cout << "kNN:" << std::endl;
    for (const Object &obj : knn_results)
    {
        std::cout << obj.name << "  " << obj.distance << std::endl;
    }

    std::pair<std::vector<double>, std::vector<double>> result = calculatePrecisionAndRecall(knn_results);
    std::vector<double> interpolated_precision = calculateInterpolatedPrecision(result.first, result.second);
    std::cout << "Tempo: " << duration.count() << "ms" << std::endl;

    return interpolated_precision;
}

int main()
{
    // Nome do arquivo de características
    std::string filename = extractor_type;

    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Não foi possível abrir o arquivo." << std::endl;
        return 1;
    }

    std::vector<Object> dataset;
    std::string line;

    while (std::getline(file, line))
    {
        std::istringstream iss(line); // Cria um stream de string a partir da linha
        std::string imageName;
        double featureValue;

        iss >> featureValue; // Leitura do valor (primeira posição), que não será utilizado.
        iss >> imageName;    // Leitura do nome da imagem (segunda posição).

        std::vector<double> features;
        while (iss >> featureValue)
        { // Leitura dos valores das características (demais posições)
            features.push_back(featureValue);
        }

        dataset.push_back(Object(imageName, features));
    }

    int queryImageNumber = image_number;

    std::vector<double> &queryImageFeatures = dataset[queryImageNumber].features; // Features da imagem de consulta

    int image_initial = first_image;
    int image_final = final_image;
    int total_relevant = relevant_images;

    // rangeQuery(dataset, queryImageFeatures, 0.40);
    knnQuery(dataset, queryImageFeatures, 20);

    // ----- Calcula e exibe os resultados da média da precisão interpolada -----
    std::vector<double> averagePrecision(11, 0.0);
    std::vector<double> sumPrecision(11, 0.0);

    for (int i = image_initial; i <= image_final; i++)
    {
        std::vector<double> interpolatedPrecision = knnQuery(dataset, dataset[i].features, total_relevant);

        for (int j = 0; j < interpolatedPrecision.size(); j++)
        {
            sumPrecision[j] += interpolatedPrecision[j];
        }
    }

    // Calcula a média da precisão interpolada
    std::cout << "Média da precisão interpolada:" << std::endl;
    for (int k = 0; k < sumPrecision.size(); k++)
    {
        std::cout << sumPrecision[k] / relevant_images << std::endl;
    }

    return 0;
}
