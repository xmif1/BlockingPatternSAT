#include <vector>
#include <cstring>
#include <fstream>
#include <iostream>
#include <functional>

using namespace std;

#include <boost/program_options.hpp>
using namespace boost::program_options;

#include "BlockingPattern/blocking_pattern.h"
#include "SATInstance.h"

typedef uint64_t UINT_T;
typedef SATInstance<UINT_T>::ClauseArray ClauseArray;
typedef uint8_t byte;

Clause<UINT_T>* getEnumeratedClause(UINT_T, unsigned short int);
string get_mask(int h, int b);

auto blocking_patterns = new vector<BlockingPattern<UINT_T>*>();

int main(int argc, char *argv[]){
    UINT_T n = 600;
    UINT_T n_clauses = 0;
    UINT_T batch_size = 100000;

    unsigned short int n_threads = 0;

    int h = 10;
    int b = 9;

    bool generate = false;

    try {
        options_description desc{"Options"};
        desc.add_options()
                ("help", "Help")
                ("parallel,p", value<unsigned short int>()->default_value(0), "Use parallel solver")
                ("batch_size,c", value<UINT_T>()->required(), "Clause batch-size per thread")
                ("dimension,n", value<UINT_T>()->required(), "Grid dimension")
                ("mask_size,h", value<int>()->required(), "Mask dimension h x h")
                ("diagonal,b", value<int>()->required(), "Number of black cells b <= h on the diagonal")
                ("generate,g", "Generate and output instance as DIMACS CNF (without solving)");

        variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);

        if(vm.count("help")) {
            std::cout << desc << '\n';
            exit(0);
        } else {
            notify(vm);

            if(vm.count("generate")){
                generate = true;
            }

            if(vm.count("parallel")){
                if (vm["parallel"].as<unsigned short int>() < 0 || vm["parallel"].as<unsigned short int>() > omp_get_num_procs()) {
                    n_threads = omp_get_num_procs();
                } else if (vm["parallel"].as<unsigned short int>() > 0) {
                    n_threads = vm["parallel"].as<unsigned short int>();
                } else {
                    n_threads = 1;
                }
            }

            if (vm.count("batch_size")) {
                batch_size = vm["batch_size"].as<UINT_T>();
            }

            if (vm.count("dimension")) {
                n = vm["dimension"].as<UINT_T>();
            }

            if (vm.count("mask_size")) {
                h = vm["mask_size"].as<int>();
            }

            if (vm.count("diagonal")) {
                b = vm["diagonal"].as<int>();
            }
        }
    } catch (const error &e) {
        cerr << e.what() << endl;
        exit(1);
    }

    auto* bp1 = new BlockingPattern<UINT_T>(get_mask(h, b), n);

    blocking_patterns->push_back(bp1); n_clauses += bp1->n_clauses;

    batch_size = batch_size < n_clauses ? batch_size : n_clauses;

    auto var_arr = new VariablesArray<UINT_T>(n * n);
    auto satInstance = new SATInstance<UINT_T>(var_arr, n_threads);

    if (generate) {
        string f_name = "n" + to_string(n) + "_h" + to_string(h) + "_b" + to_string(b) + ".cnf";
        auto out_f = new ofstream(f_name);
        satInstance->writeDIMACS(getEnumeratedClause, n_clauses, out_f);
        out_f->close();
    } else {
        auto statistics = satInstance->solve(getEnumeratedClause, n_clauses, batch_size);

        cout << "------------ STATISTICS -------------\n# Iterations\t= "
                + to_string(statistics->n_iterations)
                + "\n# Resamples\t= " + to_string(statistics->n_resamples);

        for (int t = 0; t < n_threads; t++) {
            cout << "\n\tThread " + to_string(t+1) + ": " + to_string((statistics->n_thread_resamples).at(t));
        }

        cout << "\n\nAvg. UNSAT MIS Size = " + to_string(statistics->avg_mis_size)
                + "\n-------------------------------------\n\n";
    }

    return 0;
}

Clause<UINT_T>* getEnumeratedClause(UINT_T idx, unsigned short int t_id) {
    UINT_T offset = 0;

    for (auto& blocking_pattern : *blocking_patterns) {
        if ((idx - offset) < blocking_pattern->n_clauses) {
            return new Clause<UINT_T>(blocking_pattern->getLiterals(idx - offset), t_id);
        }

        offset += blocking_pattern->n_clauses;
    }

    return nullptr;
}

string get_mask(int h, int b) {
    string mask;
    int offset = 0;

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < h; j++) {
            if (offset < b && j == offset) {
                mask += "1";
            } else {
                mask += "0";
            }
        }

        offset += 1;

        if (i != h - 1) {
            mask += "\n";
        }
    }

    return mask;
}
