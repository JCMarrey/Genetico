#include "TorcsFun.h"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

/*
* Constructor de la clase TorcsFun. LLama al inicio al constructor de la
* clase base.
*/
TorcsFun::TorcsFun(int serverID) : ProblemaOptim("TORCS simulation", 2)
{
   this->serverID = serverID;
   // Se deben asignar los valores de la clase base: 
   // num. variables y num. restricciones.

   numInputs = 24;  // Estos valores son de referencia, porque
   numOutputs = 5;  // el neurocontrolador de TORCS es quien 
   numHidden = 0;   // tiene los valores actuales de los nodos.

   // Las variables del problema son los pesos para la red neuronal.
   nVariables = numInputs * numOutputs;

   // La restricción será que la distancia faltante del auto sea igual a 0.
   nRestricciones = 1; 

   // El intervalo de los pesos lo decidimos nosotros por ensayo y error, pero
   // debemos tomar en cuenta que entre más grande el intervalo más difícil el problema.
   // TODAS las variables estarán el intervalo [-1000, 1000]
   varRanges.assign(nVariables, make_pair(-800, 800));

   // El directorio comunicacion debe existir desde donde corran el AG.
   // Deben estar preparados para tener un nombre según el servidor (%02d) 
   // para que no haya conflicto entre los archivos de los servidores TORCS.
   patronArchivoPesos.assign("./comunicacion/pesos_%02d.txt");
   patronArchivoSalida.assign("./comunicacion/salida_%02d.txt");
}

TorcsFun::~TorcsFun() {}

/* Este procedimiento evalúa la función objetivo, obtiendo el valor real de
* la función y la aptitud del individuo dado.
* Entrada: x, un vector con los pesos que represetan una solución.
* Salida: 
*    1. fun, el valor de la función objetivo.
*    2. cons, el único valor de restricción del problema.
*/
void TorcsFun::evaluateFun(vector<double> const &x, double &fun, vector<double> &cons) const
{  
   cout << "\n\nEvaluando un individuo\n" << endl;
   
   // 1. Crear el archivo de pesos (con serverID=0) usando x como los pesos.
   writePopFile(serverID, x);

   // 2. Arrancar el CLIENTE con el serverID=0 con el archivo de pesos
   //    y demás parámetros.
   int port = 3001 + serverID;
   launchClient(serverID, port);
   
   // 3. Arrancar el SERVIDOR con el serverID=0, el archivo de salida (con serverID=0), 
   //    y demás parámetros.
   launchServer(serverID);   

   // 4. Leer el archivo de salida para guardar los valores para fun y cons. 
   readEvaluation(serverID, fun, cons);

   cout << "\n\nTerminó de evaluar un individuo\n" << endl;
   
}

void TorcsFun::launchClient(int serverID, int port) const {
   // Esta cadena guarda lo que le mandemos a "imprimir"
   stringstream clientCmd;
   
   clientCmd << "./launch_torcs_client.sh " << " "
   << port << " "
   << nombreArchivoPesos(serverID)  << " "
   << nombreArchivoSalida(serverID) << " "
   << serverID << " "
   << "1";
   
   cout << clientCmd.str() << endl;
   
   // Ejecutar en la consola el cliente de TORCS.
   system(clientCmd.str().c_str());   
}

void TorcsFun::launchServer(int serverID) const {
   char trackName[] = "g-track-2"; // Nombre de una pista en TORCS
   
   char serverStr[20];
   sprintf(serverStr, "%d", serverID);
   
   char scriptServer[] = "./launch_torcs_server.sh";
   char salidaStr[50];
   sprintf(salidaStr, "%s", nombreArchivoSalida(serverID).c_str());
   
   char* args[] = {scriptServer, serverStr, salidaStr, trackName, nullptr};
   
   pid_t pid = fork(); // Crear un PROCESO hijo para que ejecute TORCS
   if (pid == 0) {     // El hijo entra en este condición.
      // Execute the server for the rally for processor i
      int res = execvp(scriptServer, args);
   
      if (res == -1) {
         cerr << "\nError al ejecutar execlp para arrancar el servidor TORCS.\n";
         exit(1);
      }
   }
   wait(NULL); // El padre espera por su PROCESO hijo (el servidor TORCS).
   cerr << "\nArchivo de resultados listo.\n";
}

void TorcsFun::writePopFile(int serverID, vector<double> const &weights) const
{
   // Crear el archivo para guardar los pesos.
   ofstream popStream(nombreArchivoPesos(serverID), std::ofstream::out);
   popStream << fixed << setw(5 + 6) << setprecision(6);

   // Escribir en el archivo de pesos los datos de la red neuronal.
   popStream << numInputs << "\n" << numOutputs << "\n" << numHidden << "\n";

   // Escribir los pesos de corrido, ya la red neuronal los acomodará
   // como matriz.
   for (auto w : weights)
      popStream << w << "\t";

   popStream.close();
}

void TorcsFun::readEvaluation(int serverID, double& fun, vector<double>& cons) const
{
   ifstream objStream(nombreArchivoSalida(serverID), std::ifstream::in);

   //***** ESTA FUNCIÓN LA TIENEN QUE COMPLETAR

   // Los 5 valores de la evaluación del auto están en un renglón en el siguiente orden:
   //        Posición  Tiempo  Daño  Gasolina_restante  Distancia_faltante
   double position, time, damage, fuel, dist2go;
   
   // En nuestro problema usaremos los siguientes valores:
   // 1. fun será el Tiempo, y
   // 2. cons será la Distancia_faltante para terminar la carrera.
   
   objStream >> position >> time >> damage >> fuel >> dist2go;
   
   // La restricción es necesaria es porque se minimizará el tiempo, pero
   // solamente tendrá sentido si se terminó la carrera (Distancia_faltante = 0).
   
   // Versión 1: SIN restricciones.
   //fun = dist2go;
   
   // Versión 2: Con restricciones.
   fun = time;
   cons[0] = dist2go;

   objStream.close();
}

string TorcsFun::nombreArchivoPesos(int i) const {
   char buffer[200];
   sprintf(buffer, patronArchivoPesos.c_str(), i);
   return string(buffer);
}

string TorcsFun::nombreArchivoSalida(int i) const {
   char buffer[200];
   sprintf(buffer, patronArchivoSalida.c_str(), i);
   return string(buffer);
}
