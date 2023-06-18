// Compilar con:

/* INSTALAR LA BIBLIOTECA fmt PARA PODER COMPILAR SIN PROBLEMAS. */


// VERSION SECUENCIAL:
// $> g++ -I./problemas/ ./problemas/*.cpp *.cpp -lfmt -o genetico

// VERSION CONCURRENTE CON MPI:
// $> mpic++ -I./problemas/ ./problemas/*.cpp *.cpp -lfmt -o genetico

#include <iostream>
//#include <mpi.h>
#include "GeneticoSimple.h"
#include "TorcsFun.h"
#include "CannonFun.h"

using namespace std;

int main( int argc, char *argv [] )
{
   /*
    * Para la versión secuencial se usa serverID=0, PERO
    * para la versión concurrente serverID debe ser igual a myRank.
    */
    
     MPI_Init(&argc, &argv);
     MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
     MPI_Comm_size(MPI_COMM_WORLD, &nPro);

   int serverID = 0;

   /* Aquí se crea la instancia de TorcsFun con el serverID indicado. */
   ProblemaOptim* problema = new TorcsFun(serverID);

   /* Para usar el problema del Controlador del Auto en TORCS
    * deben descomentar la línea de arriba y comentar la de abajo.
    */

   /* Con esta instancia se resolverá el problema del cañón. */
   //ProblemaOptim* problema = new CannonFun();


   /* La descripción de estos valores está en GeneticoSimple.h */
   ParamsGA pars = { 4, /* popSize  */
                    2, /* Gmax */
                    0.9, /* Pc */
                    0.1, /* Pm */
                      6,  /* precision */
                     10, /* nMigrantes*/
                     16, /* tamEpoca*/ };


   GeneticoSimple ga(problema, pars);
   ga.optimizar();

   delete problema;
   MPI_Finalize();

   return 0;
}
