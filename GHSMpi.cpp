//
// Created by Shikang on 4/30/2020.
//

#include "GHSMpi.hpp"


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
    } else {
        Path path;
        path.initialize();
        copy_n(tour.matrix(), Path::ncity, path.data());
        cout << "Standard answser: "
             << path.computeLength() << '\n';
    }

    comm.release();
    return 0;
}