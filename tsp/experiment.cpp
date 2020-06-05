#include <string>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <random>
#include <algorithm>
#include <chrono>
#include <pga/thread_population_farm.h>
#include <pga/omp_population.h>
#include <pga/ff_population_farm2.h>
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
    int iterations = 1;
    int chromosome_len = std::stoi(config["chromosome_len"]);
    double p_mutation = std::stod(config["p_mutation"]);
    int number_agent = std::stoi(config["population"]);

    pga::population<pga::TSPAgent> sequential_population(std::stod(config["N_keep_agent"]), number_agent);
    pga::ff_population_farm<pga::TSPAgent> farm_population(std::stod(config["N_keep_agent"]), number_agent,1);
    pga::omp_population<pga::TSPAgent> omp_population(std::stod(config["N_keep_agent"]), number_agent,1);
    pga::thread_population<pga::TSPAgent> thread_population(std::stod(config["N_keep_agent"]), number_agent,1);

    //create the agents
    for(int i = 0; i < number_agent ; i++){
        pga::TSPAgent my_agent(chromosome_len, p_mutation);
        sequential_population.add_agent(my_agent);
        thread_population.add_agent(my_agent);
        omp_population.add_agent(my_agent);
        farm_population.add_agent(my_agent);
    }   

    auto start = std::chrono::high_resolution_clock::now();

    sequential_population.simulate(iterations); 

    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    auto usec    = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    std::cout <<"the sequential simulation took: "<< usec << std::endl;

    for(int n = 16; n<256;n+=16){
            
        auto start = std::chrono::high_resolution_clock::now();
        farm_population.nw = n;
        farm_population.workers = n;
        farm_population.simulate(iterations); 

        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        auto usec    = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        std::cout <<"the ff with "<<n<<" workers took simulation took: "<< usec << std::endl;

        start = std::chrono::high_resolution_clock::now();
        omp_population.nw = n;
        omp_population.workers = n;
        omp_population.simulate(iterations); 

        elapsed = std::chrono::high_resolution_clock::now() - start;
        usec    = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        std::cout <<"the omp with "<<n<<" workers took simulation took: "<< usec << std::endl;

        start = std::chrono::high_resolution_clock::now();
        thread_population.nw = n;
        thread_population.workers = n;
        thread_population.simulate(iterations); 

        elapsed = std::chrono::high_resolution_clock::now() - start;
        usec    = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        std::cout <<"the thread with "<<n<<" workers took simulation took: "<< usec << std::endl;
    }

    //best_agent.print_solution();
}