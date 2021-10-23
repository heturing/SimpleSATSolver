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

std::pair<po::variables_map, po::options_description> parse_argument(int argc, char* argv[]){
    /*
        Parse the command line options. Return the argument map and the option descriptor as a pair.

        Usage:
            SAT_Solver benchmark_file

        Command line arguments:
            --version => print current version.
            --help => print help message.
            --input-file => (positional option) specify the benchmark file to solve.
            --dump-to-file FILE => where to output the result.
            -v | --verbose => whether output intermediate processing detail.
    */

   po::options_description generic("Generic options");
   generic.add_options()
   ("version", "print version string")
   ("help", "print help message")
   ("dump-to-file", po::value<std::string>(), "where to output the result")
   ("verbose,v", "whether output intermediate processing detail");

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

void dump_result(std::ostream &is, std::vector<Literal> model){
    is << model << std::endl;
}

int main(int argc, char** argv){

    // Parse arguments
    auto p = parse_argument(argc, argv);
    po::variables_map vm = p.first;
    po::options_description generic_desc = p.second;


    // perform operation based on given arguments.
    if(vm.count("help")){
        std::cout << generic_desc << std::endl;
        return 0;
    }

    if(vm.count("version")){
        std::cout << VERSION << std::endl;
        return 0;
    }

    bool verbose = false;
    if(vm.count("verbose")){
        verbose = true;
    }

    std::string output_file = "";
    if(vm.count("dump-to-file")){
        output_file = vm["dump-to-file"].as<std::string>();
    }

    if (vm.count("input-file")){
        for(auto fn : vm["input-file"].as<std::vector<std::string>>()){
            Benchmark_reader br(fn);
            Conjunction_clause cnf = br.get_formula();

            // Create solver object
            Solver s(cnf, verbose);

            bool b = s.solve();
            std::cout << "Benchmark " << fn << ":" << (b ? "SAT" : "UNSAT") << std::endl;
            if(b){
                std::vector<Literal> model = s.get_model();
                if(output_file != ""){
                    std::ofstream ofs(output_file, std::ios::app);
                    dump_result(ofs, model);
                }
                else{
                    dump_result(std::cout, model);
                }
            }
        }
    }
    return 0;

}