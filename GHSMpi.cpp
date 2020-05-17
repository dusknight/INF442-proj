//
// Created by Shikang on 4/30/2020.
//

#include "GHSMpi.hpp"
#include "GraphMpi.hpp"

#include<iostream>
#include<cassert>

// #define NONMPI
const bool PRINT_MSG = true;
const bool PRINT_MPI_LOG_DETAIL = true;
using namespace std;

int GHSNode::assign_id(int id_rank, int num_process)
{
	return id_rank;
}


GHSNode::GHSNode(vector<Edge> adj_edges)
{
	id = -1;
	machine = nullptr;
	GHSEdge _special_mark(-1, -1, DBL_MAX, GHSEdge::EdgeState::BASIC);
	adj_out_edges[-1] = _special_mark;
	for (int i = 0; i < adj_edges.size(); ++i) {
		if (adj_edges[i].u == adj_edges[i].v) continue;  // delete self ring
		GHSEdge ge(adj_edges[i]);
		adj_out_edges[ge.v] = ge;

		//SE[i] = GHSEdge::EdgeState::BASIC;
	}
	SN = SLEEPING;
	LN = 0;
	FN = 0;
	find_count = 0;

	best_edge = -1;
	test_edge = -1;
	in_branch = -1;

	finished = false;
}

GHSNode::GHSNode(const GHSNode& g)
{
	machine = g.machine;
	id = g.id;
	finished = g.finished;

	SN = g.SN;  // state
	// vector<GHSEdge::EdgeState> SE;
	FN = g.FN;  // fragemnt identity
	LN = g.LN;  // level
	find_count = g.find_count;

	recv_msg = g.recv_msg;
	//queue<int>recv_msg_from;

	adj_out_edges = g.adj_out_edges;  // directed edges, adjacent edges

	// for Test
	best_edge = g.best_edge;
	in_branch = g.in_branch;
	test_edge = g.test_edge;
}

GHSNode::GHSNode()
{
	id = -1;
	machine = nullptr;
	SN = SLEEPING;
	LN = 0;
	FN = 0;
	find_count = 0;

	best_edge = -1;
	test_edge = -1;
	in_branch = -1;

	finished = false;
}

void GHSNode::MsgHandler(GHSmsg msg, int from_edge)
{
	switch (msg.type) {
	case MsgType::CONNECT:
		RespConnect(msg.arg1, from_edge);
		break;
	case MsgType::INIT:
		RespInit(msg.arg1, msg.argf, (GHSNode::NodeState)msg.arg2, from_edge);  // L, F, S
		break;
	case MsgType::TEST:
		RespTest(msg.arg1, msg.argf, from_edge);  // LN, FN
		break;
	case MsgType::ACCEPT:
		RespAccept(from_edge);
		break;
	case MsgType::REJECT:
		RespReject(from_edge);
		break;
	case MsgType::REPORT:
		RespReport(msg.argf, from_edge); // arg3: weight
		break;
	case MsgType::CHANGE_CORE:
		RespChangeCore();
		break;
	default:
		std::cerr << "[Error]: unimplemented message type " << msg.type << std::endl;
	}
	return;
}

int GHSNode::find_best_edge() {
	/**
	if FOUND return adj_node_id;
	else return -1;
	*/
	double weight = DBL_MAX;
	int flag_found = -1;

	for (auto ip = adj_out_edges.begin(); ip != adj_out_edges.end(); ip++) {
		if (ip->second.state == GHSEdge::EdgeState::BASIC)
			if (ip->second.cost < weight || (ip->second.cost == weight && ip->first < best_edge)) {
				weight = ip->second.cost;
				best_edge = ip->first;
				flag_found = best_edge;
			}
	}
	//for (int ie = 0; ie<adj_out_edges.size(); ++ie) {
	//    if(adj_out_edges[ie].state == GHSEdge::EdgeState::BASIC)
	//        if (adj_out_edges[ie].cost < weight || adj_out_edges[ie].cost == weight && ie < best_edge) {
	//            weight = adj_out_edges[ie].cost;
	//            best_edge = adj_out_edges[ie].v;
	//            flag_found = best_edge;
	//            idx = ie;
	//        }
	//}
	return flag_found;
}

int GHSNode::find_test_edge() {
	/**
	if FOUND return adj_node_id;
	else return -1;
	*/
	double weight = DBL_MAX;
	int flag_found = -1;

	for (auto ip = adj_out_edges.begin(); ip != adj_out_edges.end(); ip++) {
		if (ip->second.state == GHSEdge::EdgeState::BASIC)
			if (ip->second.cost < weight || (ip->second.cost == weight && ip->first < test_edge)) {
				weight = ip->second.cost;
				test_edge = ip->first;
				flag_found = test_edge;
			}
	}

	//for (int ie = 0; ie < adj_out_edges.size(); ++ie) {
	//    if (adj_out_edges[ie].state == GHSEdge::EdgeState::BASIC)
	//        if (adj_out_edges[ie].cost < weight || adj_out_edges[ie].cost == weight && ie < test_edge) {
	//            weight = adj_out_edges[ie].cost;
	//            test_edge = adj_out_edges[ie].v;
	//            flag_found = adj_out_edges[ie].v;
	//        }
	//}
	return flag_found;
}

void GHSNode::WakeUp() {  // checked ok
	// let m be adjacent edge if minimum weight;
	int out_node = find_best_edge();
	adj_out_edges[out_node].state = GHSEdge::EdgeState::BRANCH;
	LN = 0;
	SN = NodeState::FOUND;
	FN = 0; // added
	find_count = 0;

	// send Connect(0) on edge m;
	machine->send_msg(GHSmsg(MsgType::CONNECT, 0, out_node, id));
}

void GHSNode::RespInit(int L, double F, GHSNode::NodeState S, int from_node) // ============= HERE LIES THE LAST PROBLEM
{
	// CHECK : have difference with GH.
	LN = L;
	FN = F;
	SN = S;
	in_branch = from_node;
	best_edge = -1;
	// double best_wt = DBL_MAX;
	for (auto ip = adj_out_edges.begin(); ip != adj_out_edges.end(); ip++) {
		//if (ip->first != from_node && ip->second.state == GHSEdge::EdgeState::BRANCH) {
		//	// send INIT (L, F, S) on edge i
		//	machine->send_msg(GHSmsg(MsgType::INIT, L, F, S, ip->first, id));
		//	if (S == FIND) find_count++;
		//}
		//if (S == FIND) Test();

		if (ip->first == from_node || ip->second.state != GHSEdge::EdgeState::BRANCH) continue;
		// send INIT (L, F, S) on edge i
		machine->send_msg(GHSmsg(MsgType::INIT, L, F, S, ip->first, id));
		if (S == FIND) find_count++;
	}
	if (S == FIND) Test();
}


void GHSNode::RespConnect(int level, int from_node) { // checked ok (process_connect)
	if (SN == SLEEPING) WakeUp();
	if (level < LN) {  // absorb
		adj_out_edges[from_node].state = GHSEdge::EdgeState::BRANCH;
		// send INIT (LN, FN, FN) on edge j;
		machine->send_msg(GHSmsg(MsgType::INIT, LN, FN, SN, from_node, id));
		if (SN == FIND) find_count++;
	}
	else {
		if (adj_out_edges[from_node].state == GHSEdge::EdgeState::BASIC) {  // delay
			// place received message on end of queue
			GHSmsg msg;
			msg.type = MsgType::CONNECT;
			msg.arg1 = level;
			msg.src_vid = from_node;
			msg.dest_vid = id;
			machine->emplace_recv_queue(msg);
		}
		else {  // merge
			// send Init (LN+1, w(j), FIND) on edge j
			machine->send_msg(GHSmsg(MsgType::INIT, LN + 1, adj_out_edges[from_node].cost, FIND, from_node, id));
		}
	}
}

void GHSNode::Test()  // checked
{
	// TODO - check:::: CHECKED
	test_edge = find_test_edge();
	if (test_edge > 0) { // found
		// comm.sendTest(LN, FN, test_edge);
		machine->send_msg(GHSmsg(MsgType::TEST, LN, FN, test_edge, id));
	}
	else {
		Report();
	}

}

void GHSNode::RespTest(int L, double F, int from_node)  // checked ok (ok)
{
	if (SN == SLEEPING) WakeUp();

	if (L > LN) // place the received message at the end of the queue
		machine->emplace_recv_queue(GHSmsg(MsgType::TEST, L, F, id, from_node));
	else if (F != FN) // comm.sendAccept(edge_id);
		machine->send_msg(GHSmsg(MsgType::ACCEPT, from_node, id));
	else {
		if (adj_out_edges[from_node].state == GHSEdge::EdgeState::BASIC)
			adj_out_edges[from_node].state = GHSEdge::EdgeState::REJECTED;
		if (test_edge != from_node)
			// comm.sendReject(edge_id);
			machine->send_msg(GHSmsg(MsgType::REJECT, from_node, id));
		else Test();
	}
}

void GHSNode::RespAccept(int from_node)  // checked ok (ok)
{
	test_edge = -1;
	if (Edge::cmp(adj_out_edges[from_node], adj_out_edges[best_edge])) // cost: a < b
		best_edge = from_node;

	Report();
}

void GHSNode::RespReject(int from_node)  // checked ok (ok)
{
	if (adj_out_edges[from_node].state == GHSEdge::EdgeState::BASIC)
		adj_out_edges[from_node].state = GHSEdge::EdgeState::REJECTED;

	Test();
}

void GHSNode::Report()  // checked
{
	if (find_count == 0 && test_edge < 0) {  // THIS LINE IS IMPORTANT    ######
		SN = FOUND;
		machine->send_msg(GHSmsg(MsgType::REPORT, adj_out_edges[best_edge].cost, in_branch, id));
	}
}

void GHSNode::RespReport(double w, int from_edge)  // TODO: check (OK?
{
	if (from_edge != in_branch) {
		find_count--;
		if (w < adj_out_edges[best_edge].cost) best_edge = from_edge;  // update new best edge
		Report();
	}
	else {
		if (SN == FIND) // place the received message at the end of queue
		{
			machine->emplace_recv_queue(GHSmsg(MsgType::REPORT, w, id, from_edge));
		}
		else if (w > adj_out_edges[best_edge].cost) {
			if (PRINT_MSG) cout << "[INFO] " << "REPORT to CHANGECORE [Node  " << id << "];" << endl;
			ChangeCore();
		}
		// The algo is FININSHED 
		else if (w == DBL_MAX && best_edge < 0)  // TODO: conditions not finished // TODO: check it!
		{  // HALT
			finished = true;
			/*
			We need to send notify the other nodes on the chaine that the algorithm has terminated.
			So, here, we send REPORT message with maximum weights.
			*/
			for (auto ip = adj_out_edges.begin(); ip != adj_out_edges.end(); ip++) {
				if (ip->second.state == GHSEdge::EdgeState::BRANCH && ip->first != in_branch) {
					if (PRINT_MSG) cout << "[INFO] " << "Send FINISH to [Node  " << ip->first << "], from " << id << ";" << endl;
					machine->send_msg(GHSmsg(MsgType::REPORT, DBL_MAX, ip->first, id));
				}
			}
		}
	}
}

void GHSNode::ChangeCore()  // checked ok
{
	if (adj_out_edges[best_edge].state == GHSEdge::EdgeState::BRANCH)
		// comm.sendChangeCore(best_edge);
		machine->send_msg(GHSmsg(MsgType::CHANGE_CORE, best_edge, id));
	else {
		// comm.sendConnect(LN, best_edge);
		machine->send_msg(GHSmsg(MsgType::CONNECT, LN, best_edge, id));
		adj_out_edges[best_edge].state = GHSEdge::EdgeState::BRANCH;
	}
}

void GHSNode::RespChangeCore()
{
	ChangeCore();
}

bool GHSNode::isFinished()
{
	return finished;
}

vector<int> GHSNode::get_branches()
{
	vector<int> branches;
	for (auto in = adj_out_edges.begin(); in != adj_out_edges.end(); in++) {
		if (in->second.state == GHSEdge::EdgeState::BRANCH)
			branches.push_back(in->first);
	}
	return branches;
}


int GHSMPI::assign_vertice_to_machine(int v_id)
{
	// TODO: change to M61 algo
	int m_id = v_id % count_machine;
	vert2machine[v_id] = m_id;
	return m_id;
}

bool GHSMPI::ask_all_nodes_if_finished()
{
	for (auto in = nodes.begin(); in != nodes.end(); in++) {
		if (!in->second.isFinished()) {
			is_all_finished = false;
			return false;
		}
	}
	return true;
}

void GHSMPI::print_node_states()
{
	for (auto in = nodes.begin(); in != nodes.end(); in++) {
		vector<int> branches = in->second.get_branches();
		// assert(branches.size() <= 2);
		cout << in->first << ": ";
		if (in->second.getSN() == 0)
			cout << "0000";
		else if (in->second.getSN() == 1)
			cout << "FIND (";
		else cout << "++++ (";

		for (auto i = branches.begin(); i != branches.end(); i++) {
			cout << *i << " ";
		}
		if (in->second.getSN() != 0) cout << ")";
		if (in->second.isFinished()) cout << "XX";
		cout << ";\t";
	}
	cout << endl;
}

void GHSMPI::send_msg(GHSmsg msg)
{
	to_send.emplace(msg);
}

void GHSMPI::emplace_recv_queue(GHSmsg msg)
{
	recved_later.emplace(msg);
}

void GHSMPI::exec_send_recv()
{
	while (!to_send.empty()) {
		GHSmsg msg = to_send.front();
		to_send.pop();
		int machine_id = vert2machine[msg.dest_vid];
		// if (machine_id == rank) recved.emplace(msg);  // if in this machine
		// else {
		send_buffers[machine_id].push_back(msg);
		// }
	}

	// exchange buffers
	exchange_with_all_machine();
	// TODO exchange buffers
	for (auto imsg = recv_buffers.begin(); imsg != recv_buffers.end(); imsg++)
		recved_this_time.emplace(*imsg);
	recv_buffers.clear();

	while (!recved_this_time.empty()) {
		GHSmsg msg = recved_this_time.front();
		recved_this_time.pop();
		int node_id = msg.dest_vid;
		// if(!nodes[node_id].isFinished())  // still living
		nodes[node_id].MsgHandler(msg, msg.src_vid);
	}

	while (!recved_later.empty()) {  // the messages retarded in the last loop
		GHSmsg msg = recved_later.front();
		recved_later.pop();
		recved_this_time.emplace(msg);
		//int node_id = msg.dest_vid;
		//if (!nodes[node_id].isFinished())  // still living
		//    nodes[node_id].MsgHandler(msg, msg.src_vid);
	}
}

void GHSMPI::init()
{
	string filename = "test_in.txt";
	// comm.init(0, NULL);
	comm.init(0, NULL); // MPI_Init
	comm.commitType(NULL);
	MPI_Comm_size(comm.comm, &count_machine);
	MPI_Comm_rank(comm.comm, &rank);
	// rank from 0 to N-1

	for (int i = 0; i < count_machine; i++) {
		vector<GHSmsg> gn;
		send_buffers[i] = gn;
	}
	// send_buffers.resize(count_machine);
	rdispl = new int[count_machine];
	sdispl = new int[count_machine];

	// read_file
	GraphInEdge gie;
	gie.ReadFile(filename);
	//gie.addEdge(Edge(1, 2, 1));
	//gie.addEdge(Edge(2, 1, 1));
	//gie.addEdge(Edge(2, 3, 1));
	//gie.addEdge(Edge(3, 2, 1));
	//gie.addEdge(Edge(3, 4, 1));
	//gie.addEdge(Edge(4, 3, 1));
	//gie.addEdge(Edge(4, 5, 1));
	//gie.addEdge(Edge(5, 4, 1));
	//gie.addEdge(Edge(5, 6, 1));
	//gie.addEdge(Edge(6, 5, 1));
	//gie.addEdge(Edge(6, 7, 1));
	//gie.addEdge(Edge(7, 6, 1)); 
	//gie.addEdge(Edge(7, 1, 1));
	//gie.addEdge(Edge(1, 7, 1));
	//gie.addEdge(Edge(2, 6, 1));
	//gie.addEdge(Edge(6, 2, 1));
	//gie.addEdge(Edge(3, 5, 1));
	//gie.addEdge(Edge(5, 3, 1));


	auto adj_edge = gie.toAdjecentList();
	const int id_start_from = 1; // vertex id starting from 1
	int _lcl_vert_count = gie.getVertSize() / count_machine;

	for (int i = id_start_from; i < gie.getVertSize() + id_start_from; i++) {
		int m_id = assign_vertice_to_machine(i);
		if (m_id == rank) vertices_id.push_back(i);
	}

	// init nodes
	vector<vector<Edge>> adj_edges = gie.toAdjecentList();
	for (auto v_iter = vertices_id.begin(); v_iter != vertices_id.end(); v_iter++) {  // init GHSNodes
		GHSNode gn(*v_iter, adj_edges[*v_iter]);
		gn.set_machine(this);
		nodes[*v_iter] = gn;
	}

	ready = true;

}

void GHSMPI::run_loop()
{
	if (!ready) {
		cerr << "[ERROR] Process is not ready. Initialize it first." << endl;
		return;
	}

	comm.barrier(); // MPI_Barrier synchonolise
	int beginning_node = nodes.begin()->first;

	// wake up the first node;
	nodes[beginning_node].WakeUp();

	//nodes[4].WakeUp();  // ######################################TODO delete
	//nodes[6].WakeUp();  // ######################################TODO delete

	while (!ask_all_nodes_if_finished()) {

		if (PRINT_MSG) print_node_states();
		exec_send_recv();
	}
	cout << "[INFO] --- process calculation finished. id=" << rank << endl;
	print_node_states();
	comm.barrier();
	finalize();

}

void GHSMPI::msg_handler(GHSmsg msg)
{
	int dest_vid = msg.dest_vid;
	int src_vid = msg.src_vid;
	switch (msg.type) {
	case MsgType::CONNECT:
		nodes[dest_vid].RespConnect(msg.arg1, src_vid);
		break;
	case MsgType::INIT:
		nodes[dest_vid].RespInit(msg.arg1, msg.argf, (GHSNode::NodeState)msg.arg2, src_vid);  // LN, FN, SN
		break;
	case MsgType::TEST:
		nodes[dest_vid].RespTest(msg.arg1, msg.argf, src_vid);  // LN, FN
		break;
	case MsgType::ACCEPT:
		nodes[dest_vid].RespAccept(src_vid);
		break;
	case MsgType::REJECT:
		nodes[dest_vid].RespReject(src_vid);
		break;
	case MsgType::REPORT:
		nodes[dest_vid].RespReport(msg.argf, src_vid); // arg3: weight
		break;
	case MsgType::CHANGE_CORE:
		nodes[dest_vid].RespChangeCore();
		break;
	default:
		std::cerr << "[Error]: unimplemented message type " << msg.type << std::endl;
	}
	return;
}

void GHSMPI::finalize()
{
	// TODO: join all process MPI_join()
	MPI_Finalize();
	cout << "--- Node rank " << rank << " (out of " << count_machine << ") is finished." << endl;
	system("pause");

}

//void GHSMPI::exchange_with_one_machine(int machine)
//{
//    int received = 0;
//    vector<int> send_counts;
//    vector<int> recv_counts;
//    for (int i = 0; i < count_machine; i++) send_counts.push_back(send_buffers[i].size());
//    // Determine the receive sub-buffer element counts.
//    MPI_Gather(&(send_counts[machine]), 1, MPI_INT, &recv_counts, 1, MPI_INT,
//        machine, comm.comm);
//    *messages += info->machines;
//}

void GHSMPI::exchange_with_all_machine()
{
	vector<int> send_counts;
	vector<int> recv_counts;  // length to be received
	send_counts.resize(count_machine);
	recv_counts.resize(count_machine);
	for (int i = 0; i < count_machine; i++) {
		send_counts[i] = send_buffers[i].size();
	}
	recv_counts.resize(count_machine);

	if (PRINT_MPI_LOG_DETAIL) {
		cout << "[INFO] in " << rank << ": count_machine : " << count_machine << endl;  //  << "size calculated: s=" << ssize << ", r=" << rsize << endl;
		cout << "[INFO] in " << rank << ": BEFORE SEND COUNT : ";  //  << "size calculated: s=" << ssize << ", r=" << rsize << endl;

		for (int i = 0; i < count_machine; i++) {
			cout << send_buffers[i].size() << ", ";
		}
		cout << endl;
	}
	MPI_Alltoall(send_counts.data(), 1, MPI_INT,
		recv_counts.data(), 1, MPI_INT,
		comm.comm); // exchange with each nodes
	if (PRINT_MPI_LOG_DETAIL) {
		cout << "[INFO] in " << rank << ": AFTER  SEND COUNT : ";
		for (int i = 0; i < count_machine; i++) {
			cout << recv_counts[i] << ", ";
		}
		cout << endl;
	}
	// displacement
	rdispl[0] = 0;
	sdispl[0] = 0;

	// ** important ** byte conversion
	//for (int i = 0; i < count_machine; i++) {
	//	send_counts[i] *= sizeof(GHSmsg);
	//	recv_counts[i] *= sizeof(GHSmsg);
	//}
	for (int i = 1; i < count_machine; i++) {
		sdispl[i] = send_counts[i - 1] + sdispl[i - 1];
		rdispl[i] = recv_counts[i - 1] + rdispl[i - 1];
	}
	// size
	int ssize, rsize;
	ssize = rsize = 0;
	for (int i = 0; i < count_machine; i++) {
		ssize += send_counts[i];
		rsize += recv_counts[i];
	}
	if(PRINT_MPI_LOG_DETAIL)
		cout << "[INFO] in " << rank << ", " << "size calculated: s="<<ssize<< ", r="<<rsize << endl;
	vector<GHSmsg> sbuffer;
	//recv_buffers.resize(rsize / sizeof(GHSmsg));
	recv_buffers.resize(rsize);

	for (auto im = send_buffers.begin(); im != send_buffers.end(); im++)
		for (auto imsg = im->second.begin(); imsg != im->second.end(); imsg++)
			sbuffer.push_back(*imsg);

#ifndef NONMPI
	int test_val = MPI_Alltoallv(sbuffer.data(), send_counts.data(), this->sdispl, comm.getType(),
		this->recv_buffers.data(), recv_counts.data(), this->rdispl, comm.getType(), comm.comm);
	if(PRINT_MPI_LOG_DETAIL) cout << "[MPI] in "<< rank<<", " << test_val << endl;
#endif // !NONMPI

#ifdef NONMPI
	recv_buffers = sbuffer;
#endif

	sbuffer.clear();
	for (auto iv = send_buffers.begin(); iv != send_buffers.end(); iv++) iv->second.clear();
}
