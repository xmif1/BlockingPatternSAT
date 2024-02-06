#include <vector>
#include <cstring>
#include <fstream>
#include <iostream>
#include <functional>

using namespace std;

#include "BlockingPattern/blocking_pattern.h"
#include "SATInstance.h"

typedef uint64_t UINT_T;
typedef SATInstance<UINT_T>::ClauseArray ClauseArray;
typedef uint8_t byte;

void bits_to_bmp(VariablesArray<UINT_T>* data, UINT_T n, char* out);
Clause<UINT_T>* getEnumeratedClause(UINT_T, unsigned short int);
string get_mask(int h, int b);

auto blocking_patterns = new vector<BlockingPattern<UINT_T>*>();

int main() {
    unsigned short int n_threads = 8;

    UINT_T n = 50;
    UINT_T n_clauses = 0;
    UINT_T batch_size = 50000000;

    auto* bp1 = new BlockingPattern<UINT_T>(get_mask(9, 8), n);

    blocking_patterns->push_back(bp1); n_clauses += bp1->n_clauses;

    batch_size = batch_size < n_clauses ? batch_size : n_clauses;

    auto var_arr = new VariablesArray<UINT_T>(n * n);
    auto satInstance = new SATInstance<UINT_T>(var_arr, n_threads);

    auto statistics = satInstance->solve(getEnumeratedClause, n_clauses, batch_size);

    cout << "------------ STATISTICS -------------\n# Iterations\t= "
            + to_string(statistics->n_iterations)
            + "\n# Resamples\t= " + to_string(statistics->n_resamples);

    for (int t = 0; t < n_threads; t++) {
        cout << "\n\tThread " + to_string(t+1) + ": " + to_string((statistics->n_thread_resamples).at(t));
    }

    cout << "\n\nAvg. UNSAT MIS Size = " + to_string(statistics->avg_mis_size)
            + "\n-------------------------------------\n\n";

    if (n < 256) {
        bits_to_bmp(var_arr, n, (char*) "./out.bmp");
    }

    auto out_f = new ofstream("./sat.cnf");
    satInstance->writeDIMACS(getEnumeratedClause, out_f);
    out_f->close();

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

byte* get_bmp_file_header() {
    static byte header[14];

    header[0] = (byte) 'B';
    header[1] = (byte) 'M';
    header[10] = (byte) 54;

    return header;
}

byte* get_bmp_info_header(UINT_T n) {
    static byte header[40];

    header[0] = (byte) 40;
    header[4] = (byte) n;
    header[5] = (byte) n >> 8;
    header[6] = (byte) n >> 16;
    header[7] = (byte) n >> 24;
    header[8] = (byte) n;
    header[9] = (byte) n >> 8;
    header[10] = (byte) n >> 16;
    header[11] = (byte) n >> 24;
    header[12] = (byte) 1;
    header[14] = (byte) 1;
    header[32] = (byte) 2;
    header[36] = (byte) 2;

    return header;
}

void bits_to_bmp(VariablesArray<UINT_T>* data, UINT_T n, char* out) {
    FILE* fout = fopen(out, "wb");

    fwrite(get_bmp_file_header(), sizeof(byte), 14, fout);
    fwrite(get_bmp_info_header(n), sizeof(byte), 40, fout);

    byte black_colour[4] = {0,0,0,0};
    fwrite(black_colour, 1, 4, fout);

    byte white_colour[4] = {0xff,0xff,0xff,0xff};
    fwrite(white_colour, 1, 4, fout);

    UINT_T offset, numBytes = ((n + 31) / 32) * 4;
    byte bytes[numBytes];

    for (UINT_T y = 0; y < n; y++) {
        offset = (n - y - 1) * n;
        memset(bytes, 0, (numBytes * sizeof(byte)));

        for (UINT_T x = 0; x < n; ++x) {
            if (!data->vars[offset++]) {
                bytes[x / 8] |= 1 << (7 - x % 8);
            }
        }

        fwrite(bytes, sizeof(byte), numBytes, fout);
    };

    fclose(fout);
}
