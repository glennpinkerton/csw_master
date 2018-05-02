
#include <iostream>
#include <map>
#include <string>

int main() {

    std::map<std::string, int> ages;

// The brackets are overloaded for a map to create
// a map entry with the key supplied in the brackets

    ages["Mike"] = 40;
    ages["Raj"] = 20;
    ages["Vicky"] = 30;

// For a map (as opposed to a multilap) a key must be unique
// Using the angle brackets for an existing key resets the map 
// value (in this case an int) associated with the key (in this
// case a string).

    ages["Mike"] = 70;

// Each map entry is a pair object so you can insert a pair object
// into the map 

    ages.insert(std::make_pair("Peter", 100));

// Try insert with existing key?

    ages.insert(std::make_pair("Peter", 246));
    std::cout << "Insert Existing Peter: " << ages["Peter"] << std::endl;

    ages["Peter"] = 1357;
    std::cout << "Bracket Existing Peter: " << ages["Peter"] << std::endl;

// Find returns an iterator which is end if not found

    if(ages.find("Vicky") != ages.end()) {
        std::cout << "Found Vicky" << std::endl;
    }
    else {
        std::cout << "Key not found." << std::endl;
    }

    for(std::map<std::string, int>::iterator it = ages.begin();
        it != ages.end(); it++) {
        std::pair<std::string, int> age = *it;

        std::cout << age.first << ": " << age.second << std::endl;
    }

    for(std::map<std::string, int>::iterator it = ages.begin();
        it != ages.end(); it++) {
            std::cout << it->first << ": " << it->second << std::endl;
    }

    return 0;
}
