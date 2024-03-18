#include <stdio.h>


void main(){
    double v[100], res;
    FILE *f;
    f=fopen("traza.txt","w");
    for (int i=0 ;i<100 ; i++) {
    res=res+v[i];
    fprintf(f, "%p ", &v[i]);
    }

}
