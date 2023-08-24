#include <iostream>

#include "BlockingPattern/blocking_pattern.h"

int main() {
    auto* bp = new BlockingPattern<short>("*b\nw*", 5);

    std::cout << bp->black_pattern->at(0)(0,0) << std::endl;
    std::cout << bp->black_pattern->at(0)(1,0) << std::endl;
    std::cout << bp->black_pattern->at(0)(0,1) << std::endl;
    std::cout << bp->black_pattern->at(0)(1,1) << std::endl;

    std::cout << bp->white_pattern->at(0)(0,0) << std::endl;
    std::cout << bp->white_pattern->at(0)(1,0) << std::endl;
    std::cout << bp->white_pattern->at(0)(0,1) << std::endl;
    std::cout << bp->white_pattern->at(0)(1,1) << std::endl;

    return 0;
}
