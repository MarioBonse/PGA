#ifndef UTILS_CPP
#define UTILS_CPP

#include "utils.h"

void split(const std::string& str, std::vector<std::string>& cont, char delim = ' ')
{
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delim)) {
        cont.push_back(token);
    }
}

int check_config(std::map<std::string, std::string> &config){
    std::cout<<"checking config\n";
    std::vector<std::string> must_include = {"population", "N_keep_agent", "p_mutation", "iterations"};
    for(auto i:must_include){
        if(config.find(i) == config.end()){
            std::cerr << "The configuration file has to include: " << i << "\n";
            std::cerr << "The usage is: \n ";
            for(auto j:must_include)std::cerr << j<<":value\n";
            return -1;
        }
    }
    if(config.find("seed") == config.end())
        config["seed"] = 1;
    // if I want to set default value here
    return 0;
}

int load_config(std::string file_name, std::map<std::string, std::string> &config){
    std::cout<<"loading config file \n";
    std::ifstream config_file;
    
    config_file.open(file_name);
    std::string line;
    while(std::getline(config_file, line) )
    {   
        std::vector<std::string> key_value;
        split(line, key_value, ':');
        std::cout<<key_value[0]<<" : "<<key_value[1]<<std::endl;
        try
        {
            config[key_value[0]] = key_value[1];
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            std::cerr<<"your config file is wrong. you should have:\n property:value\n";
            return -1;
        }
         
        key_value.clear();
    }
    return check_config(config);
}

int distance(std::pair<int, int> x, std::pair<int, int> y){
    return int(sqrt((x.first - y.first)*(x.first - y.first) + (x.second - y.second)*(x.second - y.second)));
}

void from_point_to_matrix(std::vector<std::pair<int, int>> points, std::vector<std::vector<int>> &graph){
    for(int i = 0; i<points.size(); i++){
        std::vector<int> row;
        for(int j = 0; j<points.size(); j++){
            row.push_back(distance(points[i], points[j]));
        }
        graph.push_back(row);
    }
}

int load_graph(std::string graph_path, std::vector<std::vector<int>> &graph){
    std::cout<<"Loading the graph from "<<graph_path<<std::endl;
    std::ifstream file;
    std::string line;
    file.open(graph_path);
    std::cout<<"file read succefully\n";

    std::getline(file, line);
    int type = 0;
    std::vector<std::string> values;
    // read the first three lines because they are note interesting
    std::getline(file, line);
    std::getline(file, line);
    // now we read the dimention 
    std::getline(file, line);
    split(line, values, ':');
    std::cout<<values[0]<<" "<<values[1]<<std::endl;
    int dimention = std::stoi(values[1]);
    // read the type of file. Now we support only the euclidian format

    values.clear();
    std::getline(file, line);
    split(line, values, ':');
    std::cout<<values[0]<<":"<<values[1]<<std::endl;

    if(values[0] == "EDGE_WEIGHT_TYPE " && (values[1] == " EUC_2D" || values[1] == " ATT"))type = 1;
    else{
        std::cerr<<"FORMAT NOT SUPPORTED\n";
        return -1;
    }
    std::getline(file, line);
    std::vector<std::pair<int, int>> points;
    int count = 1;
    while( std::getline(file, line) )
    {   
        values.clear();
        split(line, values);
        if(values[0] == "EOF")break;
        if(std::stoi(values[0])!= count){
            std::cerr<<"somenthing went relly wrong: I was at line "<<values[0]<<"but I read: "<<count<<" lines\n";
            return -1;
        }
        count++;
        points.push_back(std::make_pair(std::stoi(values[1]), std::stoi(values[2])));
    }
    from_point_to_matrix(points, graph);
    return 0;

}


void print_graph(std::vector<std::vector<int>> graph){
    for(auto l:graph){
        for(auto c:l){
            std::cout<<c<<" ";
        }
        std::cout<<std::endl;
    }
}

void show_config(std::map<std::string, std::string> config){
    for(auto it = config.cbegin(); it != config.cend(); ++it)
    {
    std::cout << it->first << " : " << it->second <<"\n";
    }
}

#endif // !UTILS_CPP
