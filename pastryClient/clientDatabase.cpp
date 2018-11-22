#include "clientDatabase.h"
#include "errorMsg.h"
#include <tgmath.h>
#include <vector>
using namespace std;

ClientDatabase::ClientDatabase(){
    this->row = ceil((log(this->N)*1.000)/log(pow(2,this->b)));
    this->col = pow(2,this->b) - 1;
    this->routingTable = vector<vector<node_Sptr>>(this->row, vector<node_Sptr>(this->col));
};

ClientDatabase &ClientDatabase::getInstance()
{
    static ClientDatabase res;
    return res;
}