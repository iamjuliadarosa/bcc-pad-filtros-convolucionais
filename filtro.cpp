#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <omp.h>

using namespace std;
using namespace cv;
namespace fs = std::filesystem;

using Image = vector<vector<int>>;
using Kernel = vector<vector<int>>;

Image matToImage(const Mat& mat) {
    int h = mat.rows, w = mat.cols;
    Image img(h, vector<int>(w));
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
            img[i][j] = mat.at<uchar>(i, j);
    return img;
}

Mat imageToMat(const Image& img) {
    int h = img.size(), w = img[0].size();
    Mat mat(h, w, CV_8UC1);
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
            mat.at<uchar>(i, j) = img[i][j];
    return mat;
}

pair<vector<vector<int>>, int> lerKernel(const string& caminho) {
    vector<vector<int>> kernel;
    int normalizador = 1;

    ifstream file(caminho);
    string linha;
    while (getline(file, linha)) {
        if (linha.empty()) continue;

        if (linha.rfind("#norm", 0) == 0) {
            istringstream ss(linha.substr(6));
            ss >> normalizador;
            continue;
        }

        istringstream ss(linha);
        vector<int> row;
        int valor;
        while (ss >> valor)
            row.push_back(valor);
        kernel.push_back(row);
    }

    return {kernel, normalizador};
}

Image aplicarFiltroSerial(const Image& input, const Kernel& kernel, int normalizador) {
    int h = input.size(), w = input[0].size();
    int k = kernel.size();
    int offset = k / 2;
    Image output(h, vector<int>(w, 0));

    for (int y = offset; y < h - offset; y++) {
        for (int x = offset; x < w - offset; x++) {
            int sum = 0;
            for (int i = 0; i < k; i++) {
                for (int j = 0; j < k; j++) {
                    sum += input[y + i - offset][x + j - offset] * kernel[i][j];
                }
            }
            int valor = sum / normalizador;
            output[y][x] = min(max(valor, 0), 255);
        }
    }
    return output;
}
Image aplicarFiltroParallel(const Image& input, const Kernel& kernel, int normalizador) {
    int h = input.size(), w = input[0].size();
    int k = kernel.size();
    int offset = k / 2;
    Image output(h, vector<int>(w, 0));

    #pragma omp parallel for collapse(2)
    for (int y = offset; y < h - offset; y++) {
        for (int x = offset; x < w - offset; x++) {
            int sum = 0;
            for (int i = 0; i < k; i++) {
                for (int j = 0; j < k; j++) {
                    sum += input[y + i - offset][x + j - offset] * kernel[i][j];
                }
            }
            int valor = sum / normalizador;
            output[y][x] = min(max(valor, 0), 255);
        }
    }
    return output;
}

void salvarResultadosCSV(
    const string& caminho,
     const string& imagem,
      const string& filtro,
       double tempoSerial,
        double tempoParalelo,
        int threads,
        double speedup,
        double eficiencia
    ) {
    ofstream file(caminho, ios::app);

    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo de resultados!\n";
        return;
    }

    if (file.tellp() == 0) {
        file << "Imagem,Filtro,Tempo Serial (ms),Tempo Paralelo (ms),Speedup,Threads,Eficiência\n";
    }

    file << imagem << "," 
         << filtro << ","
         << fixed << setprecision(2)
         << tempoSerial << "," 
         << tempoParalelo << "," 
         << speedup << "," 
         << threads << "," 
         << eficiencia
         << "\n";

    file.close();
}

int main() {
    string nomeImagem, nomeFiltro;

    cout << "Digite o nome da imagem (sem extensão): ";
    cin >> nomeImagem;

    cout << "Digite o nome do filtro (sem extensão): ";
    cin >> nomeFiltro;

    string caminhoImagem = "imagens/grayscale/" + nomeImagem + ".jpg";
    string caminhoFiltro = "kernels/" + nomeFiltro+".txt";

    Mat imagemOriginal = imread(caminhoImagem, IMREAD_GRAYSCALE);
    if (imagemOriginal.empty()) {
        cerr << "Erro ao carregar a imagem: " << caminhoImagem << endl;
        return 1;
    }

    auto input = matToImage(imagemOriginal);
    auto[kernel, norm] = lerKernel(caminhoFiltro);
    
    cout << "\nAplicando filtro serial..." << endl;
    auto startSerial = chrono::high_resolution_clock::now();
    auto serial = aplicarFiltroSerial(input, kernel, norm);
    auto endSerial = chrono::high_resolution_clock::now();
    auto tempoSerial = chrono::duration<double, milli>(endSerial - startSerial).count();
    imwrite("imagens/" + nomeFiltro + "/" + "serial_"+nomeImagem + ".jpg", imageToMat(serial));

    input = matToImage(imagemOriginal);

    cout << "Aplicando filtro paralelo..." << endl;
    auto startPar = chrono::high_resolution_clock::now();
    auto paralelo = aplicarFiltroParallel(input, kernel, norm);
    auto endPar = chrono::high_resolution_clock::now();
    auto tempoParalelo = chrono::duration<double, milli>(endPar - startPar).count();

    imwrite("imagens/" + nomeFiltro + "/" + "paralelo_"+nomeImagem + ".jpg", imageToMat(paralelo));

    int numThreads = omp_get_max_threads();
    auto speedup = tempoSerial / tempoParalelo;
    double eficiencia = speedup / numThreads;

    salvarResultadosCSV("resultados.csv",
         nomeImagem,
         nomeFiltro,
         tempoSerial,
         tempoParalelo,
         numThreads,
         speedup,
         eficiencia
    );
    
    cout << "\nTempo Serial:   " << tempoSerial   << " ms" << endl;
    cout << "Tempo Paralelo: " << tempoParalelo << " ms" << endl;
    cout << "Speedup:        " << speedup << "x" << endl;
    cout << "Eficiencia:        " << eficiencia << "\n" << endl;

    return 0;
}