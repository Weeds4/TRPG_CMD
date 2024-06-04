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

    drawHearts(x - 7, y, health,maxHealth);
    drawHunger(x + 6, y, hunger,maxHunger);
    drawMoney(x + 19, y, money ,maxMoney);
    drawWeapon(x + 29, y, items);
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

    if (player.health == 0) player.maxHealth = 3;
    if (player.hunger == 0) player.maxHunger = 3;
    if (player.money == 0) player.maxMoney = 3;

    player.health += scene.getHealthChange(choice);
    player.hunger += scene.getHungerChange(choice);
    player.money += scene.getMoneyChange(choice);

    std::string weaponChange = scene.getWeaponChange(choice);
    if (!weaponChange.empty()) {
        player.weapon = weaponChange;
        player.items[weaponChange]++;

        if (weaponChange == "방탄복") {
            player.maxHealth += 1;
        }

        if (weaponChange == "신문지") {
            player.health += 1;
        }

        if (weaponChange == "에너지바") {
            player.health += 1;
        }      

        if (weaponChange == "차용증") {
            player.maxMoney -= 1;    
        }        
        
        if (weaponChange == "의약품") {
            player.health += 2;
        }        
        
        if (weaponChange == "동료") {
            player.maxHealth += 1;
        }        
        
        if (weaponChange == "열쇠") {
            player.health += 0;
        }        
        
        if (weaponChange == "근육") {
            player.maxHealth += 1;
        }

        if (weaponChange == "스트레스") {
            player.health -= 1;
        }
     
        
    }

    if (player.health <= 0) player.health = 0;
    if (player.hunger <= 0) player.hunger = 0;
    if (player.money <= 0) player.money = 0;  
    
    if (player.health >= player.maxHealth) player.health = player.maxHealth;
    if (player.hunger >= player.maxHunger) player.hunger = player.maxHunger;
    if (player.money >= player.maxMoney) player.money = player.maxMoney;


    if (player.hunger < 0) {
        player.health += scene.getHungerChange(choice);
    }
    if (player.money < 0) {
        player.health += scene.getMoneyChange(choice);
    }

    if (player.health <= 0) {
        gameOver = true;
    }


}



void Game::addScene(const std::string& sceneId, const Scene& scene) {
    scenes[sceneId] = scene;
}

std::string Game::getNextSceneId(char choice, const std::string& currentSceneId) const {
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
        applySceneEffects(currentScene, choice);


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

                if (gameOver) {
                    gotoxy(x - 4, y - 4);  // 선택된 선택지를 표시할 위치
                    setColor(white, black);
                    std::cout << effectMessage;
                }
                gotoxy(x -4, y - 4);  // 선택된 선택지를 표시할 위치
                setColor(white, black);
                std::cout << effectMessage;
            }
            Sleep(1000);  // 2초 대기
        }

        
        currentSceneId = getNextSceneId(choice, currentSceneId);
        if (currentSceneId == "x") {
            Xending();
        }



        if (gameOver) {
            EndingDraw();
            Sleep(1000);
            system("cls");
            titleDraw();
            menuDraw();
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
      {'2', {"지나친다", "3", -1, 0, 0, ""}},
      {'3', {"공격한다", "3", 0, 0, 1, ""}}
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
            {'3', "총에 어깨 부상을 당했습니다."}
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


    Scene scene14("은신처로 갔더니 당신의 동료가 죽어있다.. ", {
    {'1', {"다가간다", "15", -1, 0, 0, ""}},
    {'2', {"지나간다", "15", 0, 0, 0, ""}},
    {'3', {"조사한다", "15", 0, 0, 0, "열쇠"}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "시체가 폭발했다."},
            {'2', "무시하고 지나간다."},
            {'3', "당신을 죽이라고 한 사람의 짓 같다..(열쇠)"}
        });


    Scene scene15("킬러들에게 붙잡혔습니다.", {
    {'1', {"협상한다", "16", 0, 0, -1, ""}},
    {'2', {"이야기한다", "16", -2, 0, -1, ""}},
    {'3', {"자결한다", "16", -10, 0, 0, ""}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "당신을 죽이려던 킬러가 아니였습니다."},
            {'2', "킬러들의 화를 돋구었습니다.."},
            {'3', "자살했습니다.."}
        });

    Scene scene16("2일차 s밤이 찾아왔습니다..", {
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
            {'1', "집에 살고있던 사람을 죽였습니다.(스트레스)"},
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
    {'2', {"넘어간다", "20", 0, 0, 0, ""}},
    {'3', {"적지로 이동한다", "2", -3, 0, 0, "동료"}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "수상한 킬러가 당신을 지켜보다 떠납니다..ㄴ"},
            {'2', "못본척 넘어갔습니다.."},
            {'3', "적지로 이동합니다.."}
        });
    
    Scene scene20("고용된 킬러들이 당신을 쫒아온다.", {
    {'1', {"영화배우 처럼 처리한다", "21", -2, 0, 0, ""}},
    {'2', {"도망친다", "21", -10, 0, -1, ""}},
    {'3', {"비겁하게 공격한다", "21", -1, 0, 0, ""}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "현실은 참혹했습니다..."},
            {'2', "도망칠 수 없었습니다.."},
            {'3', "힘겹게 이겼습니다.."}
        });
    
    Scene scene21(" 약국을 발견했습니다.", {
    {'1', {"사람이 없을때 들어간다", "22", +2, 0, 1, ""}},
    {'2', {"지나간다", "22", 0, 0, 0, ""}},
    {'3', {"들어간다", "22", 2, 0, -1, ""}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "물건을 훔쳐 달아납니다"},
            {'2', "무사히 지나갔습니다.."},
            {'3', "상처를 치료했습니다.."}
        });
    
    Scene scene22("병원에서 당신의 동료가 부릅니다", {
  {'1', {"병원으로 간다", "23", -2, 0, 0, ""}},
  {'2', {"무시한다", "23", 0, 0, 0, ""}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', " 킬러들이 기다리고 있었습니다..."},
            {'2', "무사히 지나갑니다.."}
        });


    Scene scene23("당신은 오랜만의 휴식을 취합니다.", {
    {'1', {"음식을 해먹는다", "24", 0, 2, 0, ""}},
    {'2', {"잠을잔다", "24", 3, 0, 0, ""}},
    {'3', {"쇼핑한다", "24", 0, 0, -1, "의약품"}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "맛은 없지만 배는 부릅니다..."},
            {'2', "깊은 잠에 빠졌습니다.."},
            {'3', "의약품을 구매했습니다.."}
        });


    Scene scene24("동료들이 당신을 찾습니다.", {
    {'1', {"무시한다", "25", 0, 0, 0, ""}},
    {'2', {"뒤를 잡는다", "25", 0, 0, 2, ""}},
    {'3', {"다가간다", "25", -1, 0, 0, ""}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "무사히 지나갑니다..."},
            {'2', "동료들이 아니였습니다, 처리합니다.."},
            {'3', "매복해있던 킬러가 있었습니다.."}
        });

    Scene scene25("죽은 동료의 집으로 향합니다", {
    {'1', {"동료를 죽인자를 조사한다", "26", 0, 0, 0, ""}},
    {'2', {"상처를 치료한다", "26", 2, 0, 0, ""}},
    {'3', {"휴식을 취한다", "26", 1, 1, 0, ""}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "당신이 있던곳의 보스 인 거 같다..."},
            {'2', "상처를 치료했습니다.."},
            {'3', "깊은 잠에 듭니다.."}
        });

    Scene scene26("3일차 밤.. 당신을 죽이려 만은 킬러들이 몰려옵니다..", {
    {'1', {"대비한다", "27", 3, 3, 3, ""}},
    {'2', {"숨는다", "27", -3, 0, 0, ""}},
    {'3', {"먼저 보스를 친다", "27", -10, 0, 0, ""}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "철저히 준비했습니다"},
            {'2', "숨기만 하면 해결 할 수 없는 일이 있습니다..."},
            {'3', "성 급했습니다..."}
        });

    Scene scene27("당신은 복수를 위해 보스에게 연락합니다.", {
    {'1', {"협박한다", "28", -2, 0, 0, ""}},
    {'2', {"침착하게 대화한다", "28", 0, 0, 0, ""}},
    {'3', {"전화하지 않는다", "28", 0, 0, 0, ""}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "이런 도청당했습니다..."},
            {'2', "이상한 이야기를 듣습니다.."},
            {'3', "아무일도 일어나지 않았습니다.."}
        });

    Scene scene28("새벽에 총성이 들린다.", {
    {'1', {"경계한다", "29", 0, 0, 0, ""}},
    {'2', {"도망친다", "29", 0, 0, -1, ""}},
    {'3', {"무시한다", "29", 1, 1, 0, ""}}
        }, "뭔가 이상한 기운이 느껴진다...", {
            {'1', "아무일도 일어나지 않았습니다..."},
            {'2', "다른곳에서 잠에 듭니다.."},
            {'3', "무사히 지나갑니다.."}
        });


    Scene scene29("보스가 보낸 킬러가 당신을 찾습니다", {
    {'1', {"다가간다", "30", 0, 0, 2, ""}},
    {'2', {"도망친다", "30", 0, 0, 0, ""}},
    {'3', {"공격한다", "30", -1, -1, 0, ""}}
        }, "..", {
            {'1', "보스가 벌인 일이 아니라고 해명합니단ㄴ"},
            {'2', "무사히 도망갑니다.."},
            {'3', "의미 없는 싸움이였습니다."}
        });

    Scene scene30("배가고파 사람이 많은 마트로 향합니다", {
    {'1', {"음식을 구매한다", "31", 0, 1, -1, ""}},
    {'2', {"음식을 훔친다", "31", 0, 2, 0, ""}},
    {'3', {"음식을 구걸한다", "31", 1, 2, 0, ""}}
        }, "...", {
            {'1', "식빵을 먹었습니다"},
            {'2', "다행이 걸리지 않았습니다"},
            {'3', "마음 좋은 빵집 사장이 도와주었습니다"}
        });

    Scene scene31("예전에 만들어둔 벙커가 있다", {
    {'1', {"숨는다", "32", -10, 0, 0, ""}},
    {'2', {"상처를 치료하고 나온다", "32", 5, 0, 0, ""}},
    {'3', {"잠을 잔다", "32", 3, 3, 0, ""}}
        }, "...", {
            {'1', "당신의 나약함이 당신을 죽였습니다"},
            {'2', "안전하게 치료를 끝내고 나왔습니다."},
            {'3', "방해받지 않고 잠에 들었습니다.."}
        });

    Scene scene32("보스는 범인이 아니였습니다..", {
            {'1', {"믿지 않는다", "33", -10, 0, 0, ""}},
            {'2', {"진범을 찾는다", "33", 0, 0, 0, ""}},
            {'3', {"보스에게 도움을 요청한다", "33", 2, 2, 2, ""}}
        }, "...", {
            {'1', "때론 멍청함이 독이 됩니다.."},
            {'2', "진범은 당신의 라이벌 준 이였습니다.."},
            {'3', "보스로 부터 도움을 받습니다."}
        });

    Scene scene33("진범을 조사합니다", {
    {'1', {"위치를 파악한다", "34", 0, 0, 0, ""}},
    {'2', {"당신을 죽이려는 원인을 찾는다", "34", 0, 0, 0, ""}},
    {'3', {"때를 기다린다", "34", 0, 0, 0, ""}}
        }, "...", {
            {'1', "파악하지 못했습니다.."},
            {'2', "영문 모를 살의만 보입니다"},
            {'3', "때를 기다립니다.."}
        });

    Scene scene34("4일차 밤, 당신은 철저한 준비를 시작합니다", {
    {'1', {"몸을 단련한다", "35", 0, 0, 0, "근육"}},
    {'2', {"충분한 체력을 회복한다", "35", 3, 3, 0, ""}},
    {'3', {"바로 실행한다", "35", -10, 0, 0, ""}}
        }, "...", {
            {'1', "근육을 얻었습니다(근육)"},
            {'2', "컨디션이 최상입니다"},
            {'3', "성급했습니다.."}
        });

    Scene scene35("정체를 들킨 킬러들이 당신을 노려옵니다", {
    {'1', {"복수를 시작한다", "36", -3, 0, 0, ""}},
    {'2', {"때를 기다린다", "36", 0, 0, 2, ""}},
    {'3', {"도망간다", "36", 0, 0, 0, "수치심"}}
        }, "...", {
            {'1', "꽤나 힘든 싸움이였다"},
            {'2', "아직 때가 아니다.."},
            {'3', "당신은 수치심을 얻습니다(수치심)"}
        });

    Scene scene36("적들은 계속해서 당신을 공격합니다", {
{'1', {"물러서지 않는다", "37", -2, 0, 0, ""}},
{'2', {"혼자선 불가능 하다 후퇴한다", "37", 0, 0, 0, ""}},
{'3', {"도망간다", "37", -2, 0, 0, ""}}
        }, "...", {
            {'1', "적들은 끝이 없었다.."},
            {'2', "당신은 동료들에게 도움을 요청했다"},
            {'3', "적절한 판단이였습니다."}
        });

    Scene scene37("복수를 위한 칼을 갈던 당신앞에 진범이 보입니다", {
{'1', {"진을 죽인다", "38", -2, 0, 0, ""}},
{'2', {"존을 죽인다", "38", -2, 0, 0, ""}},
{'3', {"준을 죽인다", "38", 1, 0, 0, ""}}
        }, "...", {
            {'1', "그 사람은 일반인 입니다.."},
            {'2', "죄 없는 무고한 사람을 죽였습니다.."},
            {'3', "준은 치명상을 입고 도망갔습니다."}
        });

    Scene scene38("당신은 준의 처소까지 들어갔습니다.", {
{'1', {"복수를 끝낸다", "40", -2, 0, 0, ""}},
{'2', {"이야기한다", "39", 3, 3, 3, ""}},
{'3', {"용서한다", "40", -10, 0, 0, ""}}
        }, "...", {
            {'1', "준의 부하들에게 저지 당했습니다.."},
            {'2', "준의 이야기를 들어봅니다"},
            {'3', "당신은 또 다시 배신당했습니다.."}
        });

    Scene scene39("준의 동생은 당신의 타겟이 되어 당신의 손에 \n \t\t\t\t     제거되었습니다. 이에 분노했던 준은 \n \t\t\t\t     당신이 킬러를 은퇴하였을때를 노려 당신에게\n \t\t\t\t     복수를 하려 했습니다..", {

{'1', {"다음", "40", 0, 0, 0, ""}}
        }, "...", {
            {'1', ""},
            {'2', ""},
            {'3', ""}
        });

    Scene scene40("준의 부하들이 시간을 끌었습니다.. 준은 도망쳤습니다", {
{'1', {"준의 오른팔을 공격한다", "41", -1, 0, 0, ""}},
{'2', {"준의 왼팔을 공격한다", "41", -1, 0, 0, ""}},
{'3', {"넘어간다", "42", 0, 0, 0, ""}}
        }, "...", {
            {'1', "어렵지 않은 상대였습니다."},
            {'2', "어렵지 않은 상대였습니다."},
            {'3', "당신은 준을 이해하려 했습니다."}
        });

    Scene scene41("당신은 다시 준을 쫒습니다", {
{'1', {"빌딩으로 들어간다", "43", 0, 0, 0, ""}},
{'2', {"공원으로 간다", "44", 0, 0, 0, ""}},
{'3', {"준의 집으로 간다", "45", 0, 0, 0, ""}}
        }, "...", {
            {'1', "빌딩으로 향합니다"},
            {'2', "공원으로 걸어갑니다.."},
            {'3', "준의 집을 찾아 이동합니다."}
        });

    Scene scene42("결국 복수는 또 다른 복수를 야기할 뿐.. \n \t\t\t\t     그것을 막을 방법은 용서 뿐 입니다..", {
{'x', {"끝낸다", "x", 0, 0, 0, ""}},
{'2', {"복수를 이어간다", "40", 0, 0, 0, ""}}
        }, "...", {
            {'1', "... "},
            {'3', "당신은 복수를 이어갑니다.."}
        });

    Scene scene43("빌딩 안에는 아무도 없었습니다..", {
{'1', {"나간다", "46", -10, 0, 0, ""}},
{'2', {"둘러본다", "41", 0, 0, 0, ""}},
{'3', {"대기한다", "46", -10, 0, 0, ""}}
        }, "...", {
            {'1', "당신은 빌딩이 폭파했습니다.."},
            {'2', "둘러보다 폭탄을 발견해 제거했습니다."},
            {'3', "대기중 빌딩이 폭파했습니다.."}
        });

    Scene scene44("준은 공원 밴치에 앉아있습니다", {
{'1', {"제거한다", "46", 0, 0, 0, ""}},
{'2', {"용서한다", "46", -10, 0, 0, ""}}
        }, "...", {
            {'1', "당신은 준을 죽였습니다.."},
            {'2', "준이 당신을 죽였습니다.."}
        });

    Scene scene45("준의 집은 이미 불타 버렸습니다..", {
{'1', {"돌아간다", "41", 0, 0, 0, ""}}
        }, "...", {
            {'1', "당신은 다시 왔던길을 걸어갑니다"}
        });

    Scene scene46("당신은 준의 시체를 바라봅니다", {
{'1', {"다시 도망간다", "47", 0, 0, 0, ""}},
{'x', {"자결한다", "x", 0, 0, 0, ""}},
{'3', {"또 다른 복수를 시작한다", "47", 0, 0, 0, ""}}
        }, "...", {
            {'1', "..."},
            {'2', "..."},
            {'3', "..."}
        });

    Scene scene47("킬러의 끝은 모두가 알고 있습니다..", {
{'x', {"끝낸다", "x", 0, 0, 0, ""}}
        }, "...", {
            {'1', "당신은 끝없는 도망을 시작합니다.."}
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
    game.addScene("15", scene15);
    game.addScene("16", scene16);
    game.addScene("17", scene17);
    game.addScene("18", scene18);
    game.addScene("19", scene19);
    game.addScene("20", scene20);
    game.addScene("21", scene21);
    game.addScene("22", scene22);
    game.addScene("23", scene23);
    game.addScene("24", scene24);
    game.addScene("25", scene25);
    game.addScene("26", scene26);
    game.addScene("27", scene27);
    game.addScene("28", scene28);
    game.addScene("29", scene29);
    game.addScene("30", scene30);
    game.addScene("31", scene31);
    game.addScene("32", scene32);
    game.addScene("33", scene33);
    game.addScene("34", scene34);
    game.addScene("35", scene35);
    game.addScene("36", scene36);
    game.addScene("37", scene37);
    game.addScene("38", scene38);
    game.addScene("39", scene39);
    game.addScene("40", scene40);
    game.addScene("41", scene41);
    game.addScene("42", scene42);
    game.addScene("43", scene43);
    game.addScene("44", scene44);
    game.addScene("45", scene45);
    game.addScene("46", scene46);
    game.addScene("47", scene47);



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