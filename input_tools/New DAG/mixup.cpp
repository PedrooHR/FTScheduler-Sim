#include <cstdio>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <random>

using namespace std;

#define probability 0.08
#define N 100

// Uniform probability distribution
unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
std::default_random_engine generator (seed);
std::uniform_real_distribution<float> distribution (0.0, 1.0);

pair < vector <vector <int>>, vector <int> > mixup (vector <int> dA, vector <int> dB, vector <vector <int>> A, vector <vector <int>> B);

int main (){
    vector <vector <int>> A;
    vector <vector <int>> B;
    vector <vector <int>> C;
    vector <vector <int>> D;
    vector <vector <int>> E;
    vector <vector <int>> F;
    vector <vector <int>> G;
    vector <vector <int>> H;
    vector <vector <int>> I;
    vector <vector <int>> J;
    vector <int> dA;
    vector <int> dB;
    vector <int> dC;
    vector <int> dD;
    vector <int> dE;
    vector <int> dF;
    vector <int> dG;
    vector <int> dH;
    vector <int> dI;
    vector <int> dJ;

    FILE * a = fopen("n100/1n100.cfg", "r+");
    FILE * b = fopen("n100/2n100.cfg", "r+");
    FILE * c = fopen("n100/3n100.cfg", "r+");
    FILE * d = fopen("n100/4n100.cfg", "r+");
    FILE * e = fopen("n100/5n100.cfg", "r+");
    FILE * f = fopen("n100/6n100.cfg", "r+");
    FILE * g = fopen("n100/7n100.cfg", "r+");
    FILE * h = fopen("n100/8n100.cfg", "r+");
    FILE * i = fopen("n100/9n100.cfg", "r+");
    FILE * j = fopen("n100/10n100.cfg", "r+");

    A.resize(N);
    B.resize(N);
    C.resize(N);
    D.resize(N);
    E.resize(N);
    F.resize(N);
    G.resize(N);
    H.resize(N);
    I.resize(N);
    J.resize(N);
    dA.resize(N);
    dB.resize(N);
    dC.resize(N);
    dD.resize(N);
    dE.resize(N);
    dF.resize(N);
    dG.resize(N);
    dH.resize(N);
    dI.resize(N);
    dJ.resize(N);

    int aux;

    fscanf(a, "%d", &aux);
    fscanf(b, "%d", &aux);
    fscanf(c, "%d", &aux);
    fscanf(d, "%d", &aux);
    fscanf(e, "%d", &aux);
    fscanf(f, "%d", &aux);
    fscanf(g, "%d", &aux);
    fscanf(h, "%d", &aux);
    fscanf(i, "%d", &aux);
    fscanf(j, "%d", &aux);

    for (int x = 0; x < N; x++){
        int na, nb, nc, nd, ne, nf, ng, nh, ni, nj;
        fscanf(a, "%d", &na);
        fscanf(b, "%d", &nb);
        fscanf(c, "%d", &nc);
        fscanf(d, "%d", &nd);
        fscanf(e, "%d", &ne);
        fscanf(f, "%d", &nf);
        fscanf(g, "%d", &ng);
        fscanf(h, "%d", &nh);
        fscanf(i, "%d", &ni);
        fscanf(j, "%d", &nj);

        for (int z = 0; z < na; z++){
            int number;
            fscanf(a, "%d", &number);
            //number--;
            A[x].push_back(number);
            dA[number]++;
        }
        for (int z = 0; z < nb; z++){
            int number;
            fscanf(b, "%d", &number);
            //number--;
            B[x].push_back(number);
            dB[number]++;
        }
        for (int z = 0; z < nc; z++){
            int number;
            fscanf(c, "%d", &number);
            //number--;
            C[x].push_back(number);
            dC[number]++;
        }
        for (int z = 0; z < nd; z++){
            int number;
            fscanf(d, "%d", &number);
            //number--;
            D[x].push_back(number);
            dD[number]++;
        }
        for (int z = 0; z < ne; z++){
            int number;
            fscanf(e, "%d", &number);
            //number--;
            E[x].push_back(number);
            dE[number]++;
        }
        for (int z = 0; z < nf; z++){
            int number;
            fscanf(f, "%d", &number);
            //number--;
            F[x].push_back(number);
            dF[number]++;
        }
        for (int z = 0; z < ng; z++){
            int number;
            fscanf(g, "%d", &number);
            //number--;
            G[x].push_back(number);
            dG[number]++;
        }
        for (int z = 0; z < nh; z++){
            int number;
            fscanf(h, "%d", &number);
            //number--;
            H[x].push_back(number);
            dH[number]++;
        }
        for (int z = 0; z < ni; z++){
            int number;
            fscanf(i, "%d", &number);
            //number--;
            I[x].push_back(number);
            dI[number]++;
        }
        for (int z = 0; z < nj; z++){
            int number;
            fscanf(j, "%d", &number);
            //number--;
            J[x].push_back(number);
            dJ[number]++;
        }
    }
    //Mix A com B
    pair<vector<vector<int>>, vector <int>> par = mixup(dA, dB, A, B);
    par = mixup(par.second, dC, par.first, C);
    par = mixup(par.second, dD, par.first, D);
    par = mixup(par.second, dE, par.first, E);
    par = mixup(par.second, dF, par.first, F);
    par = mixup(par.second, dG, par.first, G);
    par = mixup(par.second, dH, par.first, H);
    par = mixup(par.second, dI, par.first, I);
    par = mixup(par.second, dJ, par.first, J);

    //Save final graph to file
    FILE * save = fopen("n100/Final100.cfg", "w+");
    fprintf(save, "%d\n", par.first.size());
    for (int x = 0; x < par.first.size(); x++){
        //printf("Task %2d possui %2d dependencias e %2d dependentes diretos\n", i, ABCDE.first[i].size(), ABCDE.second[i]);
        fprintf(save, "%d", par.first[x].size());
        for (int z = 0; z < par.first[x].size(); z++)
            fprintf(save, "% d", par.first[x][z]);

        fprintf(save, "\n");
    }
    fclose(save);

    fclose(a);
    fclose(b);
    fclose(c);
    fclose(d);
    fclose(e);
    fclose(f);
    fclose(g);
    fclose(h);
    fclose(i);
    fclose(j);

    return 0;
}

pair < vector <vector <int>>, vector <int> > mixup (vector <int> dA, vector <int> dB, vector <vector <int>> A, vector <vector <int>> B)
{
    vector <vector<int>> AB;
    vector <int> dAB;
    //Mix A with B -- Connect all A tasks with no dependents with all B tasks with no dependencies (with probability p0.03;
    //B are 0 - 19 and A are 20 - 39
    for (int i = 0; i < N; i++){
        if (B[i].size() == 0){
            for (int j = 0; j < N; j++){
                if (dA[j] == 0){
                    float random_n = distribution(generator);
                    if (random_n < probability){
                        dA[j]++;
                        B[i].push_back(j + N);
                    }
                }
            }
        }
    }
    AB = B;
    dAB = dB;

    for (int i = 0; i < A.size(); i++)
        for (int j = 0; j < A[i].size(); j++)
            A[i][j] += N;

    AB.insert(AB.end(), A.begin(), A.e nd());
    dAB.insert(dAB.end(), dA.begin(), dA.end());

    return pair<vector<vector<int>>, vector <int>>(AB, dAB);
}
