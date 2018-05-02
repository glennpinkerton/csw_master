#include <iostream>
#include <set>
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
        std::cout << team << ", " << name << ": " << number << std::flush;
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

};  // end of Player class definition


int main() {

    std::set<Player> players;

    players.insert (Player("yotes", "Mike", 40));
    players.insert (Player("badguys", "Mike", 444));
    players.insert (Player("yotes", "Sue", 30));
    players.insert (Player("badguys", "Raj", 40));

    for (const auto &it : players) {
        it.print ();
        std::cout << std::endl;
    }

    auto fp = players.find (Player("yotes", "Mike", 40));
    if (fp != players.end()) {
        std::cout << "player found  " << std::flush;
        fp->print ();
        std::cout << std::endl;
    }
    else {
        std::cout << 
         "player not found when it should have been found" <<
          std::endl;
    }

    auto fp2 = players.find (Player("ytes", "Mike", 40));
    if (fp2 != players.end()) {
        std::cout <<
          "player found when it should not be found" <<
           std::endl;
    }
    else {
        std::cout << 
         "player correctly not found" <<
          std::endl;
    }

    return 0;
}
