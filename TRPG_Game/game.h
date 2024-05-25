#ifndef GAME_H
#define GAME_H

#include "util.h"

#include <string>
#include <map>

// Player 클래스 선언
class Player {
public:
    Player(int health, int hunger, int money, const std::string& weapon)
        : health(health), hunger(hunger), money(money), weapon(weapon) {}

    void displayStatus() const;

    int health;
    int hunger;
    int money;
    std::string weapon;
};

class Scene {
public:
    Scene() {}

    Scene(const std::string& description, const std::map<char, std::pair<std::string, std::string>>& choices)
        : description(description), choices(choices) {}

    int display() const;
    std::map<char, std::pair<std::string, std::string>> getChoices() const;
    char getChoice() const;
    bool isValidChoice(char choice) const;

private:
    std::string description;
    std::map<char, std::pair<std::string, std::string>> choices;
};

class Game {
public:
    Game() : player(3, 3, 0, "None") {}  // 기본 Player 상태 설정

    void addScene(const std::string& sceneId, const Scene& scene);
    void start(const std::string& startSceneId);
    std::string getNextSceneId(char choice, const std::string& currentSceneId) const;

private:
    std::map<std::string, Scene> scenes;
    Player player;
};

void runGame();

void drawHearts(int x, int y, int health);
void drawhunger(int x, int y, int hunger);
void drawmoney(int x, int y, int money);
void drawWeapon(int x, int y, int weapon, const std::string& weaponName);


#endif



