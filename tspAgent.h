#ifndef TSPAGENT_H
#define TSPAGENT_H
#include <vector>
#include <random>
#include "agent.h"

namespace pga{
    class TSPAgent : public Agent {
        void random_DNA();
        int size;
        int chromosome_length;
        std::vector<int> DNA;
        void mutate();
        void greedy_DNA();
        int find_nearer_avail_node(int starting_node, std::vector<int> visited);
        /* initializer. Start with a random path*/
        public:
        int path_lenght = 0;
        int get_path(){return path_lenght;};
        TSPAgent& operator=(const TSPAgent& a2);
        TSPAgent(const TSPAgent &);

        TSPAgent(int, double);
    
        TSPAgent(double);

        void simulate();
        friend bool operator<(const TSPAgent& a1, const TSPAgent& a2)
        {
            return a1.path_lenght < a2.path_lenght; // keep the same order
        };
        void reproduce(const TSPAgent&, const TSPAgent &);
        void print_solution();        
        void show_statistics(const std::vector<TSPAgent> &);

        ~TSPAgent();
    };
}

#include "tspAgent.cpp"

#endif