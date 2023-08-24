//
// Created by Xandru Mifsud on 19/08/2023.
//

#ifndef CLAUSE_GENERATOR_H
#define CLAUSE_GENERATOR_H

#include "cmath"
#include "Clause.h"

template<class T, class Enable = void>
class ClauseGenerator {}; // Default specialisation

template <class T> // Specialisation to integral types
class ClauseGenerator<T, typename enable_if<is_integral<T>::value>::type> {

    public:
        explicit ClauseGenerator(T k, vector<T>* blocked_cells) {
            this->k = k;
            this->blocked_cells = blocked_cells;

            k2 = (T) pow(k, 2);
            k3 = (T) pow(k, 3);
            k4 = (T) pow(k, 4);
            k5 = (T) pow(k, 5);

            n_clauses = (4 * k4) + (pow(k, 8) * (k2 - 1));

            n1 = k4;     // k^4 clauses of type 1
            n2 = 2 * n1; // k^4 clauses of type 2
            n3 = 3 * n1; // k^4 clauses of type 3
            n4 = 4 * n1; // k^4 clauses of type 4
                         // k^8 (k^2 - 1) clauses of type 5
        }

        Clause<T>* yieldClause() {
            c = (c + P) % n_clauses;

            Clause<T>* clause;

            if (c < n1) {
                clause = getType1Clause();
            } else if (c < n2) {
                clause = getType2Clause();
            } else if (c < n3) {
                clause = getType3Clause();
            } else if (c < n4) {
                clause = getType4Clause();
            } else {
                clause = getType5Clause();
            }

            if (clause == nullptr) {
                return yieldClause();
            } else {
                return clause;
            }
        }

    private:
        const uint64_t P = 9223372036854775783;
        T c = 0;

        T k, k2, k3, k4, k5, n_clauses, n1, n2, n3, n4;
        vector<T>* blocked_cells;

        inline Clause<T>* getType1Clause() {
            auto x = (T) c / k3;

            auto c_x = c - (x * k3);
            auto y = (T) c_x / k2;

            auto c_xy = c_x - (y * k2);
            auto u = (T) c_xy / k;
            auto v = c_xy - (u * k);

            auto cell = (x * k5) + (y * k4) + (u * k2) + (v * k);

            if (!includes(blocked_cells->begin(), blocked_cells->end(), cell)) {
                auto literals = new std::vector<T>();
                for (T i = 0; i < k2; i++) {
                    literals->push_back(2 * (cell + i));
                }

                return new Clause<T>(literals, 0);
            } else {
                return nullptr;
            }
        }

        inline Clause<T>* getType2Clause() {
            auto x = (T) (c - n1) / k3;

            auto c_x = c - n1 - (x * k3);
            auto u = (T) c_x / k2;

            auto s = c_x - (u * k2);
            auto offset = (x * k5) + (u * k2);

            auto literals = new std::vector<T>();
            for (T y = 0; y < k; y++) {
                for (T v = 0; v < k; v++) {
                    T cell = offset + (y * k4) + (v * k);
                    if (!includes(blocked_cells->begin(), blocked_cells->end(), cell)) {
                        literals->push_back(2 * (cell + s));
                    }
                }
            }

            if (literals->size() > 0) {
                return new Clause<T>(literals, 0);
            } else {
                return nullptr;
            }
        }

        inline Clause<T>* getType3Clause() {
            auto y = (T) (c - n2) / k3;

            auto c_y = c - n2 - (y * k3);
            auto v = (T) c_y / k2;

            auto s = c_y - (v * k2);
            auto offset = (y * k4) + (v * k);

            auto literals = new std::vector<T>();
            for (T x = 0; x < k; x++) {
                for (T u = 0; u < k; u++) {
                    T cell = (x * k5) + (u * k2) + offset;
                    if (!includes(blocked_cells->begin(), blocked_cells->end(), cell)) {
                        literals->push_back(2 * (cell + s));
                    }
                }
            }

            if (literals->size() > 0) {
                return new Clause<T>(literals, 0);
            } else {
                return nullptr;
            }
        }

        inline Clause<T>* getType4Clause() {
            auto x = (T) (c - n3) / k3;

            auto c_x = c - n3 - (x * k3);
            auto y = (T) c_x / k2;

            auto s = c_x - (y * k2);
            auto offset = (x * k5) + (y * k4);

            auto literals = new std::vector<T>();
            for (T u = 0; u < k; u++) {
                for (T v = 0; v < k; v++) {
                    T cell = offset + (u * k2) + (v * k);
                    if (!includes(blocked_cells->begin(), blocked_cells->end(), cell)) {
                        literals->push_back(2 * (cell + s));
                    }
                }
            }

            if (literals->size() > 0) {
                return new Clause<T>(literals, 0);
            } else {
                return nullptr;
            }
        }

        inline Clause<T>* getType5Clause() {
            auto d = k4 * (k4 - k2);

            auto s = (T) (c - n4) / d;
            auto c_s = c - n4 - (s * d);

            auto subcells = new vector<T>();

            T u, v, cell;

            for (T i = 0; i < k2; i++) {
                d = (T) d / k2;
                u = (T) c_s /
            }

            subcells->clear();
            delete subcells;

            return nullptr;
        }
};

#endif //CLAUSE_GENERATOR_H
