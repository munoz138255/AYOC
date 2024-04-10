#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>



//Funcion para cálcular el rango de bits(Proporcionada en el pdf)
unsigned long int rangoBits (unsigned long int n, int bitMenor, int bitMayor)
{
	unsigned long int bit2=1, bit1=1, ajuste=0;

	if (bitMenor <0 || bitMenor >47) {
		printf("Error en bitMenor. %d\n", bitMenor);
		exit(0);
	}
	else if (bitMayor <0 || bitMayor >47) {
		printf("Error en bitMayor. %d\n", bitMayor);
		exit(0);
	}
	else if(bitMenor > bitMayor){
		printf("Error en orden.\n");
		exit(0);
	}
	else if (bitMayor - bitMenor+1 <48 ) {
		// calcula valor 2 elevado a bitMenor
		bit1= bit1 << bitMenor;

		// calcula valor 2 elevado a bitMayor-bitMenor+1
		bit2= bit2 << ((bitMayor-bitMenor) + 1);

		n = n/bit1;
		n = n%bit2;
	}
	return (n);
}
//Obtención de parámetros para la cache, proporcionadas del fichero "config.txt"
void configurarCache(FILE *f, int *Tlin, int *Ncon, int *Asoc, int *Reem) {
    fscanf(f, "Tlin: %d\n", Tlin);
    fscanf(f, "Ncon: %d\n", Ncon);
    fscanf(f, "Asoc: %d\n", Asoc);
    fscanf(f, "Reem: %d\n", Reem);
}
//Pese a incluir la librería math.h, el programa no conseguía operar con log, por lo que se me ocurrio ir diviendo entre 2 y sumar las veces que se ha realizado la división para simular un log2
int calcularBits(int x){
	int result = 0;
	 while(x > 1){
		 x = x/2;
		 result++;
	 }
	 return result;
}
//Función para calcular el tamaño de la cache, no tiene funcionalidad para el simulador, es meramente informativo para el usuario.
int calcularTamCache(int x){
	int y = x;
	int nbites = 0;
	while(y > 1 && y%2==0){
		y = y/2;
		nbites++;
	}
	if(y>1){
		printf("Tamaño de la cache: %d B = %d*2^%d B\n",x,y,nbites);
	}
	else{
		printf("Tamaño de la cachè: %d B = 2^%d B\n", x, nbites);
	}
	return nbites;
}


int main(int argc, char **argv){


	int Tlin; //Tamaño de línea: tamaño en bytes potencia de dos.
	int Ncon; //Número de conjuntos: número entero potencia de dos.
	int Asoc; //Asociatividad: número entero mayor o igual que 1.
	int Reem; //Reemplazo: 0: Aleatorio, 1: LRU.
	FILE *configFile; //Declarar fichero "configFile.txt"
	FILE *trazaFile; //Declarar fichero "traza.txt"

	int bLin, bConj, bTag, tCache; //Declaración de variables donde vamos a almacenar los bits que van a ocupar en la cache la linea, los conjuntos, el tag y un entero tal que "tCache" para la función del tamaño de la caché
	int nAccesos = 0; //Entero para el número de accesos
	int nFallos = 0; //Entero para el número de fallos

	if(argc != 3){//Comprobación de introducción correcta de argumentos al ejecutar el simulador
		perror("ERROR faltan parametros de entrada");
		exit(EXIT_FAILURE);
	}
	else{
		configFile = fopen(argv[1],"r"); //Apertura en lectura del fichero "configFile.txt"
		trazaFile = fopen(argv[2],"r"); //Apertura en lectura del fichero "traza.txt"
		if(configFile == NULL || trazaFile == NULL){ //Comprobación de que se han abierto correctamente los dos ficheros
			perror("Error al abrir los ficheros\n");
			exit(EXIT_FAILURE);
		}
		configurarCache(configFile,&Tlin, &Ncon, &Asoc, &Reem);// Llamada a la función configurarCache(Lectura de parámetros)

		long int cache[Ncon][Asoc]; //Declaración de la memoria caché, donde filas es el número de conjuntos (Ncon) y las columnas el número de vias por conjunto (Asoc)
        for(int i = 0;i< Ncon;i++){
            for(int j = 0;j<Asoc;j++){
                cache[i][j] = 0; //Inicialización de la caché iniciando a 0 en todas las posiciones de esta
            }
        }

		printf("Tamaño de linea (Tlin): %d\n", Tlin);
		printf("Número de conjuntos (Ncon): %d\n", Ncon);
		printf("Número de vias por conjunto (Asoc): %d\n", Asoc);
		printf("Tipo de reemplazo (Reem | 0 Aleatorio, 1 LRU): %d\n", Reem);

		bLin = calcularBits(Tlin); //Lamada a la función calcularBits para calcular cuantos bits ocupara la linea
		bConj = calcularBits(Ncon); //Lamada a la función calcularBits para calcular cuantos bits ocupara el conjunto
		bTag = 48 - bLin - bConj; //Sabiendo que para la caché tenemos 48 bits, cálculo simple para conocer cuantos bits ocupará el tag (Sera el resultado de los bits de la caché quitando los bits necesarios para la línea y el conjunto)
		tCache = Tlin*Asoc*Ncon; //Primer cálculo para conocer el tamaño de la caché y mas tarde en la función calcularTamCache realizar los cálculos restantes para mostrar al usuario.
		int b = calcularTamCache(tCache);

		unsigned long int traza; //Declaración de variable para la traza leida
		unsigned long int conjunto; //Declaración de variable para obtener el conjunto de la traza leida
		unsigned long int tag; //Declaración de variable para obtener el tag de la traza leida
        unsigned long int aux; //Declaración de variable para ayuda con operaciones que veremos mas tarde en el acierto en la caché
        bool acierto; //Declaración de la variable booleana acierto, que nos servira para poder operar con los aciertos y fallos de la caché
		while (fscanf(trazaFile, "%lx \n", &traza) != EOF){ //Lectura de trazas del fichero "traza.txt"
			nAccesos++; //Incrementamos el número de accesos
			conjunto = rangoBits(traza, bLin, bLin + bConj - 1); //Obtenemos el conjunto en el que se va almacenar la traza leida gracias a la función rangoBits
			tag = rangoBits(traza, bLin + bConj, 47); //Obtenemos el tag de la traza leida gracias a la función rangoBits
            acierto = false; //Inicializamos a falso el booleano acierto cada vez que leemos una traza, pues no podemos saber de primeras si se encuentra el dato ya en la caché
			for(int i = 0;i < Asoc;i++){ //Recorrido para buscar el tag de la traza leida en la cache, recorreremos por columnas (Vias del conjunto) el dato obteniendo como fila el conjunto de la traza leida
				if((cache[conjunto][i]) == tag){ //En el caso de que encontremos el tag almacenado en una de las vias del conjunto de la caché entraremos en esta condición, obteniendo asi un acierto.
					acierto = true; //Cambiamos el valor del booleano a true
					if(Reem == 1){ //Reem = 1 se refiere a que nos encontramos en una memoria caché con método de reemplazo LRU, si Reem == 0 estamos en una caché método de reemplazo aleatorio
                        if(i != 0){
                            //La caché actualiza las posiciones y los datos, de manera que los datos más usados recientemente se ordenan de la vía mas a la izquierda( cache[conjunto][0] a derecha siendo                     cache[conjunto][ASoc - 1] el dato menos usado recientemente)
                            aux = cache[conjunto][i]; //Guardamos el dato buscado y que esta almacenado en la cache
                            for(;i > 0;i--){
                                cache[conjunto][i] = cache[conjunto][i-1];
                                //Actualizamos la caché moviendo los datos 1 vía a la derecha desde la vía en la que se encontraba el dato buscado hasta la primera vía, pues ahora se convierte en el dató mas buscado recientemente
                            }
                            cache[conjunto][0] = aux; //Actualizamos la primera vía con el dato que acababamos de buscar, teniendo ya ese conjunto y sus vías actualizados
                        }
					}
					break; //Break, pues ha sido acierto y no nos interesa seguir iterando
				}
			}
			if(!acierto){ //El caso de no encontrar el dato en nínguna de las vias del conjunto
				nFallos++; //Incrementamos el número de fallos
				int pos = 0; //Variable pos, que la utilizaremos para el caso en el que el método de remplazo de la caché sea alatorio
				if(Reem == 1){ //Caso en el que el método de reemplazo de la caché es LRU
					for(int k = Asoc - 1;k > 0;k--){ //Recorrido para actualizar la caché, método parecido al de acierto, solo que en este caso lo realizaremos con todas las vías, hasta la primera
                        cache[conjunto][k] = cache[conjunto][k-1]; //Actualizamos caché, eliminamos el dato menos utilizado recientemente y desplazamos a la derecha
					}
					cache[conjunto][0] = tag; //En la primera vía introducimos el dato más reciente (El que acabamos de leer)
				}
				else{ //Caso en el que el método de reemplazo es aletorio
					pos = rand() % (Asoc - 1); //Número aleatorio entre 0 y Asoc - 1
                    cache[conjunto][pos] = tag; //Introducimos el nuevo dato y lo sustituimos por el que había en esa posición
				}
			}
		}
		double porcError = ((double)nFallos/nAccesos); //Cálculo de porcentaje de error
		printf("---------------------------------------------------------------------------------------------------------------\n");
        printf("|                                       ");
		printf ("Porcentaje de fallos: %.2f -> %.2f", porcError, porcError*100);
        printf("                                   |\n");
		printf("---------------------------------------------------------------------------------------------------------------\n");

	}
	return 0;
}
