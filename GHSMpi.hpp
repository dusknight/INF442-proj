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
	double arg3;
};

class GHScomm {
	/**
	* See https://en.wikipedia.org/wiki/Message_Passing_Interface
	* See also http://www.cs.kent.edu/~farrell/cc08/lectures/mpi_datatypes.pdf
	**/
private:
	MPI_Datatype Message;
	MPI_Op op;

	std::queue<std::pair<int, GHSmsg>> recv;
	std::queue<std::pair<int, GHSmsg>> send;
	
	friend class GHSNode;
public:
	void init(MPI_User_function* f) {
		const int blocklen[] = { 1, 1, 1, 1 };
		const MPI_Aint disp[] = {
			offsetof(GHSmsg, type),
			offsetof(GHSmsg, arg1),
			offsetof(GHSmsg, arg2),
			offsetof(GHSmsg, arg3)
		};

		const MPI_Datatype types[] = {
			MPI_INT,
			MPI_INT,
			MPI_INT,
			MPI_DOUBLE
		};

		MPI_Datatype newtype;
		MPI_Type_create_struct(sizeof(types) / sizeof(*types), 
			blocklen, disp, types, &Message);
		MPI_Type_commit(&Message);

		if (f != nullptr) MPI_Op_create(f, 1, &op);
	}

	GHSmsg sendConnect(int val, int edgeId);
	GHSmsg recvConnect(int LN);

	GHSmsg sendInitiate(int LN, int FN, GHSNode::NodeState SN, int edgeId);

	GHSmsg sendTest(int LN, int FN, int edgeId);
	GHSmsg recvTest(int L, int F, int edgeId);

	GHSmsg sendAccept(int edgeId);

	GHSmsg sendReject(int edgeId);

	GHSmsg sendReport(double best_weight, int in_branch);
	GHSmsg recvReport(double in_weight, int edgeId);

	GHSmsg sendChangeCore(int edgeId);

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
	vector<GHSEdge::EdgeState> SE;
	int FN;  // fragemnt identity
	int LN;  // level
	int find_count;

	// for messages
	GHScomm comm;

	std::vector<GHSEdge> edges;  // ???

	// for Test
	int best_edge;

	//
	int in_branch;

	//
	int test_edge;

public:

	GHSNode(vector<Edge> edges);

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
};

class GHSMPI :public GraphInEdge{

};

#include <queue>
#include <unordered_map>
class GHS {
private:
	GHSNode local_nodes;
	std::unordered_map<Edge, std::queue<>> message_map;
};

#endif //INF442_P3_GHSMPI_HPP
