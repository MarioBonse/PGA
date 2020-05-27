#ifndef AGENT_H
#define AGENT_H
#include <random>
#include <iostream>

namespace pga{
    class Agent{
        protected:
        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937 gen{rd()}; //Standard mersenne_twister_engine seeded with rd()
        std::uniform_real_distribution<double> dis{0.0, 1.0};

        double p_mutation;
        double fitness;
        double probability;
        bool alive;

        public:
        double get_probability(){return probability;};
        double get_fitness(){return fitness;};
        void set_probability(double prob){probability = prob;};
        
        // function that explicit how to simulate one iteration of an agent
        virtual void simulate(){};

        // how to create the new agent given two parents
        virtual void reproduce(const Agent &,const Agent &){};
        ~Agent(){};

        // function called after one epoch. 
        // You should use it in order to plot whatever statistics you want from your
        // agent
        void show_statistics(const std::vector<Agent> &){};

    };
}

#endif // !AGENT_H
