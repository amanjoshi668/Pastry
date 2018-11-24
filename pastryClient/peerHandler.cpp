#include "peerHandler.h"
#include "networkInterfacer.h"
#include "peerMessageHandler.h"
#include "message.pb.h"
#include "logHandler.h"
#include "errorMsg.h"
#include "clientDatabase.h"
#include "unistd.h"
#include <vector>
#include <syslog.h>
using std::cout;
using std::endl;
using namespace std;
void PeerHandler::handleRpc(int client_fd)
{
    try
    {
        while (true)
        {
            NetworkReader reader(client_fd);
            auto byte_data = reader.readFromNetwork();
            message::Message reqMsg;
            reqMsg.ParseFromString(string(byte_data.data()));
            PeerMessageHandler msgHandler;
            NetworkWriter writer(client_fd);
            if (reqMsg.type() == "JoinMe")
            {
                LogHandler::getInstance().logMsg("Recieved JoinMe request");
                syslog(5,"In peer handler -> join -> recieved JoinMe request");
                message::Response resp;
                resp.set_status("SUCCESS");
                auto resp_string = resp.SerializeAsString();
                writer.writeToNetwork(vector<char>(resp_string.begin(),resp_string.end()));
                msgHandler.handleJoinMeRequest(reqMsg);
            }
            else if (reqMsg.type() == "Join")
            {
                LogHandler::getInstance().logMsg("Recieved Join request");
                message::Response resp;
                resp.set_status("SUCCESS");
                auto resp_string = resp.SerializeAsString();
                writer.writeToNetwork(vector<char>(resp_string.begin(),resp_string.end()));
                msgHandler.handleJoinRequest(reqMsg);
            }
            else if (reqMsg.type() == "RoutingUpdate")
            {
                LogHandler::getInstance().logMsg("Recieved RoutingUpdate request");
                message::Response resp;
                resp.set_status("SUCCESS");
                auto resp_string = resp.SerializeAsString();
                writer.writeToNetwork(vector<char>(resp_string.begin(),resp_string.end()));
                msgHandler.handleRoutingUpdateRequest(reqMsg);
            }
            else if (reqMsg.type() == "AllStateUpdate")
            {
                LogHandler::getInstance().logMsg("Recieved AllStateUpdate request");
                message::Response resp;
                resp.set_status("SUCCESS");
                auto resp_string = resp.SerializeAsString();
                writer.writeToNetwork(vector<char>(resp_string.begin(),resp_string.end()));
                msgHandler.handleAllStateUpdateRequest(reqMsg);
            }
            else if (reqMsg.type() == "GetVal")
            {
                LogHandler::getInstance().logMsg("Recieved GetVal request");
                message::Response resp;
                resp.set_status("SUCCESS");
                auto resp_string = resp.SerializeAsString();
                writer.writeToNetwork(vector<char>(resp_string.begin(),resp_string.end()));
                msgHandler.handleGetValRequest(reqMsg);
            }
            else if (reqMsg.type() == "SetVal")
            {
                LogHandler::getInstance().logMsg("Recieved SetVal request");
                message::Response resp;
                resp.set_status("SUCCESS");
                auto resp_string = resp.SerializeAsString();
                writer.writeToNetwork(vector<char>(resp_string.begin(),resp_string.end()));
                msgHandler.handleSetValRequest(reqMsg);
            }
        }
    }
    catch (ErrorMsg e)
    {
        syslog(5,("PeerHandler::handleRpc() Exception received: " + e.getErrorMsg() + "For fd: " + std::to_string(client_fd)).c_str());
    }
    close(client_fd);   
}