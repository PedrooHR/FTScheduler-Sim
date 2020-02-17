#include <iostream>
#include <random>
#include <chrono>
#include <cstdlib>
#include <stdio.h>
#include <vector>

using namespace std;

// arguments: 1=weibullshape (mtbf); 2=#machines; 3=#failures, 4=#minimunmachines

int main(int argc, char * argv[]){
  
  // construct a trivial random generator engine from a time-based seed:
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine generator (seed);

  //std::weibull_distribution<double> weibull (0.78, atof(argv[1]));
  std::weibull_distribution<double> weibull (0.387187, atof(argv[1]));
  std::uniform_int_distribution<int> uniform (0, atoi(argv[2]) - 1);
  std::uniform_int_distribution<int> types (1, 3); 

  std::vector<int> ids;
  std::vector <int> marked;
  ids.resize(atoi(argv[2]));
  marked.resize(atoi(argv[2]));

  for (int i = 0; i < ids.size(); i++){
    ids[i] = i;
    marked[i] = 0;
  }

  int currmachines = atoi(argv[2]);

  double first_w = weibull(generator);
  int first_u = uniform(generator);
  int ftype = types(generator);

  int nextmachine = atoi(argv[2]);

  for (int i = 0; i < atoi(argv[3]); i++) {
    printf("%.0f %d %d\n", first_w, ids[first_u], ftype);

    if (ftype == 1){
      marked[first_u] = 1;
      currmachines--;
      if (currmachines < atoi(argv[4])){
        ids[first_u] = nextmachine++;
        marked[first_u] = 0;
      }
    }

    first_w += weibull(generator);
    first_u = uniform(generator);
    ftype = types(generator);  
    while (marked[first_u] == 1)
        first_u = uniform(generator);
    
  }

  return 0;
}
