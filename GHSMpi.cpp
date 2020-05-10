//
// Created by Shikang on 4/30/2020.
//

#include "GHSMpi.hpp"
#include "GraphMpi.hpp"

#include<iostream>

using namespace std;

GHSNode::GHSNode(vector<Edge> _edges)
{
    SN = SLEEPING;
    for (int i = 0; i < _edges.size(); ++i) {
        GHSEdge ge(_edges[i]);
        edges.push_back(ge);
        SE[i] = GHSEdge::EdgeState::BASIC;
    }
    LN = 0;
    FN = 0;
    find_count = 0;

    best_edge = -1;
    test_edge = -1;
    in_branch = -1;

    finished = false;

    comm.init(NULL);
}

int GHSNode::find_best_edge() {
    /**
    if FOUND return id;
    else return -1;
    */
    double weight = DBL_MAX;
    int flag_found = -1;
    for (int ie = 0; ie<edges.size(); ++ie) {
        if(SE[ie] == GHSEdge::EdgeState::BASIC)
            if (SE[ie] < weight || SE[ie] == weight && ie < best_edge) {
                weight = edges[ie].cost;
                best_edge = ie;
                flag_found = ie;
            }
    }
    return flag_found;
}

int GHSNode::find_test_edge() {
    /**
    if FOUND return id;
    else return -1;
    */
    double weight = DBL_MAX;
    int flag_found = -1;
    for (int ie = 0; ie < edges.size(); ++ie) {
        if (SE[ie] == GHSEdge::EdgeState::BASIC)
            if (SE[ie] < weight || SE[ie] == weight && ie < test_edge) {
                weight = edges[ie].cost;
                test_edge = ie;
                flag_found = ie;
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

    for (int i = 0; i < edges.size(); i++) {
        if (i != edge_id && SE[i] == GHSEdge::EdgeState::BRANCH) {
            // send INIT (L, F, S) on edge i
            comm.sendInitiate(L, F, S, i);
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
            comm.sendInitiate(LN + 1, edges[edge_id].cost, FIND, edge_id);
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
    if (Edge::cmp(edges[edge_id], edges[best_edge])) // cost: a < b
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
        comm.sendReport(edges[best_edge].cost, in_branch);
    }
}

void GHSNode::RespReport(int w, int edge_id)
{
    // TODO: not identic HALT?
    if (edge_id != in_branch) {
        find_count--;
        if (w < edges[best_edge].cost) best_edge = edge_id;
        Report();
    }
    else if (SN == FIND) // place the received message at the end of queue
        comm.recvReport(w, edge_id);
    else if (w > edges[best_edge].cost) ChangeCore();
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

/////////

GHSmsg GHScomm::sendConnect(int val, int edgeId)
{
    GHSmsg msg;
    msg.type = MsgType::CONNECT;
    msg.arg1 = val;
    send.emplace(edgeId, msg);
    
    return msg;
}

GHSmsg GHScomm::recvConnect(int LN)
{
    GHSmsg msg;
    msg.type = MsgType::CONNECT;
    msg.arg1 = LN;
    recv.emplace(LN, msg);
    return msg;
}

GHSmsg GHScomm::sendInitiate(int LN, int FN, GHSNode::NodeState SN, int edgeId)
{
    GHSmsg msg;
    msg.type = MsgType::INIT;
    msg.arg1 = LN;
    msg.arg2 = FN;
    msg.arg3 = SN;
    send.emplace(edgeId, msg);
    return msg;
}

GHSmsg GHScomm::sendTest(int LN, int FN, int edgeId)
{
    GHSmsg msg;
    msg.type = MsgType::TEST;
    msg.arg1 = LN;
    msg.arg2 = FN;
    send.emplace(edgeId, msg);
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
    send.emplace(edgeId, msg);
    return msg;
}

GHSmsg GHScomm::sendReject(int edgeId)
{
    GHSmsg msg;
    msg.type = MsgType::REJECT;
    send.emplace(edgeId, msg);
    return msg;
}

GHSmsg GHScomm::sendReport(double best_weight, int in_branch)
{
    GHSmsg msg;
    msg.type = MsgType::REPORT;
    msg.arg3 = best_weight;
    send.emplace(in_branch, msg);
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
    send.emplace(edgeId, msg);
    return msg;
}



int main(int argc, char* argv[]) {
    ios_base::sync_with_stdio(false);

    Communicator::init(&argc, &argv);

    Communicator comm;

    if (comm.emit(argc < 2)) {
        comm.release();
        comm(cerr) << "Error: need TSPLIB file name\n";
        exit(1);
    }

    TspFile tsp;
    string name = string("./tsp/") + argv[1];

    if (comm.emit(!tsp.read((name + ".tsp").c_str()))) {
        comm(cerr) << "Error: can not read TSPLIB file: " << argv[1] << '\n';
        exit(1);
    }

    communication_interval = 10;
    temperature_thresh = 5;

    if (argc >= 3)
        communication_interval = atoi(argv[2]);
    if (argc >= 4)
        temperature_thresh = atoi(argv[3]);

    cout << "Communication interval: " << communication_interval << '\n'
        << "Temperature threshold: " << temperature_thresh << '\n';

    comm.bcast(&communication_interval, 1);
    comm.bcast(&temperature_thresh, 1);

    Path::init(tsp.size(), tsp.matrix());

    comm.bcast(&Path::ncity, 1);
    comm.bcast(Path::matrix, Path::ncity * Path::ncity);

    // wait for master
    comm.barrier();

    start(comm);

    TspFile tour;
    if (!tour.read((name + ".opt.tour").c_str())) {
        comm(cerr) << "Can not read TOUR file: " << argv[1] << '\n';
    }
    else {
        Path path;
        path.initialize();
        copy_n(tour.matrix(), Path::ncity, path.data());
        cout << "Standard answser: "
            << path.computeLength() << '\n';
    }

    comm.release();
    return 0;
}
