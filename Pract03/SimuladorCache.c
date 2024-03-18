#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>





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


void main (int argc, char **argv){


	int Tlin; //Tamaño de línea: tamaño en bytes potencia de dos.
	int Ncon; //Número de conjuntos: número entero potencia de dos.
	int Asoc; //Asociatividad: número entero mayor o igual que 1.
	int Reem; //Reemplazo: 0: Aleatorio, 1: LRU.
	FILE *configFile;
	FILE *trazaFile;

	int bLin, bConj, bTag, tCache;

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



	}



}

