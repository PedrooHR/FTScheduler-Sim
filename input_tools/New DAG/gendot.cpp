#include <cstdio>
#include <cstdlib>

using namespace std;


int main (int argc, char * argv[]){
    if (argc != 3){
        printf("invalid arguments\n");
        exit(0);
    }

    FILE * p = fopen(argv[1] , "r+");
    FILE * q = fopen(argv[2] , "w+");

    int n;
    fscanf(p, "%d", &n);
    fprintf(q, "digraph G {\n");
    for (int i = 0; i < n; i++) {
        int nl;
        fscanf(p, "%d", &nl);
        fprintf(q, "\t%d;\n", i);
        for (int j = 0; j < nl; j++){
            int number;
            fscanf(p, "%d", &number);
            fprintf(q, "\t%d -> %d;\n", i, number);
        }
    }

    fprintf(q, "}\n");

    fclose(p);
    fclose(q);

    return 0;
}
