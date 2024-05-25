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
        for (int i = x - 7; i <= x + 52; ++i) {
            gotoxy(i, y + 2);
            std::cout << "1";
            gotoxy(i, y + 2);
            std::cout << "1";
        }
    }

    // Set text color back to default (e.g., white text on black background)
    setColor(white, black);

    //gotoxy(x-5, y);
    drawHearts(x - 5, y, 3);
    drawhunger(x + 5, y, 3);
    drawmoney(x + 15, y, 3);
    drawWeapon(x + 25, y, 3, "없음");
}

int Scene::display() const {
    int x = 43;
    int y = 13;

    {
        // Draw white border
        setColor(white, white);
        for (int i = x - 9; i <= x + 49; ++i) {
            gotoxy(i, y - 13);
            std::cout << "1";
            gotoxy(i, y + choices.size() + 4); // 테두리 아래쪽 경계를 2만큼 더 아래로 이동
            std::cout << "1";
        }
        for (int j = y - 13; j <= y + choices.size() + 4; ++j) { // 테두리 아래쪽 경계를 2만큼 더 아래로 이동
            gotoxy(x - 10, j);
            std::cout << "1";
            gotoxy(x + 50, j);
            std::cout << "1";
        }
    }

    // Set text color back to default (e.g., white text on black background)
    setColor(white, black);

    gotoxy(x - 2, y - 8);
    std::cout << description;

    int index = 0;
    std::vector<char> keys;
    for (const auto& choice : choices) {
        gotoxy(x - 2, y + index);
        if (index == 0) {
            std::cout << "> " << choice.second.first;
        }
        else {
            std::cout << "  " << choice.second.first;
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
                std::cout << "  " << choices.at(keys[currentSelection]).first;
                --currentSelection;
                gotoxy(x - 2, y + currentSelection);
                std::cout << "> " << choices.at(keys[currentSelection]).first;
            }
            break;
        }

        case DOWN: {
            if (currentSelection < keys.size() - 1) {
                gotoxy(x - 2, y + currentSelection);
                std::cout << "  " << choices.at(keys[currentSelection]).first;
                ++currentSelection;
                gotoxy(x - 2, y + currentSelection);
                std::cout << "> " << choices.at(keys[currentSelection]).first;
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

std::map<char, std::pair<std::string, std::string>> Scene::getChoices() const {
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

void Game::addScene(const std::string& sceneId, const Scene& scene) {
    scenes[sceneId] = scene;
}

std::string Game::getNextSceneId(char choice, const std::string& currentSceneId) const {
    if (choice == 'q') {
        return "q";  // '자결하기' 선택 시 바로 종료
    }

    const Scene& currentScene = scenes.at(currentSceneId);
    std::vector<std::string> nextScenes;

    for (const auto& ch : currentScene.getChoices()) {
        if (ch.first != 'q') {  // 'q'를 제외한 선택지만 추가
            nextScenes.push_back(ch.second.second);
        }
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, nextScenes.size() - 1);

    return nextScenes[dis(gen)];
}

void Game::start(const std::string& startSceneId) {
    std::string currentSceneId = startSceneId;

    while (true) {
        system("cls");

        const Scene& currentScene = scenes.at(currentSceneId);
        player.displayStatus();  // 현재 플레이어 상태를 출력
        char choice = currentScene.display();

        currentSceneId = getNextSceneId(choice, currentSceneId);

        if (currentSceneId == "q") {
            break;
        }
    }
}

void runGame() {
    system("cls");
    Game game;

    Scene scene1("나를 더 이상 쓸모없다고 판단해 폐기하려 한다.", {
        {'1', {"도망간다", "2"}},
        {'q', {"자결한다", "q"}}
        });

    Scene scene2("거지를 만났다 그는 나를 잘 알고 있는듯 보인다", {
        {'1', {"공격한다", "3"}},
        {'2', {"이야기한다", "3"}},
        {'3', {"돈을 준다", "3"}}
        });

    Scene scene3("예전에 함께 일했던 동료가 보인다", {
        {'1', {"다가간다", "1"}},
        {'2', {"먼저 공격한다", "1"}},
        {'3', {"도망친다", "1"}}
        });

    game.addScene("1", scene1);
    game.addScene("2", scene2);
    game.addScene("3", scene3);

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

void drawhunger(int x, int y, int hunger) {
    const std::string hungericon = "Ψ";
    const std::string emptyhunger = " ";

    gotoxy(x, y);
    for (int i = 0; i < hunger; ++i) {
        std::cout << hungericon << " ";
    }
    for (int i = hunger; i < 3; ++i) {
        std::cout << emptyhunger << " ";
    }
}

void drawmoney(int x, int y, int money) {
    const std::string moneyicon = "$";
    const std::string emptymoney = " ";

    gotoxy(x, y);
    for (int i = 0; i < money; ++i) {
        std::cout << moneyicon << " ";
    }
    for (int i = money; i < 3; ++i) {
        std::cout << emptymoney << " ";
    }
}

void drawWeapon(int x, int y, int weapon, const std::string& weaponName) {
    const std::string weaponIcon = "W";
    const std::string emptyWeapon = " ";

    gotoxy(x, y);
    std::cout << weaponName << ": ";
    for (int i = 0; i < weapon; ++i) {
        std::cout << weaponIcon << " ";
    }
    for (int i = weapon; i < 3; ++i) {
        std::cout << emptyWeapon << " ";
    }
}

