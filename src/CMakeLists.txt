set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Boost QUIET COMPONENTS program_options REQUIRED)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost Not found")
endif()

include_directories(${Boost_INCLUDE_DIR})

message(${Boost_INCLUDE_DIR})

add_executable(SAT_Solver main.cpp benchmark_reader.cpp conjunction_clause.cpp disjunction_clause.cpp implication_graph.cpp node.cpp solver.cpp variable.cpp literal.cpp)

target_link_libraries(SAT_Solver PUBLIC Boost::program_options)