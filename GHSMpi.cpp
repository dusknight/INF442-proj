//
// Created by Shikang on 4/30/2020.
//

#include "GHSMpi.hpp"
#include "GraphMpi.hpp"

#include<iostream>

using namespace std;

int GHSNode::assign_id(int id_rank, int num_process)
{
    return id_rank;
}

GHSNode::GHSNode(vector<Edge> adj_edges)
{
    
    for (int i = 0; i < adj_edges.size(); ++i) {
        GHSEdge ge(adj_edges[i]);
        adj_out_edges.push_back(ge);
        SE[i] = GHSEdge::EdgeState::BASIC;
    }
    GHSNode();

    
}

GHSNode::GHSNode()
{
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
        RespInit(msg.arg1, msg.arg2, msg.arg3, from_edge);;
        break;
    case MsgType::TEST:
        RespTest(msg.arg1, msg.arg2, from_edge);
        break;
    case MsgType::ACCEPT:
        RespAccept(from_edge);
        break;
    case MsgType::REJECT:
        RespReject(from_edge);
        break;
    case MsgType::REPORT:
        RespReport(msg.arg3, from_edge); // arg3: weight
        break;
    case MsgType::CHANGE_CORE:
        RespChangeCore();
        break;
    default:
        std::cerr << "Error: unimplemented message type " << msg.type << std::endl;
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
    for (int ie = 0; ie<adj_out_edges.size(); ++ie) {
        if(SE[ie] == GHSEdge::EdgeState::BASIC)
            if (adj_out_edges[ie].cost < weight || adj_out_edges[ie].cost == weight && ie < best_edge) {
                weight = adj_out_edges[ie].cost;
                best_edge = adj_out_edges[ie].v;
                flag_found = best_edge;
            }
    }
    return flag_found;
}

int GHSNode::find_test_edge() {
    /**
    if FOUND return adj_node_id;
    else return -1;
    */
    double weight = DBL_MAX;
    int flag_found = -1;
    for (int ie = 0; ie < adj_out_edges.size(); ++ie) {
        if (SE[ie] == GHSEdge::EdgeState::BASIC)
            if (adj_out_edges[ie].cost < weight || adj_out_edges[ie].cost == weight && ie < test_edge) {
                weight = adj_out_edges[ie].cost;
                test_edge = adj_out_edges[ie].v;
                flag_found = adj_out_edges[ie].v;
            }
    }
    return flag_found;
}

void GHSNode::WakeUp() {
    // let m be adjacent edge if minimum weight;
    int m = find_best_edge();

    SE[m] = GHSEdge::EdgeState::BRANCH;
    LN = 0;
    SN = NodeState::FOUND;
    find_count = 0;
    
    // send Connect(0) on edge m;
    comm.sendConnect(0, m);
}

void GHSNode::RespInit(int L, int F, GHSNode::NodeState S, int edge_id)
{
    // CHECK : have difference with GH.
    LN = L;
    FN = F; 
    SN = S;
    in_branch = -1;
    // double best_wt = DBL_MAX;

    for (int i = 0; i < adj_out_edges.size(); i++) {
        if (adj_out_edges[i].v != edge_id && SE[i] == GHSEdge::EdgeState::BRANCH) {
            // send INIT (L, F, S) on edge i
            comm.sendInitiate(L, F, S, adj_out_edges[i].v);
            if (S == FIND) find_count++;
        }
        if (S == FIND) Test();
    }
    
}

void GHSNode::RespConnect(int level, int edge_id) {
    if (SN == SLEEPING) WakeUp();
    if (level < LN) {
        SE[edge_id] = GHSEdge::EdgeState::BRANCH;
        // send INIT (LN, FN, FN) on edge j;
        comm.sendInitiate(LN, FN, SN, edge_id);
        if (SN == FIND) find_count++;
    }
    else {
        if (SE[edge_id] == GHSEdge::EdgeState::BASIC) {
            // place received message on end of queue
            comm.recvConnect(level);
        }
        else {
            // send Init (LN+1, w(j), FIND) on edge j
            comm.sendInitiate(LN + 1, adj_out_edges[edge_id].cost, FIND, edge_id);
            // TODO: check double and int conversion (cost);
        }
    }
}

void GHSNode::Test()
{ 
    // TODO - check
    int test_edge = find_test_edge();
    if (test_edge > 0){ // found
        comm.sendTest(LN, FN, test_edge);
    }
    else {
        Report();
    }

}

void GHSNode::RespTest(int L, int F, int edge_id)
{
    if (SN == SLEEPING) WakeUp();

    if (L > LN) // place the received message at the end of the queue
        comm.recvTest(L, F, edge_id);
    else if (F != FN) comm.sendAccept(edge_id);
    else {
        if (SE[edge_id] == GHSEdge::EdgeState::BASIC)
            SE[edge_id] == GHSEdge::EdgeState::REJECTED;

        if (test_edge != edge_id)
            comm.sendReject(edge_id);
        else Test();
    }
}

void GHSNode::RespAccept(int edge_id)
{
    test_edge = -1;
    if (Edge::cmp(adj_out_edges[edge_id], adj_out_edges[best_edge])) // cost: a < b
        best_edge = edge_id;

    Report();
}

void GHSNode::RespReject(int edge_id)
{
    if(SE[edge_id] == GHSEdge::EdgeState::BASIC) 
        SE[edge_id] == GHSEdge::EdgeState::REJECTED;
    comm.sendReject(edge_id);
}

void GHSNode::Report()
{
    if (find_count == 0 && test_edge < 0) {
        SN = FOUND;
        comm.sendReport(adj_out_edges[best_edge].cost, in_branch);
    }
}

void GHSNode::RespReport(int w, int edge_id)
{
    // TODO: not identic HALT?
    if (edge_id != in_branch) {
        find_count--;
        if (w < adj_out_edges[best_edge].cost) best_edge = edge_id;
        Report();
    }
    else if (SN == FIND) // place the received message at the end of queue
        comm.recvReport(w, edge_id);
    else if (w > adj_out_edges[best_edge].cost) ChangeCore();
}

void GHSNode::ChangeCore()
{
    if (SE[best_edge] == GHSEdge::EdgeState::BRANCH)
        comm.sendChangeCore(best_edge);
    else {
        comm.sendConnect(LN, best_edge);
        SE[best_edge] = GHSEdge::EdgeState::BRANCH;
    }
}

void GHSNode::RespChangeCore()
{
    ChangeCore();
}

bool GHSNode::isFinished()
{
    return finished || merge_condition() || absorb_condition() ||
        test_reply_condition() || report_condition() || fragment_connect_condition();
}

void GHSNode::Finish()
{
    comm.finalise();
}

void GHSNode::RUN(int argc, char* argv[])
{
    string filename = "test_in.txt";
    // comm.init(0, NULL);
    comm.init(argc, argv); // MPI_Init

    int num_process, my_rank;
    MPI_Comm_size(comm.comm, &num_process);
    MPI_Comm_rank(comm.comm, &my_rank);
    // rank from 0 to N-1


    // read_file
    GraphInEdge gie;
    gie.ReadFile(filename);
    auto adj_edge = gie.toAdjecentList();
    const int id_start_from = 1; // FOR vertex name starting from 1
    int _lcl_vert_count = gie.getVertSize() / num_process;

    // TODO if num_processs < num nodes

    finished = false;
    id = assign_id(my_rank, num_process);
    int _lcl_id_from, _lcl_id_to;
    _lcl_id_from = id * _lcl_vert_count;
    _lcl_id_to = (id+1) * _lcl_vert_count;
    if (id == num_process - 1) _lcl_id_to = gie.getVertSize();  // last worker

    // TODO convert graph

    for (int i = _lcl_id_from; i < _lcl_id_to; i++) {
        for (auto j = adj_edge[i + id_start_from].begin(); j != adj_edge[i + id_start_from].end(); j++) {
            adj_out_edges.push_back(*j);
        }
    }

    comm.barrier(); // MPI_Barrier synchonolise
    WakeUp();

    while (! isFinished()) {
        if (!recv_msg.empty()) {
            GHSmsg msg = recv_msg.front();
            recv_msg.pop();
            MsgHandler(msg, from_edge);
        }
        GHSmsg msg = comm.recv();
        if msg.type != not
    }

}

/////////

GHSmsg GHScomm::sendConnect(int val, int edgeId)
{
    GHSmsg msg;
    msg.type = MsgType::CONNECT;
    msg.arg1 = val;
    //send.emplace(edgeId, msg);
    MPI_Send(&msg, 1, GHSmsgType, edgeId, MPI_ANY_TAG, comm);
    return msg;
}

GHSmsg GHScomm::recvConnect(int LN)
{
    GHSmsg msg;
    msg.type = MsgType::CONNECT;
    msg.arg1 = LN;
    // recv.emplace(LN, msg);
    MPI_Status status;
    MPI_Recv(&msg, 1, GHSmsgType, LN, MPI_ANY_TAG, comm, &status);
    return msg;
}

GHSmsg GHScomm::sendInitiate(int LN, int FN, GHSNode::NodeState SN, int edgeId)
{
    GHSmsg msg;
    msg.type = MsgType::INIT;
    msg.arg1 = LN;
    msg.arg2 = FN;
    msg.arg3 = SN;
    // send.emplace(edgeId, msg);
    MPI_Send(&msg, 1, GHSmsgType, edgeId, MPI_ANY_TAG, comm);
    return msg;
}

GHSmsg GHScomm::sendTest(int LN, int FN, int edgeId)
{
    GHSmsg msg;
    msg.type = MsgType::TEST;
    msg.arg1 = LN;
    msg.arg2 = FN;
    // send.emplace(edgeId, msg);
    MPI_Send(&msg, 1, GHSmsgType, edgeId, MPI_ANY_TAG, comm);
    return msg;
}

GHSmsg GHScomm::recvTest(int L, int F, int edgeId)
{
    GHSmsg msg;
    msg.type = MsgType::TEST;
    msg.arg1 = L;
    msg.arg2 = F;
    recv.emplace(edgeId, msg); // TODO: check not identique
}

GHSmsg GHScomm::sendAccept(int edgeId)
{
    GHSmsg msg;
    msg.type = MsgType::ACCEPT;
    // send.emplace(edgeId, msg);
    MPI_Send(&msg, 1, GHSmsgType, edgeId, MPI_ANY_TAG, comm);
    return msg;
}

GHSmsg GHScomm::sendReject(int edgeId)
{
    GHSmsg msg;
    msg.type = MsgType::REJECT;
    // send.emplace(edgeId, msg);
    MPI_Send(&msg, 1, GHSmsgType, edgeId, MPI_ANY_TAG, comm);
    return msg;
}

GHSmsg GHScomm::sendReport(double best_weight, int in_branch)
{
    GHSmsg msg;
    msg.type = MsgType::REPORT;
    msg.arg3 = best_weight;
    // send.emplace(in_branch, msg);
    MPI_Send(&msg, 1, GHSmsgType, in_branch, MPI_ANY_TAG, comm);
    return msg;
}

GHSmsg GHScomm::recvReport(double in_weight, int edgeId)
{
    GHSmsg msg;
    msg.type = MsgType::REPORT;
    msg.arg3 = in_weight;
    recv.emplace(edgeId, msg);
    return msg;
}

GHSmsg GHScomm::sendChangeCore(int edgeId)
{
    GHSmsg msg;
    msg.type = MsgType::CHANGE_CORE;
    // send.emplace(edgeId, msg);
    MPI_Send(&msg, 1, GHSmsgType, edgeId, MPI_ANY_TAG, comm);
    return msg;
}

