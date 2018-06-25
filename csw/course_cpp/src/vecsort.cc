#include <iostream>
#include <algorithm>
#include <vector>
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

    void print() const {
        std::cout << team << ", " << number << ": " << name << std::flush;
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

};  // end of Player class definition

bool pcomp (Player &a, Player &b) {
    return !(a < b);
}


int main() {

    std::vector<Player> players;

    players.push_back (Player("yotes", "Mike", 40));
    players.push_back (Player("badguys", "Mike", 444));
    players.push_back (Player("yotes", "Sue", 30));
    players.push_back (Player("badguys", "Raj", 40));
    players.push_back (Player("yotes", "Jim", 33));
    players.push_back (Player("badguys", "Jill", 4));
    players.push_back (Player("yotes", "Sally", 3));
    players.push_back (Player("badguys", "Rojer", 10));

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
        it.print ();
        std::cout << std::endl;
    }

    return 0;
}
