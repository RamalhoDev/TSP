#include "readData.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

double **matrizAdj; // matriz de adjacencia
int dimension;      // quantidade total de vertices

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
bool Ordena(tLocais a, tLocais b);
double Reinsertion(vector<int> &solucao, double distancia, int tamanho);
double Swap(vector<int> &solucao, double distancia);
double Two_OPT(vector<int> &solucao, double distancia);
//+++++++++++++++++++++++++++++++++++++++++ MAIN +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int main(int argc, char **argv)
{
  readData(argc, argv, &dimension, &matrizAdj);
  printData();

  srand((unsigned)time(0));

  vector<double> conjuntoDeLocais;
  vector<tLocais> melhorCaminho;

  vector<int> solucao{1, 1};
  int tamanho, escolhido, distancia = 0, i, j;

  bool flag = false, usados[dimension + 1] = {};

  for (int i = 2; i <= dimension; i++)
    conjuntoDeLocais.push_back(i);

  for (int i = 0; i < 3; i++)
  {
    tamanho = conjuntoDeLocais.size();
    escolhido = GenerateRandomNumber(tamanho);
    solucao.emplace(solucao.begin() + 1, conjuntoDeLocais[escolhido - 1]);
    conjuntoDeLocais.erase(conjuntoDeLocais.begin() + escolhido - 1);
  }

  for (int i = 0; i < solucao.size() - 1; i++)
  {
    distancia += matrizAdj[solucao[i]][solucao[i + 1]];
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


  // int valor = 0;
  // for (i = 0; i < solucao.size()-1; i++)
  // {
  //   valor+= matrizAdj[ solucao[i]][solucao[i+1]];
  // }
  // cout << endl
  //      << "Valor = " << valor << endl;


  for (i = 0; i < solucao.size(); i++)
  {
    cout << solucao[i] << " ";
  }

  cout << endl
       << "Distancia = " << distancia << endl;

  return 0;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ END MAIN ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

double VerificarOMelhor(double distancia, double novaDistancia){
  if(distancia < novaDistancia)
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
  srand((unsigned)time(0));
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
  for (int i = 1; i < solucao.size(); i++)
  {
    vector<tLocais> custo;
    double CustoDeRetirarArcoIniciail = - matrizAdj[solucao[i]][solucao[i - 1]];

    for (int j = i + 1; j < solucao.size() - 1; j++)
    {
      double CustoTotalDeSwap;
      if (j != i + 1)
      {
        CustoTotalDeSwap =  CustoDeRetirarArcoIniciail - matrizAdj[solucao[i]][solucao[i + 1]];
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
  for (int i = 1; i < solucao.size() - 1; i++)
  {
    vector<tLocais> custo;
    int posicao = i + tamanho - 1;
    double CustoTotalDeTirarArcosIniciais = - matrizAdj[solucao[i - 1]][solucao[i]] - matrizAdj[solucao[posicao]][solucao[i + tamanho]] + matrizAdj[solucao[i + tamanho]][solucao[i - 1]];
    
    for (int j = i + tamanho; j < solucao.size() - tamanho; j++)
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
      if (i + tamanho >= solucao.size())
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

      bool flag = true;

      for (int j = 0; j < aux.size(); j++)
      {

        if (custo[0].i <= j && custo[0].i + tamanho > j)
        {
          continue;
        }

        if (custo[0].localInsercao < j && flag)
        {

          for (int x = 0; x < tamanho; x++)
          {
            solucao.push_back(aux[custo[0].i + x]);
          }
          flag = false;
        }
        solucao.push_back(aux[j]);
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

double Two_OPT(vector<int> &solucao, double distancia){
  for (int i = 1; i < solucao.size()-1; i++)           
  {
    vector<tLocais> custo;
    double CustoTotalDeTirarPrimeiroArco = matrizAdj[solucao[i]][solucao[i+1]];

    for (int j = i + 2; j < solucao.size()-1; j++)
    {
      
      double CustoTotalDois_OPT = -CustoTotalDeTirarPrimeiroArco  - matrizAdj[solucao[j]][solucao[j+1]];
      CustoTotalDois_OPT += matrizAdj[solucao[i]][solucao[j]] + matrizAdj[solucao[i+1]][solucao[j+1]];
      
      if (CustoTotalDois_OPT < 0)
      {
        tLocais local;
        local.distancia = CustoTotalDois_OPT;
        local.i = i+1;
        local.localInsercao = j+1;
        custo.push_back(local);
      }
    }

    if (!custo.empty())
    {

      sort(custo.begin(), custo.end(), Ordena);
      distancia = distancia + custo[0].distancia;

      vector<int> aux;

      for(int j = custo[0].localInsercao-1; j >= custo[0].i; j--)
      {
        aux.push_back(solucao[j]);
      }

      swap_ranges(solucao.begin()+custo[0].i, solucao.begin()+custo[0].localInsercao, aux.begin());
    }
  }

  return distancia;
}

