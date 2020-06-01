#ifndef TSPAGENT_CPP
#define TSPAGENT_CPP
#include "tspAgent.h"
#include <algorithm>

std::vector<std::vector<int>> my_graph;

std::vector<int> find_candidates(std::vector<int> nodes, std::vector<int> visited){
    std::vector<int> candidates;
    for(int i = 0; i<nodes.size(); i++){
        // if the edge exists and we never visited it 
        if(nodes[i] && !visited[i]){
            candidates.push_back(i);
        }
    }
    return candidates;
}

int find(std::vector<int> candidates, int node){
    for(auto c: candidates){
        if(c == node)return c;
    }
    return 0;
}


void pga::TSPAgent::mutate(){
    for(int i = 0; i<size;i++){
        double sample = dis(gen);
        if(sample < p_mutation){
            int node_2 = rand()%size;
            // switch with the random node
            int app = DNA[i];
            DNA[i] = DNA[node_2];
            DNA[node_2] = app;
        }
    }
}

pga::TSPAgent& pga::TSPAgent::operator=(const pga::TSPAgent& a2){
    if(this != &a2){
        DNA = a2.DNA;
        fitness = a2.fitness;
        p_mutation = a2.p_mutation;
        size = a2.size;
        probability = a2.probability;
        chromosome_length = a2.chromosome_length;
        path_lenght = a2.path_lenght;
    }
    return *this;   
}



int next_random_node(std::vector<int> nodes, std::vector<int> visited){
    
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(0, 1.0);
    std::vector<int> candidates = find_candidates(nodes, visited);
    int num_candidates = candidates.size();
    if (num_candidates == 0)return -1;
    if (num_candidates == 1) return candidates[0];
    int rand_index = rand() % num_candidates;
    return candidates[rand_index];
}

int pga::TSPAgent::find_nearer_avail_node(int starting_node, std::vector<int> visited){
    int index = -1;
    int length = INT32_MAX;
    for(int i = 0; i<size; i++){
        if (!visited[i] && (my_graph[starting_node][i] < length)){length = my_graph[starting_node][i]; index = i;}
    }
    return index;
}

// funtion for init the DNA with greedy version of the path starting from a random city
void pga::TSPAgent::greedy_DNA(){
    int node = rand()%size;
    std::vector<int> visited(size);
    for(int i = 0; i < size; i++){
        DNA[i] = node;
        visited[node] = 1;
        node = find_nearer_avail_node(node, visited);
        if(i < size - 1 && node == -1){
            std::cerr<<"ERROR IT IS NOT EXPECTED not to find nearer node\n";
            return;
        }
    }
    mutate();
}

void pga::TSPAgent::random_DNA(){
    // pick random starting node
    std::vector<int> visited(size);
    int current_node = rand() % size;
    DNA[0] = current_node;
    for(int i = 1; i<size; i++){
        visited[current_node] = 1;
        //pick random second edge
        int next_node = next_random_node(my_graph[current_node], visited);
        if(next_node == -1) {DNA[i] = -1; alive = false; return;}
        DNA[i] = next_node;
        current_node = next_node;
    }
}

pga::TSPAgent::TSPAgent(int chromosome_len, double p_mutati = 0.01)
{
    chromosome_length = chromosome_len; 
    p_mutation = p_mutati;
    size = my_graph[0].size();
    DNA.resize(size);
    greedy_DNA();
}

pga::TSPAgent::TSPAgent(const TSPAgent &a)
{
    DNA = a.DNA;
    fitness = a.fitness;
    size = a.size; 
    chromosome_length = a.chromosome_length; 
    p_mutation = a.p_mutation;
    path_lenght = a.path_lenght;
}

pga::TSPAgent::TSPAgent(double p_mutati = 0.01){
    p_mutation = p_mutati;
    size = my_graph[0].size();
    chromosome_length = size/2;
    DNA.resize(size);
    greedy_DNA();
}

/* 
for calculate the fitness just calculate the lenght of the path. If the path does not exists 
the fitness is 0
*/
void pga::TSPAgent::simulate(){
    path_lenght = 0;
    int start = DNA[0], end;
    for(int i = 1; i < DNA.size();i++){
        end = DNA[i];
        if(end == -1){fitness = 0; return;}
        if(my_graph[start][end]) 
            path_lenght += my_graph[start][end];
        else {fitness = 0; return;}
        start = end;
    }
    path_lenght += my_graph[0][size - 1];
    // the fitness has to be > 0 and higher for better solution so we invert the lenght of the path
    fitness = 1.0/double(path_lenght);
}


/* 
first method: 
p1 = 1-4-5-6-2-3-7-8 
p2 = 4-5-6-3-2-8-7-1
-----------------------
Pick pieces of the first DNA (e.d. 1-4-5-*-*-*-7-8)
then complete with * with nodes from p2 in order if possible -> 6-3-2.
With probability p_mutation one node of the path(aka a chromosome of the DNA) is picked randomly 

I'M ASSUMING THATH CHE GRAPH IS FULLY CONNECTED, FROM EACH NODE I CAN GO TO EVERY OTHER NODE
*/
void pga::TSPAgent::reproduce(const pga::TSPAgent &p1,const  pga::TSPAgent &p2){
    size = p1.size;
    p_mutation = p1.p_mutation;
    chromosome_length = p1.chromosome_length;
    int n_chromosome = size/chromosome_length;
    std::vector<int> visited(size);
    int start;

    // clone the pieece from the first parent
    for(int chromosome = 0; chromosome < (size/chromosome_length) + 1; chromosome += 2){
        for(int i = chromosome_length*chromosome; i < size && i < chromosome_length*(chromosome + 1); i++){
            DNA[i] = p1.DNA[i];
            visited[DNA[i]] = 1;
        }
    }
    //now clone the piece from the second parent
    for(int chromosome = 1; chromosome < (size/(chromosome_length)) + 1; chromosome += 2){
        int i = chromosome_length*chromosome; 
        while(i < size && i < chromosome_length*(chromosome + 1)){
            // here we don't search for mutation. We just search from where we are if we are able to 
            // find a path that uses nodes from order in DNA 2
            // find from start a "free node" and pick the first one present in p2
            // std::vector<int> candidates = find_candidates(graph[start], visited);
            // if I have no possible nodes then the agent is dead then the fitness is 0
            // if(candidates.size() == 0){DNA[i] = -1; alive = false; return;}

            int DNA2_index = 0;
            // increase the index of DNA2 until we find a node never visited 
            // the first available nodes. Then from this point one we try to continue to copy until we reach
            // a visited node 

            // FIND THE FIRST NODE NEVER VISITED
            while(DNA2_index < size && (visited[p2.DNA[DNA2_index]]))DNA2_index++;
            // until we don't find an already visited node we copy the DNA from parent2.
            // we check also not to exit from the window(aka chromosome) we are copyng
            while( i < size && i < chromosome_length*(chromosome + 1) 
                                                && !visited[p2.DNA[DNA2_index]]){
                DNA[i] = p2.DNA[DNA2_index];
                visited[p2.DNA[DNA2_index]] = true;
                DNA2_index++;
                i++;
            }    
        }
    }

    //mutate the DNA
    mutate();
}


void pga::TSPAgent::show_statistics(const std::vector<TSPAgent> &agents){
    std::cout<<" max value is: "<<agents[0].path_lenght;
}

pga::TSPAgent::~TSPAgent(){
}

void pga::TSPAgent::print_solution(){
    for(int i = 0; i<size; i++){
        std::cout<<DNA[i]<<"-";
    }
    std::cout<<std::endl;
}

#endif // !TSPAGENT_CPP