# 🖼️ Filtros Convolucionais em Imagens com C++ e OpenMP

Este projeto demonstra a aplicação de filtros convolucionais em imagens, tanto de forma **serial** quanto **paralela** com **OpenMP**, utilizando **C++** e **OpenCV**.

## 🎯 Objetivos

- Implementar filtros convolucionais clássicos como **Gaussian Blur**, **Sharpen** e **Sobel**.
- Comparar a execução **serial** e **paralela** em tempo de processamento.
- Permitir entrada interativa do usuário: nome da imagem e tipo de filtro.
- Explorar o paralelismo por meio da API **OpenMP**, aproveitando múltiplos núcleos da CPU.

## 🧠 Conceitos Utilizados

- **Convolução em imagens**: aplicação de máscaras (kernels) sobre regiões da imagem.
- **OpenMP**: paralelismo com diretivas em C++, utilizando `#pragma omp`.
- **OpenCV**: biblioteca para leitura, manipulação e escrita de imagens.

## 📁 Estrutura do Projeto

```
.
|── filtro.cpp                 # Versão comparativa entre serial e paralela
├── filtro_serial.cpp          # Versão serial
├── filtro_parallel.cpp        # Versão paralela com OpenMP
├── convert_grayscale_copy.cpp # Converte uma imagem .jpg para sua versão em escala de cinza
├── kernels/                   # Diretório com os kernels em .txt
│   ├── gaussian.txt
│   ├── sharpen.txt
│   └── sobelx.txt
├── imagens/{nome do filtro}/  # Imagens resultantes dos filtros convolucionais
├── imagens/grayscale/         # Imagem tratada para escala cinza
├── imagens/originais/         # Imagem de entrada original
|── LICENSE                  # Documento de licença: MIT License
└── README.md                  # Este documento
```

## ⚙️ Requisitos

- Sistema Linux (ou WSL no Windows)
- C++ Compiler (`g++`)
- [OpenCV 4](https://opencv.org/)
- [OpenMP](https://www.openmp.org) (`libgomp`)

## 🚀 Compilação e Execução

### Versão Serial

```bash
g++ filtro_serial.cpp -o serial `pkg-config --cflags --libs opencv4`
./serial
```

### Versão Paralela (OpenMP)

```bash
g++ filtro_parallel.cpp -o parallel `pkg-config --cflags --libs opencv4` -fopenmp
./parallel
```

### Versão Comparativa
```bash
g++ filtro.cpp -o filtro `pkg-config --cflags --libs opencv4` -fopenmp
./filtro
```

## 📝 Formato dos Arquivos de Kernel

Cada kernel deve estar salvo em um arquivo `.txt` dentro da pasta `kernels/`.

### Exemplo: `kernels/gaussian.txt`

```txt
#norm 16
1 2 1
2 4 2
1 2 1
```

- A linha `#norm 16` indica que o somatório do filtro será dividido por 16.
- Essa normalização é opcional, mas recomendada para filtros como Gaussian para manter o equilibrio do brilho na imagem.

## 🧪 Exemplo de Uso

Ao executar o programa:

1. Digite o nome da imagem localizada na pasta 'imagens/grayscale/' (ex: `01.jpg`)
2. Digite o nome do filtro desejado (ex: `sharpen`)
3. O kernel será lido de `kernels/sharpen.txt`
4. A imagem filtrada será salva como `imagens/sharpen/01.jpg`

## 📚 Referências

- Gonzalez, R. C., & Woods, R. E. (2010). *Processamento Digital de Imagens* (3ª ed.). Pearson Prentice Hall.
- OpenMP Official Docs: https://www.openmp.org
- OpenCV Documentation: https://docs.opencv.org/

## 👩‍💻 Autor

**Julia da Rosa**  
Curso: Ciência da Computação
Disciplina: Programação de Alto Desempenho  
Email: julia.rosa.ifc.riodosul@gmail.com

## 🏁 Licença

Distribuído sob a licença MIT. Uso acadêmico.
