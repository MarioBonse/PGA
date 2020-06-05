#ifndef TSPAGENT_H
#define TSPAGENT_H
#include <vector>
#include <random>
#include <pga/agent.h>

/*
here we override the method from agent.h and 
we have also a lot of usefull methods
*/

namespace pga{
    class TSPAgent : public Agent<TSPAgent> {

        int size;
        int chromosome_length;
        std::vector<int> DNA;

        void random_DNA();
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