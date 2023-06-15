#ifndef CANNONFUN_H
#define CANNONFUN_H

#include "ProblemaOptim.h"

class CannonFun : public ProblemaOptim {
public:
   CannonFun(double centro = 15.0);
   virtual ~CannonFun();

   void evaluateFun(vector<double> const &x, double &fun, vector<double> &cons) const;

private:
   double C; // Distancia al centro de la diana.
};

#endif
