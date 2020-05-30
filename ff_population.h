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



namespace pga{
    template <class T> 
    class ff_population : private population<T> {
        public:
        ff::ParallelForReduce<std::vector<T>>* parallel_simulation;
        ff::ParallelFor* parallel_reproduction;
        int workers;
        void add_agent(T& a){
            population<T>::add_agent(a);
        };
        T best_agent(){
            return population<T>::best_agent();
        };

        ff_population(double N_to_keep): population<T>(N_to_keep)
        {
            workers = 4;
            parallel_simulation = new ff::ParallelForReduce<std::vector<T>>(workers);
            parallel_reproduction = new ff::ParallelFor(workers);
        };

        ff_population(double N_to_keep, int nw): population<T>(N_to_keep)
        {
            workers = nw;
            parallel_simulation = new ff::ParallelForReduce<std::vector<T>>(nw);
            parallel_reproduction = new ff::ParallelFor(nw);
        };
        void simulate();
    };
}

template <class T> 
void pga::ff_population<T>::simulate(){
    // simluation with a parllel for reduce (the reduce for sorting)
    std::vector<T> simulated_agent;
    simulated_agent.reserve(population<T>::current_population.size());
    std::atomic<double> cum_fitness_atomic{0.0};
    parallel_simulation->parallel_reduce(simulated_agent, {}, 0, population<T>::current_population.size(),    // start, stop indexes
                            1,          // step size
                            0,          // chunk size (0=static, >0=dynamic) maybe dimanic is better for this task?
                            [&](int index, std::vector<T> &simulated_agent)  {
                                population<T>::current_population[index].simulate();
                                simulated_agent.push_back(population<T>::current_population[index]);
                                double to_add;
                                // update the global fitness
                                if(population<T>::norm_type == linear) to_add = population<T>::current_population[index].get_fitness();
                                else if (population<T>::norm_type == softmax)  to_add = exp(population<T>::current_population[index].get_fitness());
                                for (double g = cum_fitness_atomic; !cum_fitness_atomic.compare_exchange_strong(g, g + to_add););
                            },
                            [](std::vector<T> &simulated_agent, const std::vector<T>& elem) {
                                // the reduce part we merge in sorted order the result
                                //std::sort(elem.begin(), elem.end(), std::greater<T>());
                                std::vector<T> out = simulated_agent; 
                                std::merge(out.begin(), out.end(), elem.begin(), elem.end(), simulated_agent.begin());
                            }
                            );
    population<T>::cum_fitness = cum_fitness_atomic;
    population<T>::normalize();
    parallel_reproduction->parallel_for(0, population<T>::current_population.size(),    // start, stop indexes
                        1,          // step size
                        0,          // chunk size (0=static, >0=dynamic) maybe dimanic is better for this task?
                        [&](int index)  {
                            if(index < int(population<T>::current_population.size()*population<T>::percentage_to_keep)){
                                population<T>::new_population[index] = population<T>::current_population[index];
                            }else{
                                int index1 = population<T>::pick_random_parent();
                                int index2 = population<T>::pick_random_parent();
                                population<T>::new_population[index].reproduce(population<T>::current_population[index1], population<T>::current_population[index2]);
                            }
                        });
}

#endif // !FF_POPULATION_H
