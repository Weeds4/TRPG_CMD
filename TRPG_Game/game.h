#ifndef GAME_H
#define GAME_H

#include "util.h"

#include <string>
#include <map>
#include <tuple>

// Player 클래스 선언
class Player {
public:
    Player(int health, int hunger, int money, const std::string& weapon)
        : health(health), hunger(hunger), money(money), weapon(weapon) {}

    void displayStatus() const;

    void setHealth(int newHealth) {
        if (newHealth < 0) newHealth = 0;
        if (newHealth > 3) newHealth = 3;
        health = newHealth;
    }

    int health;
    int hunger;
    int money;
    std::string weapon;
};

class Scene {
public:
    Scene() {}

    Scene(const std::string& description, const std::map<char, std::tuple<std::string, std::string, int, int, int, std::string>>& choices, const std::string& transitionMessage)
        : description(description), choices(choices), transitionMessage(transitionMessage) {}

    int display() const;
    std::map<char, std::tuple<std::string, std::string, int, int, int, std::string>> getChoices() const;
    char getChoice() const;
    bool isValidChoice(char choice) const;

    int getHealthChange(char choice) const { return std::get<2>(choices.at(choice)); }
    int getHungerChange(char choice) const { return std::get<3>(choices.at(choice)); }
    int getMoneyChange(char choice) const { return std::get<4>(choices.at(choice)); }
    std::string getWeaponChange(char choice) const { return std::get<5>(choices.at(choice)); }

private:
    std::string description;
    std::string transitionMessage; // 다음 씬으로 넘어갈 때 출력할 메시지
    std::map<char, std::tuple<std::string, std::string, int, int, int, std::string>> choices;
};

class Game {
public:
    Game() : player(3, 3, 0, "None") {}  // 기본 Player 상태 설정

    void addScene(const std::string& sceneId, const Scene& scene);
    void start(const std::string& startSceneId);
    std::string getNextSceneId(char choice, const std::string& currentSceneId) const;


private:

    bool gameOver;

    void applySceneEffects(const Scene& scene, char choice);

    std::map<std::string, Scene> scenes;
    Player player;
};

void runGame();

void drawHearts(int x, int y, int health);
void drawHunger(int x, int y, int hunger);
void drawMoney(int x, int y, int money);
void drawWeapon(int x, int y, const std::string& weaponName);

#endif