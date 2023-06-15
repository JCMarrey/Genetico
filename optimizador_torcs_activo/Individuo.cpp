#include "Individuo.h"
#include <cmath>
#include <fmt/core.h>
#include <fmt/ranges.h>

mt19937 Individuo::twister{};

Individuo::Individuo(ProblemaOptim* p, int precision) {
   chromoSize = 0;
   aptitud = 0.0;
   eval = 0.0;

   // Variables para llevar estadísticas.
   xsite1 = 0;
   nMutaciones = 0;
   padres[0] = 0;
   padres[1] = 0;

   random_device rd;
   twister.seed(rd());

   insuflar(p, precision);
}

Individuo::Individuo() {
   chromoSize = 0;
   aptitud = 0.0;
   eval = 0.0;

   // Variables para llevar estadísticas.
   xsite1 = 0;
   nMutaciones = 0;
   padres[0] = 0;
   padres[1] = 0;

   random_device rd;
   twister.seed(rd());
}

/* Para este individuo, crea su cromosoma aleatorio, el tamaño de cada gene, y
 * el vector de variables decodificadas x.
 */
void Individuo::insuflar(ProblemaOptim* p, int precision) {
   xRanges = p->xRanges();
   x.assign(p->numVariables(), 0.0);
   cons.assign(p->numRestricciones(), 0.0);

   double numValores;
   int numBits;

   geneSizes.clear();
   chromoSize = 0;
   for (auto r : xRanges) {
      numValores = 1 + (r.second - r.first) * pow(10, precision);
      numBits = ceil( log2( numValores ) );
      geneSizes.push_back(numBits);
      chromoSize += numBits;
   }

   static uniform_int_distribution<int> unif{0, 1};
   cromo.assign(chromoSize, 0);
   for (int k=0; k < chromoSize; k++)
      cromo[k] = unif(twister); // Pone 0 o 1 aleatoriamente.

   decodificar();
}

void Individuo::copiar(Individuo* source) {
   cromo = source->cromo;
   x = source->x;
   aptitud = source->aptitud;
   eval = source->eval;
   cons = source->cons;
   xRanges = source->xRanges;
}

void Individuo::decodificar() {
   double entero;
   long double factor;
   int finGene, inicioGene = 0;

   // Decodificar cada gene para obtener su valor x real.
   for (unsigned i = 0; i < x.size(); ++i)
   {
      factor = 1.0;
      entero = 0.0;
      finGene = inicioGene + geneSizes[i] - 1;
      for (int j = finGene; j >= inicioGene; j--) {
         if (cromo[j] == 1)
            entero += factor;

         factor *= 2.0; // Se incrementa como 1, 2, 4, 8,...
      }

      Range r = xRanges[i];
      x[i] = r.first +
             ((r.second - r.first)*entero) / (pow(2, geneSizes[i]) - 1.0);

      inicioGene += geneSizes[i];
   }
}

/*
 * Convierte el vector x del individuo que tiene representación 
 * real (vector<double> x) a su codificación binaria, dejando el   
 * resultado en su cromosoma interno (vector<unsigned> cromo).
 */
void Individuo::x2cromosoma(ProblemaOptim* p) {
   double d;    // representación real del entero.
   int entero;  // redondeo del valor d. 
   int finGene, inicioGene = 0;
   for (int i = 0; i < x.size(); i++) {
      
      Range r = xRanges[i];
      
      d = ( pow(2, geneSizes[i]) - 1.0 ) * 
          ( (x[i]     - r.first) / 
            (r.second - r.first) );
      
      entero = round( d );

      // Convertir el entero a binario para guardarlo en el cromosoma
      finGene = inicioGene + geneSizes[i] - 1;

      for (int j = finGene; j >= inicioGene ; j--) 
      {
         if ( j >= inicioGene ) {
            cromo[j] = entero % 2;
            entero = entero / 2;
         }
         else {
            cerr << "codificación de x: se necesitaban más bits y no debía." << endl;
            exit(0);
         }
      }
      inicioGene += geneSizes[i];
   }
}

void Individuo::imprimeIndi(ostream& salida) {
   for (auto alelo : cromo)
      salida << alelo;

   salida << "   ";

   for (auto valor : x)
      salida << valor;

   salida << "   ";

   salida << aptitud << "   " << valesp;
}
