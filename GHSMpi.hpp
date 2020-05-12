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
	GHSmsg() {};
	GHSmsg(MsgType mt) :type(mt) {};
	GHSmsg(MsgType mt, int _arg1) :type(mt), arg1(_arg1) {};
	GHSmsg(MsgType mt, int _arg1, int _arg2) :type(mt), arg1(_arg1), arg2(_arg2) {};
	GHSmsg(MsgType mt, int _arg1, int _arg2, int _arg3) :type(mt), arg1(_arg1), arg2(_arg2), arg3(_arg3) {};
	GHSmsg(MsgType mt, double _f) :type(mt), argf(_f) {};
	GHSmsg(const GHSmsg& msg) :type(msg.type), arg1(msg.arg1), arg2(msg.arg2), arg3(msg.arg3), argf(msg.argf) {};
};

class GHScomm {
	/**
	* See https://en.wikipedia.org/wiki/Message_Passing_Interface
	* See also http://www.cs.kent.edu/~farrell/cc08/lectures/mpi_datatypes.pdf
	**/
private:
	int PE_num;
	int nodeId;
	GHSmsg demomsg;
	MPI_Datatype GHSmsgType;
	MPI_Op op;

	MPI_Comm comm=MPI_COMM_WORLD;

	std::queue<std::pair<int, GHSmsg>> recv_queue;
	//std::queue<std::pair<int, GHSmsg>> send_queue;
	
	friend class GHSNode;
public:

	void init(int argc, char* argv[]) { MPI_Init(&argc, &argv); }
	int commRank() {  return MPI_Comm_rank(comm, &PE_num); }
	void finalise() { MPI_Finalize(); }
	void commitType(MPI_User_function* f) {
		const int blocklen[] = { 1, 1, 1, 1, 1};
		MPI_Aint disp[54] = {
			offsetof(GHSmsg, type),
			offsetof(GHSmsg, arg1),
			offsetof(GHSmsg, arg2),
			offsetof(GHSmsg, arg3),
			offsetof(GHSmsg, argf)
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
			MPI_DOUBLE
		};

		MPI_Type_create_struct(sizeof(types) / sizeof(*types), 
			blocklen, disp, types, &GHSmsgType);
		MPI_Type_commit(&GHSmsgType);

		if (f != nullptr) MPI_Op_create(f, 1, &op);
	}

	void send(GHSmsg id_to_send, int dest) {
		MPI_Send(&id_to_send, 1, GHSmsgType, dest, 10, comm);
	}
	GHSmsg recv(int &from_node) {
		MPI_Status status;
		GHSmsg msg;
		MPI_Recv(&msg, 1, GHSmsgType, MPI_ANY_SOURCE, MPI_ANY_TAG, comm, &status);
		return msg;
	}
	void barrier() { MPI_Barrier(comm); }
	void sendConnect(int val, int edgeId);
	//GHSmsg recvConnect(int LN);

	void sendInitiate(int LN, int FN, int SN, int edgeId);

	void sendTest(int LN, int FN, int edgeId);
	//GHSmsg recvTest(int L, int F, int edgeId);

	void sendAccept(int edgeId);

	void sendReject(int edgeId);

	void sendReport(double best_weight, int in_branch);
	//GHSmsg recvReport(double in_weight, int edgeId);

	void sendChangeCore(int edgeId);

};


class GHSNode : public GraphInEdge {
	/**
	* See https://raw.github.com/arjungmenon/Distributed-Graph-Algorithms/master/Minimum-Spanning-Tree/papers/GHS_enhanced.pdf
	* for reference and algorithm details
	**/
protected:
public:
	static const enum NodeState { SLEEPING = 0, FIND, FOUND };
	
private:

	// friend class GHScomm;

	int id;
	bool finished;

	NodeState SN;  // state
	// vector<GHSEdge::EdgeState> SE;
	int FN;  // fragemnt identity
	int LN;  // level
	int find_count;

	// for messages
	GHScomm comm;
	//queue<GHSmsg> recv_msg;
	//queue<int>recv_msg_from;

	std::vector<GHSEdge> adj_out_edges;  // directed edges, adjacent edges

	// for Test
	int best_edge;

	//
	int in_branch;

	//
	int test_edge;

	int assign_id(int id_rank, int num_process);
	int get_idx_adj_out_node(int out_node);
public:

	GHSNode(vector<Edge> edges);
	GHSNode();
	void MsgHandler(GHSmsg msg, int from_edge);

	int find_best_edge(int & idx);

	int find_test_edge(int & idx);

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
	void Finish();

	bool merge_condition();
	bool absorb_condition();
	bool test_reply_condition();
	bool report_condition();
	bool fragment_connect_condition();

	void RUN(int argc, char* argv[]);

};

class GHSMPI :public GraphInEdge{

};

//#include <queue>
//#include <unordered_map>
//class GHS {
//private:
//	GHSNode local_nodes;
//	std::unordered_map<Edge, std::queue<>> message_map;
//};

#endif //INF442_P3_GHSMPI_HPP
