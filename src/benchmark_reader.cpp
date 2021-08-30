#include "benchmark_reader.h"
#include <stdexcept>
#include <sstream>
#include "disjunction_clause.h"

Benchmark_reader::Benchmark_reader(std::string file_name){
    std::ifstream ifs(file_name);
    if(!ifs){
        throw std::invalid_argument("Cannot open file " + file_name);
    }

    std::string line;
    size_t num_var, num_clauses;
    std::vector<int> v;
    Conjunction_clause cnf;

    while(getline(ifs, line)){
        if(line[0] == 'c'){
            // Comment line
            continue;
        }
        else if(line[0] == 'p'){
            std::istringstream iss(line);
            std::string term;
            
            // There should be exactly 4 terms in a 'p' line.
            iss >> term >> term;
            if(term != "cnf"){
                std::cerr << term << std::endl;
                throw std::runtime_error("Benchmark " + file_name + " does not contain formula in CNF.");
            }
            iss >> num_var;
            iss >> num_clauses;
        }
        else if(line[0] == '%'){
            break;
        }
        else{
            // A clause line
            std::istringstream iss(line);
            int num;
            while(iss >> num){
                if(num == 0){
                    // Create variables, then create a disjunction_clause.
                    Disjunction_clause dc;
                    for(auto e : v){
                        dc.add_variable(Variable("x" + std::to_string(abs(e)), abs(e) == e));
                    }
                    cnf.add_clause(dc);

                    v.clear();
                }
                else{
                    v.push_back(num);
                }
                
            }
        }
    }

    this->cc = cnf;
};