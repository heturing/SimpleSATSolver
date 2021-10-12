#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <boost/program_options.hpp>

#include "version.h"
#include "benchmark_reader.h"
#include "conjunction_clause.h"
#include "solver.h"

namespace po = boost::program_options;

template<typename T> std::ostream& operator<<(std::ostream &os, const std::vector<T> &v){
    
    std::copy(v.begin(), v.end(), std::ostream_iterator<T>(os, " "));
    return os;
}

std::pair<po::variables_map, po::options_description> parse_argument(int argc, char* argv[]){
    /*
        Config the a parser on how to parse command line options.
    */

   po::options_description generic("Generic options");
   generic.add_options()
   ("version,v", "print version string")
   ("help", "print help message");

   po::options_description hidden("Hidden options");
   hidden.add_options()
   ("input-file", po::value<std::vector<std::string>>(), "benchmark being run");

   po::options_description cmd_line("Command Line arguments");
   cmd_line.add(generic).add(hidden);

   po::positional_options_description p;
   p.add("input-file", -1);

   po::variables_map vm;

   po::store(po::command_line_parser(argc, argv).options(cmd_line).positional(p).run(), vm);
   po::notify(vm);

   return std::pair<po::variables_map, po::options_description>(vm, generic);
}

int main(int argc, char** argv){

    // Parse arguments
    auto p = parse_argument(argc, argv);
    auto vm = p.first;
    auto generic_desc = p.second;


    if(vm.count("help")){
        std::cout << generic_desc << std::endl;
        return 0;
    }

    if(vm.count("version")){
        std::cout << VERSION << std::endl;
        return 0;
    }

    if (vm.count("input-file")){
        std::cout << "Input files " 
                  << ((vm["input-file"].as<std::vector<std::string>>().size() > 1) ? "are :" : "is :") 
                  << vm["input-file"].as<std::vector<std::string>>() 
                  << std::endl;

        for(auto fn : vm["input-file"].as<std::vector<std::string>>()){
            // For each benchmark, read its content and create solver object.
            std::cout << fn << "\n" << std::string(80, '-') << std::endl;

            // Read benchmark file into Benchmark_reader file. The CNF is available by get_formula().
            Benchmark_reader br(fn);

            // This << operator call operator(ostream&, const Conjunction_clause) bc br.get_formula() is a rvalue here.
            std::cout << br.get_formula() << std::endl;

            // Create solver object
            Solver s(br.get_formula());

            std::cout << "created a solver." << std::endl;

            bool b = s.solve();
            std::cout << "solved:" << b << std::endl;
            auto model = s.get_model();

            for(auto l : model){
                std::cout << l << " ";
            }
            std::cout << std::endl;

        }
    }


    

    

    // Assign SAT problem to the solver

    // Call solver.solve()

    // Output the result
    return 0;

}