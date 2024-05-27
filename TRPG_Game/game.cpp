#include "game.h"
#include "util.h"
#include <iostream>
#include <map>
#include <string>
#include <random>
#include <vector>

// Player 상태 출력 함수
void Player::displayStatus() const {
    int x = 41;
    int y = 1;
    {
        // Draw white border
        setColor(white, white);
        for (int i = x - 9; i <= x + 47; ++i) {
            gotoxy(i, y + 1);
            std::cout << " ";
            gotoxy(i, y + 1);
            std::cout << " ";
        }
    }

    // Set text color back to default (e.g., white text on black background)
    setColor(white, black);

    drawHearts(x - 5, y, health);
    drawHunger(x + 5, y, hunger);
    drawMoney(x + 15, y, money);
    drawWeapon(x + 25, y, weapon);
}

int Scene::display() const {
    int x = 43;
    int y = 13;

    {
        // Draw white border
        setColor(white, white);
        for (int i = x - 12; i <= x + 45; ++i) {
            gotoxy(i, y - 13);
            std::cout << " ";
            gotoxy(i, y + 6);
            std::cout << " ";
        }
        for (int j = y - 13; j <= y + 6; ++j) {
            gotoxy(x - 12, j);
            std::cout << " ";
            gotoxy(x + 45, j);
            std::cout << " ";
        }
    }

    // Set text color back to default (e.g., white text on black background)
    setColor(white, black);

    gotoxy(x - 6, y - 8);
    std::cout << description;

    int index = 0;
    std::vector<char> keys;
    for (const auto& choice : choices) {
        gotoxy(x - 2, y + index);
        if (index == 0) {
            std::cout << "> " << std::get<0>(choice.second);
        }
        else {
            std::cout << "  " << std::get<0>(choice.second);
        }
        keys.push_back(choice.first);
        ++index;
    }

    int currentSelection = 0;
    while (true) {
        int n = keyControl();
        switch (n) {
        case UP: {
            if (currentSelection > 0) {
                gotoxy(x - 2, y + currentSelection);
                std::cout << "  " << std::get<0>(choices.at(keys[currentSelection]));
                --currentSelection;
                gotoxy(x - 2, y + currentSelection);
                std::cout << "> " << std::get<0>(choices.at(keys[currentSelection]));
            }
            break;
        }

        case DOWN: {
            if (currentSelection < keys.size() - 1) {
                gotoxy(x - 2, y + currentSelection);
                std::cout << "  " << std::get<0>(choices.at(keys[currentSelection]));
                ++currentSelection;
                gotoxy(x - 2, y + currentSelection);
                std::cout << "> " << std::get<0>(choices.at(keys[currentSelection]));
            }
            break;
        }

        case SUBMIT: {
            char choice = keys[currentSelection];
            if (isValidChoice(choice)) {
                return choice;
            }
            break;
        }
        default:
            break;
        }
    }
}
 

std::map<char, std::tuple<std::string, std::string, int, int, int, std::string>> Scene::getChoices() const {
    return choices;
}

char Scene::getChoice() const {
    char choice;
    std::cin >> choice;
    return choice;
}

bool Scene::isValidChoice(char choice) const {
    return choices.find(choice) != choices.end();
}

void Game::applySceneEffects(const Scene& scene, char choice) {
    player.health += scene.getHealthChange(choice);
    player.hunger += scene.getHungerChange(choice);
    player.money += scene.getMoneyChange(choice);

    if (!scene.getWeaponChange(choice).empty()) {
        player.weapon = scene.getWeaponChange(choice);
    }

    if (player.health < 0) player.health = 0;
    if (player.hunger < 0) player.hunger = 0;
    if (player.money < 0) player.money = 0;

    if (player.health <= 0) {
        gameOver = true;
    }
}

void Game::addScene(const std::string& sceneId, const Scene& scene) {
    scenes[sceneId] = scene;
}

std::string Game::getNextSceneId(char choice, const std::string& currentSceneId) const {
    if (choice == 'q') {
        return "q";
    }

    const Scene& currentScene = scenes.at(currentSceneId);
    return std::get<1>(currentScene.getChoices().at(choice));
}

void Game::start(const std::string& startSceneId) {
    std::string currentSceneId = startSceneId;

    while (true) {
        if (gameOver) {
            EndingDraw();
            break;
        }

        system("cls");

        const Scene& currentScene = scenes.at(currentSceneId);
        player.displayStatus();
        char choice = currentScene.display();

        // Scene의 효과를 적용
        applySceneEffects(currentScene, choice);

        if (gameOver) {
            EndingDraw();
            Sleep(3000);
            system("cls");
            titleDraw();
            menuDraw();
            break;
        }

        std::string effectMessage = currentScene.getEffectMessage(choice);
        if (!effectMessage.empty()) {
            int x = 43;
            int y = 13;
            {
                system("cls");
                // 효과 메시지 출력
                setColor(white, white);
                for (int i = x - 12; i <= x + 45; ++i) {
                    gotoxy(i, y - 13);
                    std::cout << " ";
                    gotoxy(i, y + 6);
                    std::cout << " ";
                }
                for (int j = y - 13; j <= y + 6; ++j) {
                    gotoxy(x - 12, j);
                    std::cout << " ";
                    gotoxy(x + 45, j);
                    std::cout << " ";
                }

                gotoxy(x + 1, y - 4);  // 선택된 선택지를 표시할 위치
                setColor(white, black);
                std::cout << effectMessage;
            }
            Sleep(2000);  // 2초 대기
        }

        currentSceneId = getNextSceneId(choice, currentSceneId);
        if (currentSceneId == "q") {
            break;
        }
    }
}


void runGame() {
    system("cls");
    Game game;

    Scene scene1("저 멀리 킬러들이 많이 보인다.", {
     {'1', {"다가간다", "2", -3, 0, 0, "맨손"}},
     {'2', {"지나친다", "2", 0, 0, 0, "맨손"}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "킬러에게 다가갑니다..."},
            {'2', "킬러를 무시하고 지나갑니다..."}
        });

    Scene scene2("저 멀리 킬러들이 많이 보인다.", {
    {'1', {"다가간다", "2", -3, 0, 0, "맨손"}},
    {'2', {"지나친다", "2", 0, 0, 0, "맨손"}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "킬러에게 다가갑니다..."},
            {'2', "킬러를 무시하고 지나갑니다..."}
        });

    Scene scene3("저 멀리 킬러들이 많이 보인다.", {
    {'1', {"다가간다", "2", -3, 0, 0, "맨손"}},
    {'2', {"지나친다", "2", 0, 0, 0, "맨손"}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "킬러에게 다가갑니다..."},
            {'2', "킬러를 무시하고 지나갑니다..."}
        });

    Scene scene4("저 멀리 킬러들이 많이 보인다.", {
     {'1', {"다가간다", "2", -3, 0, 0, "맨손"}},
     {'2', {"지나친다", "2", 0, 0, 0, "맨손"}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "킬러에게 다가갑니다..."},
            {'2', "킬러를 무시하고 지나갑니다..."}
        });

     
    game.addScene("1", scene1);
    game.addScene("2", scene2);
    game.addScene("3", scene3);
    game.addScene("4", scene4);



    game.start("1");

   
}

void drawHearts(int x, int y, int health) {
    const std::string heart = "♥";
    const std::string emptyHeart = " ";

    gotoxy(x, y);
    for (int i = 0; i < health; ++i) {
        std::cout << heart << " ";
    }
    for (int i = health; i < 3; ++i) {
        std::cout << emptyHeart << " ";
    }
}

void drawHunger(int x, int y, int hunger) {
    const std::string hungerIcon = "Ψ";
    const std::string emptyHunger = " ";

    gotoxy(x, y);
    for (int i = 0; i < hunger; ++i) {
        std::cout << hungerIcon << " ";
    }
    for (int i = hunger; i < 3; ++i) {
        std::cout << emptyHunger << " ";
    }
}

void drawMoney(int x, int y, int money) {
    const std::string moneyIcon = "$";
    const std::string emptyMoney = " ";

    gotoxy(x, y);
    for (int i = 0; i < money; ++i) {
        std::cout << moneyIcon << " ";
    }
    for (int i = money; i < 3; ++i) {
        std::cout << emptyMoney << " ";
    }
}

void drawWeapon(int x, int y, const std::string& weaponName) {
    gotoxy(x, y);
    std::cout << weaponName;
}