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

std::vector<std::vector<int>> my_graph;

int main(int argc, char *argv[]) {    
    if (argc<3) {
        std::cerr << "use: " << argv[0]  << " graph config\n";
        return -1;
    }
    std::string graph_path = argv[1];
    std::vector<std::vector<int>> graph;
    if(load_graph(graph_path, my_graph) == -1)
        return -1;
    std::string config_file = argv[2];
    std::map<std::string, std::string> config;
    if(load_config(config_file, config) == -1){
        return -1;
    }

    // show_config(config);
    int iterations = std::stoi(config["iterations"]);
    int chromosome_len = std::stoi(config["chromosome_len"]);
    double p_mutation = std::stod(config["p_mutation"]);
    pga::population<pga::TSPAgent> pop(std::stod(config["N_keep_agent"]));
    int number_agent = std::stoi(config["population"]);


    //create the agents
    for(int i = 0; i < number_agent ; i++){
        pga::TSPAgent my_agent(graph, chromosome_len, p_mutation);
        pop.add_agent(my_agent);
    }   

    for(int i = 0; i<iterations;++i){
        // simulate and calculate the fitness
        pop.simulate();
    }
    std::cout<<"end simulation\n";
    pga::TSPAgent best_agent = pop.best_agent();
    best_agent.print_solution();
}