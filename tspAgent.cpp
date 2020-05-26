#include "tspAgent.h"
#include <algorithm>

int find(std::vector<int> candidates, int node){
    for(auto c: candidates){
        if(c == node)return c;
    }
    return 0;
}

std::vector<int> find_candidates(std::vector<int> nodes, std::vector<bool> visited){
    std::vector<int> candidates;
    for(int i = 0; i<nodes.size(); i++){
        // if the edge exists and we never visited it 
        if(nodes[i] && !visited[i]){
            candidates.push_back(i);
        }
    }
    return candidates;
}

int next_random_node(std::vector<int> nodes, std::vector<bool> visited){
    
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



pga::TSPAgent::TSPAgent(std::vector<std::vector<int>> const my_graph){
    graph = my_graph;
    size = graph.size();
    chromosome_length = size/2;
    random_DNA();
}

void pga::TSPAgent::random_DNA(){
    // pick random starting node
    std::vector<bool> visited(0, size);
    int current_node = rand() % size;
    DNA.push_back(current_node);
    for(int i = 0; i<size; i++){
        visited[current_node] = true;
        //pick random second edge
        int next_node = next_random_node(graph[current_node], visited);
        if(next_node == -1) {DNA[i] = -1; alive = false;}
        DNA.push_back(next_node);
        current_node = next_node;
    }
}


/* 
for calculate the fitness just calculate the lenght of the path. If the path does not exists 
the fitness is 0
*/
void pga::TSPAgent::simulate(){
    int path_lenght = 0;
    int start = DNA[0], end;
    for(int i = 1; i < DNA.size();i++){
        end = DNA[i];
        if(end == -1){fitness = 0; return;}
        if(graph[start][end]) 
            path_lenght += graph[start][end];
        else {fitness = 0; return;}
        start = end;
    }
    // the fitness has to be > 0 and higher for better solution so we invert the lenght of the path
    fitness = 1/path_lenght;
}


/* 
first method: 
p1 = 1-4-5-6-2-3-7-8 
p2 = 4-5-6-3-2-8-7-1
-----------------------
Pick pieces of the first DNA (e.d. 1-4-5-*-*-*-7-8)
then complete with * with nodes from p2 in order if possible -> 6-3-2.
With probability p_mutation one node of the path(aka a chromosome of the DNA) is picked randomly 
*/
void pga::TSPAgent::reproduce(pga::TSPAgent p1, pga::TSPAgent p2){
    std::vector<bool> visited(0, size);
    int start = DNA[0];

    // clone the pieece from the first parent
    for(int chromosome = 0; chromosome < (size/(2*chromosome_length)) + 1; chromosome += 2){
        for(int i = chromosome_length*chromosome; i < size || i < chromosome_length*chromosome + chromosome_length; i++){
            visited[start] = true;
            double sample = dis(gen);
            if(sample < p_mutation){
                DNA[i] = next_random_node(graph[start], visited);
            }
            else DNA[i] = p1.DNA[i];
            start = DNA[i];
        }
    }
    //now clone the piece from the second parent
    for(int chromosome = 1; chromosome < (size/(chromosome_length)) + 1; chromosome += 2){
        int i = chromosome_length*chromosome*2; 
        while(i < size || i < chromosome_length*chromosome*2 + chromosome_length){
            // here we don't search for mutation. We just search from where we are if we are able to 
            // find a path that uses nodes from order in DNA 2
            start = DNA[i - 1];
            // find from start a "free node" and pick the first one present in p2
            std::vector<int> candidates = find_candidates(graph[start], visited);
            // if I have no possible nodes then the agent is dead then the fitness is 0
            if(candidates.size() == 0){DNA[i] = -1; alive = false; return;}

            int DNA2_index = 0;
            // increase the index of DNA2 until we find a node never visited 
            // the first available nodes. Then from this point one we try to continue to copy until we reach
            // a visited node 
            while(visited[p2.DNA[DNA2_index]] || !find(candidates, p2.DNA[DNA2_index]))DNA2_index++;
            // until we don't find an already visited node we copy the DNA from parent2.
            // we check also not to exit from the window(aka chromosome) we are copyng
            while( i < size || i < chromosome_length*chromosome*2 + chromosome_length 
                && !visited[p2.DNA[DNA2_index]]){
                DNA[i] = p2.DNA[DNA2_index];
                visited[p2.DNA[DNA2_index]] = true;
                DNA2_index++;
                i++;
            }    
        }
    }
}

pga::TSPAgent::~TSPAgent(){
    std::cout<<"destroy_agent";
}
