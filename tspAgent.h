#ifndef TSPAGENT_H
#define TSPAGENT_H
#include <vector>
#include <random>
#include "agent.h"

namespace pga{
    class TSPAgent : public Agent<TSPAgent> {
        void random_DNA();
        int size;
        int chromosome_length;
        std::vector<int> DNA;
        void mutate();
        void greedy_DNA();
        int find_nearer_avail_node(int starting_node, std::vector<int> visited);
        /* initializer. Start with a random path*/
        public:
        int path_lenght;
        int get_path()const {return path_lenght;};
        TSPAgent& operator=(const TSPAgent&);
        TSPAgent(const TSPAgent &);

        TSPAgent(int, double);
    
        TSPAgent(double);

        void simulate() override;

        void reproduce(const TSPAgent&, const TSPAgent &) override;
        void print_solution();        
        void show_statistics(const std::vector<TSPAgent> &);

        ~TSPAgent();
    };
}

#include "tspAgent.cpp"

#endif