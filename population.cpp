#include "population.h"


template <class T> pga::population<T>::population(){  
    std::cout<<"create new population\n";
}

template <class T> void pga::population<T>::normalize(){
    T cumsum = 0.0;
    for(auto a: current_population){
        cumsum+=a.fitness;
    }
    for(auto a: current_population){
        a.probability = double(a.fitness)/double(cumsum);
    }
}
 

/* 
sample one parent with probability 
proportional to the fitness
*/
template <class T> int pga::population<T>::pick_random_parent(){
    double sample = dis(gen);
    double cum_sum = 0;
    int index = -1;
    while(sample > cum_sum){
        index ++;
        cum_sum += current_population[index].fitness;
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
template <class T> void pga::population<T>::simulate(){
    for(auto a: current_population){
        a.simulate();
    }
    // reproduce
    // create the pool of parents
    sort();
    normalize();

    for(int i = 0; i<current_population.size(); i++){
        int index1 = pick_random_parent();
        int index2 = pick_random_parent();
        new_population[i].reproduce(current_population[index1], current_population[index2]);
    }
    std::swap(current_population, new_population);
}

/*
Returns the best agent until now 
*/
template <class T> T pga::population<T>::best_agent(){
    std::sort(new_population.begin(), new_population.end());

    return new_population[0];
}

/*
Add a new agent to the pool
*/
template <class T> void pga::population<T>::add_agent(T &a){
    T new_agent1 = a;
    T new_agent2 = a;
    current_population.push_back(new_agent1);
    new_population.push_back(new_agent2);
}
