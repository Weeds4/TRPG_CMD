#ifndef GAME_H
#define GAME_H

#include "util.h"

#include <string>
#include <map>

// Player Ŭ���� ����
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

    Scene(const std::string& description, const std::map<char, std::pair<std::string, std::string>>& choices, int healthChange = 0, int hungerChange = 0, int moneyChange = 0, const std::string& weaponChange = "")
        : description(description), choices(choices), healthChange(healthChange), hungerChange(hungerChange), moneyChange(moneyChange), weaponChange(weaponChange) {}

    int display() const;
    std::map<char, std::pair<std::string, std::string>> getChoices() const;
    char getChoice() const;
    bool isValidChoice(char choice) const;

    int getHealthChange() const { return healthChange; }
    int getHungerChange() const { return hungerChange; }
    int getMoneyChange() const { return moneyChange; }
    std::string getWeaponChange() const { return weaponChange; }

private:
    std::string description;
    std::map<char, std::pair<std::string, std::string>> choices;
    int healthChange; // �÷��̾��� ü�� ��ȭ��
    int hungerChange; // �÷��̾��� ����� ��ȭ��
    int moneyChange; // �÷��̾��� �� ��ȭ��
    std::string weaponChange; // �÷��̾��� ���� ��ȭ��
};

class Game {
public:
    Game() : player(3, 3, 0, "None") {}  // �⺻ Player ���� ����

    void addScene(const std::string& sceneId, const Scene& scene);
    void start(const std::string& startSceneId);
    std::string getNextSceneId(char choice, const std::string& currentSceneId) const;

private:
    void applySceneEffects(const Scene& scene);

    std::map<std::string, Scene> scenes;
    Player player;
};

void runGame();

void drawHearts(int x, int y, int health);
void drawHunger(int x, int y, int hunger);
void drawMoney(int x, int y, int money);
void drawWeapon(int x, int y, const std::string& weaponName);

#endif
