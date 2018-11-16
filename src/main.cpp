#include "readData.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include "Util.h"

using namespace std;

double **matrizAdj; // matriz de adjacencia
int dimension;      // quantidade total de vertices
int tamanhoSolucao;
const int iteracoesMaxima = 50;
int counterSwaps[iteracoesMaxima] = {0}, counterReinsertion[iteracoesMaxima] = {0}, counterReinsertion_2[iteracoesMaxima] = {0}, counterReinsertion_3[iteracoesMaxima] = {0}, counterTwo_Opt[iteracoesMaxima] = {0}, counterDoubleBridge[iteracoesMaxima] = {0}; 
double tempoTotalSwap = 0, tempoTotalReinsertion = 0, tempoTotalReinsertion_2 = 0, tempoTotalReinsertion_3 = 0, tempoTotalTwo_Opt = 0;
  
struct tLocais
{
  int distancia;
  int i;
  int localInsercao;
};

int GenerateRandomNumber(int tamanho);
void printData();
void MelhorInsercao(vector<int> &solucao, int escolhido, vector<tLocais> &melhorDistancia, vector<double> &conjuntoLocais);
void ExcluirValorEscolhido(vector<double> &conjuntoDeLocais, int localInsercao);
void InsercaoMaisBarata(vector<double> &conjuntoDeLocais, vector<int> &solucao, vector<tLocais> &melhorCaminho);
void Limitar_Variacoes_Dos_Indices(int &indiceInicial, int &indiceFinal);
void EscreverResultadosNosArquivos(fstream &File, int * counters, int iteracoesMaxima,  char * Nomearquivo);
bool Ordena(tLocais a, tLocais b);
double Algoritmo_RVND(vector<int> &solucao, double distancia, int interacaoNoMomento);
double Reinsertion(vector<int> &solucao, double distancia, int tamanho);
double Swap(vector<int> &solucao, double distancia);
double Two_OPT(vector<int> &solucao, double distancia);
double DoubleBridge_Pertubation(vector<int> &solucao, double distancia);

//+++++++++++++++++++++++++++++++++++++++++ MAIN +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int main(int argc, char **argv)
{
  readData(argc, argv, &dimension, &matrizAdj);
  //printData();
  tamanhoSolucao = dimension+1;
  srand((unsigned)time(0));

  vector<int> solucaoFinal;
  int custoFinal = 10000000;

  fstream fileSwap, fileReinsertion, fileReinsertion_2, fileReinsertion_3, fileTwo_Opt, fileDoubleBridge;

  
  double tempo_inicial_TSP = cpuTime();

  for (int i = 0; i < iteracoesMaxima; i++)
  {
    vector<int> solucao{1, 1};
    vector<double> conjuntoDeLocais;
    vector<tLocais> melhorCaminho;

    int tamanho, escolhido, distancia = 0;

    bool flag = false, usados[solucao.size() + 1] = {};

    for (int j = 2; j <= dimension; j++)
      conjuntoDeLocais.push_back(j);

    int quantidadeDeInsercoesIniciais = 3;

    for (int j = 0; j < quantidadeDeInsercoesIniciais; j++)
    {
      tamanho = conjuntoDeLocais.size();
      escolhido = GenerateRandomNumber(tamanho);
      solucao.emplace(solucao.begin() + 1, conjuntoDeLocais[escolhido - 1]);
      conjuntoDeLocais.erase(conjuntoDeLocais.begin() + escolhido - 1);
    }

    for (int j = 0; j < solucao.size() - 1; j++)
    {
      distancia += matrizAdj[solucao[j]][solucao[j + 1]];
    }

    while (!conjuntoDeLocais.empty())
    {
      InsercaoMaisBarata(conjuntoDeLocais, solucao, melhorCaminho);

      tamanho = melhorCaminho.size();
      int alfa = (rand() % 6);
      int quantidade = ((alfa / 10.0) * tamanho) + 1;
      int escolhaAleatoriaDoVertice = rand() % quantidade;

      sort(melhorCaminho.begin(), melhorCaminho.end(), Ordena);

      solucao.emplace(solucao.begin() + melhorCaminho[escolhaAleatoriaDoVertice].i, melhorCaminho[escolhaAleatoriaDoVertice].localInsercao);
      distancia += melhorCaminho[escolhaAleatoriaDoVertice].distancia;

      ExcluirValorEscolhido(conjuntoDeLocais, melhorCaminho[escolhaAleatoriaDoVertice].localInsercao);

      melhorCaminho.clear();
    }

    distancia = Algoritmo_RVND(solucao, distancia, i);
    int iterMaxIls = 4 * dimension;
    
    while (iterMaxIls--)
    {
      int novaDistancia = distancia;
      vector<int> copiaSolucao = solucao;

      novaDistancia = DoubleBridge_Pertubation(copiaSolucao, novaDistancia);
      novaDistancia = Algoritmo_RVND(copiaSolucao, novaDistancia, i);

      if (novaDistancia < distancia)
      {
        counterDoubleBridge[i]++;
        iterMaxIls = 4 * dimension;
        distancia = novaDistancia;
        solucao = copiaSolucao;
      }
    }

    if (distancia < custoFinal)
    {
      solucaoFinal = solucao;
      custoFinal = distancia;
    }
  }

  double tempo_final_TSP = cpuTime() - tempo_inicial_TSP;
  
  fileSwap.open("resultadosInstancias/Swap.txt", ios::app);
  EscreverResultadosNosArquivos(fileSwap, counterSwaps, iteracoesMaxima, argv[1]);
  fileSwap.close();
  
  fileReinsertion.open("resultadosInstancias/Reinsertion.txt", ios::app);
  EscreverResultadosNosArquivos(fileReinsertion, counterReinsertion, iteracoesMaxima, argv[1]);
  fileReinsertion.close();
  
  fileReinsertion_3.open("resultadosInstancias/Reinsertion_3.txt", ios::app);
  EscreverResultadosNosArquivos(fileReinsertion_3, counterReinsertion_3, iteracoesMaxima, argv[1]);
  fileReinsertion_3.close();
  
  fileReinsertion_2.open("resultadosInstancias/Reinsertion_2.txt", ios::app);
  EscreverResultadosNosArquivos(fileReinsertion_2, counterReinsertion_2, iteracoesMaxima, argv[1]);
  fileReinsertion_2.close();
  
  fileTwo_Opt.open("resultadosInstancias/Two_opt.txt", ios::app);
  EscreverResultadosNosArquivos(fileTwo_Opt, counterTwo_Opt, iteracoesMaxima, argv[1]);
  fileTwo_Opt.close();
  
  fileDoubleBridge.open("resultadosInstancias/DoubleBridge.txt", ios::app);
  EscreverResultadosNosArquivos(fileDoubleBridge, counterDoubleBridge, iteracoesMaxima, argv[1]);
  fileDoubleBridge.close();
  
  // int valor = 0;
  // for (int i = 0; i < dimension; i++)
  // {
  //   valor+= matrizAdj[solucaoFinal[i]][solucaoFinal[i+1]];
  // }
  // cout << endl
  //      << "Valor = " << valor << endl;

  fstream file;
  string nameFile = argv[1];

  int indice = nameFile.find('/');
  int indice2 = nameFile.find('.');

  nameFile = nameFile.substr(indice + 1, indice2);
  nameFile.append(".txt");

  file.open("resultadosInstancias/" + nameFile, ios::app);

  file << "=========================================== " << argv[1] << " =============================================" << endl;

  // for (int i = 0; i < solucaoFinal.size(); i++)
  // {
  //   file << solucaoFinal[i] << " ";
  // }

  file << endl
       << "Distancia = " << custoFinal << endl;
  file << "Tempo Total de Swaps = " << tempoTotalSwap << endl;
  file << "Tempo Total de Or-Opt = " << tempoTotalReinsertion << endl;
  file << "Tempo Total de Or-Opt2 = " << tempoTotalReinsertion_2 << endl;
  file << "Tempo Total de Or-Opt3 = " << tempoTotalReinsertion_3 << endl;
  file << "Tempo Total de Two-Opt = " << tempoTotalTwo_Opt << endl;
  file << endl << "Tempo Total TSP = " << tempo_final_TSP << endl;
  file << "=========================================== "
       << "END"
       << " =============================================" << endl;

  file.close();
  return 0;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ END MAIN ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

double VerificarOMelhor(double distancia, double novaDistancia)
{
  if (distancia < novaDistancia)
    return novaDistancia;
  return distancia;
}

void printData()
{
  cout << "dimension: " << dimension << endl;
  for (size_t i = 1; i <= dimension; i++)
  {
    for (size_t j = 1; j <= dimension; j++)
    {
      cout << matrizAdj[i][j] << " ";
    }
    cout << endl;
  }
}

int GenerateRandomNumber(int tamanho)
{
  return (rand() % tamanho) + 1;
}

bool Ordena(tLocais a, tLocais b)
{
  return a.distancia < b.distancia;
}

void ExcluirValorEscolhido(vector<double> &conjuntoDeLocais, int localInsercao)
{
  for (int i = 0; i < conjuntoDeLocais.size(); i++)
  {
    if (conjuntoDeLocais[i] == localInsercao)
    {
      conjuntoDeLocais.erase(conjuntoDeLocais.begin() + i);
      break;
    }
  }
}

double Swap(vector<int> &solucao, double distancia)
{

  for (int i = 1; i < tamanhoSolucao; i++)
  {
    vector<tLocais> custo;
    double CustoDeRetirarArcoIniciail = -matrizAdj[solucao[i]][solucao[i - 1]];

    for (int j = i + 1; j < dimension; j++)
    {
      double CustoTotalDeSwap;
      if (j != i + 1)
      {
        CustoTotalDeSwap = CustoDeRetirarArcoIniciail - matrizAdj[solucao[i]][solucao[i + 1]];
        CustoTotalDeSwap += -matrizAdj[solucao[j]][solucao[j - 1]] - matrizAdj[solucao[j]][solucao[j + 1]];
        CustoTotalDeSwap += matrizAdj[solucao[i]][solucao[j - 1]] + matrizAdj[solucao[i]][solucao[j + 1]];
        CustoTotalDeSwap += matrizAdj[solucao[j]][solucao[i - 1]] + matrizAdj[solucao[j]][solucao[i + 1]];
      }
      else
      {
        CustoTotalDeSwap = CustoDeRetirarArcoIniciail - matrizAdj[solucao[j]][solucao[j + 1]];
        CustoTotalDeSwap += matrizAdj[solucao[i]][solucao[j + 1]] + matrizAdj[solucao[j]][solucao[i - 1]];
      }

      if (CustoTotalDeSwap < 0)
      {
        tLocais local;
        local.distancia = CustoTotalDeSwap;
        local.i = i;
        local.localInsercao = j;
        custo.push_back(local);
      }
    }

    if (!custo.empty())
    {
      sort(custo.begin(), custo.end(), Ordena);
      distancia = distancia + custo[0].distancia;

      int aux = solucao[custo[0].i];
      solucao[custo[0].i] = solucao[custo[0].localInsercao];
      solucao[custo[0].localInsercao] = aux;
    }
  }

  return distancia;
}

double Reinsertion(vector<int> &solucao, double distancia, int tamanho)
{
  int quantidadeDeIteracoes = tamanhoSolucao - tamanho;

  for (int i = 1; i < quantidadeDeIteracoes; i++)
  {
    vector<tLocais> custo;
    int posicao = i + tamanho - 1;

    double CustoTotalDeTirarArcosIniciais = -matrizAdj[solucao[i - 1]][solucao[i]] - matrizAdj[solucao[posicao]][solucao[i + tamanho]] + matrizAdj[solucao[i + tamanho]][solucao[i - 1]];

    for (int j = i + tamanho; j < quantidadeDeIteracoes; j++)
    {
      double CustoTotalDeReinsercao = CustoTotalDeTirarArcosIniciais - matrizAdj[solucao[j]][solucao[j + 1]];
      CustoTotalDeReinsercao += matrizAdj[solucao[j]][solucao[i]] + matrizAdj[solucao[j + 1]][solucao[posicao]];

      if (CustoTotalDeReinsercao < 0)
      {
        tLocais local;
        local.distancia = CustoTotalDeReinsercao;
        local.i = i;
        local.localInsercao = j;

        custo.push_back(local);
      }
    }

    for (int j = i - tamanho; j > 0; j--)
    {
      if (i + tamanho >= tamanhoSolucao)
        break;

      double CustoTotalDeReinsercao = CustoTotalDeTirarArcosIniciais;
      CustoTotalDeReinsercao += matrizAdj[solucao[j - 1]][solucao[i]] + matrizAdj[solucao[posicao]][solucao[j]] - matrizAdj[solucao[j]][solucao[j - 1]];

      if (CustoTotalDeReinsercao < 0)
      {
        tLocais local;
        local.distancia = CustoTotalDeReinsercao;
        local.i = i;
        local.localInsercao = j - 1;

        custo.push_back(local);
      }
    }

    if (!custo.empty())
    {
      sort(custo.begin(), custo.end(), Ordena);
      distancia = distancia + custo[0].distancia;

      vector<int> aux = solucao;

      solucao.clear();

      int j = 0;

      while (j < tamanhoSolucao){
        if (custo[0].i == j)
          j =  custo[0].i + tamanho;

        if (custo[0].localInsercao+1 == j)
        {
          for (int x = 0; x < tamanho; x++)
          {
            solucao.push_back(aux[custo[0].i + x]);
          }
        }

        solucao.push_back(aux[j]);
        j++;
      } 
    }
  }
  return distancia;
}

void InsercaoMaisBarata(vector<double> &conjuntoDeLocais, vector<int> &solucao, vector<tLocais> &melhorCaminho)
{
  vector<double> auxLocais = conjuntoDeLocais;
  int tamanho, escolhido;

  while (!auxLocais.empty())
  {
    tamanho = auxLocais.size();
    escolhido = GenerateRandomNumber(tamanho);
    MelhorInsercao(solucao, escolhido, melhorCaminho, auxLocais);
    auxLocais.erase(auxLocais.begin() + escolhido - 1);
  }
}

void MelhorInsercao(vector<int> &solucao, int escolhido, vector<tLocais> &melhorDistancia, vector<double> &conjuntoLocais)
{
  tLocais local;
  int distancia;

  for (int i = 1; i < solucao.size(); i++)
  {
    distancia = matrizAdj[solucao[i - 1]][(int)conjuntoLocais[escolhido - 1]] + matrizAdj[solucao[i]][(int)conjuntoLocais[escolhido - 1]] - matrizAdj[solucao[i - 1]][solucao[i]];
    local.distancia = distancia;
    local.i = i;
    local.localInsercao = conjuntoLocais[escolhido - 1];

    melhorDistancia.push_back(local);
  }
}

double Two_OPT(vector<int> &solucao, double distancia)
{

  for (int i = 1; i < dimension; i++)
  {
    vector<tLocais> custo;
    double CustoTotalDeTirarPrimeiroArco = matrizAdj[solucao[i]][solucao[i + 1]];

    for (int j = i + 2; j < dimension; j++)
    {

      double CustoTotalDois_OPT = -CustoTotalDeTirarPrimeiroArco - matrizAdj[solucao[j]][solucao[j + 1]];
      CustoTotalDois_OPT += matrizAdj[solucao[i]][solucao[j]] + matrizAdj[solucao[i + 1]][solucao[j + 1]];

      if (CustoTotalDois_OPT < 0)
      {
        tLocais local;
        local.distancia = CustoTotalDois_OPT;
        local.i = i + 1;
        local.localInsercao = j + 1;
        custo.push_back(local);
      }
    }

    if (!custo.empty())
    {
      sort(custo.begin(), custo.end(), Ordena);
      distancia = distancia + custo[0].distancia;

      vector<int> aux;

      for (int j = custo[0].localInsercao - 1; j >= custo[0].i; j--)
      {
        aux.push_back(solucao[j]);
      }

      swap_ranges(solucao.begin() + custo[0].i, solucao.begin() + custo[0].localInsercao, aux.begin());
    }
  }

  return distancia;
}

double Algoritmo_RVND(vector<int> &solucao, double distancia, int interacaoNoMomento)
{
  vector<string> algoritmos{"swap", "reinsertion", "reinsertion-2", "reinsertion-3", "two_opt"};
  vector<string> copiaAlgoritmos = algoritmos;

  while (1)
  {
    if (copiaAlgoritmos.size() == 0)
      break;

    int escolhaDeAlgoritmoAleatoria = rand() % copiaAlgoritmos.size();
    double novaDistancia;

    if (algoritmos[escolhaDeAlgoritmoAleatoria] == "swap")
    {
      double startTimeSwap = cpuTime();

      novaDistancia = Swap(solucao, distancia);

      double finalTimeSwap = cpuTime() - startTimeSwap;
      tempoTotalSwap += finalTimeSwap;
    }
    else if (algoritmos[escolhaDeAlgoritmoAleatoria] == "reinsertion")
    {
      int tamanhoDeBlocosParaReinsercao = 1;
      double startTimeReinsertion = cpuTime();

      novaDistancia = Reinsertion(solucao, distancia, tamanhoDeBlocosParaReinsercao);

      double finalTimeReinsertion = cpuTime() - startTimeReinsertion;
      tempoTotalReinsertion += finalTimeReinsertion;
    }
    else if (algoritmos[escolhaDeAlgoritmoAleatoria] == "reinsertion-2")
    {
      int tamanhoDeBlocosParaReinsercao = 2;
      double startTimeReinsertion_2 = cpuTime();

      novaDistancia = Reinsertion(solucao, distancia, tamanhoDeBlocosParaReinsercao);

      double finalTimeReinsertion_2 = cpuTime() - startTimeReinsertion_2;
      tempoTotalReinsertion_2 += finalTimeReinsertion_2;
    }
    else if (algoritmos[escolhaDeAlgoritmoAleatoria] == "reinsertion-3")
    {
      int tamanhoDeBlocosParaReinsercao = 3;
      double startTimeReinsertion_3 = cpuTime();

      novaDistancia = Reinsertion(solucao, distancia, tamanhoDeBlocosParaReinsercao);

      double finalTimeReinsertion_3 = cpuTime() - startTimeReinsertion_3;
      tempoTotalReinsertion_3 += finalTimeReinsertion_3;
    }
    else if (algoritmos[escolhaDeAlgoritmoAleatoria] == "two_opt")
    {
      double startTimeTwo_Opt = cpuTime();

      novaDistancia = Two_OPT(solucao, distancia);

      double finalTimeTwo_Opt = cpuTime() - startTimeTwo_Opt;
      tempoTotalTwo_Opt += finalTimeTwo_Opt;
    }

    if (distancia > novaDistancia)
    {

      if (algoritmos[escolhaDeAlgoritmoAleatoria] == "swap")
        counterSwaps[interacaoNoMomento]++;
      else if (algoritmos[escolhaDeAlgoritmoAleatoria] == "reinsertion")
        counterReinsertion[interacaoNoMomento]++;
      else if (algoritmos[escolhaDeAlgoritmoAleatoria] == "reinsertion-2")
        counterReinsertion_2[interacaoNoMomento]++;
      else if (algoritmos[escolhaDeAlgoritmoAleatoria] == "reinsertion-3")
        counterReinsertion_3[interacaoNoMomento]++;
      else if (algoritmos[escolhaDeAlgoritmoAleatoria] == "two_opt")
        counterTwo_Opt[interacaoNoMomento]++;

      distancia = novaDistancia;
      copiaAlgoritmos = algoritmos;
    }
    else
    {
      copiaAlgoritmos.erase(copiaAlgoritmos.begin() + escolhaDeAlgoritmoAleatoria);
    }
  }

  return distancia;
}

double DoubleBridge_Pertubation(vector<int> &solucao, double distancia)
{

  vector<int> copiaDaSolucao;
  int indiceInicial1 = (rand() % (dimension - 1)) + 1;
  int indiceFinal1 = (rand() % (dimension - 1)) + 1;
  int indiceInicial2 = (rand() % (dimension - 1)) + 1;
  int indiceFinal2 = (rand() % (dimension - 1)) + 1;

  Limitar_Variacoes_Dos_Indices(indiceInicial1, indiceFinal1);
  Limitar_Variacoes_Dos_Indices(indiceInicial2, indiceFinal2);

  if (indiceInicial1 <= indiceInicial2 && indiceInicial2 <= indiceFinal1)
  {
    int quantidadeDeVerticesContidosNoPrimeiroArco = (indiceFinal1 - indiceInicial1);

    if (indiceInicial2 + quantidadeDeVerticesContidosNoPrimeiroArco + 1 >= dimension)
    {

      indiceInicial2 -= quantidadeDeVerticesContidosNoPrimeiroArco + 1;
      indiceFinal2 -= quantidadeDeVerticesContidosNoPrimeiroArco + 1;

      if (indiceInicial1 <= indiceFinal2 && indiceFinal2 <= indiceFinal1)
        indiceFinal2 -= (indiceFinal2 - indiceInicial1 + 1);
    }
    else
    {
      indiceInicial2 += quantidadeDeVerticesContidosNoPrimeiroArco + 1;
      indiceFinal2 += quantidadeDeVerticesContidosNoPrimeiroArco + 1;

      if (indiceFinal2 >= dimension)
        indiceFinal2 = dimension - 1;
    }
  }
  else if (indiceInicial1 <= indiceFinal2 && indiceInicial1 > indiceInicial2)
  {
    int quantidadeDeVerticesEntreIndiceInicial1_IndiceFinal2 = indiceFinal2 - indiceInicial1;
    indiceFinal2 -= (quantidadeDeVerticesEntreIndiceInicial1_IndiceFinal2 + 1);
  }

  if (indiceFinal1 + 1 != indiceInicial2 && indiceInicial1 != indiceFinal2 + 1)
  {

    distancia -= (matrizAdj[solucao[indiceFinal2 + 1]][solucao[indiceFinal2]] + matrizAdj[solucao[indiceInicial2]][solucao[indiceInicial2 - 1]]);
    distancia -= (matrizAdj[solucao[indiceFinal1 + 1]][solucao[indiceFinal1]] + matrizAdj[solucao[indiceInicial1]][solucao[indiceInicial1 - 1]]);
    distancia += (matrizAdj[solucao[indiceFinal1 + 1]][solucao[indiceFinal2]] + matrizAdj[solucao[indiceInicial1 - 1]][solucao[indiceInicial2]]);
    distancia += (matrizAdj[solucao[indiceFinal2 + 1]][solucao[indiceFinal1]] + matrizAdj[solucao[indiceInicial2 - 1]][solucao[indiceInicial1]]);
  }
  else
  {
    if (indiceFinal1 + 1 == indiceInicial2)
    {
      distancia -= matrizAdj[solucao[indiceFinal2 + 1]][solucao[indiceFinal2]];
      distancia -= (matrizAdj[solucao[indiceInicial2]][solucao[indiceFinal1]] + matrizAdj[solucao[indiceInicial1]][solucao[indiceInicial1 - 1]]);
      distancia += (matrizAdj[solucao[indiceInicial1]][solucao[indiceFinal2]] + matrizAdj[solucao[indiceInicial1 - 1]][solucao[indiceInicial2]]);
      distancia += matrizAdj[solucao[indiceFinal2 + 1]][solucao[indiceFinal1]];
    }
    else if (indiceFinal2 + 1 == indiceInicial1)
    {
      distancia -= matrizAdj[solucao[indiceInicial2]][solucao[indiceInicial2 - 1]];
      distancia -= (matrizAdj[solucao[indiceFinal1 + 1]][solucao[indiceFinal1]] + matrizAdj[solucao[indiceInicial1]][solucao[indiceFinal2]]);
      distancia += (matrizAdj[solucao[indiceFinal1]][solucao[indiceInicial2]] + matrizAdj[solucao[indiceFinal2]][solucao[indiceFinal1 + 1]]);
      distancia += matrizAdj[solucao[indiceInicial2 - 1]][solucao[indiceInicial1]];
    }
  }

  for (int j = 0; j < tamanhoSolucao; j++)
  {
    if (indiceInicial2 <= j && j <= indiceFinal2)
    {
      for (int x = indiceInicial1; x <= indiceFinal1; x++)
      {
        copiaDaSolucao.push_back(solucao[x]);
      }
      j = indiceFinal2;
    }
    else if (indiceInicial1 <= j && j <= indiceFinal1)
    {
      for (int x = indiceInicial2; x <= indiceFinal2; x++)
      {
        copiaDaSolucao.push_back(solucao[x]);
      }
      j = indiceFinal1;
    }
    else if (!(indiceInicial2 <= j && j <= indiceFinal2) && !(indiceInicial1 <= j && j <= indiceFinal1))
      copiaDaSolucao.push_back(solucao[j]);
  }

  solucao = copiaDaSolucao;
  return distancia;
}

void Limitar_Variacoes_Dos_Indices(int &indiceInicial, int &indiceFinal)
{
  if (indiceFinal < indiceInicial)
  {
    int aux = indiceFinal;
    indiceFinal = indiceInicial;
    indiceInicial = aux;
  }
  if ((indiceFinal - indiceInicial) > dimension / 4)
    indiceFinal = indiceInicial + (dimension / 4);
}

void EscreverResultadosNosArquivos(fstream &File, int * counters,int iteracoesMaxima, char * NomeArquivo){
  File << "------------------------------------------ " << NomeArquivo << " ------------------------------------------" << endl;
  for(int i = 0; i < iteracoesMaxima; i++){
    File << "Quantidade de Vezes Que Houve Melhora a Cada Iteração " << i << " = " << counters[i] << endl; 
  }
  File << "__________________________________________ "
           << "END"
           << " __________________________________________" << endl;
}