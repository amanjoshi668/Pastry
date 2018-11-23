
//Name: Praveen Balireddy
//Roll: 2018201052

#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include "node.h"

uint32_t nvtouint32(const std::vector<char> &arr);
std::vector<char> uint32tonv(uint32_t val);
std::vector<char> readBytes(int n, int sock_fd);
std::vector<std::string> extractArgs(std::string command);
int createTCPClient(std ::string ip, std::string port);
std::string getHash(std::string bytes, int len);
std::string trimString(std::string name, int len);
struct leafComparator
{
    bool operator()(node_Sptr a, node_Sptr b);
};
struct neighbourComparator
{
    bool operator()(node_Sptr a, node_Sptr b);
};
int prefixMatchLen(std ::string x, std::string y);
bool is_better_node(node_Sptr node1, node_Sptr node2, string nodeID); //is node1 more closer to nodeID than node2
#endif
