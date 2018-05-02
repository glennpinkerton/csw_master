#include <iostream>
#include <algorithm>
#include <deque>
#include <string>

class Player {
private:
    std::string team;
    std::string name;
    int number;

public:

// Must define a "default" constructor for the map[key] = value stuff to work
// when the value is an object of this class.

    Player() :
        team(""), name(""), number(0) {

    }

    Player(std::string team, std::string name, int number) :
        team(team), name(name), number(number) {

    }

    Player(const Player& other) {
        team = other.team;
        name = other.name;
        number = other.number;
    }

    bool operator<(const Player &other) const {

        if (team == other.team) {
            if (number == other.number) {
                return name < other.name;
            } else {
                return number < other.number;
            }
        }
        else {
            return team < other.team;
        }

    }

    friend bool pcomp (Player &a, Player &b);

    void print() const {
        std::cout << team << ", " << number << ": " << name << std::flush;
    }

// overload the << operatgor when it is use with ostream.  The
// && overload does the same as the print method.

//    friend std::ostream &operator<< (std::ostream &cout, const Player &p);

};  // end of Player class definition

bool pcomp (Player &a, Player &b) {
    return !(a < b);
}

// overloaded operator does not have to be in a class definition.

std::ostream &operator<< (std::ostream &cout, const Player &p) {
    p.print ();
    return cout;
}


int main() {

    std::deque<Player> players;

    players.push_back (Player("yotes", "Mike", 40));
    players.push_back (Player("badguys", "Mike", 444));
    players.push_back (Player("yotes", "Sue", 30));
    players.push_back (Player("badguys", "Raj", 40));
    players.push_front (Player("yotes", "Jim", 33));
    players.push_front (Player("badguys", "Jill", 4));
    players.push_front (Player("yotes", "Sally", 3));
    players.push_front (Player("badguys", "Rojer", 10));

    std::cout << "**** original unsorted ****" << std::endl;
    for (const auto &it : players) {
        it.print ();
        std::cout << std::endl;
    }

    std::cout << "**** hopefully reverse sorted ****" << std::endl;
    std::sort (players.begin(), players.end(), pcomp);
    for (const auto &it : players) {
        it.print ();
        std::cout << std::endl;
    }

    std::cout << "**** hopefully forward sorted ****" << std::endl;
    std::sort (players.begin(), players.end());
    for (const auto &it : players) {
//        it.print ();
        std::cout << "op_overload: " << it << std::flush;
        std::cout << std::endl;
    }

    return 0;
}
