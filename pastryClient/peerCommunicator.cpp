
#include "networkInterfacer.h"
#include "utils.h"
#include <syslog.h>
#include <arpa/inet.h>
#include <string.h>
#include "utils.h"
#include "unistd.h"
#include "errorMsg.h"
#include "peerCommunicator.h"
#include <vector>
using namespace std;

PeerCommunicator::PeerCommunicator(Node peer)
{
    try
    {
        this->peer_fd = createTCPClient(peer.getIp(), peer.getPort());
    }
    catch (ErrorMsg e)
    {
        throw ErrorMsg("Unable to establish connection");
    }
    // cout << "### Created TrackerServiceServer with fd: " << this->tracker_fd << endl;
}

PeerCommunicator::PeerCommunicator(std::string ip, std::string port)
{
    try
    {
        this->peer_fd = createTCPClient(ip, port);
    }
    catch (ErrorMsg e)
    {
        throw ErrorMsg("Unable to establish connection");
    }
    // cout << "### Created TrackerServiceServer with fd: " << this->tracker_fd << endl;
}



PeerCommunicator::~PeerCommunicator()
{
    syslog(LOG_INFO, "### Closing PeerCommunicator() with fd: %d", this->peer_fd);
    close(this->peer_fd);
}

Response PeerCommunicator::sendMsg(Message msg)
{
    NetworkWriter writer(this->peer_fd);
    auto msg_string = msg.SerializeAsString();
    writer.writeToNetwork(vector<char>(msg_string.begin(), msg_string.end()));
    NetworkReader reader(this->peer_fd);
    auto resp_bytes = reader.readFromNetwork();
    message::Response resp;
    resp.ParseFromString(string(resp_bytes.data()));
    return resp;
}