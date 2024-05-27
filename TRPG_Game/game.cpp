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

    drawHearts(x - 6, y, health,maxHealth);
    drawHunger(x + 5, y, hunger,maxHunger);
    drawMoney(x + 16, y, money ,maxMoney);
    drawWeapon(x + 27, y, items);
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
            return keys[currentSelection];
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

bool Scene::isValidChoice(char choice, const Player& player) const {
    int moneyChange = getMoneyChange(choice);
    int healthChange = getHealthChange(choice);

    // 플레이어의 배고픔과 돈이 없을 때 체력이 깎이도록 처리
    if ((player.money + moneyChange < 0 && healthChange < 0) || player.hunger <= 0) {
        if (player.health - healthChange <= 0) {
            return false; // 선택이 유효하지 않음
        }
    }

    return true; // 선택이 유효함
}


void Game::applySceneEffects(const Scene& scene, char choice) {

    player.maxHealth = 3;
    player.maxHunger = 3;
    player.maxMoney = 3;
   
    player.health += scene.getHealthChange(choice);
    player.hunger += scene.getHungerChange(choice);
    player.money += scene.getMoneyChange(choice);

    std::string weaponChange = scene.getWeaponChange(choice);
    if (!weaponChange.empty()) {
        player.weapon = weaponChange;
        player.items[weaponChange]++;

        if (weaponChange == "방탄복") {
            player.health += 2;
        }
    }

    if (player.health < 0) player.health = 0;
    if (player.hunger < 0) player.hunger = 0;
    if (player.money < 0) player.money = 0;  
    
    if (player.health > player.maxHealth) player.health = player.maxHealth;
    if (player.hunger > player.maxHunger) player.hunger = player.maxHunger;
    if (player.money > player.maxMoney) player.money = player.maxMoney;


    if (player.hunger < 0) {
        player.health += scene.getHungerChange(choice);
    }
    if (player.money < 0) {
        player.health += scene.getMoneyChange(choice);
    }

    if (player.health < 0) {
        gameOver = true;
    }

    //if (!scene.getWeaponChange(choice).empty()) {
//    player.weapon = scene.getWeaponChange(choice);
// 
// 
//}
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

        // Scene의 효과 메시지를 출력
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

                gotoxy(x -4, y - 4);  // 선택된 선택지를 표시할 위치
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
    {'1', {"다가간다", "2", -3, 0, 0, ""}},
    {'2', {"지나친다", "2", 2, 0, 0, ""}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "당신은 많은 킬러들에게 무참히 살해당했다.."},
            {'2', "조용히 그 자리를 벗어났다."}
        });

    Scene scene2("길을 가다 예전 동료를 마주했다", {
      {'1', {"다가간다($ -1)", "3", -2, 0, -1, ""}},
      {'2', {"지나친다", "4", -1, 0, 0, ""}},
      {'3', {"공격한다", "4", 0, 0, 1, ""}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "이런.. 당신을 노리고 접근한 동료였습니다.."},
            {'2', "지나치는 당신을 향해 칼을 던졌습니다."},
            {'3', "무사히 처리 했습니다."}
        });

    Scene scene3("도망치다 약국을 발견했습니다.", {
    {'1', {"들어간다($ - 1)", "4", 2, 0, -1, ""}},
    {'2', {"지나친다", "4", 0, 0, 0, ""}},
    {'3', {"약사를 위협한다", "4", 2, 0, 1, ""}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "붕대를 구했습니다."},
            {'2', "무시하고 지나갑니다..."},
            {'3', "붕대와 현금다발을 훔쳤습니다."}
        });
    
    Scene scene4("저 멀리서 당신을 저격하고 있습니다.", {
    {'1', {"숨는다", "5", 0, 0, 0, ""}},
    {'2', {"빠르게 뛴다", "6", -1, 0, 0, ""}},
    {'3', {"대응 사격한다", "6", -2, 0, 0, ""}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "근처에 있는 건물로 들어갔습니다."},
            {'2', "총탄이 당신의 팔을 스쳤습니다.."},
            {'3', "붕대와 현금다발을 훔쳤습니다."}
        });

    Scene scene5("마트에 들어왔습니다,다양한 물건이 있습니다.", {
    {'1', {"붕대($ - 1)", "6", 2, 0, -1, ""}},
    {'2', {"식빵($ - 1)", "6", 0, 2, -1, ""}},
    {'3', {"방탄복($ - 1)", "7", 0, 0, -1, "방탄복"}},
    {'4', {"돌아간다", "6", 0, 0, 0, ""}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "붕대를 감았습니다."},
            {'2', "허기가 사라졌습니다.."},
            {'3', "좀 더 든든해졌습니다."},
            {'4', "무시하고 지나갑니다..."}
        });

    Scene scene6("지친 몸을 이끌고 당신은 은신처를 찾고 있습니다.", {
    {'1', {"지하철역", "8", 0, 0, -1, ""}},
    {'2', {"빈집", "8", 1, 2, 0, ""}},
    {'3', {"믿을 수 있는 동료의 집", "8", 0, 0, 0, ""}},
    {'4', {"당신의 집", "8", -1, -1, 0, ""}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "노숙자들이 당신을 지켜봅니다.."},
            {'2', "다행히 아무도 들어오지 않았습니다.."},
            {'3', "당신의 동료가 밤동안 당신을 지켜줬습니다.."},
            {'4', "이런.. 잠복하고 있던 킬러들에게 걸렸습니다.."}
        });

    Scene scene7("저 멀리 fgfg이 많이 보인다.", {
    {'1', {"다가간다", "2", -3, 0, 0, "맨손"}},
    {'2', {"지나친다", "2", 0, 0, 0, "맨손"}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "킬러에게 다가갑니다..."},
            {'2', "킬러를 무시하고 지나갑니다..."}
        });

    Scene scene8("저 멀리 fgfg이 많이 보인다.", {
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
    game.addScene("5", scene5);
    game.addScene("6", scene6);
    game.addScene("7", scene7);
    game.addScene("8", scene8);



    game.start("1");
}

void drawHearts(int x, int y, int health, int maxHealth) {
    const std::string heart = "♥";
    const std::string emptyHeart = " ";

    gotoxy(x, y);
    for (int i = 0; i < health; ++i) {
        std::cout << heart << " ";
    }
    for (int i = health; i < maxHealth; ++i) {
        std::cout << emptyHeart << " ";
    }
}

void drawHunger(int x, int y, int hunger, int maxHunger) {
    const std::string hungerIcon = "Ψ";
    const std::string emptyHunger = " ";

    gotoxy(x, y);
    for (int i = 0; i < hunger; ++i) {
        std::cout << hungerIcon << " ";
    }
    for (int i = hunger; i < maxHunger; ++i) {
        std::cout << emptyHunger << " ";
    }
}

void drawMoney(int x, int y, int money, int maxMoney) {
    const std::string moneyIcon = "$";
    const std::string emptyMoney = " ";

    gotoxy(x, y);
    for (int i = 0; i < money; ++i) {
        std::cout << moneyIcon << " ";
    }
    for (int i = money; i < maxMoney; ++i) { 
        std::cout << emptyMoney << " ";
    }
}

void drawWeapon(int x, int y, const std::map<std::string, int>& items) {
    const std::string itemsIcon = "★";
    const std::string emptyitems = " ";

    gotoxy(x, y);
    for (const auto& item : items) {
        std::cout << itemsIcon << " ";
    }
    std::cout << std::endl;
}