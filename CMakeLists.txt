cmake_minimum_required(VERSION 3.15)
project (INF442-P3)
set(CMAKE_CXX_STANDARD 14)
#SET(CMAKE_CXX_COMPILER "mpicc")

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

add_executable(task1 Graph.h Graph.cpp PrimAndBoruvka.cpp PrimAndBoruvka.hpp Kruskal.hpp Kruskal.cpp Task1.cpp)

add_executable(task2 Graph.h Graph.cpp PrimAndBoruvka.cpp PrimAndBoruvka.hpp Kruskal.hpp Kruskal.cpp Task2.cpp)

add_executable(task3Prim Graph.h  PrimDistributed.hpp PrimDistributed.cpp PrimAndBoruvka.cpp PrimAndBoruvka.hpp Kruskal.hpp Kruskal.cpp 
    Graph.cpp  Task3Prim.cpp)
target_link_libraries(task3Prim ${MPI_LIBRARIES})

add_executable(task3Boruvka Graph.h  BoruvkaDistributed.hpp BoruvkaDistributed.cpp PrimAndBoruvka.cpp PrimAndBoruvka.hpp Kruskal.hpp Kruskal.cpp Task3Boruvka.cpp
    Graph.cpp  )
target_link_libraries(task3Boruvka ${MPI_LIBRARIES})


add_executable(task4 Graph.h  GHSMpi.hpp
    Graph.cpp  GHSMpi.cpp Task4.cpp)
target_link_libraries(task4 ${MPI_LIBRARIES})

add_executable(task5 Graph.h Graph.cpp PrimAndBoruvka.cpp PrimAndBoruvka.hpp Kruskal.hpp Kruskal.cpp 
    kmeans.hpp kmeans.cpp 
    KruskalClustering.hpp KruskalClustering.cpp
    Task5.cpp)


add_executable(task6 Graph.h Graph.cpp PrimAndBoruvka.cpp PrimAndBoruvka.hpp Kruskal.hpp Kruskal.cpp 
    kmeans.hpp kmeans.cpp 
    KruskalClustering.hpp KruskalClustering.cpp
    Task6.cpp)

if(MPI_COMPILE_FLAGS)
  set_target_properties(task3 PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
  set_target_properties(task4 PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
  set_target_properties(task3 PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
  set_target_properties(task4 PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()