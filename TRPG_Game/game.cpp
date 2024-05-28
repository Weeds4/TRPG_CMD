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
    {'3', {"방탄복($ - 1)", "6", 0, 0, -1, "방탄복"}},
    {'4', {"돌아간다", "6", 0, 0, 0, ""}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "붕대를 감았습니다."},
            {'2', "허기가 사라졌습니다.."},
            {'3', "좀 더 든든해졌습니다."},
            {'4', "무시하고 지나갑니다..."}
        });

    Scene scene6("지친 몸을 이끌고 당신은 은신처를 찾고 있습니다.", {
    {'1', {"지하철역", "7", 0, 0, 0, ""}},
    {'2', {"빈집", "8", 0, 0, 0, ""}},
    {'3', {"믿을 수 있는 동료의 집", "9", 0, 0, 0, ""}},
    {'4', {"당신의 집", "10", 0, 0, 0, ""}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "지하철역으로 이동합니다.."},
            {'2', "빈집으로 이동합니다.."},
            {'3', "동료의 집으로 이동합니다.."},
            {'4', "당신의 집으로 이동합니다.."}
        });

    Scene scene7("주위에서 당신을 향한 시선이 느껴집니다..", {
    {'1', {"이야기한다", "11", 1, 0, -1, "신문지"}},
    {'2', {"무시하고 잔다", "11", 1, 0, -2, ""}},
    {'2', {"도망친다", "11", 0, -1, 0, ""}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "당신은 신문지를 얻었습니다..."},
            {'2', "당신의 주머니를 털어갔습니다..."},
            {'3', "당신은 밤을 세웠습니다..."}
        });

    Scene scene8("아무도 없는 거 같습니다.", {
    {'1', {"자고간다", "11", 2, 1, 0, ""}},
    {'2', {"나간다", "11", 0, -1, 0, ""}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "밤새 아무도 오지 않았습니다."},
            {'2', "당신은 밤새 거리를 돌아다녔습니다..."}
        });


    Scene scene9("동료가 집에서 기다리고 있습니다.", {
    {'1', {"하룻밤 지낸다", "11", 2, 2, 1, ""}},
    {'2', {"밤새 이야기한다", "11", 0, 1, 0, "에너지바"}},
    {'3', {"공격한다", "11", -1, -1, 0, ""}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "동료가 밤새 당신을 지켰습니다."},
            {'2', "오랜만의 휴식을 만끽했습니다.. (에너지바 획득)"},
            {'3', "밤새 죄책감에 빠졌습니다.."}
        });


    Scene scene10("집 안에서 인기척이 느껴집니다.", {
    {'1', {"조용히 빠져나간다", "11", 0, 0, 0, ""}},
    {'2', {"숨는다", "11", 1, -1, 0, ""}},
    {'3', {"찾아본다", "11", -1, 0, 1, ""}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "집 안에는 킬러가 숨어있던 거 같다.."},
            {'2', "새벽 늦게까지 숨어있던 킬러가 집을 나갑니다.."},
            {'3', "숨어있던 킬러를 발견했습니다"}
        });

    Scene scene11("지나가다 은행을 발견했습니다.", {
    {'1', {"돈을 빌린다", "12", 0, 0, 2, "차용증"}},
    {'2', {"돈을 훔친다", "12", -2, -1, 3, ""}},
    {'3', {"지나친다", "12", 0, 0, 0, ""}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "돈을 빌렸습니다 (차용증 획득)"},
            {'2', "경찰이 당신을 공격했습니다."},
            {'3', "무사히 지나갑니다.."}
        });


    Scene scene12("아무도 들어가지 않는 음식점을 발견했습니다.", {
    {'1', {"들어간다", "13", 0, 2, -1, ""}},
    {'2', {"정찰한다", "13", 0, -1, 0, ""}},
    {'3', {"지나친다", "13", 0, 0, 0, ""}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "맛있는 밥을 먹었습니다."},
            {'2', "아무도 오지 않았습니다.."},
            {'3', "무사히 지나갑니다.."}
        });


    Scene scene13("골목에서 수상해 보이는 사람들을 만났습니다.", {
    {'1', {"먼저 공격한다", "14", -1, -1, 0, ""}},
    {'2', {"한명씩 제거한다", "14", -2, 0, 0, ""}},
    {'3', {"도망간다", "15", -1, -1, 0, ""}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "무사히 처리했습니다."},
            {'2', "한명을 제거하려다가 걸려버렸습니다.. "},
            {'3', "도망치려다 킬러들에게 당했습니다.."}
        });


    Scene scene14("사람들이 많이 모여있다.", {
    {'1', {"다가간다", "16", 0, 0, 0, ""}},
    {'2', {"지나간다", "16", 0, 0, 0, ""}},
    {'3', {"조사한다", "16", 0, 0, 0, "열쇠"}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "시체가 있었다.."},
            {'2', "무시하고 지나간다."},
            {'3', "당신을 죽이라고 한 사람이 벌인거 같다..(열쇠 획득)"}
        });


    Scene scene15("킬러들에게 붙잡혔습니다.", {
    {'1', {"협상한다", "16", 0, 0, 0, ""}},
    {'2', {"이야기한다", "16", -2, 0, -1, ""}},
    {'3', {"자결한다", "16", -10, 0, 0, ""}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "당신을 죽이려던 킬러가 아니였습니다."},
            {'2', "킬러들의 화를 돋구었습니다.."},
            {'3', "자살했습니다.."}
        });

    Scene scene16("2일차 저녁밤이 찾아왔습니다..", {
    {'1', {"빈집을 간다", "17", 0, 0, 0, ""}},
    {'2', {"지하철역을 간다", "18", 0, 0, 0, ""}},
    {'3', {"동료에게 부탁한다", "19", 0, 0, 0, ""}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "빈집을 찾았습니다."},
            {'2', "다시 노숙자들을 향해 갑니다"},
            {'3', "동료에게 연락했습니다"}
        });

    Scene scene17("인기척이 느껴집니다.", {
    {'1', {"공격한다", "20", 0, 2, 2, "스트레스"}},
    {'2', {"이야기한다", "20", 1, 1, -2, ""}},
    {'3', {"지나간다", "20", 0, 0, 0, ""}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "집에 살고있던 사람을 죽였습니다.(스트레스 획득)"},
            {'2', "돈을 가져갔습니다."},
            {'3', "무사히 빠져나갔습니다.."}
        });

    Scene scene18("노숙자들이 보이지 않습니다..", {
  {'1', {"자리를 떠난다", "19", 0, 0, 0, ""}},
  {'2', {"지하철역에 숨는다", "20", -2, 0, -1, ""}},
  {'3', {"싸운다", "20", -2, +2, 0, ""}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "밤새 불안에 떱니다.."},
            {'2', "이런 당신은 들켰습니다."},
            {'3', "노숙자들을 구했습니다.."}
        });
    
    Scene scene19("동료가 연락이 되지 않습니다..", {
    {'1', {"찾아간다", "20", -2, 0, 0, ""}},
    {'2', {"무시한다", "20", 0, 0, 0, ""}},
    {'3', {"적지로 이동한다", "2", -3, 0, 0, "동료"}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "동료를 납치하고 당신을 노려옵니다"},
            {'2', "두 눈을 감고 자결했습니다.."},
            {'3', "적지로 이동합니다.."}
        });
    
    Scene scene20("킬러들에게 붙잡혔습니다.", {
    {'1', {"협상한다", "2", 0, 0, 0, ""}},
    {'2', {"이야기한다", "2", -2, 0, -1, ""}},
    {'3', {"자결한다", "2", -10, 0, 0, ""}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "당신을 죽이려던 킬러가 아니였습니다."},
            {'2', "킬러들의 화를 돋구었습니다.."},
            {'3', "자살했습니다.."}
        });
    
    
    
    game.addScene("1", scene1);
    game.addScene("2", scene2);
    game.addScene("3", scene3);
    game.addScene("4", scene4);
    game.addScene("5", scene5);
    game.addScene("6", scene6);
    game.addScene("7", scene7);
    game.addScene("8", scene8);
    game.addScene("9", scene9);
    game.addScene("10", scene10);
    game.addScene("11", scene11);
    game.addScene("12", scene12);
    game.addScene("13", scene13);
    game.addScene("14", scene14);
    game.addScene("15", scene16);
    game.addScene("16", scene17);
    game.addScene("17", scene17);
    game.addScene("18", scene18);
    game.addScene("19", scene19);
    game.addScene("20", scene20);



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