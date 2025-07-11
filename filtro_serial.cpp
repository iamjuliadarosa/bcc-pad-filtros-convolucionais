#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>

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
        // Ignora linhas vazias
        if (linha.empty()) continue;

        // Se a linha começa com "#norm", extrai o fator
        if (linha.rfind("#norm", 0) == 0) {
            istringstream ss(linha.substr(6)); // Pega depois de "#norm "
            ss >> normalizador;
            continue;
        }

        // Caso contrário, é linha de matriz
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

            //output[y][x] = min(max(sum, 0), 255);
        }
    }
    return output;
}

int main() {
    string nomeImagem, nomeFiltro;
    cout << "Nome da imagem (ex: entrada.jpg): ";
    cin >> nomeImagem;
    cout << "Nome do filtro (sharpen,gaussian): ";
    cin >> nomeFiltro;

    string caminhoEntrada = "imagens/grayscale/"+nomeImagem;
    string caminhoKernel = "kernels/" + nomeFiltro + ".txt";
    string caminhoSaida = "imagens/" + nomeFiltro + "/" + nomeImagem;

    Mat imagem = imread(caminhoEntrada, IMREAD_GRAYSCALE);
    if (imagem.empty()) {
        cerr << "Erro ao abrir imagem: " << caminhoEntrada << endl;
        return 1;
    }

    auto[kernel, normalizador] = lerKernel(caminhoKernel);
    if (kernel.empty()) {
        cerr << "Erro ao ler kernel: " << caminhoKernel << endl;
        return 1;
    }

    Image img = matToImage(imagem);
    Image filtrada = aplicarFiltroSerial(img, kernel, normalizador);
    Mat saida = imageToMat(filtrada);

    imwrite(caminhoSaida, saida);
    cout << "Imagem salva em: " << caminhoSaida << endl;
    return 0;
}
