//
// Created by Xandru Mifsud on 19/08/2023.
//

#ifndef CLAUSE_GENERATOR_H
#define CLAUSE_GENERATOR_H

#include "cmath"
#include "Clause.h"
#include "blocking_pattern.h"

template<class T, class Enable = void>
class ClauseGenerator {}; // Default specialisation

template <class T> // Specialisation to integral types
class ClauseGenerator<T, typename enable_if<is_integral<T>::value>::type> {

    public:
        T n_clauses = 0;

        explicit ClauseGenerator(vector<BlockingPattern<T>*>* blocking_patterns) {
            this->blocking_patterns = blocking_patterns;

            for (int i = 0; i < blocking_patterns->size(); i++) {
                n_clauses += blocking_patterns->at(i)->n_clauses;
            }
        }

        explicit ClauseGenerator(BlockingPattern<T>* blocking_pattern) {
            this->blocking_patterns = new vector<BlockingPattern<T>*>();
            this->blocking_patterns->push_back(blocking_pattern);

            n_clauses = blocking_pattern->n_clauses;
        }

        Clause<T>* yieldClause() {
            c = (c + P) % n_clauses;
            return getEnumerateClause(c);
        }

        Clause<T>* getEnumerateClause(T idx) {
            T offset = 0;

            for (int i = 0; i < blocking_patterns->size(); i++) {
                if ((idx - offset) < blocking_patterns->at(i)->n_clauses) {
                    return new Clause<T>(blocking_patterns->at(i)->getLiterals(idx - offset), 0);
                }

                offset += blocking_patterns->at(i)->n_clauses;
            }

            return nullptr;
        }

    private:
        const uint64_t P = 9223372036854775783;
        T c = 0;

        vector<BlockingPattern<T>*>* blocking_patterns;
};

#endif //CLAUSE_GENERATOR_H
