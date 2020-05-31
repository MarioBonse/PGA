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


union task_type{
    int index;
    double cum_fitness;
};

template <class T> 
struct task{
    std::vector<T>* A;
    task_type extra_data;
};

// code of one of the worker of the farm. It takes a group of agent and simulate 
// their behaviur, calculate the cumulative fitness and send the sorted agent with the cum fitness
template <class T> 
struct simulation_: ff_node_t<task<T>> {
    task<T>* svc(task<T>* run) {
        double cum_fitness = 0.0;
        for(auto r:result){
            r.simulate();
            if(P->norm_type == linear)cum_fitness += r.get_fitness();
            else if (P->norm_type == softmax) += exp(r.get_fitness());
        }
        std::sort(run->A.begin(), run->A.end(), std::greater<T>());
        // the reduce like seaction
        run->extra_data.cum_fitness = cum_fitness;
        //write cumulative fitness
        ff_send_out(run);
        return GO_ON;    
    }

    simluation(pga::ff_population_farm<T> *Pop):P(Pop);
    pga::ff_population_farm<T>*P;
};


//reproduce and normalize
template <class T> 
struct reproduce_: ff_node_t<task<T>> {
    task<T>* svc(task<T>* run) {
        // normalize
        // for(auto r:result){
        //     r.set_probabilty(r.get_fitness()/cum_fitness);
        // }
        // wait barrier

        // copy or simulate section
        int number_to_copy = int(run->A.size()*P->percentage_to_keep);
        for(int i = 0; i < run->A.size(); i++){
            if(i < number_to_copy){
                run->A[i] = P->current_population[run->extra_data.index*number_to_copy + i];
            }else{
                int index1 = P->pick_random_parent();
                int index2 = P->pick_random_parent();
                run->A[i].reproduce(P->current_population[index1], P->current_population[index2]);
            }
        }
        ff_send_out(run);
        return GO_ON;   
    }
    reproduce_(pga::ff_population_farm<T> *Pop):P(Pop);
    pga::ff_population_farm<T>*P;
};

// merge the output from the simulation phase in sorted vector. 
// then send out again to the reproduction phase
template <class T> 
struct collector_: ff_node_t<task<T>> {
    task<T>* svc(task<T>* run) {
        task<T> app;
        std::merge(run->A.begin(), run->A.end(), global_result.begin(), global_result.end(), std::back_inserter(app));
        std::swap(global_result, app);

        P->cum_fitness += run->extra_data.cum_fitness;

        if(global_result.size() ==  P->size){
            P->normalize();
            P->current_population = global_result;
            // send out again
            int step = (0 - P->size)/P->nw;
            int new_start = 0;
            task<T> *send;
            send->A.reserve(step);
            for(; new_start < P->size; new_start += step) {
                for(int i = 0; i < step; i ++){
                    send->A.append(P->current_population[new_start + i]);
                }
                send->extra_data.index = new_start/step;//if needed
                ff_send_out(send);
            }

        }
        return GO_ON;

    }
    collector_emitter(pga::ff_population_farm<T> *Pop):P(Pop);
    std::vector<T> global_result;
    pga::population<T>*P;

};

// emitter: this function send the agents from current_population 
// to the simulation section which is a farm. 
template <class T> 
struct emitter: ff_node_t<task<T>>{
    task<T>* svc(task<T>* run) {
        if(run == nullptr){//sendo out the first time
            curr_iteration ++;
            int step = (0 - P->size)/P->nw;
            int new_start = 0;
            task<T> *send;
            send->A.reserve(step);
            for(; new_start < P->size; new_start += step) {
                for(int i = 0; i < step; i ++){
                    send->A.append(P->current_population[new_start + i]);
                }
                send->extra_data.index = new_start/step;//if needed
                ff_send_out(send);
            }
            current_index = 0;
            return GO_ON; 
        }
        // copy the result into new_population
        for(int i = 0; i < run->A.size(); i++ ){
            P->new_population[current_index] = run->A[i];
            current_index++;
        }
        //until we don't fill the new population vector we just merge it
        if(current_index < P->size){
            return GO_ON;
        }
        P->show_statistics();
        current_index = 0;
        //eventually, if we still have iterations to do, we send the data again
        if(curr_iteration < P->iterations){
            std::swap(P->current_population, P->new_population);
            curr_iteration ++;
            int step = (0 - P->size)/P->nw;
            int new_start = 0;
            task<T> *send;
            send->A.reserve(step);
            for(; new_start < P->size; new_start += step) {
                for(int i = 0; i < P->size && i < step; i ++){
                    // copy into the send structure the data we want to send
                    send->A.append(P->current_population[new_start + i]);
                }
                send->extra_data.index = new_start/step;//if needed
                ff_send_out(send);
            }
            return GO_ON; 
        }
        return EOS;       
    }
    emitter(pga::ff_population_farm<T> *Pop):P(Pop);
    pga::population<T>*P;
    int current_index = 0;
};



namespace pga{
    template <class T> 
    class ff_population_farm : private population<T> {
        public:
        int workers;
        int size;
        int interations;
        void add_agent(T& a){
            population<T>::add_agent(a);
        };
        T best_agent(){
            return population<T>::best_agent();
        };

        ff_population(double N_to_keep, int iter = 1, int workers = 4): population<T>(N_to_keep)
        {
            interations = iter;
            workers = nw;
        };

        void simulate();
    };
}

template <class T> 
void pga::ff_population<T>::simulate(){
    size = current_population.size();
    emitter E(this);
    collector_emitter CE(this);

    std::vector<std::unique_ptr<ff_node>> S;
    for(int i = 0; i < workers; i++)S.push_back(make_unique<simulation_>(this));

    std::vector<std::unique_ptr<ff_node>> R;
    for(int i = 0; i < workers; i++)R.push_back(make_unique<reproduce_>(this));
    // create the farm and remove the collector
    ff_Farm<std::vector<ull>> simulation_farm(std::move(E));
    simulation_farm.add_emitter(E);
    simulation_farm.add_collector(CE);
    ff_Farm<std::vector<ull>> reprodcution_farm(std::move(R));
    reprodcution_farm.add_emitter(CE);
    reprodcution_farm.add_collector(E);
    ff_sequential
    if (pipe.run_and_wait_end()<0) {
        error("running pipe");
        return -1;
    }
    // simluation with a parllel for reduce (the reduce for sorting)
    
}

#endif // !FF_POPULATION_H
