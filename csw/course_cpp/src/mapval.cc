#include <iostream>
#include <map>
#include <string>

class Person {
private:
    std::string name;
    int age;

public:

// Need "default" constructor fore map[key] = value to
// work when the value is a Person (or any other custom class)
// object

    Person() :
        name(""), age(0) {

    }

    Person(std::string name, int age) :
        name(name), age(age) {

    }

    Person(const Person& other){
        std::cout << "    Copy constructor running!" << std::endl;
        name = other.name;
        age = other.age;
    }

// Good idea to make methods akin to print const.
// There is no sensible reason to change the object here.
// In fact, if an object of this class is used as a map key,
// the print function many times must be const because a key cannot be
// changed from it's initial "value".

    void print() const {
        std::cout << name << ": " << age << std::flush;
    }

};

int main() {
    std::map<int, Person> people;

    people[50] = Person("Mike", 40);
    people[32] = Person("Vicky", 30);
    people[1] = Person("Raj", 20);

// These 2 lines should use the copy constructor some number of times.
    std::cout << "Copy constructor should be used here\n" << std::endl;
    people.insert(std::make_pair(55, Person("Bob", 45 )));
    people.insert(std::make_pair(55, Person("Sue", 24 )));

    for (std::map<int, Person>::iterator it = people.begin(); it != people.end();
            it++) {
        std::cout << it->first << ": " << std::flush;
        it->second.print();
        std::cout << std::endl;
    }

    return 0;
}
