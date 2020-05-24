#include <string>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <random>
#include <algorithm>
#include <chrono>
#include "tspAgent.h"
#include "population.h"


int main(int argc, char *argv[]) {    
    if (argc<4) {
        std::cerr << "use: " << argv[0]  << " graph config nw\n";
        return -1;
    }
    std::string graph_path = argv[1];
    std::vector<std::vector<int>> graph = load_graph(graph_path);
    std::string config = argv[1];
    int number_agent;
    int seed = 1;
    int iterations;
    pga::population<pga::TSPAgent> Population;

    // set seed
    srand(seed);

    //create the agents
    for(int i = 0; i< number_agent ; i++){
        Population.add_agent(pga::TSPAgent(graph));
    }   

    for(int i = 0; i<iterations;++i){
        // simulate and calculate the fitness
        Population.simulate();
    }
    pga::TSPAgent best_agent = Population.best_agent();
}