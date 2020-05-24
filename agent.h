#include "utils.h"
#include <random>
#include <iostream>

namespace pga{
    template <class T> class Agent{
        public:
        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
        std::uniform_real_distribution<double> dis(0, 1.0);
        double p_mutation = 0.01;
        T fitness = 0;
        bool alive = true;
        virtual void simulate();
        Agent();
        virtual void reproduce(Agent, Agent);
        friend bool operator<(const Agent& a1, const Agent& a2)
        {
            return a1.fitness < a2.fitness; // keep the same order
        }
    };
}


