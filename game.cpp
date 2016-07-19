#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <vector>

//There might be one minor bug when computer is able to play spells on board,
//I have not located the problem yet nor have I seen that play again
//Game was desinged on Windows machine, on Linux - line system("cls"),
//will not work and screen wont properly clear

using namespace std;

typedef enum {MONSTER,SPELL} card_type;
typedef enum {FRONT,BACK} card_display;
typedef enum {DECK,HAND,PLAY} card_place;

const string list_of_names[25] = {"Troll  ", "Dwarf  ", "Spider ", "Pikachu", "Shark  ",
                                  "Wolf   ", "Chicken", "Panda  ", "Snake  ", "Falcon ",
                                  "Giraffe", "Monkey ", "Bat    ", "Lion   ", "Bear   ",
                                  "Duck   ", "Dragon ", "Frog   ", "Turtle ", "Vampire",
                                  "Zombie ", "Rat    ", "Octopus", "Penguin", "Donkey "};

const string list_of_spells[5] = {"Fire   ", "Icebolt", "Curse  ", "Light  ", "Wind   "};

class Card {

    string _name;
    card_type _type;

    int _cost;
    int _attack;
    int _health_pool;
    int _health_curr;

    bool _playable;

public:

    Card(): _playable(false) {
        int choose_card_type = rand() % 3;
        if (choose_card_type == 1 || choose_card_type == 2) {
            _type = MONSTER;
            _name = list_of_names[rand() % 25 ];
            _cost = 1 + rand() % 5;
            _attack = 1 + rand() % 5;
            _health_pool = 1 + rand() % 5;
            _health_curr = _health_pool;
        } else /* choose_card_type == 0 */ {
            _type = SPELL;
            _name = list_of_spells[rand() % 5 ];
            _cost = 1 + rand() % 5;
            _attack = _cost + 1;
            _health_pool = 0;
            _health_curr = 0;
        }
    }

    Card(const Card&) = delete;
    Card& operator=(const Card&) = delete;
    ~Card() = default;

    string get_name() const{
        return _name;
    }

    card_type get_type() const {
        return _type;
    }

    int get_cost() const{
        return _cost;
    }

    int get_attack() const{
        return _attack;
    }

    int get_health_pool() const{
        return _health_pool;
    }
    int get_health_current() const{
        return _health_curr;
    }

    bool get_playable() const{
        return _playable;
    }

    void card_played() {
        _playable = false;
    }

    void reset_card() {
        _playable = true;
    }

    void attack_card(unique_ptr<Card> & defender) {
        defender->_health_curr -= this->get_attack();
    }
};

class Player {

    string _name;
    int _health_pool;
    int _source;
    bool _card_drawn;
    vector< unique_ptr<Card> > _deck;
    vector< unique_ptr<Card> > _hand;
    vector< unique_ptr<Card> > _in_play;
    friend class Game;
public:
    Player(string name):_name(name),
                        _health_pool(30),
                        _source(0),
                        _card_drawn(false) {
        _deck.reserve(30);
        _hand.reserve(5);
        _in_play.reserve(5);
    }
    Player(const Player&) = delete;
    Player& operator=(const Player&) = delete;
    ~Player() = default;

    string get_name() {
        return _name;
    }

    int get_health_pool() {
        return _health_pool;
    }

    int get_source() {
        return _source;
    }

    bool get_card_draw_status() {
        return _card_drawn;
    }

    void inc_source() {
        _source++;
    }

    void refill_source(int turn) {
        if (turn <= 6) {
            _source = turn;
        } else {
            _source = 6;
        }
    }

    void reset_draw() {
        _card_drawn = false;
    }

    void reset_cards() {
        for (unique_ptr<Card> const& c : _in_play) {
            c->reset_card();
        }
    }

    void play_card(unsigned int position) {
        unique_ptr<Card> card = move(_hand[position]);
        _source -= card->get_cost();
        _hand.erase(_hand.begin() + position);
        _in_play.push_back(move(card));
    }

    void remove_card(unsigned int position) {
        _in_play.erase(_in_play.begin() + position);
    }

    void take_damage(int damage) {
        _health_pool -= damage;
    }

    void draw() {
        unique_ptr<Card> card;

        if (_deck.size() != 0) {
            card = move(_deck.back());
            _deck.pop_back();
        } else {
            cout << "Your deck is empty!" << endl;
        }

        if (_hand.size() < 5) {
            _hand.push_back(move(card));
        } else {
            cout << "Your hand is too full, discarded card: "
                 << "N: " << card->get_name()
                 << " C:" << card->get_cost()
                 << " A:" << card->get_attack()
                 << " H:" << card->get_health_pool() << endl;
            system("pause");
        }

        _card_drawn = true;
    }

    void print_available_source() {
        for (int i = 0; i < 6; i++) {
            if (i < _source) {
                cout << "|@@@@@@@|";
            } else {
                cout << "|-------|";
            }
        }
        cout << endl;
    }

    void print_cards(card_place place, card_display display) {

        int len;
        if (place == HAND) {
            len = _hand.size();
        } else if (place == DECK) {
            len = _deck.size();
        } else if (place == PLAY) {
            len = _in_play.size();
        }

        //######################################
        //1-row
        for (int i = 0; i < len; i++) {
            cout << " _______ ";
        }
        if (place == HAND) {
            for (int i = 0; i < 5 - len; i++) {
                cout << "         ";
            }
            cout << " _______ ";
        }
        cout << endl;
        //######################################

        //######################################
        //2-row
        if (place == PLAY) {
            for (unique_ptr<Card> &c : _in_play) {
                cout << "|";
                if (c->get_playable()) {
                    cout << "Rested ";
                } else {
                    cout << "Tired  ";
                }
                cout << "|";
            }
        } else if (place == HAND) {
            for (unique_ptr<Card> &c : _hand) {
                if (display == FRONT && c->get_type() == MONSTER) {
                    cout << "|" << c->get_cost() <<"      |";
                } else if (display == FRONT && c->get_type() == SPELL) {
                    cout <<"|Spell " << c->get_cost() << "|";
                } else {
                    cout << "|       |";
                }
            }
            for (int i = 0; i < 5 - len; i++) {
                cout << "         ";
            }
            cout << "|" << get_name() << "|";
        }
        cout << endl;
        //######################################

        //######################################
        //3-row
        for (int i = 0; i < len; i++) {
            if (display == FRONT) {
                cout << "|       |";
            } else {
                cout << "|#######|";
            }
        }
        if (place == HAND){
            for (int j = 0; j < 5 - len; j++) {
                cout << "         ";
            }
            cout << "|#######|";
        }
        cout << endl;
        //######################################

        //######################################
        //4-row
        for (int i = 0; i < len; i++) {
            if (display == FRONT) {
                cout << "|       |";
            } else {
                cout << "|#######|";
            }
        }
        if (place == HAND){
            for (int i = 0; i < 5 - len; i++) {
                cout << "         ";
            }
            if (_deck.size() >= 10) {
                cout << "|Draw:" << _deck.size() << "|";
            } else {
                cout << "|Draw: " << _deck.size() << "|";
            }
        }
        cout << endl;
        //######################################

        //######################################
        //5-row
        if (place == PLAY) {
            for (unique_ptr<Card> &c : _in_play) {
                cout << "|" << c->get_name() <<"|";
            }
        } else if (place == HAND) {
            for (unique_ptr<Card> &c : _hand) {
                if (display == FRONT) {
                    cout << "|" << c->get_name() <<"|";
                } else {
                    cout << "|#######|";
                }
            }
            for (int i = 0; i < 5 - len; i++) {
                cout << "         ";
            }
            cout << "|#######|";
        }
        cout << endl;
        //######################################

        //######################################
        //6-row
        for (int i = 0; i < len; i++) {
            if (display == FRONT) {
                cout << "|       |";
            } else {
                cout << "|#######|";
            }
        }
        if (place == HAND){
            for (int j = 0; j < 5 - len; j++) {
                cout << "         ";
            }
            if (get_health_pool() >= 10) {
                cout << "|HP  :" << get_health_pool() << "|";
            } else {
                cout << "|HP  : " << get_health_pool() << "|";
            }
        }
        cout << endl;
        //######################################

        //######################################
        //7-row
        for (int i = 0; i < len; i++) {
            if (display == FRONT) {
                cout << "|       |";
            } else {
                cout << "|#######|";
            }
        }
        if (place == HAND){
            for (int j = 0; j < 5 - len; j++) {
                std::cout << "         ";
            }
            cout << "|#######|";
        }
        cout << endl;
        //######################################

        //######################################
        //8-row
        if (place == PLAY) {
            for (unique_ptr<Card> &c : _in_play) {
                cout << "|" << c->get_attack() << "___" << c->get_health_current() << "/" << c->get_health_pool() << "|";
            }


        }if (place == HAND) {
            for (unique_ptr<Card> &c : _hand) {
                if (display == FRONT && c->get_type() == MONSTER) {
                    cout << "|" << c->get_attack() << "_____" << c->get_health_pool() << "|";
                } else if (display == FRONT && c->get_type() == SPELL) {
                    cout << "|" << c->get_attack() << "______" << "|";
                } else {
                    cout << "|_______|";
                }

            }
            for (int i = 0; i < 5 - len; i++) {
                cout << "         ";
            }
            cout << "|_______|";
        }
        cout << endl << endl;
        //######################################
    }

};


class Game {

    unique_ptr< Player > _player;
    unique_ptr< Player > _computer;

    int _turn;

public:
    Game(string p_name, string c_name): _turn(0) {
        unique_ptr<Player> p (new Player(p_name));
        unique_ptr<Player> c (new Player(c_name));
        _player = move(p);
        _computer = move(c);
    }

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    ~Game() = default;

    int get_turn() const {
        return _turn;
    }

    void inc_turn() {
        _turn++;
    }

    void generate_decks(unsigned int n_of_cards) {
        for (unsigned int i = 0; i < n_of_cards; i++) {
            unique_ptr<Card> card(new Card());
            _player->_deck.push_back(move(card));
        }
        for (unsigned int i = 0; i < n_of_cards; i++) {
            unique_ptr<Card> card(new Card());
            _computer->_deck.push_back(move(card));
        }
    }

    void begin_draw(unsigned int n_of_cards) {
        for (unsigned int i = 0; i < n_of_cards; i++) {
            _player->draw();
            _computer->draw();
        }
    }

    void print_options() {
        cout << "1. Draw card once per turn(optional)." << endl;
        cout << "2. Play monster from your hand to play field." << endl;
        cout << "3. Attack Enemy Player with monster." << endl;
        cout << "4. Attack Enemy Monster with monster." << endl;
        cout << "5. Cast a spell." << endl;
        cout << "6. Finish turn." << endl;
        cout << "7. Rage quit!" << endl;
    }

    void show_computer() {
        _computer->print_available_source();
        _computer->print_cards(HAND, BACK);
        _computer->print_cards(PLAY, FRONT);
    }

    void show_player() {
        _player->print_cards(PLAY, FRONT);
        _player->print_cards(HAND, FRONT);
        _player->print_available_source();
    }

    void attack_player(unique_ptr<Player> &target, int damage) {
        target->take_damage(damage);
        if (target->get_health_pool() <= 0) {
            if (target == _player) {
                cout << "You lose!" << endl;
                exit(0);
            } else /* target == computer */ {
                cout << "You win!" << endl;
                exit(0);
            }
        }
    }

    void cast_spell(unique_ptr<Player> &a, unsigned int a_pos, unique_ptr<Player> &target,unsigned int t_pos) {
        unique_ptr<Card> card = move(a->_hand[a_pos]);
        a->_source -= card->get_cost();
        card->attack_card(target->_in_play[t_pos]);
        a->_hand.erase(a->_hand.begin() + a_pos);
    }

    void read_mistake() {
        getchar();
        getchar();
    }

    void play() {

        //dokud neni vitez
        do {
            inc_turn();
            _player->refill_source(get_turn());
            _player->reset_draw();
            _player->reset_cards();

            //Player
            while(1) {
                system("cls");

                show_computer();
                show_player();
                print_options();

                int play_option = 0;
                cin >> play_option;
                if (play_option == 1) { //########## VAR 1
                    if (!_player->get_card_draw_status()) {
                           _player->draw();
                    } else {
                        cout << "You drew card this turn already!" << endl;
                        //wait for user to read wrong usage
                        read_mistake();
                    }
                } else if (play_option == 2) { //########## VAR 2
                    system("cls");
                    show_computer();
                    show_player();

                    cout << "What monster do you want to play?: ";
                    unsigned int position = 0;
                    cin >> position;

                    if (_player->_hand.size() <= position - 1) {
                        cout << "You do not have card in that position!" << endl;
                        read_mistake();
                    } else if (_player->_in_play.size() == 5) {
                        cout << "You cannot play another monster!" << endl;
                        read_mistake();
                    } else if (_player->_hand[position - 1]->get_type() != MONSTER) {
                        cout << "This card is not a monster!" << endl;
                        read_mistake();
                    } else if (_player->_hand[position - 1]->get_cost() <= _player->get_source()) {
                        _player->play_card(position - 1);
                    } else {
                        cout << "You do not have enough mana to play this card!" << endl;
                        read_mistake();
                    }
                } else if (play_option == 3) { //########## VAR 3
                    system("cls");
                    show_computer();
                    show_player();

                    cout << "Choose monster to attack with: ";
                    unsigned int position = 0;
                    cin >> position;

                    if (_player->_in_play.size() <= position - 1) {
                        cout << "You dont have any monster in that position!" << endl;
                        read_mistake();
                    } else {
                        if (_player->_in_play[position - 1]->get_playable()) {
                            attack_player(_computer, _player->_in_play[position - 1]->get_attack());
                            _player->_in_play[position - 1]->card_played();
                        }   else {
                            cout << "Give this monster turn to rest!" << endl;
                            read_mistake();
                        }
                    }
                } else if (play_option == 4) { //########### VAR 4
                    system("cls");
                    show_computer();
                    show_player();

                    bool succes = false;
                    //attacker
                    cout << "Choose monster to attack with: ";
                    unsigned int attacker_position = 0;
                    cin >> attacker_position;
                    if (_player->_in_play.size() <= attacker_position - 1) { //no monster in position
                        cout << "You dont have card in that position!" << endl;
                        read_mistake();
                    } else if (!_player->_in_play[attacker_position - 1]->get_playable()) {
                        cout << "Give this monster turn to rest!" << endl;
                        read_mistake();
                    } else {
                        cout << "Chosen monster: "
                             << "position-" << attacker_position
                             << " name: " << _player->_in_play[attacker_position - 1]->get_name()
                             << endl;
                        succes = true;
                    }
                    //target
                    if (succes) {
                        cout << "Choose your target: ";
                        unsigned int target_position = 0;
                        cin >> target_position;
                        if (_computer->_in_play.size() <= target_position - 1) { //no monster in position
                            cout << "Your enemy does not have card in that position!" << endl;
                            read_mistake();
                            succes = false;
                        } else {
                            cout << "Chosen target: "
                                 << "position-" << target_position
                                 << " name: " << _computer->_in_play[target_position - 1]->get_name()
                                 << endl;
                            read_mistake();
                        }
                        if (succes) {
                            _player->_in_play[attacker_position - 1]->attack_card(_computer->_in_play[target_position - 1]);
                            _player->_in_play[attacker_position - 1]->card_played();
                            _computer->_in_play[target_position - 1]->attack_card(_player->_in_play[attacker_position - 1]);

                            if (_player->_in_play[attacker_position - 1]->get_health_current() < 1) {
                                _player->remove_card(attacker_position - 1);
                            }
                            if (_computer->_in_play[target_position - 1]->get_health_current() < 1) {
                                _computer->remove_card(target_position - 1);
                            }
                        }
                    }
                } else if (play_option == 5) { //########## VAR 5
                    system("cls");
                    show_computer();
                    show_player();

                    cout << "Choose a spell: ";
                    unsigned int spell_position = 0;
                    cin >> spell_position;

                    if (_player->_hand.size() <= spell_position - 1) {
                        cout << "You do not have spell in that position!" << endl;
                        read_mistake();
                    } else if (_player->_hand[spell_position - 1]->get_type() != SPELL ) {
                        cout << "This card - "
                             << _player->_hand[spell_position - 1]->get_name()
                             << " is not a spell!" << endl;
                        read_mistake();
                    } else if (_player->_hand[spell_position - 1]->get_cost() <= _player->get_source()) {
                        cout << "Choose your target: ";
                        unsigned int target_position = 0;
                        cin >> target_position;
                        if (_computer->_in_play.size() <= target_position - 1) { //no monster in position
                            cout << "Your enemy does not have card in that position!" << endl;
                            read_mistake();
                        } else {
                            cout << "Chosen target: "
                                 << "position-" << target_position
                                 << " name: " << _computer->_in_play[target_position - 1]->get_name()
                                 << endl;
                            read_mistake();
                            cast_spell(_player, spell_position - 1, _computer, target_position - 1);
                            if (_computer->_in_play[target_position - 1]->get_health_current() < 1) {
                                _computer->remove_card(target_position - 1);
                            }
                        }

                    } else {
                        cout << "You do not have enough mana to play this spell!" << endl;
                        read_mistake();
                    }

                } else if (play_option == 6) { //########## VAR 6
                    break;
                } else if (play_option == 7) { //########## VAR 7
                    cout << "You, sir, just rage-quitted, shame on you!" << endl;
                    exit(0);
                } else {
                    cin.clear();
                    cin.ignore();
                    cin >> play_option;
                    continue;
                }
            }

            _computer->refill_source(get_turn());
            _computer->reset_draw();
            _computer->reset_cards();

            //UI #################
            system("cls");

            show_computer();
            show_player();
            print_options();

            //draw every round if possible
            if (!(_computer->get_card_draw_status()) && (_computer->_hand.size() < 5)) {
                _computer->draw();
            }

            //play every card possible each turn and cast spells
            int monster_position = 0;
            for (unsigned int card_position = 0; card_position < _computer->_hand.size(); card_position++) {
                monster_position++;

                if ((_computer->_hand[card_position]->get_cost() <= _computer->get_source()) && (_computer->_in_play.size() != 5)) {
                    if (_computer->_hand[card_position]->get_type() == MONSTER) {
                        _computer->play_card(monster_position - 1);
                        monster_position--;
                    } else {
                        //cast
                        if (_player->_in_play.size() != 0) {
                            int target_position = rand() % _player->_in_play.size();
                            cast_spell(_computer, card_position, _player, target_position);
                            if (_player->_in_play[target_position]->get_health_current() < 1) {
                                _player->remove_card(target_position);
                            }
                        }
                    }
                }
            }

            //attack monsters or player
            if (_computer->_in_play.size() > 0 ) {
                int random_variable = 0;
                int defender_position = 1;
                for (unsigned int card_position = 0; card_position < _computer->_in_play.size(); card_position++) {
                    random_variable = rand() % 2;
                    if (_computer->_in_play[card_position]->get_playable()) {
                        //attack player if random_variable is 0
                        if (random_variable == 0) {

                            attack_player(_player, _computer->_in_play[card_position]->get_attack());
                            _computer->_in_play[card_position]->card_played();

                        } else /* var_rand == 1 */ {
                            if (_player->_in_play.size() != 0) {
                                defender_position = rand() % _player->_in_play.size();

                                _computer->_in_play[card_position]->attack_card(_player->_in_play[defender_position]);
                                _computer->_in_play[card_position]->card_played();
                                _player->_in_play[defender_position]->attack_card(_computer->_in_play[card_position]);

                                if (_player->_in_play[defender_position]->get_health_current() < 1) {
                                    _player->remove_card(defender_position);
                                }
                                if (_computer->_in_play[card_position]->get_health_current() < 1) {
                                    _computer->remove_card(card_position);
                                }

                            } else {
                                attack_player(_player, _computer->_in_play[card_position]->get_attack());
                                _computer->_in_play[card_position]->card_played();
                            }
                        }
                    }
                }
            }
            //######## end of Ui playing
        } while(1);
    }

};

int main()
{
    srand(time(0));

    unique_ptr<Game> game(new Game("Player ", "Enemy  "));
    game->generate_decks(30);
    game->begin_draw(3);
    game->play();

    return 0;
}
