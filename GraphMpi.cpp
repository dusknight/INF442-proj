//
// Created by Shikang on 4/30/2020.
//

#include "GraphMpi.hpp"
#include <mpi.h>

Communicator::Communicator(const MPI_Comm& comm)
    : _comm(comm) {
    MPI_Comm_rank(_comm, &_rank);
    MPI_Comm_size(_comm, &_size);
}

void Communicator::init(int* argc, char*** argv) {
    MPI_Init(argc, argv);
}

void Communicator::barrier() {
    MPI_Barrier(_comm);
}

Communicator Communicator::split1D(int color, int key) {
    MPI_Comm comm;
    MPI_Comm_split(_comm, color, key, &comm);
    return Communicator(comm);
}

void Communicator::release() {
    MPI_Finalize();
}

void Communicator::gatherInt(int* data) {
    int send_value = 0;
    MPI_Gather(&send_value, 1, MPI_INT, data, 1, MPI_INT,
        MASTER_RANK, _comm);
}

void Communicator::gatherInt(int data) {
    MPI_Gather(&data, 1, MPI_INT, nullptr, 0, MPI_INT,
        MASTER_RANK, _comm);
}

void Communicator::gather(int* data, int size, int root) {
    if (root == _rank) {
        MPI_Gather(MPI_IN_PLACE, size, MPI_INT, data, size, MPI_INT, root, _comm);
    }
    else {
        MPI_Gather(data, size, MPI_INT, nullptr, 0, MPI_INT, root, _comm);
    }
}

void Communicator::bcast(int* data, int size, int rank) {
    MPI_Bcast(data, size, MPI_INT, rank, _comm);
}

void Communicator::gatherAll(int* send, int* recv,
    const int* disp_array,
    const int* count_array) {
    MPI_Allgatherv(send, count_array[_rank], MPI_INT, recv,
        count_array, disp_array, MPI_INT, _comm);
}

void Communicator::gatherAll(int* send, int send_size,
    int* recv, int recv_size) {
    MPI_Allgather(send, send_size, MPI_INT, recv, recv_size, MPI_INT, _comm);
}

void Communicator::send(int* data, int size, int dst) {
    MPI_Send(data, size, MPI_INT, dst, _rank, _comm);
}
void Communicator::recv(int* data, int size, int src) {
    MPI_Status status;
    MPI_Recv(data, size, MPI_INT, src, src, _comm, &status);
}
void Communicator::scatter(int* data, int size, int root) {
    if (root == _rank)
        MPI_Scatter(data, size, MPI_INT,
            MPI_IN_PLACE, size, MPI_INT, root, _comm);
    else
        MPI_Scatter(nullptr, 0, MPI_INT, data, size, MPI_INT, root, _comm);
}

bool Communicator::emit(bool signal) {
    int val = signal;
    MPI_Bcast(&val, 1, MPI_INT, MASTER_RANK, _comm);
    return val;
}

bool Communicator::signal() {
    int val;
    MPI_Bcast(&val, 1, MPI_INT, MASTER_RANK, _comm);
    return val;
}

int Communicator::sum(int n) {
    int ret;
    MPI_Allreduce(&n, &ret, 1, MPI_INT, MPI_SUM, _comm);
    return ret;
}
