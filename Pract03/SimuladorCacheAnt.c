#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>




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

unsigned long int leerNumeroHexadecimal(FILE * f){
	unsigned long int n;
	fscanf(f, "%lx", &n);
	return (n);
}

void configurarCache(FILE *f, int *Tlin, int *Ncon, int *Asoc, int *Reem) {
    fscanf(f, "Tlin: %d\n", Tlin);
    fscanf(f, "Ncon: %d\n", Ncon);
    fscanf(f, "Asoc: %d\n", Asoc);
    fscanf(f, "Reem: %d\n", Reem);
}

int calcularBits(int x){
	int result = 0;
	 while(x > 1){
		 x = x/2;
		 result++;
	 }
	 return result;
}
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
	FILE *configFile;
	FILE *trazaFile;

	int bLin, bConj, bTag, tCache;
	int nAccesos = 0;
	int nFallos = 0;

	if(argc != 3){
		perror("ERROR faltan parametros de entrada");
		exit(EXIT_FAILURE);
	}
	else{
		configFile = fopen(argv[1],"r");
		trazaFile = fopen(argv[2],"r");
		if(configFile == NULL || trazaFile == NULL){
			perror("Error al abrir los ficheros\n");
			exit(EXIT_FAILURE);
		}
		configurarCache(configFile,&Tlin, &Ncon, &Asoc, &Reem);

		unsigned long int **cache;
        cache  = (unsigned long int**)malloc(Ncon*sizeof(unsigned long int*));
        for(int i = 0;i< Ncon;i++){
            cache[i] = (unsigned long int*)malloc(Asoc*sizeof(unsigned long int));
            for(int j = 0;j<Asoc;j++){
                cache[i][j] = 0;
            }
        }

		printf("Tlin: %d\n", Tlin);
		printf("Ncon: %d\n", Ncon);
		printf("Asoc: %d\n", Asoc);
		printf("Reem: %d\n", Reem);

		bLin = calcularBits(Tlin);
		bConj = calcularBits(Ncon);
		bTag = 48 - bLin - bConj;
		tCache = Tlin*Asoc*Ncon;
		int b = calcularTamCache(tCache);
		int y = 0;
		unsigned long int traza;
		unsigned long int conjunto;
		unsigned long int tag;
        unsigned long int aux;
		int z = 0;
		while (fscanf(trazaFile, "%lx \n", &traza) != EOF){
			nAccesos++;
			conjunto = rangoBits(traza, bLin, bLin + bConj - 1);
			tag = rangoBits(traza, bLin + bConj, 47);
			unsigned long int aux;
			bool acierto = false;
			for(int i = 0;i < Asoc;i++){
				aux = cache[conjunto][i];
				//printf("Comprobando correcto conjunto cache[%ld][%d] = %ld, tag a comprobar %ld de acceso %d \n", conjunto, i, aux, tag, z);
				if((cache[conjunto][i]) == tag){
					//printf("ACIERTAZO EN CONJUNTO[%ld][%d] de iteración %d\n", conjunto, i, z);
					acierto = true;
					if(Reem == 1){
                        if(i != 0){
                            aux = cache[conjunto][i];
                            for(;i > 0;i--){
                                cache[conjunto][i] = cache[conjunto][i-1];
                            }
                            cache[conjunto][0] = aux;
                        }
					}
					break;
				}
			}
			if(!acierto){
				//printf("Fallo en while %d: \n", y);
				nFallos++;
				int pos = 0;
				int aux = 0;
				if(Reem == 1){
					for(int k = Asoc - 1;k > 0;k--){
                        cache[conjunto][k] = cache[conjunto][k-1];
					}
					cache[conjunto][0] = tag;
				}
				else{
					pos = rand() % Asoc;
                    cache[conjunto][pos] = tag;
				}
			}
			z++;
		}
		double porcError = ((double)nFallos/nAccesos);
		printf("---------------------------------------------------------------------------------------------------------------\n");
		printf("|                                  Porcentaje de fallos: %.2f -> %.2f%_                                      |\n", porcError, porcError*100);
		printf("---------------------------------------------------------------------------------------------------------------\n");
        for(int i = 0;i < Ncon;i++){
            free(cache[i]);
        }
		free(cache);

		return 0;
	}
}
