
#ifndef POPULATION_H
#define POPULATION_H  
#include <vector>
#include "agent.h"


namespace pga{
    template <class T> class population{
        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
        std::uniform_real_distribution<double> dis(0, 1.0);

        std::vector<T> current_population;
        std::vector<T> new_population;
        
        public:
        population();

        void normalize();

        void sort(){std::sort(current_population.begin(), current_population.end());};

        void add_agent(T &a);

        int pick_random_parent();

        T best_agent(); //return the best agent from the previus simulation

        void simulate();

    };
}


#endif