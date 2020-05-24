#include <iostream>

#include <vector>
#include <Part.hpp>

int main(int argc, char ** argv ){
    printf("ORI test\n");

    std::vector<Part*> part_list;

    part_list.push_back(new Part1());
    part_list.push_back(new Part2());
    part_list.push_back(new Part4());

    for(int i(0); i<3; ++i){
        printf("%d part operation time: %f\n", i, part_list[i]->getOpTime());
    }

    return 0;
}
