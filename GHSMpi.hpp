//
// Created by Shikang on 4/30/2020.
//

#ifndef INF442_P3_GHSMPI_HPP
#define INF442_P3_GHSMPI_HPP

#include "Graph.h"
#include <mpi.h>
#include <queue>
#include <map>


struct GHSEdge :public Edge {
	static const enum EdgeState { REJECTED = 0, BRANCH, BASIC };
	EdgeState state;
	GHSEdge(int u_, int v_, double cost_, EdgeState es_) :Edge(u_, v_, cost_) {
		state = es_;
	}
	GHSEdge(Edge e, EdgeState es_) :Edge(e) {
		state = es_;
	}
	GHSEdge(Edge e) :Edge(e) {
		state = BASIC;
	}
	GHSEdge(const GHSEdge& e):Edge(e){
		state = e.state;
	}
	GHSEdge() {
		state = BASIC;
		u = -1;
		v = -1;
		cost = -1;
	}
};

enum MsgType {
	INIT = 0,
	REPORT,
	TEST,
	CHANGE_CORE,
	CONNECT,
	REJECT,
	ACCEPT,
};

struct GHSmsg {
	int type;
	int arg1;
	int arg2;
	int arg3;
	double argf;
	int dest_vid;
	int src_vid;
	GHSmsg() { type = -1; arg1 = arg2 = arg3 = argf = dest_vid = src_vid = -1; }
	GHSmsg(MsgType mt, int _dest, int _src) :type(mt), dest_vid(_dest), src_vid(_src) {};
	GHSmsg(MsgType mt, int _arg1, int _dest, int _src) :type(mt), arg1(_arg1), dest_vid(_dest), src_vid(_src) {};
	GHSmsg(MsgType mt, int _arg1, int _arg2, int _dest, int _src) :type(mt), arg1(_arg1), arg2(_arg2), dest_vid(_dest), src_vid(_src) {};
	GHSmsg(MsgType mt, int _arg1, int _arg2, int _arg3, int _dest, int _src) :type(mt), arg1(_arg1), arg2(_arg2), arg3(_arg3), dest_vid(_dest), src_vid(_src) {};
	GHSmsg(MsgType mt, double _f, int _dest, int _src) :type(mt), argf(_f), dest_vid(_dest), src_vid(_src) {};
	GHSmsg(const GHSmsg& msg) :type(msg.type), arg1(msg.arg1), arg2(msg.arg2), arg3(msg.arg3), argf(msg.argf), dest_vid(msg.dest_vid), src_vid(msg.src_vid) {};
};

class GHScomm {
	/**
	* See https://en.wikipedia.org/wiki/Message_Passing_Interface
	* See also http://www.cs.kent.edu/~farrell/cc08/lectures/mpi_datatypes.pdf
	**/
	friend class GHSMPI;
private:
	int PE_num;
	int nodeId;
	GHSmsg demomsg;
	MPI_Datatype GHSmsgType;
	MPI_Op op;

	MPI_Comm comm=MPI_COMM_WORLD;

	std::queue<std::pair<int, GHSmsg>> recv_queue;
	// std::queue<std::pair<int, GHSmsg>> send_queue;
	
	friend class GHSNode;
public:

	void init(int argc, char* argv[]) { MPI_Init(&argc, &argv); }
	int commRank() {  return MPI_Comm_rank(comm, &PE_num); }
	void finalise() { MPI_Finalize(); }
	void commitType(MPI_User_function* f) {
		const int blocklen[] = { 1, 1, 1, 1, 1, 1};
		MPI_Aint disp[6] = {
			offsetof(GHSmsg, type),
			offsetof(GHSmsg, arg1),
			offsetof(GHSmsg, arg2),
			offsetof(GHSmsg, arg3),
			offsetof(GHSmsg, argf),
			offsetof(GHSmsg, dest_vid)
		};
		//MPI_Get_address((void*)demomsg.type, disp);
		//MPI_Get_address((void*)demomsg.arg1, disp+1);
		//MPI_Get_address((void*)demomsg.arg2, disp+2);
		//MPI_Get_address((void*)demomsg.arg3, disp+3);


		const MPI_Datatype types[] = {
			MPI_INT,
			MPI_INT,
			MPI_INT,
			MPI_INT,
			MPI_DOUBLE,
			MPI_INT
		};

		MPI_Type_create_struct(sizeof(types) / sizeof(*types), 
			blocklen, disp, types, &GHSmsgType);
		MPI_Type_commit(&GHSmsgType);

		if (f != nullptr) MPI_Op_create(f, 1, &op);
	}
	MPI_Datatype getType() {
		return GHSmsgType;
	}

	void send(GHSmsg msg_to_send, int dest) {
		MPI_Send(&msg_to_send, 1, GHSmsgType, dest, 10, comm);
	}
	GHSmsg recv(int &from_m_node) {
		MPI_Status status;
		GHSmsg msg;
		MPI_Recv(&msg, 1, GHSmsgType, MPI_ANY_SOURCE, MPI_ANY_TAG, comm, &status);
		return msg;
	}
	void barrier() { MPI_Barrier(comm); }
	void sendConnect(int val, int edgeId, int src_eid, int machineId);
	//GHSmsg recvConnect(int LN);

	void sendInitiate(int LN, int FN, int SN, int edgeId, int src_eid, int machineId);

	void sendTest(int LN, int FN, int edgeId, int src_eid, int machineId);
	//GHSmsg recvTest(int L, int F, int edgeId);

	void sendAccept(int edgeId, int src_eid, int machineId);

	void sendReject(int edgeId, int src_eid, int machineId);

	void sendReport(double best_weight, int in_branch, int src_eid, int machineId);
	//GHSmsg recvReport(double in_weight, int edgeId);

	void sendChangeCore(int edgeId, int src_eid, int machineId);

};

class GHSMPI;

class GHSNode : public GraphInEdge {
	/**
	* See https://raw.github.com/arjungmenon/Distributed-Graph-Algorithms/master/Minimum-Spanning-Tree/papers/GHS_enhanced.pdf
	* for reference and algorithm details
	**/
protected:
public:
	static const enum NodeState { SLEEPING = 0, FIND, FOUND };
	
private:
	GHSMPI* machine;
	// friend class GHScomm;

	int id;
	bool finished;

	NodeState SN;  // state
	// vector<GHSEdge::EdgeState> SE;
	int FN;  // fragemnt identity
	int LN;  // level
	int find_count;

	// for messages
	// GHScomm comm;
	queue<GHSmsg> recv_msg;
	//queue<int>recv_msg_from;

	map<int, GHSEdge> adj_out_edges;  // directed edges, adjacent edges

	// for Test
	int best_edge;

	//
	int in_branch;

	//
	int test_edge;

	int assign_id(int id_rank, int num_process);
	// int get_idx_adj_out_node(int out_node);
public:

	GHSNode(int _id, vector<Edge> _edges) :id(_id) { 
		for (int i = 0; i < _edges.size(); ++i) {
			GHSEdge ge(_edges[i]);
			adj_out_edges[ge.v] = ge;
			// adj_out_edges.push_back(ge);

			//SE[i] = GHSEdge::EdgeState::BASIC;
		}
		machine = NULL;
		SN = SLEEPING;
		LN = 0;
		FN = 0;
		find_count = 0;

		best_edge = -1;
		test_edge = -1;
		in_branch = -1;

		finished = false;
	}
	GHSNode(vector<Edge> edges);
	GHSNode();
	void set_machine(GHSMPI* _machine) { machine = _machine; }
	int getSN() { return SN; }
	void MsgHandler(GHSmsg msg, int from_edge);

	int find_best_edge();
	int find_test_edge();

	void WakeUp();

	void RespInit(int L, int F, GHSNode::NodeState S, int edge_id);

	void RespConnect(int level, int edge_id);

	void Test();
	void RespTest(int L, int F, int edge_id);

	void RespAccept(int edge_id);

	void RespReject(int edge_id);

	void Report();
	void RespReport(int w, int edge_id);

	void ChangeCore();
	void RespChangeCore();

	bool isFinished();
	//void Finish();
	vector<int> get_branches();

	bool merge_condition();
	bool absorb_condition();
	bool test_reply_condition();
	bool report_condition();
	bool fragment_connect_condition();

	//void RUN(int argc, char* argv[]);
};

class GHSMPI :public GraphInEdge{
	/**
	Machine : each instance of this class should be executed as one MPI node.
	*/
	friend class GHSNode;
private:

	bool ready;
	int rank;
	int count_machine;
	GHScomm comm;
	vector<int> vertices_id;
	map<int, GHSNode> nodes;
	map<int, int> vert2machine; //vertice id to machine;
	bool is_all_finished;

	queue<GHSmsg> to_send;
	queue<GHSmsg> recved_this_time;
	queue<GHSmsg> recved_later;

	map<int, vector<GHSmsg>> send_buffers; // idx == machine_rank (machine_id)
	vector<GHSmsg> recv_buffers;

	int* sdispl, *rdispl;
	
public:
	GHSMPI() {
		ready = false;
		is_all_finished = false;
		count_machine = NULL;
		rank = NULL;
	}

	int assign_vertice_to_machine(int v_id);  // values will be added to vert2machine
	bool ask_all_nodes_if_finished();
	void print_node_states();

	void send_msg(GHSmsg msg);
	void emplace_recv_queue(GHSmsg msg);
	void exec_send_recv();

	void init();
	void run_loop();
	void msg_handler(GHSmsg msg);
	void finalize();

	//void exchange_with_one_machine(int machine);
	void exchange_with_all_machine();
};

//#include <queue>
//#include <unordered_map>
//class GHS {
//private:
//	GHSNode local_nodes;
//	std::unordered_map<Edge, std::queue<>> message_map;
//};

#endif //INF442_P3_GHSMPI_HPP
