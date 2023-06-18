/*
* GeneticoSimple.cpp
* Implemetación de un algoritmo genético simple con representación binaria,
* selección proporcional de ruleta, cruza en un punto, y mutación uniforme.
*/

#include <ctime>
#include <cmath>
#include <limits>
#include "GeneticoSimple.h"
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <mpi.h>


GeneticoSimple::GeneticoSimple(ProblemaOptim* p, ParamsGA& params) {
   problema = p;
   popSize = params.popSize;
   Gmax = params.Gmax;
   Pc = params.Pc;                                                      
   Pm = params.Pm;         
   precision = params.precision;
   nMigrantes = params.nMigrantes; 
   tamEpoca = parmas.tamEpoca;  
    
   random_device rd;
   rng.seed(rd());

   oldpop = new Individuo[popSize];
   newpop = new Individuo[popSize];
   padres.assign(popSize, 0);
   stats.reset(problema, precision);

   tiempoMaximo = 0.0;  // el PEOR tiempo en terminar la carrera.

   // Crear directorio de salida, si aún no existe.
   /*** En la versión CONCURRENTE/MPI solamente la isla RAIZ debería crear el directorio de salida. ***/
   if(myRank == 0){
      if ( !stats.dirExists("./salidafinal/") ) {
      const int dir_err = mkdir("./salidafinal/", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
      if (dir_err == -1 ) {
         cerr << "\nNo se pudo crear el directorio de salida '/salidafinal/" << "\n\n";
         exit(1);
      }
   }
   }
   
   /*** En la versión CONCURRENTE/MPI, todas las islas deben esperar la creación del directorio de salida con Barrier. ***/
}

GeneticoSimple::~GeneticoSimple() {
   delete[] newpop;
   delete[] oldpop;
   //if (myRank == RAIZ)
   //   delete[] globalpop;
}

void GeneticoSimple::optimizar()
{
   Individuo* temp;

   /* Inicializar la población y reportar las estadísticas iniciales */
   gen = 1; /* La generación aleatoria es la primera. */
   stats.initial_report(cout, popSize, Gmax, Pc, Pm);
   
   inicalizarPob();
      
   evaluarPoblacion(oldpop);
   elitismo(oldpop, gen);
   stats.statistics(oldpop, popSize);

   for (gen=2; gen <= Gmax; gen++) {
      /* Seleccionar los padres guiados por la aptitud. */
      seleccionPadres(oldpop);

      /* Cruzar los pares de padres para producir la población de hijos. */
      cruza(oldpop, newpop);

      /* Mutar a los hijos según Pm. */
      mutacion(newpop);

      /* Evaluar la nueva generación */
      evaluarPoblacion(newpop);
      elitismo(newpop, gen); // Encontrar el mejor individuo.

      /* Calcular las estadísticas sobre la aptitud en la nueva generación */
      stats.statistics(newpop, popSize);

      /* Imprimir los resultados de las estadísticas */
      //stats.report(cout, oldpop, newpop, popSize, gen);  // Esto imprime TODA la información.    
      stats.shortReport(cout, oldpop, newpop, popSize, gen);

      /* Ahora, la nueva generación será la vieja */
      temp = oldpop;
      oldpop = newpop;
      newpop = temp;
   }


   // Para dejar las variables (PESOS) de la población final en este archivo.
   // ***salidafinal*** debe estar donde corren este programa.
   ofstream archVariables("./salidafinal/pesos_pob.txt", std::ofstream::out);

   // Para dejar la evaluación (tiempo y distancia restante) de la población.
   ofstream archEvaluacion("./salidafinal/evals_pob.txt", std::ofstream::out);
   
   stats.writeVariables(archVariables, oldpop, popSize);
   stats.writeEvaluation(archEvaluacion, oldpop, popSize);
   archVariables.close();
   archEvaluacion.close();
}

/* Evaluación de cada uno de los popsize individuos */
void GeneticoSimple::evaluarPoblacion(Individuo* pop)
{

   for (int i = 0; i < popSize; ++i)
   {
      problema->evaluateFun( pop[i].x, pop[i].eval, pop[i].cons );

      stats.nevals++;

      /*** Para considerar una restricción en el problema que se resuelve
       * eval: tiempo de carrera.
       * cons[0]: distancia para terminar la carrera.
       */

      /*** tiempoMaximo debe agregarse a la clase e inciarse en CERO en el contructor ***/

      // Actualizar el tiempo máximo
      if (tiempoMaximo < pop[i].eval)
         tiempoMaximo = pop[i].eval;

      if ( pop[i].eval == 0.0 )  // el tiempo es 0, NO terminó el auto.
         pop[i].aptitud = 1 / (pop[i].cons[0] + tiempoMaximo);
      else                       // el auto sí terminó.
         pop[i].aptitud = 1 / pop[i].eval;
   }
}


/* 
 *  En la versión CONCURRENTE/MPI, deben iniciar el arreglo globalpop de 
 *  manera parecida a como se inicia oldpop o newpop.
 *
 *  El método insuflar reserva espacio para el vector x, cons, y el cromosoma.
 *
 */

/* Creación aleatoria de la población inicial */
void GeneticoSimple::inicalizarPob()
{
   for (int j=0; j < popSize; j++) {
      oldpop[j].insuflar(problema, precision);
      newpop[j].insuflar(problema, precision);
   }

   Pm = 1.0 / oldpop[0].chromoSize;
}


/* Acá deben podrían poner sus métodos nuevos para la versión CONCURRENTE/MPI */


/*
 **************************************************************
 * LOS MÉTODOS QUE ESTÁN ABAJO YO NO NECESITAN MODIFICARLOS
 *
 *
 **/












/* Sección de los padres según su aptitud. */
void GeneticoSimple::seleccionPadres(Individuo* pop) {
   calcularValEsperado(pop);

   for (int i = 0; i < popSize; i++)
      padres[i] = seleccionRuleta(pop);
}

/* El algoritmo de selección proporcional de Ruleta */
int GeneticoSimple::seleccionRuleta(Individuo* pop) {
   double suma, ale;
   int j;
   uniform_real_distribution<> rdis(0.0, sumvalesp);

   ale = rdis(rng);
   suma = 0.0;
   j = 0;

   do {
      suma += pop[j].valesp;
      j++;
   } while ( suma < ale  &&  j < popSize );

   return j-1;
}

/* Crear la nueva generación por medio de selección, cruza y mutación */
void GeneticoSimple::cruza(Individuo* oldpop, Individuo* newpop)
{
   int mate1, mate2, pcruza;

   // En cada iteración cruzar dos padres y mutar los 2 hijos
   for (int j = 0; j < popSize-1; j += 2) // <--- j avanza de 2 en 2.
   {
      mate1 = padres[j];   // índice del padre 1
      mate2 = padres[j+1]; // índice del padre 2

      /* Se efectúa la cruza para producir dos nuevos individuos */
      pcruza = cruza1Punto(oldpop[mate1].cromo, oldpop[mate2].cromo,
                           newpop[j].cromo, newpop[j+1].cromo);
      newpop[j  ].xsite1 = pcruza;
      newpop[j+1].xsite1 = pcruza;
      newpop[j  ].padres[0] = mate1+1;
      newpop[j  ].padres[1] = mate2+1;
      newpop[j+1].padres[0] = mate1+1;
      newpop[j+1].padres[1] = mate2+1;
   }
}

/* El operador de cruza aplicado a un individuo dado */
int GeneticoSimple::cruza1Punto(Cromosoma& padre1, Cromosoma& padre2,
                                Cromosoma& hijo1,  Cromosoma& hijo2)
{
   int pcruza, j;
   int chromoSize = padre1.size();

   /* Para elegir el punto de cruza entre 0 y chromoSize-1 */
   uniform_int_distribution<int> unif(0, chromoSize - 1);

   /* Realizar la cruza con una probabilidad Pc */
   if( flip(Pc) ) {
      pcruza = unif(rng);
      /* Realizar la cruza */
      for ( j = (chromoSize-1); j >= (chromoSize - pcruza); j-- ) {
         hijo1[j] = padre1[j];
         hijo2[j] = padre2[j];
      }

      for ( j = (chromoSize - pcruza)-1; j >= 0 ; j-- ) {
         hijo1[j] = padre2[j];
         hijo2[j] = padre1[j];
      }

      stats.ncruzas++; /* Guardar el número de cruzas */
   }
   else { /* los padres pasan idénticos a la siguiente generación */
      hijo1 = padre1;
      hijo2 = padre2;
      pcruza = 0;
   }

   return pcruza;
}

void GeneticoSimple::mutacion(Individuo* pop)
{
   // En cada iteración cruzar dos padres y mutar los 2 hijos
   for (int j = 0; j < popSize; j++) {
      /* Mutar (si es el caso) los hijos resultantes de la cruza. */
      pop[j].nMutaciones = mutacionUniforme(pop[j].cromo);
      pop[j].decodificar();

      if (pop[j].nMutaciones > 0)
         stats.nmutaciones++; /* Solamente registra cuántos individuos mutaron. */
   }

   /* Mantener al mejor individuo colocándolo en una posición aleatoria */
   uniform_int_distribution<int> unif(0, popSize - 1);
   int ale = unif(rng);
   pop[ale].copiar(&stats.bestfit);
   stats.positionBestFit = ale+1;
}

/*  El operador de mutación aplicado a un individuo dado */
int GeneticoSimple::mutacionUniforme(Cromosoma& cromo)
{
   int numMutations = 0;
   /* Recorrer todos los bits y mutar si es necesario. */
   for (unsigned k=0; k < cromo.size(); k++) {
      if ( flip(Pm) ) {
         numMutations++;
         cromo[k] = (cromo[k] == 0) ? 1 : 0;
      }
   }

   return numMutations;
}

void GeneticoSimple::calcularValEsperado(Individuo* pop)
{
   /* Sumar la aptitud */
   double sumaptitud = 0.0;
   for (int j=0; j < popSize; j++ )
      sumaptitud = sumaptitud + pop[j].aptitud;

   /* Calcular el promedio de la aptitud */
   stats.avgApt = sumaptitud/popSize;

   /* Calcular la suma del valor esperado de los individuos */
   sumvalesp = 0.0; /* Este valor se usa en la selección por ruleta. */
   for (int j=0; j < popSize; j++) {
      if ( stats.avgApt != 0.0 )
         pop[j].valesp = pop[j].aptitud / stats.avgApt;
      else
         pop[j].valesp = 0.0f;

      sumvalesp += pop[j].valesp;
   }
}

void GeneticoSimple::elitismo(Individuo* pop, int gen)
{
   for (int j=0; j < popSize; j++ )
   {
      /* Probar si se ha encontrado un nuevo máximo GLOBAL */
      if (pop[j].aptitud > stats.bestfit.aptitud) {
         stats.bestfit.copiar(&pop[j]);
         stats.generationBestFit = gen;
         stats.positionBestFit = j+1;
      }
   }
}

/* Lanzamiento de una moneda sesgada - true si cae cara */
int GeneticoSimple::flip(double prob)
{
   uniform_real_distribution<> rdis(0.0, 1.0);

   if( rdis(rng) <= prob)
      return true;
   else
      return false;
}

string GeneticoSimple::nombreArchivoPesos(int i) const {
   char buffer[200];
   sprintf(buffer, patronArchivoVars.c_str(), i);
   return string(buffer);
}

string GeneticoSimple::nombreArchivoEvals(int i) const {
   char buffer[200];
   sprintf(buffer, patronArchivoEval.c_str(), i);
   return string(buffer);
}
