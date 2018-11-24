#include "peerMessageHandler.h"
#include "message.pb.h"
#include "clientDatabase.h"
#include "networkInterfacer.h"
#include "utils.h"
#include <syslog.h>
#include <string>
#include <vector>
#include "peerCommunicator.h"
using namespace std;

void PeerMessageHandler::handleJoinMeRequest(message::Message msg)
{
	syslog(7,"In peer handler -> join -> recieved JoinMe request from ip %s, port %s, nodeID %s",
					msg.joinmemsg().ip().c_str(), msg.joinmemsg().port().c_str(), msg.joinmemsg().nodeid().c_str());
	auto req = msg.joinmemsg();
	auto next_node_sptr = ClientDatabase::getInstance().getNextRoutingNode(req.nodeid());
	message::Message routingUpdate;
	routingUpdate.set_type("RoutingUpdate");
	auto *temp = routingUpdate.mutable_routingupdate();
	temp->set_buddy(true);

	auto new_routingList = temp->mutable_routingentires();
	auto new_neighbour_set = temp->mutable_neighbours();

	//adding neighbourSet
	auto neighbourSet = ClientDatabase::getInstance().getNeighbourSet();
	for (auto neighbour_node : neighbourSet)
	{
		auto nnode = new_neighbour_set->add_node();
		nnode->set_ip(neighbour_node->getIp());
		nnode->set_port(neighbour_node->getPort());
		nnode->set_nodeid(neighbour_node->getNodeID());
	}

	//adding routing entires
	auto routingTable = ClientDatabase::getInstance().getRoutingTable();
	auto prefix_match_len = prefixMatchLen(req.nodeid(), ClientDatabase::getInstance().getListener()->getNodeID());
	for (int i = 0; i <= prefix_match_len; i++)
	{
		auto temp_list = temp->add_routingentires();
		temp_list->set_index(i);
		auto routing_row = temp_list->mutable_nodelist();
		for (auto node : routingTable[i])
		{
			auto list_node = routing_row->add_node();
			if (node)
			{
				list_node->set_ip(node->getIp());
				list_node->set_port(node->getPort());
				list_node->set_nodeid(node->getNodeID());
			}
			else
			{
				list_node->set_ip("-1");
				list_node->set_port("-1");
				list_node->set_nodeid("-1");
			}
		}
	}

	//adding leaf node
	if (next_node_sptr->getNodeID() == ClientDatabase::getInstance().getListener()->getNodeID())
	{
		auto new_leaf_set = temp->mutable_leaf();
		temp->set_terminal(true);
		auto leafSet = ClientDatabase::getInstance().getLeafSet();
		for (auto leaf_node : leafSet.first)
		{
			auto lnode = new_leaf_set->add_node();
			lnode->set_ip(leaf_node->getIp());
			lnode->set_port(leaf_node->getPort());
			lnode->set_nodeid(leaf_node->getNodeID());
		}
		for (auto leaf_node : leafSet.second)
		{
			auto lnode = new_leaf_set->add_node();
			lnode->set_ip(leaf_node->getIp());
			lnode->set_port(leaf_node->getPort());
			lnode->set_nodeid(leaf_node->getNodeID());
		}
	}
	PeerCommunicator peercommunicator(Node(req.ip(), req.port(),req.nodeid()));
	auto resp = peercommunicator.sendMsg(routingUpdate);

	if (next_node_sptr->getNodeID() != ClientDatabase::getInstance().getListener()->getNodeID())
	{
		PeerCommunicator peercommunicator(*next_node_sptr);
		auto resp = peercommunicator.sendMsg(routingUpdate);
	}
}
void PeerMessageHandler::handleJoinRequest(message::Message msg)
{
	auto req = msg.joinmsg();
	auto next_node_sptr = ClientDatabase::getInstance().getNextRoutingNode(req.nodeid());
	message::Message routingUpdate;
	routingUpdate.set_type("RoutingUpdate");
	auto *temp = routingUpdate.mutable_routingupdate();
	temp->set_buddy(true);

	auto new_routingList = temp->mutable_routingentires();

	//adding routing entires
	auto routingTable = ClientDatabase::getInstance().getRoutingTable();
	auto prefix_match_len = prefixMatchLen(req.nodeid(), ClientDatabase::getInstance().getListener()->getNodeID());
	auto temp_list = temp->add_routingentires();
	temp_list->set_index(prefix_match_len);
	auto routing_row = temp_list->mutable_nodelist();
	for (auto node : routingTable[prefix_match_len])
	{
		auto list_node = routing_row->add_node();
		if (node)
		{
			list_node->set_ip(node->getIp());
			list_node->set_port(node->getPort());
			list_node->set_nodeid(node->getNodeID());
		}
		else
		{
			list_node->set_ip("-1");
			list_node->set_port("-1");
			list_node->set_nodeid("-1");
		}
	}

	//adding leaf node
	if (next_node_sptr->getNodeID() == ClientDatabase::getInstance().getListener()->getNodeID())
	{
		auto new_leaf_set = temp->mutable_leaf();
		temp->set_terminal(true);
		auto leafSet = ClientDatabase::getInstance().getLeafSet();
		for (auto leaf_node : leafSet.first)
		{
			auto lnode = new_leaf_set->add_node();
			lnode->set_ip(leaf_node->getIp());
			lnode->set_port(leaf_node->getPort());
			lnode->set_nodeid(leaf_node->getNodeID());
		}
		for (auto leaf_node : leafSet.second)
		{
			auto lnode = new_leaf_set->add_node();
			lnode->set_ip(leaf_node->getIp());
			lnode->set_port(leaf_node->getPort());
			lnode->set_nodeid(leaf_node->getNodeID());
		}
	}
	PeerCommunicator peercommunicator(Node(req.ip(), req.port(),req.nodeid()));
	auto resp = peercommunicator.sendMsg(routingUpdate);

	if (next_node_sptr->getNodeID() != ClientDatabase::getInstance().getListener()->getNodeID())
	{
		PeerCommunicator peercommunicator(*next_node_sptr);
		auto resp = peercommunicator.sendMsg(routingUpdate);
	}
}
void PeerMessageHandler::handleRoutingUpdateRequest(message::Message msg)
{
	auto req = msg.routingupdate();
	auto routingList = req.routingentires();
	for (int i = 0; i < req.routingentires_size(); i++)
	{
		auto temp = req.routingentires().Get(i);
		vector<node_Sptr> tempNodeList;
		for (int j = 0; j < temp.nodelist().node_size(); j++)
		{
			auto nodeFrmMsg = temp.nodelist().node().Get(j);
			node_Sptr new_node;
			if (nodeFrmMsg.nodeid() == "-1")
			{
				new_node = NULL; //make_shared<Node>(nullptr);
			}
			else
			{
				new_node = make_shared<Node>(nodeFrmMsg.ip(), nodeFrmMsg.port(), nodeFrmMsg.nodeid());
			}
			tempNodeList.push_back(new_node);
			//update proximity
			ClientDatabase::getInstance().updateRoutingTable(tempNodeList, temp.index());
		}
	}
	if (req.buddy())
	{
		for (int j = 0; j < req.neighbours().node_size(); j++)
		{
			auto nodeFrmMsg = req.neighbours().node().Get(j);
			node_Sptr new_node;
			if (nodeFrmMsg.nodeid() != "-1")
			{
				new_node = make_shared<Node>(nodeFrmMsg.ip(), nodeFrmMsg.port(), nodeFrmMsg.nodeid());
				//calculate proximity
				ClientDatabase::getInstance().addToLeafSet(new_node);
			}
		}
	}
	if (req.terminal())
	{
		for (int j = 0; j < req.leaf().node_size(); j++)
		{
			auto nodeFrmMsg = req.leaf().node().Get(j);
			node_Sptr new_node;
			if (nodeFrmMsg.nodeid() != "-1")
			{
				new_node = make_shared<Node>(nodeFrmMsg.ip(), nodeFrmMsg.port(), nodeFrmMsg.nodeid());
				ClientDatabase::getInstance().addToLeafSet(new_node);
			}
		}
	}
	ClientDatabase::getInstance().incrementRecievedUpdateCount();
	if(ClientDatabase::getInstance().getRecievedUpdateCount() == ClientDatabase::getInstance().getTotalRouteLength())
	{
		this->sendAllStateUpdate();
	}
}

void PeerMessageHandler::sendAllStateUpdate()
{
	// add code
}


void PeerMessageHandler::handleAllStateUpdateRequest(message::Message msg)
{
	auto req = msg.allstateupdate();
	for (int i = 0; i < req.leaf().node_size(); i++)
	{
		auto leaf_entry = req.leaf().node().Get(i);
		node_Sptr node_from_msg;
		node_from_msg = make_shared<Node>(leaf_entry.ip(), leaf_entry.port(), leaf_entry.nodeid());
		ClientDatabase::getInstance().addToLeafSet(node_from_msg);
	}
	for (int i = 0; i < req.neighbours().node_size(); i++)
	{
		auto neighbour = req.neighbours().node().Get(i);
		node_Sptr node_from_msg;
		node_from_msg = make_shared<Node>(neighbour.ip(), neighbour.port(), neighbour.nodeid());
		ClientDatabase::getInstance().addToNeighhbourSet(node_from_msg);
	}
	for (int i = 0; i < req.routingtable_size(); i++)
	{
		auto row_of_routing_table = req.routingtable().Get(i);
		for (int j = 0; j < row_of_routing_table.node_size(); j++)
		{
			auto nodeFrmMsg = row_of_routing_table.node().Get(j);
			node_Sptr new_node;
			new_node = make_shared<Node>(nodeFrmMsg.ip(), nodeFrmMsg.port(), nodeFrmMsg.nodeid());
			ClientDatabase::getInstance().addToRoutingTable(new_node);
		}
	}
}
void PeerMessageHandler::handleGetValRequest(message::Message msg)
{
	auto req = msg.getvalmsg();
	auto next_node_sptr = ClientDatabase::getInstance().getNextRoutingNode(req.key());
	
	if (next_node_sptr->getNodeID() == ClientDatabase::getInstance().getListener()->getNodeID())
	{
		message::Message resp;
		resp.set_type("GetValResponse");
		auto temp = resp.mutable_getvalresponse();
		temp->set_key(req.key());
		//set value from hash table
		PeerCommunicator peercommunicator(Node(req.node().ip(), req.node().port(),req.node().nodeid()));
		peercommunicator.sendMsg(resp);
	}
	else {
		PeerCommunicator peercommunicator(*next_node_sptr);
		peercommunicator.sendMsg(msg);
	}
}
void PeerMessageHandler::handleSetValRequest(message::Message msg)
{
	auto req = msg.setvalmsg();
	auto next_node_sptr = ClientDatabase::getInstance().getNextRoutingNode(req.key());
	
	if (next_node_sptr->getNodeID() == ClientDatabase::getInstance().getListener()->getNodeID())
	{
		//update local hash table
	}
	else {
		PeerCommunicator peercommunicator(*next_node_sptr);
		auto resp = peercommunicator.sendMsg(msg);
	}	
}
// vector<pair<string, string>> PeerMessageHandler ::getRelevantKeyValuePairs(string nodeID){
// 	string myNodeId = ClientDatabase::getInstance().getListener()->getNodeID();
// 	auto hash_table = ClientDatabase::getInstance()->getHashTable();
// 	vector<pair<string, string> > result;
// 	for(auto message: hash_table){
// 		if(is_better_node_for_message(nodeID, myNodeId, message.first)){
// 			result.push_back(message);
// 		}
// 	}
// 	return result;
// }