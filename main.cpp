
#include "BlockingPattern/blocking_pattern.h"
#include "BlockingPattern/clause_generator.h"

#include "SATInstance.h"

typedef unsigned int UINT_T;
typedef SATInstance<UINT_T>::ClauseArray ClauseArray;
typedef unsigned char byte;

void bits_to_bmp(VariablesArray<UINT_T>* data, UINT_T n, char* out);

int main() {
    UINT_T n = 64;
    UINT_T batch_size = 100000;

    auto* bp1 = new BlockingPattern<UINT_T>("10\n01", n);
    auto* bp2 = new BlockingPattern<UINT_T>("01\n10", n);

    auto bp = new vector<BlockingPattern<UINT_T>*>();
    bp->push_back(bp1);
    bp->push_back(bp2);

    auto* cg = new ClauseGenerator<UINT_T>(bp);
    batch_size = batch_size < cg->n_clauses ? batch_size : cg->n_clauses;

    auto clauses = new vector<ClauseArray*>();
    clauses->push_back(new ClauseArray());

    auto var_arr = new VariablesArray<UINT_T>(n * n);
    auto satInstance = new SATInstance<UINT_T>(var_arr, 1);
    bool isValid = false;

    while (!isValid) {
        isValid = true;

        UINT_T global_clause_count = 0;

        while (global_clause_count < cg->n_clauses) {
            UINT_T clause_count = 0;

            while (clause_count < batch_size) {
                auto clause = cg->yieldClause();
                if (clause != nullptr) {
                    clauses->at(0)->push_back(clause);

                    clause_count++;
                    global_clause_count++;
                } else {
                    exit(1);
                }
            }

            satInstance->solve(clauses, true);

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
                clauses->at(0)->push_back(clause);
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

    bits_to_bmp(var_arr, n, (char*) "./out.bmp");

    return 0;
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
