cmake_minimum_required(VERSION 3.15)
project (INF442-P3)
set(CMAKE_CXX_STANDARD 14)

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

add_executable(test Graph.h Graph.cpp )

add_executable(task3-master Graph.h GraphMpi.hpp
    Graph.cpp GraphMpi.cpp)
target_link_libraries(task3-master ${MPI_LIBRARIES})

add_executable(task3-worker Graph.h GraphMpi.hpp
    Graph.cpp GraphMpi.cpp)
target_link_libraries(task3-worker ${MPI_LIBRARIES})

add_executable(task4-master Graph.h GraphMpi.hpp GHSMpi.hpp
    Graph.cpp GraphMpi.cpp GHSMpi.cpp )
target_link_libraries(task4-master ${MPI_LIBRARIES})

add_executable(task4-worker Graph.h GraphMpi.hpp GHSMpi.hpp
    Graph.cpp GraphMpi.cpp GHSMpi.cpp)
target_link_libraries(task4-worker ${MPI_LIBRARIES})

if(MPI_COMPILE_FLAGS)
  set_target_properties(task3-worker PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
  set_target_properties(task3-master PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
  set_target_properties(task4-worker PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
  set_target_properties(task4-master PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
  set_target_properties(task3-worker PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
  set_target_properties(task3-master PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
  set_target_properties(task4-worker PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
  set_target_properties(task4-master PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()