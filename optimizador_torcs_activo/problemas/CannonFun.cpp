#include "CannonFun.h"
#include <cmath>

                       /* Nombre problema, Número de variables */
CannonFun::CannonFun(double centro) : ProblemaOptim("Cannon", 2)
{
   C = centro;
   
   // Intervalo para el ángulo.
   varRanges[0] = {0, 3.14159/2.0};
   
   // Intervalo para la velocidad
   varRanges[1] = {0, 30};
}

CannonFun::~CannonFun() {}

/* Este procedimiento evalúa la función objetivo, obtiendo el valor real de
* la función y la aptitud del individuo dado.
* Entrada: x, un arreglo de dos elementos.
* Salida: fun, el valor de la función objetivo.
*/
void CannonFun::evaluateFun(vector<double> const &x, double &fun, vector<double> &cons) const
{
   double g = 9.81;
   double theta = x[0]; // ángulo
   double v     = x[1]; // velocidad

   // Utilizar la función de tiro parabólico para calcular a qué distancia
   // cayó la bala del centro de la DIANA.
   fun = abs( v*v*sin(2*theta) / g   - C );
}
