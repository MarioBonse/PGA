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
#include "utils.h"


int main(int argc, char *argv[]) {    
    if (argc<4) {
        std::cerr << "use: " << argv[0]  << " graph config\n";
        return -1;
    }
    std::string graph_path = argv[1];
    std::vector<std::vector<int>> graph;
    if(load_graph(graph_path, graph) == -1)
        return -1;
    std::string config_file = argv[1];
    std::map<std::string, std::string> config;
    if(load_config(config_file, config) == -1){
        return -1;
    }
    int iterations;
    pga::population<pga::TSPAgent> Population;


    //create the agents
    for(int i = 0; i < std::stoi(config["number_of_agent"]) ; i++){
        pga::TSPAgent my_agent(graph);
        Population.add_agent(my_agent);
    }   

    for(int i = 0; i<iterations;++i){
        // simulate and calculate the fitness
        Population.simulate();
    }
    pga::TSPAgent best_agent = Population.best_agent();
}