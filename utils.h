// basic file operations
#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <fstream>
#include <map>
#include <cmath>
#include <sstream>
#include <vector>

void split(const std::string&, std::vector<std::string>& , char);

int check_config(std::map<std::string, std::string> &);

int load_config(std::string file_name, std::map<std::string, std::string> &);

int distance(std::pair<int, int> , std::pair<int, int> );

void from_point_to_matrix(std::vector<std::pair<int, int>> , std::vector<std::vector<int>> &);

int load_graph(std::string , std::vector<std::vector<int>> &);

void print_graph(std::vector<std::vector<int>>);

void show_config(std::map<std::string, std::string>);

#include "utils.cpp"
#endif // !UTILS_H
