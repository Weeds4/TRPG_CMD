#ifndef GAME_H
#define GAME_H

#include "util.h"

#include <string>
#include <map>
#include <tuple>

// Player 클래스 선언
class Player {
public:
    Player(int health, int maxHealth, int hunger, int maxHunger, int money, int maxMoney, const std::string& weapon)
        : health(health), maxHealth(maxHealth), hunger(hunger), maxHunger(maxHunger), money(money), maxMoney(maxMoney), weapon(weapon) {}

    void displayStatus() const;

    void setHealth(int newHealth) {
        if (newHealth < 0) newHealth = 0;
        if (newHealth > maxHealth) newHealth = maxHealth;
        health = newHealth;
    }

    void setHunger(int newHunger) {
        if (newHunger < 0) newHunger = 0;
        if (newHunger > maxHunger) newHunger = maxHunger;
        hunger = newHunger;
    }

    void setMoney(int newMoney) {
        if (newMoney < 0) newMoney = 0;
        if (newMoney > maxMoney) newMoney = maxMoney;
        money = newMoney;
    }

    int health;
    int maxHealth;
    int hunger;
    int maxHunger;
    int money;
    int maxMoney;
    std::string weapon;
    std::map<std::string, int> items;
};

class Scene {
public:
    Scene() {}

    Scene(const std::string& description, const std::map<char, std::tuple<std::string, std::string, int, int, int, std::string>>& choices, const std::string& transitionMessage, const std::map<char, std::string>& effectMessages)
        : description(description), choices(choices), transitionMessage(transitionMessage), effectMessages(effectMessages) {}

    int display(const Player& player) const;
    int display() const;
    std::map<char, std::tuple<std::string, std::string, int, int, int, std::string>> getChoices() const;
    char getChoice() const;
     bool isValidChoice(char choice, const Player& player) const;

    int getHealthChange(char choice) const { return std::get<2>(choices.at(choice)); }
    int getHungerChange(char choice) const { return std::get<3>(choices.at(choice)); }
    int getMoneyChange(char choice) const { return std::get<4>(choices.at(choice)); }
    std::string getWeaponChange(char choice) const { return std::get<5>(choices.at(choice)); }
    std::string getTransitionMessage() const { return transitionMessage; }
    std::string getEffectMessage(char choice) const {
        if (effectMessages.find(choice) != effectMessages.end()) {
            return effectMessages.at(choice);
        }
        else {
            return "";
        }
    }

private:
    std::string description;
    std::string transitionMessage;
    std::map<char, std::tuple<std::string, std::string, int, int, int, std::string>> choices;
    std::map<char, std::string> effectMessages;
};


class Game {
public:
    Game() : player(3, 3, 3, 3, 2, 3, "None") {}  // 기본 Player 상태 설정

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

void drawHearts(int x, int y, int health, int maxHealth);
void drawHunger(int x, int y, int hunger, int maxHunger);
void drawMoney(int x, int y, int money, int maxMoney);
void drawWeapon(int x, int y, const std::map<std::string, int>& items);

#endif