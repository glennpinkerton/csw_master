
#include <iostream>
#include <vector>
#include <string>


// Player is an abstract base class,
// with all pure virtual methods.

class Player {

  protected:

    int    number = 0;
    std::string  league = "";
    std::string  team = "";
    std::string  name = "";
    std::string  sport = "";

  public:

    virtual std::string GetName () = 0;

    virtual int  GetNumber () = 0;
    virtual std::string GetLeague () = 0;
    virtual std::string GetTeam () = 0;
    virtual std::string GetSport () = 0;
    virtual std::string GetPosition () = 0;

    virtual void  SetNumber (int num) = 0;
    virtual void  SetLeague (std::string league) = 0;
    virtual void  SetTeam (std::string team) = 0;

};



// LaxPlayer cannot be abstract.  It needs to implement all of
// the Player pure virtual methods.

class LaxPlayer : public Player {

  protected:
 
    int    goals = 0;
    int    assists = 0;

    std::string  position = "field";

  public:

    LaxPlayer () {sport = "lacrosse";};
    LaxPlayer (std::string pname)
      {name = pname; sport = "lacrosse";};
    LaxPlayer (std::string pname, std::string pos)
      {name = pname; sport = "lacrosse"; position = pos;};

// The "virtual" keyword is optional in the next 8 lines.
// I use it because it is clearer to me that this is the
// "base implementation" of the pure virtual function.

    virtual std::string GetName () {return name;};
    virtual void SetNumber (int num) {number = num;};
    virtual void SetLeague (std::string sleague) {league = sleague;};
    virtual void SetTeam (std::string steam) {team = steam;};
    virtual int GetNumber () {return number;};
    virtual std::string GetLeague () {return league;};
    virtual std::string GetTeam () {return team;};
    virtual std::string GetSport () {return sport;};

// regular methods not needed for goalies

    void SetGoals (int gval) {goals = gval;};
    void SetAssists (int aval) {assists = aval;};
    int GetGoals () {return goals;};
    int GetAssists () {return assists;};

// Methods overridden by derived class for specific position

    virtual std::string GetPosition () {return position;};

};


class LaxGoalie : public LaxPlayer 
{

  protected:

    int    goals_allowed = 0;
    int    saves = 0;
    int    minutes_played = 0;
    int    games_played = 0;

  public:

    LaxGoalie () {position = "goalie";};
    LaxGoalie (std::string gname) {
        position = "goalie";
        name = gname;
    };

// Cannot use initializer list for protected data in base class
//    LaxGoalie () : position ("goalie") {};

};



void test_ref (Player &pr) {
    std::cout << "from test_ref: " << std::endl;
    std::cout << pr.GetSport() << std::endl;
    std::cout << "  " << pr.GetName() << " : " << std::flush;
    std::cout << pr.GetPosition() << std::endl;
}




int main() {

    std::vector<Player *> all_players;

    LaxPlayer  lp1 ("Joe","Attack");
    LaxPlayer  lp2 ("Jim","Attack");
    LaxPlayer  lp3 ("Tom","Attack");
    LaxPlayer  lp4 ("Mike", "midfield");
    LaxPlayer  lp5 ("Arnie", "midfield");
    LaxPlayer  lp6 ("Seth", "midfield");
    LaxPlayer  lp7 ("Larry", "defense");
    LaxPlayer  lp8 ("Curly", "defense");
    LaxPlayer  lp9 ("Shemp", "defense");
    LaxGoalie  lg0 ("Maxwell");

// Need to put pointers to abstract base into vector

    all_players.push_back (&lp1);
    all_players.push_back (&lp2);
    all_players.push_back (&lp3);
    all_players.push_back (&lp4);
    all_players.push_back (&lp5);
    all_players.push_back (&lp6);
    all_players.push_back (&lp7);
    all_players.push_back (&lp8);
    all_players.push_back (&lp9);
    all_players.push_back (&lg0);

    for (const auto pp : all_players) {
        std::cout << pp->GetSport() << std::endl;
        std::cout << "  " << pp->GetName() << " : " << std::flush;
        std::cout << pp->GetPosition() << std::endl;
    }

    std::cout << std::endl;

// Passing derived class to function specifying reference to
// abstract base class works.

    test_ref (lp1);
    test_ref (lg0);

    

    return 0;

}
