#ifndef GENETICO_H
#define GENETICO_H

/*
 *genetico.h
 * Contiene la declaración de la clase GeneticoSimple que define la
 * la interfaz de un algoritmo genético simple con:
 *  - representación binaria.
 *  - cruza de un punto.
 *  - mutación uniforme.
 *  - selección por ruleta.
 */

#include "Individuo.h"
#include "ProblemaOptim.h"
#include "Estadisticas.h"
#include <fstream>
#include <vector>
#include <random>
#include <sys/stat.h>

#define RAIZ 0

using namespace std;
using Cromosoma = vector<unsigned int>;


typedef struct {
   int popSize;                /* tamaño de la población */
   int    Gmax;                /* número máximo de generaciones */
   double  Pc;                 /* porcentaje de cruza */
   double  Pm;                 /* porcentaje de mutación */
   int precision;              /* dígitos de precisión */
   int nMigrantes;             /* número de emigrantes/inmigrantes en cada migración */
   int tamEpoca;               /* número de generaciones entre cada migración */
} ParamsGA;


class GeneticoSimple {
public:
   GeneticoSimple(ProblemaOptim*, ParamsGA&);
   ~GeneticoSimple();
   void optimizar();

private:
   
   void seleccionaIndividuos();

   void migracion(Individuo* pop);
   void elegidos(vector<int>& elegidos, int nMigrantes);
   void unionPoblaciones(Individuo* pop);
   
   void inicalizarPob();
   void reporteInicial();
   void evaluarPoblacion(Individuo*);
   double aleatorio();
   void seleccionPadres(Individuo*);
   int seleccionRuleta(Individuo*);
   void mutacion(Individuo*);
   int mutacionUniforme(Cromosoma&);
   void cruza(Individuo*, Individuo*);
   int cruza1Punto(Cromosoma&, Cromosoma&, Cromosoma&, Cromosoma&);
   int calcularTamCromosoma();
   void calcularValEsperado(Individuo*);
   void elitismo(Individuo* pop, int g);
   int flip(double prob);
   string nombreArchivoPesos(int i) const;
   string nombreArchivoEvals(int i) const;

   ProblemaOptim* problema;    /* el problema que se resolverá */
   Individuo* oldpop;         /* última generación de individuos */
   Individuo* newpop;         /* siguiente generación de individuos */
   Individuo* globalpop;
   vector<int> padres;         /* lista de padres seleccionados. */

   int popSize;                /* tamaño de la población */
   int nMigrantes;
   int    Gmax;                /* número máximo de generaciones */
   int tamEpoca;
   double  Pc;                 /* porcentaje de cruza */
   double  Pm;                 /* porcentaje de mutación */
   int precision;              /* dígitos de precisión */

   int chromoSize;             /* longitud del cromosoma de caa individuo */
   int geneSize;               /* longitud de cada gene en el cromosoma */
   double sumAptitud;          /* suma de la aptitud de toda la población */
   double sumvalesp;           /* suma de los valores esperados de la población */
   int    gen;                 /* número de la generación actual */
   EstadisticasGA stats;
   ofstream archVariables;
   ofstream archEvaluacion;
   string patronArchivoVars;
   string patronArchivoEval;


   mt19937 rng;                // Mersenne-Twister random number engine
   
   double tiempoMaximo;        // PEOR tiempo en terminar la carrera en TORCS.

   /* Para la migración */
   vector <int> elegidos;
   char* buffer;
   int bufSize;
   int myRank;
   int nPro;
   int numIslas;
};


#endif
