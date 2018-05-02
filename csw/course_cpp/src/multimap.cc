#include <iostream>
#include <map>
 
int main()
{
    //Multi Map of char and int
    // Initializing with initializer list
    std::multimap<char, int> mmapOfPos ={
            {'t', 1},
            {'h', 1},
            {'i', 2},
            {'s', 3},
            {'i', 5},
            {'s', 6},
            {'i', 8},
    };
 
// Inserting an element in map
    mmapOfPos.insert(std::pair<char, int>('t', 9));
 
// Iterate over the multimap using Iterator
    for (std::multimap<char, int>::iterator it = mmapOfPos.begin();
            it != mmapOfPos.end(); it++) {
        std::cout << it->first << " :: " << it->second << std::endl;
    }
 
    std::cout << "****************************************" << std::endl;
 
// Iterate over the multimap using range based for loop
    for (std::pair<char, int> elem : mmapOfPos) {
        std::cout << elem.first << " :: " << elem.second << std::endl;
    }
 
    std::cout << "++++++++++++++++++++++++++++++++++++++++" << std::endl;

    for (const auto &elem : mmapOfPos) {
        std::cout << elem.first << " :: " << elem.second << std::endl;
    }

// use equal range to find multi keys

    std::cout << "========================================" << std::endl;

    auto rp = mmapOfPos.equal_range ('i');
    for (auto rit = rp.first; rit != rp.second; rit++) {
        std::cout << "for key = " << rit->first << ":  " << std::flush;
        std::cout << "value = " << rit->second << std::endl;
    }
 
    return 0;
}
