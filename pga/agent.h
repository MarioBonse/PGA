#ifndef AGENT_H
#define AGENT_H
#include <random>
#include <iostream>


/*
Abstract class for the agents. 
simulate and reproduce has to be overwritten
*/
namespace pga{
    template <typename T> class Agent{
        public:
        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937 gen{rd()}; //Standard mersenne_twister_engine seeded with rd()
        std::uniform_real_distribution<double> dis{0.0, 1.0};

        double p_mutation;
        double fitness;
        double probability;
        bool alive;
 
        double get_probability() const{return probability;};
        double get_fitness() const {return fitness;};
        void set_probability(double prob){probability = prob;};
        
        // function that explicit how to simulate one iteration of an agent
        virtual void simulate() = 0;

        // how to create the new agent given two parents
        friend bool operator<(const T& a1, const T& a2)
        {
            return a1.fitness < a2.fitness; // keep the same order
        };

        friend bool operator>(const T& a1, const T& a2)
        {
            return a1.fitness > a2.fitness; // keep the same order
        };

        virtual void reproduce(const T &,const T &) = 0;
        ~Agent(){};

        // function called after one epoch. 
        // You should use it in order to plot whatever statistics you want from your
        // agent. Could also not be implemented
        void show_statistics(const std::vector<T> &) {};

    };
}

#endif // !AGENT_H
