#include <vector>
#include "population.h"
#include <thread>

#ifdef GET_STATISTICS
#include <chrono>
#endif

namespace pga{

    typedef struct {
        int start;
        int end;
    }RANGE;


    template <typename T> 
    struct omp_population : population<T> {
        int nw;
        int curr_iterations;

        omp_population(double P_to_keep, int number_agent, int nworw): population<T>(P_to_keep, number_agent)
        {
            nw = nworw;
            this->workers = nw;
            //my_barrier.set_barrier(nw);
        };

        void merge(std::vector<RANGE> &);
        void simulate(int);
    };


    template <typename T>
    void omp_population<T>::merge(std::vector<RANGE> & range){
        int start = range[0].end;
        for(int i = 1; i < range.size(); i++){
            std::inplace_merge(this->current_population.begin(), this->current_population.begin() + range[i].start,
                this->current_population.begin() + range[i].end, std::greater<T>());
        }
    }
}

template <class T> 
void pga::omp_population<T>::simulate(int iter){
    for(int i = 0; i<iter; i++){
        this->cum_fitness = 0.0;
        #ifdef GET_STATISTICS
        auto start = std::chrono::high_resolution_clock::now();
        #endif // GET_STATISTICS

        double cum_fitness = 0.0;
        // the best schedule depends on the task. Some task have very different behaviur between agents
        #pragma omp parallel for num_threads(nw)  reduction(+:cum_fitness) schedule(dynamic) 
        for(int i = 0; i< this->current_population.size(); i++){
            this->current_population[i].simulate();
            if(this->norm_type == linear)cum_fitness += this->current_population[i].get_fitness();
            else if(this->norm_type == softmax) cum_fitness += exp(this->current_population[i].get_fitness());
        }
        this->cum_fitness = cum_fitness;
        #ifdef GET_STATISTICS
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        auto usec    = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        std::cout <<"simulation took: "<< usec << std::endl;
        #endif // GET_STATISTICS
        // reproduce
        // create the pool of parents
        this->iterations++;

        #ifdef GET_STATISTICS
        start = std::chrono::high_resolution_clock::now();

        #endif // GET_STATISTICS
        this->sort();
        #ifdef GET_STATISTICS
        elapsed = std::chrono::high_resolution_clock::now() - start;
        usec    = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        std::cout <<"sorting took: "<< usec << std::endl;
        #endif // GET_STATISTICS


        #ifdef GET_STATISTICS
        start = std::chrono::high_resolution_clock::now();

        #endif // GET_STATISTICS

        this->normalize();

        #ifdef GET_STATISTICS
        elapsed = std::chrono::high_resolution_clock::now() - start;
        usec    = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        std::cout <<"normalization took: "<< usec << std::endl;
        #endif // GET_STATISTICS

        #ifdef GET_STATISTICS
        start = std::chrono::high_resolution_clock::now();

        #endif // GET_STATISTICS


        #pragma omp parallel for num_threads(nw) schedule(dynamic)
        for(int i = 0; i<this->current_population.size(); i++){
            // keep the best part of the population
            if(i < int(this->current_population.size()*this->percentage_to_keep)){
                this->new_population[i] = this->current_population[i];
            }else{
                int index1 = this->pick_random_parent();
                int index2 = this->pick_random_parent();
                this->new_population[i].reproduce(this->current_population[index1], this->current_population[index2]);
            }
        }
        #ifdef GET_STATISTICS
        elapsed = std::chrono::high_resolution_clock::now() - start;
        usec    = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        std::cout <<"reproduce took: "<< usec << std::endl;
        #endif // GET_STATISTICS

        #ifndef GET_STATISTICS
        this->show_statistics();
        #endif // GET_STATISTICS
        this->current_population.swap(this->new_population);
    }
}