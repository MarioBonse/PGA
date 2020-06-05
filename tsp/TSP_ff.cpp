#include <string>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <random>
#include <algorithm>
#include <chrono>
#include "pga/ff_population_farm2.h"
#include "tspAgent.h"
#include "utils.h"
#include <chrono>


int main(int argc, char *argv[]) {    
    if (argc<3) {
        std::cerr << "use: " << argv[0]  << " graph config\n";
        return -1;
    }
    std::string graph_path = argv[1];

    if(load_graph(graph_path, my_graph) == -1)
        return -1;
    std::string config_file = argv[2];
    std::map<std::string, std::string> config;
    if(load_config(config_file, config) == -1){
        return -1;
    }
    int nw = 4;
    if(argc == 4)
        nw = atoi(argv[3]);
    // show_config(config);
    int iterations = std::stoi(config["iterations"]);
    int chromosome_len = std::stoi(config["chromosome_len"]);
    double p_mutation = std::stod(config["p_mutation"]);
    int number_agent = std::stoi(config["population"]);

    pga::ff_population_farm<pga::TSPAgent> pop(std::stod(config["N_keep_agent"]), number_agent,  nw);


    //create the agents
    for(int i = 0; i < number_agent ; i++){
        pga::TSPAgent my_agent(chromosome_len, p_mutation);
        pop.add_agent(my_agent);
    }   

    auto start = std::chrono::high_resolution_clock::now();

    pop.simulate(iterations); 

    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    auto usec    = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    std::cout <<"the whole simulation took: "<< usec << std::endl;

    pga::TSPAgent best_agent = pop.best_agent();
    //best_agent.print_solution();
}