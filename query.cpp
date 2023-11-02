#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <chrono> 
#include <algorithm>
#include <cmath>

class Object {
public:
    std::string name;  // Nome da imagem
    std::vector<double> features;  // Vetor de características Zernike ou Color Layout
    double distance; // Variável para armazenar a distância
    int numberObj;

    Object(const std::string& n, const std::vector<double>& f) : name(n), features(f), distance(0.0), numberObj(0) {}
};

double euclideanDistance(const std::vector<double>& v1, const std::vector<double>& v2) {
    double distance = 0.0;
    for (size_t i = 0; i < v1.size(); ++i) {
        double diff = v1[i] - v2[i];
        distance += diff * diff;
    }
    return std::sqrt(distance);
}

void rangeQuery(std::vector<Object> dataset, std::vector<double>& queryImageFeatures, double range_threshold) {
    std::vector<Object> range_results;

    auto start = std::chrono::high_resolution_clock::now(); // Inicia a contagem do tempo

    for (Object& obj : dataset) {
        obj.distance = euclideanDistance(obj.features, queryImageFeatures); // Atualiza a distância no objeto
        if (obj.distance <= range_threshold) {
            range_results.push_back(obj);
        }
    }

    auto stop = std::chrono::high_resolution_clock::now();  // Marca o fim da execução
    std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(stop-start); // Calcula o tempo de execução em milissegundos

    // Exibe os resultados da Range Query - Sem ordenação
    // std::cout << "Range:" << std::endl;
    // for (const Object& obj : range_results) {
    //     std::cout << obj.name << "  " << obj.distance << std::endl;
    // }

    //std::cout << "Tempo: " << duration.count() << " ms" << std::endl;

    //Ordena os resultados do range
    std::sort(range_results.begin(), range_results.end(), [](const Object& a, const Object& b) { // Ordena os resultados por distância
        return a.distance < b.distance; 
    });
    
    // int numberObj = 0;
    std::cout << "" << std::endl;
    for (const Object& obj : range_results) {
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
    //std::cout << "\nNumber images: " << numberObj << std::endl;

    std::cout << "Tempo: " << duration.count() << "ms" << std::endl;
}

void knnQuery(std::vector<Object> dataset, std::vector<double>& queryImageFeatures, int k) {
    std::vector<Object> knn_results;
    
    auto start = std::chrono::high_resolution_clock::now(); // Inicia a contagem do tempo  
    int numberObject = 0;
    for (Object& obj : dataset) {
        obj.distance = euclideanDistance(obj.features, queryImageFeatures); // Atualiza a distância no objeto
        obj.numberObj = numberObject; 
        numberObject++;

        if (knn_results.size() < k) {
            knn_results.push_back(obj);
        } else {
            double max_distance = euclideanDistance(knn_results[k - 1].features, queryImageFeatures);
            if (obj.distance < max_distance) {
                knn_results.pop_back();
                knn_results.push_back(obj);
                std::sort(knn_results.begin(), knn_results.end(), [](const Object& a, const Object& b) { // Ordena os resultados por distância
                    return a.distance < b.distance;
                });
            }

        /* Segundo modo de comparar para obter k-vizinhos mais próximos (Sem Ordenação)
            // Encontre o objeto com a maior distância atual entre os k-NN atuais
            double max_distance = 0;
            int max_index = -1;

            for (int i = 0; i < k; ++i) {
                if (knn_results[i].distance > max_distance) {
                    max_distance = knn_results[i].distance;
                    max_index = i;
                }
            }

            // Se a distância do objeto atual é menor que a maior distância atual, substitua a maior distância
            if (obj.distance < max_distance) {
                knn_results[max_index] = obj;
            }
        }
        */
        }

    }
    
    auto stop = std::chrono::high_resolution_clock::now();  // Marca o fim da execução
    std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(stop-start); // Calcula o tempo de execução em milissegundos

    // Exibe os resultados do k-NN

    std::vector<double> array_precision;
    std::vector<double> array_recall;

    int total_true = 0;
    int total_retrieved = 0;


    std::cout << "" << std::endl;
    int i = 0;
    for (const Object& obj : knn_results) {
        //std::cout << obj.name << " " << obj.distance << std::endl;
    
        total_retrieved++;

        if(obj.numberObj >= 400 && obj.numberObj < 500){
            total_true++;
        } 

        double precision = static_cast<double> (total_true) / total_retrieved;
        double recall = static_cast<double> (total_true) / 100;

        array_precision.push_back(precision);
        array_recall.push_back(recall);
    }

     int count = 0 ;
    // for (const double value : array_precision) {
    //     std::cout << value<< std::endl;
    //     std::cout << "count" << count<< std::endl;
    //     count++;
    // }


        for (const double value : array_recall) {
        std::cout << value<< std::endl;
        std::cout << "count" << count<< std::endl;
        count++;
    }


    std::cout << "Tempo: " << duration.count() << "ms" << std::endl;
}

int main() {
    // Nome do arquivo de características
    std::string filename = "zernike-result.txt";

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Não foi possível abrir o arquivo." << std::endl;
        return 1;
    }

    std::vector<Object> dataset;
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream iss(line); // Cria um stream de string a partir da linha
        std::string imageName; 
        double featureValue; 

        iss >> featureValue; // Leitura do valor (primeira posição), que não será utilizado.
        iss >> imageName;  // Leitura do nome da imagem (segunda posição).

        std::vector<double> features;
        while (iss >> featureValue) { // Leitura dos valores das características (demais posições)
            features.push_back(featureValue);
        }

        dataset.push_back(Object(imageName, features));
    }

    int queryImageNumber = 488;

    std::vector<double>& queryImageFeatures = dataset[queryImageNumber].features; // Features da imagem de consulta


    // for(int i = 400; i < 500; i++){
    //     knnQuery(dataset, dataset[i].features, 100);
    // }

    // rangeQuery(dataset, queryImageFeatures, 0.452020);
    // rangeQuery(dataset, queryImageFeatures, 0.509368);
    // rangeQuery(dataset, queryImageFeatures, 0.557044);

     knnQuery(dataset, queryImageFeatures, 100); 
    // knnQuery(dataset, queryImageFeatures, 16);
    // knnQuery(dataset, queryImageFeatures, 21); 

    return 0;
}
