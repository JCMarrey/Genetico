#ifndef INDIVIDUO_H
#define INDIVIDUO_H

#include "ProblemaOptim.h"
#include <vector>
#include <iostream>
#include <random>

using namespace std;
using Range = pair<double, double>;

class Individuo {
public:
   Individuo(ProblemaOptim* p, int precision);
   Individuo();

   void insuflar(ProblemaOptim* p, int precision);
   void decodificar();
   void x2cromosoma(ProblemaOptim* p);
   void copiar(Individuo* source);
   void imprimeIndi(ostream& salida);

   vector<unsigned> cromo;     /* cadena binaria del cromosoma del individuo. */
   int chromoSize;             /* longitud del cromosoma del individuo */
   vector<unsigned> geneSizes; /* longitud de cada gene del cromosoma. */
   vector<double> x;           /* Decodificación del cromosoma (fenotipo) */
   vector<Range> xRanges;      /* intervalo de las variables. */
   double    aptitud;    /* aptitud del individuo */
   double    eval;       /* resultado de evaluar la función objetivo */
   vector<double> cons;  /* valor de cada una de las restricciones que pueda tener el problema a resolver */
   double    valesp;     /* valor esperado de la aptitud del individuo */
   int       xsite1;     /* punto de cruza */
   int       nMutaciones;   /* inidica si se realizó una mutación */
   int       padres[2];   /* los padres del individuo */

private:
   static mt19937 twister;     // Mersenne-Twister random number engine
};

#endif
