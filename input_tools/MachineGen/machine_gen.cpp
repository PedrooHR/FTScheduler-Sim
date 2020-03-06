#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <string>

using namespace std;

int main(int argc, char * argv[]){
    int N;
    FILE * p;
    N = stoi(argv[1]);
    string dpath(argv[2]);
    string path = dpath + "_m.txt";

    p = fopen(path.c_str(), "w+");

    fprintf(p, "%d\n", N);

    srand(time(NULL));

    for (int i = 0; i < N; i++){

        float e_coef = (100 - rand() % 20) / 100.0;

        fprintf(p, "%f  \n", e_coef);
    }

    fclose(p);

    return 0;
}
