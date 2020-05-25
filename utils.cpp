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
    std::vector<std::string> must_include = {"population", "N_keep_agent", "p_mutation"};
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
    std::ifstream config_file;
    
    config_file.open(file_name);
    std::string line;
    while( std::getline(config_file, line) )
    {   
        std::vector<std::string> key_value;
        split(line, key_value, ':');
        config[key_value[0]] = key_value[1];
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
    std::ifstream file;
    std::string line;
    std::getline(file, line);
    int type = 0;
    std::vector<std::string> values;
    // read the first three lines because they are note interesting
    std::getline(file, line);
    std::getline(file, line);
    std::getline(file, line);
    // now we read the dimention 
    std::getline(file, line);
    split(line, values, ':');
    int dimention = std::stoi(values[1]);
    // read the type of file. Now we support only the euclidian format
    std::getline(file, line);
    split(line, values, ':');
    if(values[0] == "EDGE_WEIGHT_TYPE" && values[1] == "EUC_2D")type = 1;
    else{
        std::cerr<<"FORMAT NOT SUPPORTED\n";
        return -1;
    }
    std::getline(file, line);
    std::vector<std::pair<int, int>> points;
    int count = 1;
    while( std::getline(file, line) )
    {   
        std::vector<std::string> values;
        split(line, values);
        if(std::stoi(values[0])!= count){
            std::cerr<<"somenthing went relly wrong: I was at line "<<values[0]<<"but I read: "<<count<<" lines\n";
            return -1;
        }
        points.push_back(std::make_pair(std::stoi(values[1]), std::stoi(values[2])));
    }
    from_point_to_matrix(points, graph);
    return 0;

}
