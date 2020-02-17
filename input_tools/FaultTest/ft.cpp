#include <iostream>
#include <random>
#include <chrono>
#include <cstdlib>
#include <stdio.h>
#include <vector>

using namespace std;

int main(int argc, char * argv[]){
    std::vector <int> machines;
  // construct a trivial random generator engine from a time-based seed:
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine generator (seed);

  std::weibull_distribution<double> weibull (0.78, atof(argv[1]));
  std::uniform_int_distribution<int> uniform (0, atoi(argv[2]) - 1);

  machines.resize(atoi(argv[2]));
  double first_w = weibull(generator);
  int first_u = uniform(generator);
  machines[first_u] = 1;

  for (int i = 0; i < atoi(argv[2]); i++){
    printf("%.0f %d \n", first_w, first_u);

    first_w += weibull(generator);
    first_u = uniform(generator);
    while (machines[first_u] == 1 && i < atoi(argv[2]) - 1)
        first_u = uniform(generator);

    machines[first_u] = 1;
  }

    return 0;
}
