
#include <iostream>
#include <list>

int main() {

    std::list<int> numbers;

    numbers.push_back(1);
    numbers.push_back(2);
    numbers.push_back(3);
    numbers.push_front(0);

    std::list<int>::iterator it = numbers.begin();
    it++;
    numbers.insert(it, 100);
    std::cout << "Element: " << *it << std::endl;

    std::list<int>::iterator eraseIt = numbers.begin();
    eraseIt++;
    eraseIt = numbers.erase(eraseIt);
    std::cout << "Element: " << *eraseIt << std::endl;

    std::cout << "Before erase in loop" << std::endl;
    for(std::list<int>::iterator it=numbers.begin();
        it != numbers.end(); it++) {
        std::cout << *it << std::endl;
    }

    std::list<int>::iterator it2 = numbers.begin();

// The numbers.erase line will implicitly increment thr it2
// iterator when returning its result into it2.  When no 
// erase is done, it2 needs to be explicitly incremented.
    while (it2 != numbers.end()) {
        if (*it2 == 2) {
            it2 = numbers.erase(it2);
        }
        else {
            it2++;
        }
    }

    std::cout << "After erase in loop" << std::endl;
    for(std::list<int>::iterator it=numbers.begin();
        it != numbers.end(); it++) {
        std::cout << *it << std::endl;
    }

    return 0;
}
