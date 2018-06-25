#include <iostream>
#include <vector>
#include <string>

#include <functional>


// See if struct and typedef work same as class, which in theory
// they pretty much should.  Struct members are public by devault.

struct Player {
    Player (std::string nm, int ba, int h, int r) :
      name (nm), bavg (ba), hr (h), rbi (r) {};
    std::string  name = "";
    int     bavg = 0;
    int     hr = 0;
    int     rbi = 0;
};


void  test1 (int ival, double dval)
{
    std::cout << "in test1 ival * dval = " << ival * dval << std::endl;
}


template <typename T, typename U>
void test_tpl (T tval, U uval)
{
    std::cout << "tpl: T * U = " << tval * uval << std::endl;
}

// non templated function with same name to print a specific type.
// If gprintnl is called with no <> and with an int parameter, this
// non templated version is inferenced.
void gprintnl (int val)
{
    std::cout << "From non-templated gprintnl: " << val << " " << std::endl;
}


//
// Example of how to pass function pointer to another function
// The list of baseball players and their stats is traversed by
// the count_certain_players function.  The function pointer 
// which is a parametger to count_certain_players provides the
// selection criterria for the "certain" players.
//
bool check_over_300 (Player const &p) {
    return (p.bavg > 300);
}

bool check_good_hitter (Player const &p) {
    return (check_over_300 (p)  &&
            p.hr > 20  &&  p.rbi > 75);
}

int count_certain_players (std::vector<Player> players, 
                           bool (*pfunc)(Player const &p))
{
    int  ntot = 0;

    for (const auto &p : players) {
        if (pfunc (p)) ntot++;
    }

    return ntot;
}


// Use the C++11 fumction templated class (in the functional header).

int count_certain_players (std::vector<Player> players, 
                           std::function<int(Player const &)> pfunc)
{
    int  ntot = 0;

    for (const auto &p : players) {
        if (pfunc (p)) ntot++;
    }

    return ntot;
}



int main() {

    void (*ptest1)(int, double) = test1;
    void (*ptest_tpl)(int, double) = test_tpl<int, double>;

    std::cout << "from direct func calls" << std::endl;
    test1 (5, 2.5);
    test_tpl (5, 2.5);

    std::cout << "from func ptr calls" << std::endl;
    ptest1 (5, 2.5);
    ptest_tpl (5, 2.5);

    bool (*p300)(Player const &p) = check_over_300;
    bool (*pgood)(Player const &p) = check_good_hitter;

    std::vector<Player> player_list;

    player_list.push_back (Player ("Mays", 300, 40, 100));
    player_list.push_back (Player ("JoeBlow", 200, 0, 20));
    player_list.push_back (Player ("Musial", 320, 30, 120));
    player_list.push_back (Player ("NoPower", 350, 4, 20));
    player_list.push_back (Player ("Crap", 100, 1, 10));
    player_list.push_back (Player ("Yaz", 305, 30, 110));
    player_list.push_back (Player ("Minors", 105, 0, 10));
    player_list.push_back (Player ("Williams", 345, 40, 130));

    int n300 = count_certain_players (player_list, p300);
    std::cout << "Number of over 300 hitters = " << n300 << std::endl;
    int ngood = count_certain_players (player_list, pgood);
    std::cout << "Number of very good hitters = " << ngood << std::endl;


// define c++11 function objects and use them in a version of
// count_certain_players.  I (Glenn) think this is a bit cleaner
// and a function object is more flexible than a raw function pointer.

    std::function<bool (Player const &)> sf300 = check_over_300;
    std::function<bool (Player const &)> sfgood = check_good_hitter;

    std::cout << std::endl << "using function object from c++11" << std::endl;
    n300 = count_certain_players (player_list, sf300);
    std::cout << "Number of over 300 hitters = " << n300 << std::endl;
    n300 = count_certain_players (player_list, sfgood);
    std::cout << "Number of over very good hitters = " << n300 << std::endl;

    std::cout << std::flush;

    return 0;
}
