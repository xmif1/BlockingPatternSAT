//
// Created by Xandru Mifsud on 23/08/2023.
//

#ifndef BLOCKING_PATTERN_H
#define BLOCKING_PATTERN_H

#include <string>

using namespace std;
template<class T, class Enable = void>
class BlockingPattern {}; // Default specialisation

template <class T> // Specialisation to integral types
class BlockingPattern<T, typename enable_if<is_integral<T>::value>::type> {
    public:
        T h, w;

        explicit BlockingPattern(string patternString, T n) {
            black_pattern = new vector<function<T(T, T)>>();
            white_pattern = new vector<function<T(T, T)>>();

            h = 0;
            w = 0;

            T pos;
            string token;

            while ((pos = patternString.find('\n')) != string::npos) {
                _ingest_token(patternString.substr(0, pos), n);
                patternString.erase(0, pos + 1);
            }

            _ingest_token(patternString, n);
        }

//    private:
        vector<function<T(T, T)>>* black_pattern;
        vector<function<T(T, T)>>* white_pattern;

        void _ingest_token(const string& token, T n) {
            T _w = 0;
            T _h = h;

            for (auto c : token) {
                switch(c) {
                    case '*': break;
                    case 'b': {
                        black_pattern->push_back(
                                [_h, _w, n](int x, int y) {
                                    return 2 * (n * (y + _h) + x + _w) + 1;
                                }
                        );

                        break;
                    }
                    case 'w': {
                        white_pattern->push_back(
                                [_h, _w, n](int x, int y) {
                                    return 2 * (n * (y + _h) + x + _w);
                                }
                        );

                        break;
                    }
                    default:
                        exit(1);
                }

                _w++;
            }

            h++;
            if (h == 1) {
                w = _w;
            } else if (_w != w) {
                exit(1);
            }
        }
};

#endif //BLOCKING_PATTERN_H
