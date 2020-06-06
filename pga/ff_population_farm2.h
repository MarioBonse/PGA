#ifndef FF_POPULATION_H
#define FF_POPULATION_H

#include "population.h"
#include <ff/utils.hpp>
#include <ff/ff.hpp>
#include <ff/parallel_for.hpp>
#include <ff/farm.hpp>
#include <vector>
#include <algorithm>
#include <random>
#include <bits/stdc++.h> 
#include <math.h> 
#include "barrier.h"

// #define DEBUG 

namespace pga{
    template <class agent_type> 
    struct ff_population_farm : population<agent_type> {
        int curr_iterations;
        int nw;
        ff_population_farm(double P_to_keep, int number_agent, int nw): population<agent_type>(P_to_keep, number_agent)
        { 
            this->workers = nw;
        };

        void train(int);
    };

    struct task_type{
        int index;
        double cum_fitness;
    };

    template <class T> struct task{
        task(ff_population_farm<T>*P, int step, int start){
            A.reserve(step);
            for(int i = 0; i + start < P->size && i<step; i++)
                A.push_back(P->current_population[start + i]);
            extra_data.index = start/step;
        }
        std::vector<T> A;
        task_type extra_data;

    };

    // code of one of the worker of the farm. It takes a group of agent and simulate 
    // their behaviur, calculate the cumulative fitness and send the agent with the cum fitness
    template <typename T> struct simulation_ : ff::ff_node_t<task<T>>{
        simulation_(pga::ff_population_farm<T> *Pop){P = Pop;};
        pga::ff_population_farm<T>*P;

        task<T>* svc(task<T>* run) {
            double cum_fitness = 0.0;
            for(int i = 0; i < run->A.size(); i++){
                run->A[i].simulate();
                if(P->norm_type == linear)cum_fitness += run->A[i].get_fitness();
                else if (P->norm_type == softmax) cum_fitness += exp(run->A[i].get_fitness());
            }
            //std::sort(run->A.begin(), run->A.end(), std::greater<T>());
            // the reduce like seaction
            run->extra_data.cum_fitness = cum_fitness;
            //write cumulative fitness
            this->ff_send_out(run);
            return this->GO_ON;    
        }

    };


    //reproduce and normalize
    template <typename T> struct reproduce_ : ff::ff_node_t<task<T>>{

        reproduce_(ff_population_farm<T> *Pop){P = Pop;};

        pga::ff_population_farm<T>*P;
        
        task<T>* svc(task<T>* run) {
            // normalize
            int size = run->A.size();
            int index = run->extra_data.index;
            // for(int i = 0; i < size; i++){
            //     if(P->norm_type == linear)
            //         P->current_population[index*size + i].set_probability(P->current_population[index*size + i].get_fitness()/P->cum_fitness);
            //     else if(P->norm_type == softmax)
            //         P->current_population[index*size + i].set_probability(exp(P->current_population[index*size + i].get_fitness())/P->cum_fitness);

            // }
            // P->my_barrier.Wait();

            #ifdef DEBUG
            std::cout<<"Reproduction\n";
            #endif // DEBUG
            // copy or simulate section
            int number_to_copy = int(P->size*P->percentage_to_keep)/P->workers;
            // if we can't divide the agents to keep we add the spare part to the first worker
            int extra_shift = int(P->size*P->percentage_to_keep)%P->workers;
            if(index == 0)number_to_copy += extra_shift;
            for(int i = 0; i < run->A.size(); i++){
                if(i < number_to_copy){
                    run->A[i] = P->current_population[run->extra_data.index*number_to_copy + i + bool(run->extra_data.index)*extra_shift];
                }else{
                    int index1 = P->pick_random_parent();
                    int index2 = P->pick_random_parent();
                    run->A[i].reproduce(P->current_population[index1], P->current_population[index2]);
                }
            }
            this->ff_send_out(run);
            return this->GO_ON;   
        }

    };

    // merge the output from the simulation phase in sorted vector. 
    // then send out again to the reproduction phase
    template <typename T> struct collector_emitter : ff::ff_node_t<task<T>>{
        collector_emitter(ff_population_farm<T> *Pop){P = Pop;};
        std::vector<T> global_result;
        ff_population_farm<T>*P;
        int current_index = 0;
        task<T>* svc(task<T>* run) {
            // std::vector<T> app;
            // app.reserve(run->A.size() + global_result.size());
            // std::merge(run->A.begin(), run->A.end(), global_result.begin(), global_result.end(), std::back_inserter(app),  std::greater<T>());
            // global_result.swap(global_result, app);

            // merging the results
            for(int i = 0; i < run->A.size(); i++ ){
                P->current_population[current_index] = run->A[i];
                current_index++;
            }
            P->cum_fitness += run->extra_data.cum_fitness;
            delete run;
            #ifdef DEBUG
            std::cout<<"reciving from simuatioon\n";
            #endif // DEBUG

            if(current_index == P->size){
                #ifdef GET_STATISTICS
                ff::ffTime(ff::STOP_TIME);
                std::cout << "simulation took: " << ff::ffTime(ff::GET_TIME) << " (ms)\n";
                ff::ffTime(ff::START_TIME);
                #endif // GET_STATISTICS
                current_index = 0;

                P->sort();

                #ifdef GET_STATISTICS
                ff::ffTime(ff::STOP_TIME);
                std::cout << "sorting took: " << ff::ffTime(ff::GET_TIME) << " (ms)\n";
                ff::ffTime(ff::START_TIME);
                #endif // GET_STATISTICS
                P->normalize();
                // send out again
                #ifdef DEBUG
                std::cout<<"sending to reproduction\n";
                #endif // DEBUG
                int step = P->size/P->workers;
                for(int new_start = 0; new_start < P->size; new_start += step) {
                    this->ff_send_out(new task<T>(P, step, new_start));
                }
                #ifdef DEBUG
                std::cout<<"send all to reproduction\n";
                #endif // DEBUG

            }
            return this->GO_ON;

        }


    };

    // emitter: this function send the agents from current_population 
    // to the simulation section which is a farm. 
    template <typename T> struct emitter : ff::ff_node_t<task<T>>{
        emitter(pga::ff_population_farm<T> *Pop, int iter):P(Pop),iterations(iter){};

        // int iterations;
        pga::ff_population_farm<T>*P;
        int current_index = 0;
        int curr_iteration = 0;
        int iterations;

        task<T>* svc(task<T>* run) {
            if(run == nullptr){//sendo out the first time
                #ifdef DEBUG
                std::cout<<"Start [only once] sending out from emitter to sim\n";
                #endif // DEBUG
                
                #ifdef GET_STATISTICS
                ff::ffTime(ff::START_TIME);
                #endif // GET_STATISTICS
                int step = P->size/P->workers;
                for(int new_start = 0; new_start < P->size; new_start += step) {
                    this->ff_send_out(new task<T>(P, step, new_start));
                }
                return this->GO_ON; 
            }
            // copy the result into new_population
            #ifdef DEBUG
            std::cout<<"starting again. Reciving iteration before from reproduction\n";
            #endif // DEBUG
            for(int i = 0; i < run->A.size(); i++ ){
                P->new_population[current_index] = run->A[i];
                current_index++;
            }
            delete run;
            //until we don't fill the new population vector we just merge it
            if(current_index < P->size){
                return this->GO_ON;
            }     


            //eventually, if we still have iterations to do, we send the data again
            if(curr_iteration < iterations){
                curr_iteration ++;
                current_index = 0;
                P->iterations = curr_iteration;
                P->show_statistics();
                P->current_population.swap(P->new_population);
                P->cum_fitness = 0.0;
                int step = P->size/P->workers;
                #ifdef DEBUG
                std::cout<<"seding again. new iteration. TO simulate\n";
                #endif // DEBUG

                #ifdef GET_STATISTICS
                ff::ffTime(ff::STOP_TIME);
                std::cout << "reproduction took: " << ff::ffTime(ff::GET_TIME) << " (ms)\n";
                ff::ffTime(ff::START_TIME);
                #endif // GET_STATISTICS
                
                for(int new_start = 0; new_start < P->size; new_start += step) {
                    this->ff_send_out(new task<T>(P, step, new_start));
                }
                return this->GO_ON; 
            }
            return this->EOS;       
        }
    };

    template <class T> 
    void pga::ff_population_farm<T>::train(int iter){
        //opt.blocking_mode = true;
        this->curr_iterations = 0;
        this->size = this->size;
        emitter<T> E(this, iter);
        collector_emitter<T> CE(this);
         // create the farm for the simulation phase
        std::vector<std::unique_ptr<ff::ff_node>> S;
        for(int i = 0; i < this->workers; i++)S.push_back(ff::make_unique<simulation_<T>>(this));

        // farm for the reproduction phase
        std::vector<std::unique_ptr<ff::ff_node>> R;
        for(int i = 0; i < this->workers; i++)R.push_back(ff::make_unique<reproduce_<T>>(this));
        // create the farm and remove the collector
        ff::ff_Farm<task<T>> simulation_farm(std::move(S));
        // the second farm
        ff::ff_Farm<task<T>> reprodcution_farm(std::move(R));
        
        
        simulation_farm.add_emitter(E);
        reprodcution_farm.add_emitter(CE);
        simulation_farm.remove_collector();
        reprodcution_farm.remove_collector();

        ff::ff_Pipe pipe(simulation_farm, reprodcution_farm);

        // ff::optimize_static(pipe, opt);

        pipe.wrap_around();
        if (pipe.run_and_wait_end()<0) {
            std::cerr<<("running pipe");
            return ;
        }
        
    }
}

#endif // !FF_POPULATION_H
