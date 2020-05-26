#ifndef AGENT_H
#define AGENT_H
#include <random>
#include <iostream>

namespace pga{
    class Agent{
        public:
        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937 gen{rd()}; //Standard mersenne_twister_engine seeded with rd()
        std::uniform_real_distribution<double> dis{0.0, 1.0};

        double p_mutation = 0.01;
        double fitness = 0;
        double probability = 0;
        bool alive = true;

        virtual void simulate(){};
        virtual void reproduce(Agent, Agent){};
        friend bool operator<(const Agent& a1, const Agent& a2)
        {
            return a1.fitness < a2.fitness; // keep the same order
        }
        ~Agent(){};
    };
}

#endif // !AGENT_H
