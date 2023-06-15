#include "ProblemaOptim.h"

ProblemaOptim::ProblemaOptim(const char *name, int numVars, int numRestr)
{
   nombre.assign(name);
   nVariables = numVars;
   nRestricciones = numRestr;
   varRanges.assign(nVariables, make_pair(0.0, 1.0));
}

ProblemaOptim::~ProblemaOptim() {}

/* Version para problemas que no tienen restricciones. */
void ProblemaOptim::evaluateFun(vector<double> const &x, double &fun)
{
   static vector<double> dummy;
   evaluateFun(x, fun, dummy);
}

const int ProblemaOptim::numVariables() const {
   return nVariables;
}

const int ProblemaOptim::numRestricciones() const {
   return nRestricciones;
}

vector<Range> ProblemaOptim::xRanges() const {
   return varRanges;
}
