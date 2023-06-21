// Compilar con:

/* INSTALAR LA BIBLIOTECA fmt PARA PODER COMPILAR SIN PROBLEMAS. */


// VERSION SECUENCIAL:
// $> g++ -I./problemas/ ./problemas/*.cpp *.cpp -lfmt -o genetico

// VERSION CONCURRENTE CON MPI:
// $> mpic++ -I./problemas/ ./problemas/*.cpp *.cpp -lfmt -o genetico

#include <iostream>
#include <mpi.h> //modificar el mpi
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

      
    
     int serverID;
     MPI_Init(&argc, &argv);
  

    //invocar my rank
    MPI_Comm_rank(MPI_COMM_WORLD,  &serverID);
    //serverID = 0;
   /* Aquí se crea la instancia de TorcsFun con el serverID indicado. */
   ProblemaOptim* problema = new TorcsFun(serverID);

   /* Para usar el problema del Controlador del Auto en TORCS
    * deben descomentar la línea de arriba y comentar la de abajo.
    */

   /* Con esta instancia se resolverá el problema del cañón. */
   //ProblemaOptim* problema = new CannonFun();


   /* La descripción de estos valores está en GeneticoSimple.h */
   //12 y 18 individuos
   ParamsGA pars = { 10, /* popSize  */
                    2, /* Gmax */
                    0.9, /* Pc */
                    0.1, /* Pm */
                      6,  /* precision */
                     4, /* nMigrantes*/
                     3, /* tamEpoca*/ };

   GeneticoSimple ga(problema, pars);
   ga.optimizar();

   delete problema;
   MPI_Finalize();

   return 0;
}


