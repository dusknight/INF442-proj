//
// Created by Shikang on 4/30/2020.
//

#ifndef INF442_P3_GRAPHMPI_HPP
#define INF442_P3_GRAPHMPI_HPP

#include <mpi.h>
#define MASTER_RANK 0

class Communicator {
public:
    Communicator(const MPI_Comm& comm = MPI_COMM_WORLD);

    static void init(int* argc, char*** argv);
    static void release();

    int rank() { return _rank; }
    int size() { return _size; }

    Communicator split1D(int color, int key);
    void barrier();

    void bcast(int* data, int size, int rank = MASTER_RANK);

    void gatherInt(int* data); // for master
    void gatherInt(int data); // for worker

    void gather(int* data, int size, int root);

    void gatherAll(int* send, int* recv,
        const int* disp_array, const int* count_array);

    void gatherAll(int* send, int send_size, int* recv, int recv_size);

    void send(int* data, int size, int dst);
    void recv(int* data, int size, int src);
    void scatter(int* data, int size, int root);

    bool emit(bool signal); // for master
    bool signal(); // for worker

    template < typename OUT >
    OUT& operator()(OUT& out) {
        return out << "[Process " << _rank << "] ";
    }

    int sum(int n);

private:
    int _size;
    int _rank;
    MPI_Comm _comm;
};

class GraphMPI {

};


#endif //INF442_P3_GRAPHMPI_HPP
