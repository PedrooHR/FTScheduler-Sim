#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace std;

int main(){
    int N;
    FILE * p;
    N = 50;

    p = fopen("50machines.cfg", "a+");

    fprintf(p, "%d\n", N);

    srand(time(NULL));

    for (int i = 0; i < N; i++){
        int cp_speed = 200 + rand() % 50;
        float e_coef = (100 - rand() % 20) / 100.0;

        fprintf(p, "%f %d \n", e_coef, cp_speed);
    }

    fclose(p);

    return 0;
}
