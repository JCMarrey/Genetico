#ifndef TORCSFUN_H
#define TORCSFUN_H

#include "ProblemaOptim.h"

class TorcsFun : public ProblemaOptim {
public:
   TorcsFun(int serverID = 0);
   virtual ~TorcsFun();

   void evaluateFun(vector<double> const &x, double &fun, vector<double> &cons) const;

private:
   // Estas tres variables tendrán el número de nodos en las capas
   // de entrada, salida y oculta de la Red Neuronal.
   int numInputs;
   int numOutputs;
   int numHidden;

   int serverID;

   string patronArchivoPesos;
   string patronArchivoSalida;

   void launchClient(int serverID, int port) const;
   void launchServer(int serverID) const;
   void writePopFile(int serverID, vector<double> const &weights) const;
   void readEvaluation(int serverID, double& fun, vector<double>& cons) const;

   string nombreArchivoPesos(int i) const;
   string nombreArchivoSalida(int i) const;   
   
};

#endif
