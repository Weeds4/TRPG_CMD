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
    int healthChange; // 플레이어의 체력 변화량
    int hungerChange; // 플레이어의 배고픔 변화량
    int moneyChange; // 플레이어의 돈 변화량
    std::string weaponChange; // 플레이어의 무기 변화량
};

class Game {
public:
    Game() : player(3, 3, 0, "None") {}  // 기본 Player 상태 설정

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
