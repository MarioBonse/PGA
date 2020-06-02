#ifndef FF_POPULATION_H
#define FF_POPULATION_H
    
#include "population.h"
#include <ff/utils.hpp>
#include <ff/ff.hpp>
#include <ff/parallel_for.hpp>
#include <vector>
#include <algorithm>
#include <random>
#include <bits/stdc++.h> 
#include <math.h> 
#include <atomic>


namespace pga{
    template <class T> 
    struct ff_population : population<T> {
        public:
        ff::ParallelForReduce<std::vector<T>>* parallel_simulation;
        ff::ParallelFor* parallel_reproduction;
        int workers;

        ff_population(double N_to_keep, int agents, int nw): population<T>(N_to_keep, agents)
        {
            workers = nw;
            parallel_simulation = new ff::ParallelForReduce<std::vector<T>>(workers);
            parallel_reproduction = new ff::ParallelFor(workers);
        };

        ff_population(double N_to_keep, int nw): population<T>(N_to_keep)
        {
            workers = nw;
            parallel_simulation = new ff::ParallelForReduce<std::vector<T>>(nw);
            // parallel_reproduction = new ff::ParallelFor(nw);

        };
        void simulate(int);
    };


    template <class T> 
    void ff_population<T>::simulate(int iterations){
        // simluation with a parllel for reduce (the reduce for sorting)
        for(int i = 0; i<iterations;i++){
            std::vector<T> simulated_agent;
            simulated_agent.reserve(this->current_population.size());
            std::atomic<double> cum_fitness_atomic{0.0};
            parallel_simulation->parallel_reduce(simulated_agent, {}, 0, this->current_population.size(),    // start, stop indexes
                                    1,          // step size
                                    0,          // chunk size (0=static, >0=dynamic) maybe dimanic is better for this task?
                                    [&](int index, std::vector<T> &simulated_agent)  {
                                        this->current_population[index].simulate();
                                        T to_add_agent = this->current_population[index];
                                        simulated_agent.push_back(to_add_agent);
                                        double to_add;
                                        // update the global fitness
                                        if(this->norm_type == linear) to_add = this->current_population[index].get_fitness();
                                        else if (this->norm_type == softmax)  to_add = exp(this->current_population[index].get_fitness());
                                        for (double g = cum_fitness_atomic; !cum_fitness_atomic.compare_exchange_strong(g, g + to_add););
                                    },
                                    [](std::vector<T> &simulated_agent, const std::vector<T>& elem) {
                                        // the reduce part we merge in sorted order the result
                                        std::sort(elem.begin(), elem.end(), std::greater<T>());
                                        std::vector<T> out; 
                                        out.reserve(simulated_agent.size() + elem.size());

                                        std::merge(simulated_agent.begin(), simulated_agent.end(), elem.begin(), elem.end(),  std::back_inserter(out),  std::greater<T>());
                                        std::swap(simulated_agent, out);
                                        }
                                    );
            this->cum_fitness = cum_fitness_atomic;
            this->normalize();
            parallel_reproduction->parallel_for(0, this->current_population.size(),    // start, stop indexes
                                1,          // step size
                                0,          // chunk size (0=static, >0=dynamic) maybe dimanic is better for this task?
                                [&](int index)  {
                                    if(index < int(this->current_population.size()*this->percentage_to_keep)){
                                        this->new_population[index] = this->current_population[index];
                                    }else{
                                        int index1 = this->pick_random_parent();
                                        int index2 = this->pick_random_parent();
                                        this->new_population[index].reproduce(this->current_population[index1], population<T>::current_population[index2]);
                                    }
                                });
            this->show_statistics();
            this->cum_fitness = 0;
            std::swap(this->current_population, this->new_population);
        }
    }
}


#endif // !FF_POPULATION_H
