#include <iostream>

#include "BlockingPattern/blocking_pattern.h"
#include "BlockingPattern/generator.h"

#include "SATInstance.h"
#include "omp.h"

int main() {
    typedef unsigned int UINT_T;

    UINT_T n = 2;
    UINT_T batch_size = 100000;

    auto* bp = new BlockingPattern<UINT_T>("bw\nwb", n);
    auto* cg = new ClauseGenerator<UINT_T>(bp);
    batch_size = batch_size < cg->n_clauses ? batch_size : cg->n_clauses;

    auto clauses = new vector<SATInstance<UINT_T>::ClauseHashSet*>;
    clauses->push_back(new SATInstance<UINT_T>::ClauseHashSet());

    clauses->at(0)->set_empty_key(new Clause<UINT_T>(nullptr, 0));
    clauses->at(0)->set_deleted_key(new Clause<UINT_T>(nullptr, 0));

    auto var_arr = new VariablesArray<UINT_T>(n * n);
    auto satInstance = new SATInstance<UINT_T>(var_arr, 1, 0);
    bool isValid;

    while (!isValid) {
        isValid = true;

        UINT_T global_clause_count = 0;

        while (global_clause_count < cg->n_clauses) {
            UINT_T clause_count = 0;

            while (clause_count < batch_size) {
                auto clause = cg->yieldClause();
                if (clause != nullptr) {
                    clauses->at(0)->insert(clause);

                    clause_count++;
                    global_clause_count++;
                } else {
                    exit(1);
                }
            }

            auto statistics = satInstance->solve(clauses, true);

            std::cout << "------------ STATISTICS -------------\n#Clauses, # Resamples\t= "
                         + to_string(statistics->n_resamples) << std::endl;

            std::cout << "\n\tThread 1: " + to_string(clauses->at(0)->size()) + ", " +
                         to_string((statistics->n_thread_resamples).at(0)) << std::endl;

            std::cout << "\n\nAvg. UNSAT MIS Size = " + to_string(statistics->avg_mis_size) +
                         "\n-------------------------------------\n" << std::endl;

            // Memory management
            for (auto c : *clauses->at(0)) {
                c->literals->clear();
                delete c->literals;
                delete c;
            }

            clauses->at(0)->clear();
        }

        // Verifying...

        for (UINT_T i = 0; i < cg->n_clauses; i++) {
            auto clause = cg->getEnumerateClause(i);
            if (clause != nullptr) {
                clauses->at(0)->insert(clause);
            } else {
                exit(1);
            }
        }

        isValid = isValid & satInstance->verify_validity(clauses);

        // Memory management
        for (auto c : *clauses->at(0)) {
            c->literals->clear();
            delete c->literals;
            delete c;
        }

        clauses->at(0)->clear();
    }

    for (int i = 0; i < var_arr->n_vars; i++) {
        cout << i << ": " << var_arr->vars[i] << endl;
    }

    return 0;
}
