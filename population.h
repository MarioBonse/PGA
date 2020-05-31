
#ifndef POPULATION_H
#define POPULATION_H  
#include <vector>
#include <algorithm>
#include <random>
#include <bits/stdc++.h> 
#include <math.h> 
enum normalization { linear, softmax};

#define GET_STATISTICS
#ifdef GET_STATISTICS
#include <chrono>
#endif

namespace pga{
    template <class T> class population{
        protected:
        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937 gen{rd()}; //Standard mersenne_twister_engine seeded with rd()
        
        std::uniform_real_distribution<> dis{double(0.0) , double(1.0)};

        std::vector<T> current_population;
        std::vector<T> new_population;
        // cumulative fitness
        double cum_fitness; 
        int iterations = 0;
        double percentage_to_keep;
        normalization norm_type = softmax;
        
        public:
        population(double N_to_keep):percentage_to_keep(N_to_keep){};
        void linear_normilize();
        void normalize();
        void linear_normalize();
        void softmax_normalize();

        void sort(){std::sort(current_population.begin(), current_population.end(), std::greater<T>());};
        void add_agent(T &a);

        int pick_random_parent();

        T best_agent(); //return the best agent from the previus simulation

        void simulate();
        void show_statistics();

    };
}


template <class T> 
void pga::population<T>::show_statistics(){
    double avg_fitness = cum_fitness/double(current_population.size());
    std::cout<<"After "<<iterations<<" we have an average fitness of "<<avg_fitness;
    // function that we can override in order to plot some statistics of the best agent
    current_population[0].show_statistics(current_population);
    std::cout<<std::endl;
}

template <class T> 
void pga::population<T>::normalize(){
    if(norm_type == linear)linear_normalize();
    else if (norm_type == softmax)softmax_normalize();
    else std::cerr<<"Normalization not supported\n";
}

template <class T> 
void pga::population<T>::linear_normalize(){
    for(int i = 0; i< current_population.size(); i++){
        current_population[i].set_probability( current_population[i].get_fitness()/cum_fitness);
    }
}
 
template <class T> 
void pga::population<T>::softmax_normalize(){
    for(int i = 0; i< current_population.size(); i++){
        current_population[i].set_probability( exp(current_population[i].get_fitness())/cum_fitness);
    }
}
 

/* 
sample one parent with probability 
proportional to the fitness
*/
template <class T> 
int pga::population<T>::pick_random_parent(){
    double sample = dis(gen);
    double cum_sum = 0;
    int index = -1;
    while(sample > cum_sum || index > current_population.size()){
        index ++;
        cum_sum += current_population[index].get_probability();
    }
    return index;
}


/*
One step of simulation.
1) It simulates the behaviur of all the agents. The simulation step has to implement update the fitness
2) it sorts the agent in respect to the fitness (bacuse it optimize the next operation)
3) sample two parents with probability proportional to the fitness
4) create the new pool of agent
*/
template <class T> 
void pga::population<T>::simulate(){
    cum_fitness = 0.0;
    #ifdef GET_STATISTICS
    auto start = std::chrono::high_resolution_clock::now();
    #endif // GET_STATISTICS
    for(int i = 0; i< current_population.size(); i++){
        current_population[i].simulate();
        if(norm_type == linear)cum_fitness += current_population[i].get_fitness();
        else if(norm_type == softmax) cum_fitness += exp(current_population[i].get_fitness());
    }
    #ifdef GET_STATISTICS
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    auto usec    = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    std::cout <<"simulation took: "<< usec << std::endl;
    #endif // GET_STATISTICS
    // reproduce
    // create the pool of parents
    iterations++;

    #ifdef GET_STATISTICS
    start = std::chrono::high_resolution_clock::now();

    #endif // GET_STATISTICS
    sort();
    #ifdef GET_STATISTICS
    elapsed = std::chrono::high_resolution_clock::now() - start;
    usec    = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    std::cout <<"sorting took: "<< usec << std::endl;
    #endif // GET_STATISTICS

    normalize();



    #ifdef GET_STATISTICS
    start = std::chrono::high_resolution_clock::now();

    #endif // GET_STATISTICS

    for(int i = 0; i<current_population.size(); i++){
        // keep the best part of the population
        if(i < int(current_population.size()*percentage_to_keep)){
            new_population[i] = current_population[i];
        }else{
            int index1 = pick_random_parent();
            int index2 = pick_random_parent();
            new_population[i].reproduce(current_population[index1], current_population[index2]);
        }
    }
    #ifdef GET_STATISTICS
    elapsed = std::chrono::high_resolution_clock::now() - start;
    usec    = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    std::cout <<"reproduce took: "<< usec << std::endl;
    #endif // GET_STATISTICS

    #ifndef GET_STATISTICS
    show_statistics();
    #endif // GET_STATISTICS
    current_population.swap(new_population);
}

/*
Returns the best agent until now 
*/
template <class T> 
T pga::population<T>::best_agent(){
    std::sort(new_population.begin(), new_population.end());

    return new_population[0];
}

/*
Add a new agent to the pool
*/
template <class T> 
void pga::population<T>::add_agent(T &a){
    T new_agent2 = a;
    current_population.push_back(a);
    new_population.push_back(new_agent2);
}


#endif