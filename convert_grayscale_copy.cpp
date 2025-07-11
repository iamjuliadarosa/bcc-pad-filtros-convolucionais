#include <opencv2/opencv.hpp>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;
using namespace cv;
using namespace std;

int main() {
    string pastaEntrada = "imagens/originais/";
    string pastaSaida = "imagens/grayscale/";
    string nomeImagem = "04.jpg";

    cout << "Digite o nome da imagem jpg localizada em 'imagens/originais/' para conversão para escala cinza (ex: 'exemplo.jpg'):";
    cin >> nomeImagem;

    string caminhoEntrada = pastaEntrada + nomeImagem;
    string caminhoSaida = pastaSaida + nomeImagem;

    Mat imagem = imread(caminhoEntrada, IMREAD_GRAYSCALE);
    if (imagem.empty()) {
        cerr << "Erro ao ler imagem: " << caminhoEntrada << endl;
        return 1;
    }

    if (!imwrite(caminhoSaida, imagem)) {
        cerr << "Erro ao salvar imagem: " << caminhoSaida << endl;
        return 1;
    }

    cout << "Imagem copiada com sucesso para: " << caminhoSaida << endl;
    return 0;
}