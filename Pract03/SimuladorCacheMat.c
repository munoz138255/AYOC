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
		printf("Tamaño de la cache: %d B = %d*2^%d B",x,y,nbites);
	}
	else{
		printf("Tamaño de la cachè: %d B = 2^%d B", x, nbites);
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
		/*unsigned long int **LRU;
        LRU  = (unsigned long int**)malloc(Ncon*sizeof(unsigned long int));
        for(int i = 0;i< Ncon;i++){
            LRU[i] =  = (unsigned long int*)malloc(Asoc*sizeof(unsigned long int));
            for(int j = 0;j<Asoc;j++){
                LRU[i][j] = 0;
            }
        }
		for(int i = 0; i < Ncon*Asoc;i++){
				cache[i] = 0;
				LRU[i] = 0;
		}*/
		printf("Tamaño cache: %d\n", Ncon*Asoc);
		printf("Tamaño LRU: %d\n", Ncon*Asoc);

		printf("Tlin: %d\n", Tlin);
		printf("Ncon: %d\n", Ncon);
		printf("Asoc: %d\n", Asoc);
		printf("Reem: %d\n", Reem);

		bLin = calcularBits(Tlin);
		printf("Bits para el bloque: %d\n", bLin);
		bConj = calcularBits(Ncon);
		printf("Bits para el conjunto: %d\n", bConj);
		bTag = 48 - bLin - bConj;
		printf("Bits para el tag: %d\n",bTag);
		tCache = Tlin*Asoc*Ncon;
		int b = calcularTamCache(tCache);
		int y = 0;
		unsigned long int traza;
		unsigned long int conjunto;
		unsigned long int tag;
        unsigned long int aux;
		printf("----------------------------------While----------------------------------\n");
		while (fscanf(trazaFile, "%lx \n", &traza) != EOF){
			//sleep(1);
			//printf("Traza leida: %lx\n", traza);
			nAccesos++;
			conjunto = rangoBits(traza, bLin, bLin + bConj - 1);
			printf("Conjunto: %ld\n", conjunto);
			tag = rangoBits(traza, bLin + bConj, 47);
			printf("Tag: %ld\n", tag);
			unsigned long int aux;
			bool acierto = false;
			//printf("---Cache conjunto %ld\n", conjunto);
			for(int i = 0;i < Asoc;i++){
				//printf("For %d while %d\n", i, y);
				//printf("---Cache tag para %d : %ld\n", i, aux);
				//printf("---Tag a comparar: %ld\n", tag);
				if((cache[conjunto][i]) == tag){
					acierto = true;
					printf("ACIERTO!!!!!!añañn!: \n", i, y);
					if(Reem){
                        if(i != 0){
                            aux = cache[conjunto][i];
                            for(;i > 0;i--){
                                cache[conjunto][i-1];
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
				if(Reem){
					for(int k = Asoc;k > 1;k--){
                        cache[conjunto][k] = cache[conjunto][k-1];
					}
					cache[conjunto][0] = tag;
				}
				else{
					pos = rand() % Asoc;
                    cache[conjunto][pos] = tag;
				}
			}
			y++;
		}
		printf("Cache -> %d\n", y);
		double porcError = (double)nFallos/nAccesos;
		printf("Porcentaje de fallos: %.2f \n", porcError);
        for(int i = 0;i < Ncon;i++){
            free(cache[i]);
        }
		free(cache);

		return 0;
	}
}
