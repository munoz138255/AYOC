#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 10

void main(){
    int i, j;
    double v[6][128], res;
    FILE *f;
    f=fopen("traza.txt","w");
    for (j=0; j<128; j++){
        printf ("%d\n", j);
        for (i=0; i<6; i++){
            fprintf(f, "%p ", &v[i][j]);
        }
    }
}
