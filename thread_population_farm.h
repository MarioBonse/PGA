#include <thread>
#include <vector>
#include "population.h"
#include <atomic>

namespace pga{

    typedef struct {
        int start;
        int end;
    }RANGE;


    template <typename T> 
    struct thread_population : population<T> {
        int nw;
        int curr_iterations;

        thread_population(double P_to_keep, int number_agent, int nworw): population<T>(P_to_keep, number_agent)
        {
            nw = nworw;
            this->workers = nw;
            //my_barrier.set_barrier(nw);
        };

        void merge(std::vector<RANGE> &);
        void simulate(int);
    };


    template <typename T>
    void thread_population<T>::merge(std::vector<RANGE> & range){
        int start = range[0].end;
        for(int i = 1; i < range.size(); i++){
            std::inplace_merge(this->current_population.begin(), this->current_population.begin() + range[i].start,
                this->current_population.begin() + range[i].end, std::greater<T>());
        }
    }

    template <typename T>
    void thread_population<T>::simulate(int iterations){

        for(int i = 0; i< iterations; i++){

            std::atomic<double> cum_fitness_atomic{0.0};

            std::vector<RANGE> ranges(this->workers);                     // vector to compute the ranges 
            int delta { this->size / this->workers };
            std::vector<std::thread> tids;
            
            for(int i=0; i<this->workers; i++) {                     // split the board into peaces
                ranges[i].start = i*delta;
                ranges[i].end   = (i != (this->workers-1) ? (i+1)*delta : this->size); 
            }

            auto simulations_phase = [&](RANGE r){
                    double to_add = 0.0;
                    for(int i = r.start; i < r.end; i++){
                        this->current_population[i].simulate();
                        // TODO atomic variable for cum_fitness
                        if(this->norm_type == linear)to_add += this->current_population[i].get_fitness();
                        else if (this->norm_type == softmax) to_add += exp(this->current_population[i].get_fitness());
                    }
                    for (double g = cum_fitness_atomic; !cum_fitness_atomic.compare_exchange_strong(g, g + to_add););
                    std::sort(this->current_population.begin() + r.start, this->current_population.begin() + r.end,std::greater<T>());
                };

            auto reproduction_phase = [&](RANGE r){
                    //calculate how many agents we want to keep 
                    int number_to_copy = int(this->size*this->percentage_to_keep)/this->workers;
                    int extra_shift = int((this->size)* (this->percentage_to_keep))%this->workers;
                    int index = r.start/delta;
                    if(index == 0)number_to_copy += extra_shift;

                    for(int i = r.start; i < r.end; i++){
                    // if we can't divide the agents to keep we add the spare part to the first worker
                        if(i < number_to_copy){
                            this->new_population[i] = this->current_population[index * number_to_copy + i + extra_shift];
                        }else{
                            int index1 = this->pick_random_parent();
                            int index2 = this->pick_random_parent();
                            this->new_population[i].reproduce(this->current_population[index1], this->current_population[index2]);
                        }
                    }
            };
            
            for(int i=0; i<nw; i++) {                     // assign chuncks to threads
                tids.push_back(std::thread(simulations_phase, ranges[i]));
            }
            
            for(std::thread& t: tids) {                        // await thread termination
                t.join();
            }
            this->cum_fitness = cum_fitness_atomic;
            this->merge(ranges);
            this->normalize();

            tids.clear();
            for(int i=0; i<nw; i++) {                     // assign chuncks to threads
                tids.push_back(std::thread(reproduction_phase, ranges[i]));
            }
            
            for(std::thread& t: tids) {                        // await thread termination
                t.join();
            }
            this->show_statistics();
            this->cum_fitness = 0.0;
            std::swap(this->current_population, this->new_population);

        }
    }   

}