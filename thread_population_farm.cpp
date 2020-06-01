#include <thread>
#include <vector>


namespace pga{

    typedef struct {
        int start;
        int end;
    }RANGE;


    template <typename T> 
    struct thread_population : population<T> {



        int curr_iterations;

        thread_population(double P_to_keep, int number_agent, int nw): population<agent_type>(P_to_keep, number_agent)
        {
            
            this->workers = nw;
            my_barrier.set_barrier(nw);
        };

        void merge(std::vector<RANGE> &);
        void simulate(int);
    };


    template <typename T>
    void thread_population<T>::merge(std::vector<RANGE> & range){
        std::vector<T> app;
        app.reserve(this->size);
        int start = 0;
        for(int i = 1; i < range.size(); i++){
            std::merge(this->current_population, this->current_population + range[i].start - 1; 
                this->current_population + range[i].start, this->current_population + range[i].end, app, std::greater<T>());
        }
    }

    template <typename T>
    void thread_population<T>::simulate(int interations){

        int range_size; 


        for(int i = 0; i< iterations; i++){
            std::vector<RANGE> ranges(this->workers);                     // vector to compute the ranges 
            int delta { this->size / this->workers };
            std::vector<std::thread> tids;
            
            for(int i=0; i<this->workers; i++) {                     // split the board into peaces
                ranges[i].start = i*delta;
                ranges[i].end   = (i != (this->workers-1) ? (i+1)*delta : this->size); 
            }
            auto simulations_phase = [&](RANGE r){
                    for(int i = r.start; i < r.end; i++){
                        this->current_population[i].simulate();
                        if(P->norm_type == linear)cum_fitness += run->A[i].get_fitness();
                        else if (P->norm_type == softmax) cum_fitness += exp(run->A[i].get_fitness());
                    }
                    std::sort(this->current_population + r.start, this->current_population + r.end);
                };

            auto reproduction_phase = [&](RANGE r){
                    //calculate how many agents we want to keep 
                    int number_to_copy = int(this->size*this->percentage_to_keep)/this->workers;
                    int extra_shift = int((this->size)* (this->percentage_to_keep))%this->workers;
                    if(index == 0)number_to_copy += extra_shift;
                    int index = r.start/range_size;
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

            this->merge(r);

            tids.clear();
            for(int i=0; i<nw; i++) {                     // assign chuncks to threads
                tids.push_back(std::thread(simulations_phase, ranges[i]));
            }
            
            for(std::thread& t: tids) {                        // await thread termination
                t.join();
            }

        }
    }   

}