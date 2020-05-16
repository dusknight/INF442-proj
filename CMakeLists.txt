cmake_minimum_required(VERSION 3.15)
project (INF442-P3)
set(CMAKE_CXX_STANDARD 14)
#SET(CMAKE_CXX_COMPILER "mpicc")

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

add_executable(task1 Graph.h Graph.cpp PrimAndBoruvka.cpp PrimAndBoruvka.hpp Kruskal.hpp Kruskal.cpp Task1.cpp)

add_executable(task2 Graph.h Graph.cpp PrimAndBoruvka.cpp PrimAndBoruvka.hpp Kruskal.hpp Kruskal.cpp Task2.cpp)

add_executable(task3-master Graph.h GraphMpi.hpp
    Graph.cpp GraphMpi.cpp)
target_link_libraries(task3-master ${MPI_LIBRARIES})

add_executable(task3-worker Graph.h GraphMpi.hpp
    Graph.cpp GraphMpi.cpp)
target_link_libraries(task3-worker ${MPI_LIBRARIES})

add_executable(task4 Graph.h GraphMpi.hpp GHSMpi.hpp
    Graph.cpp GraphMpi.cpp GHSMpi.cpp Task4.cpp)
target_link_libraries(task4 ${MPI_LIBRARIES})

add_executable(task5 Graph.h Graph.cpp PrimAndBoruvka.cpp PrimAndBoruvka.hpp Kruskal.hpp Kruskal.cpp kmeans.cpp Task5.cpp)


if(MPI_COMPILE_FLAGS)
  set_target_properties(task3-worker PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
  set_target_properties(task3-master PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
  set_target_properties(task4 PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
  set_target_properties(task3-worker PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
  set_target_properties(task3-master PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
  set_target_properties(task4 PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()