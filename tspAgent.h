#ifndef TSPAGENT_H
#define TSPAGENT_H
#include <vector>
#include <random>
#include "agent.h"

namespace pga{
class TSPAgent : public pga::Agent<double> {
    public:
    int size;
    std::vector<std::vector<int>>  graph;
    int chromosome_length;
    std::vector<int> DNA;
    /* initializer. Start with a random path*/
    TSPAgent(const TSPAgent &a){DNA = a.DNA ;graph = a.graph;fitness = a.fitness;};

    TSPAgent(std::vector<std::vector<int>> const graph, int chromosome_length):
         graph(graph),chromosome_length(chromosome_length) {size = graph.size(); random_DNA();};

    TSPAgent(std::vector<std::vector<int>> const);
    void random_DNA();
    void simulate();
    void reproduce(TSPAgent, TSPAgent);
    ~TSPAgent();
};

}

#endif