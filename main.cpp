#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

const int COMPLEXITY = 280;
int WINDOW_WIDTH = COMPLEXITY;
int WINDOW_HEIGHT = COMPLEXITY;
const int CELL_SIZE = 40;
const int FIELD_WIDTH = WINDOW_WIDTH / CELL_SIZE;
const int FIELD_HEIGHT = WINDOW_HEIGHT / CELL_SIZE;
const int MINES = COMPLEXITY / 40;

enum class CellState {
    Hidden,
    Revealed,
    Mine
};

struct Cell {
    CellState state = CellState::Hidden;
    bool hasMine = false;
};

void resetField(std::vector<std::vector<Cell>>& field) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
        for (int y = 0; y < FIELD_HEIGHT; y++) {
            field[x][y].state = CellState::Hidden;
            field[x][y].hasMine = false;
        }
    }
}

void initializeField(std::vector<std::vector<Cell>>& field, int mineCount) {
    srand(time(NULL));
    while (mineCount > 0) {
        int x = rand() % FIELD_WIDTH;
        int y = rand() % FIELD_HEIGHT;
        if (!field[x][y].hasMine) {
            field[x][y].hasMine = true;
            mineCount--;
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Minefield");

    sf::Texture hiddenTexture;
    if (!hiddenTexture.loadFromFile("hidden.png")) {
        std::cout << "Error hidden.png" << std::endl;
        return 1;
    }

    sf::Texture revealedTexture;
    if (!revealedTexture.loadFromFile("revealed.png")) {
        std::cout << "Error revealed.png" << std::endl;
        return 1;
    }

    sf::Texture mineTexture;
    if (!mineTexture.loadFromFile("mine.png")) {
        std::cout << "Error mine.png" << std::endl;
        return 1;
    }

    sf::Texture explodedMineTexture;
    if (!explodedMineTexture.loadFromFile("exploded_mine.png")) {
        std::cout << "Error exploded_mine.png" << std::endl;
        return 1;
    }

    sf::Font font;
    if (!font.loadFromFile("Pixeled.ttf")) {
        std::cout << "Error Pixeled.ttf" << std::endl;
        return 1;
    }

    sf::Sprite hiddenSprite(hiddenTexture);
    sf::Sprite revealedSprite(revealedTexture);
    sf::Sprite mineSprite(mineTexture);
    sf::Sprite explodedMineSprite(explodedMineTexture);

    std::vector<std::vector<Cell>> field(FIELD_WIDTH, std::vector<Cell>(FIELD_HEIGHT));

    bool running = true;
    bool restartRequested = false;

    while (running) {
        if (restartRequested) {
            resetField(field);
            initializeField(field, MINES);
        }

        // Menu
        sf::Text startText;
        startText.setFont(font);
        startText.setCharacterSize(COMPLEXITY / 40 * 2);
        startText.setString("START");
        startText.setFillColor(sf::Color::White);
        startText.setPosition(WINDOW_WIDTH / 2 - startText.getLocalBounds().width / 2,
                              WINDOW_HEIGHT / 2 - startText.getLocalBounds().height);

        sf::Text restartText;
        restartText.setFont(font);
        restartText.setCharacterSize(COMPLEXITY / 40 * 2);
        restartText.setString("RESTART");
        restartText.setFillColor(sf::Color::White);
        restartText.setPosition(WINDOW_WIDTH / 2 - restartText.getLocalBounds().width / 2,
                                WINDOW_HEIGHT / 2 + restartText.getLocalBounds().height);

        window.clear();
        window.draw(startText);
        window.display();

        sf::Event event;
        while (window.waitEvent(event)) {
            if (event.type == sf::Event::Closed) {
                running = false;
                break;
            } else if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    int mouseX = event.mouseButton.x;
                    int mouseY = event.mouseButton.y;

                    if (startText.getGlobalBounds().contains(mouseX, mouseY) || restartRequested) {
                        restartRequested = false;

                        resetField(field);
                        initializeField(field, MINES);

                        bool gameOver = false;
                        bool win = false;

                        // Пауза перед началом игры для показа мин
                        for (int x = 0; x < FIELD_WIDTH; x++) {
                            for (int y = 0; y < FIELD_HEIGHT; y++) {
                                if (field[x][y].hasMine) {
                                    field[x][y].state = CellState::Mine;
                                }
                            }
                        }

                        // Отображение мин на паузе
                        window.clear();
                        for (int x = 0; x < FIELD_WIDTH; x++) {
                            for (int y = 0; y < FIELD_HEIGHT; y++) {
                                int posX = x * CELL_SIZE;
                                int posY = y * CELL_SIZE;

                                if (field[x][y].state == CellState::Mine) {
                                    mineSprite.setPosition(posX, posY);
                                    window.draw(mineSprite);
                                }
                            }
                        }
                        window.display();

                        // Пауза на 5 секунд
                        std::this_thread::sleep_for(std::chrono::seconds(5));

                        // Сброс состояния поля
                        for (int x = 0; x < FIELD_WIDTH; x++) {
                            for (int y = 0; y < FIELD_HEIGHT; y++) {
                                if (field[x][y].hasMine) {
                                    field[x][y].state = CellState::Hidden;
                                }
                            }
                        }

                        while (window.isOpen()) {
                            while (window.pollEvent(event)) {
                                if (event.type == sf::Event::Closed) {
                                    window.close();
                                } else if (event.type == sf::Event::MouseButtonReleased) {
                                    if (!gameOver && !win) {
                                        if (event.mouseButton.button == sf::Mouse::Left) {
                                            int mouseX = event.mouseButton.x / CELL_SIZE;
                                            int mouseY = event.mouseButton.y / CELL_SIZE;

                                            if (mouseX >= 0 && mouseX < FIELD_WIDTH && mouseY >= 0 &&
                                                mouseY < FIELD_HEIGHT) {
                                                if (field[mouseX][mouseY].state == CellState::Hidden) {
                                                    if (field[mouseX][mouseY].hasMine) {
                                                        gameOver = true;
                                                        std::cout << "You lost!" << std::endl;
                                                    } else {
                                                        field[mouseX][mouseY].state = CellState::Revealed;
                                                        std::cout << "Opened!" << std::endl;
                                                    }
                                                }
                                            }
                                        }

                                        // Проверка на победу
                                        bool allNonMinesRevealed = true;
                                        for (int x = 0; x < FIELD_WIDTH; x++) {
                                            for (int y = 0; y < FIELD_HEIGHT; y++) {
                                                if (!field[x][y].hasMine &&
                                                    field[x][y].state != CellState::Revealed) {
                                                    allNonMinesRevealed = false;
                                                    break;
                                                }
                                            }
                                            if (!allNonMinesRevealed) {
                                                break;
                                            }
                                        }

                                        if (allNonMinesRevealed) {
                                            win = true;
                                            std::cout << "You win!" << std::endl;
                                        }
                                    }
                                }
                            }

                            window.clear();

                            for (int x = 0; x < FIELD_WIDTH; x++) {
                                for (int y = 0; y < FIELD_HEIGHT; y++) {
                                    int posX = x * CELL_SIZE;
                                    int posY = y * CELL_SIZE;

                                    if (field[x][y].state == CellState::Hidden) {
                                        hiddenSprite.setPosition(posX, posY);
                                        window.draw(hiddenSprite);
                                    } else if (field[x][y].state == CellState::Revealed) {
                                        revealedSprite.setPosition(posX, posY);
                                        window.draw(revealedSprite);
                                    } else if (field[x][y].state == CellState::Mine) {
                                        mineSprite.setPosition(posX, posY);
                                        window.draw(mineSprite);
                                    }
                                }
                            }

                            if (gameOver) {
                                sf::RectangleShape overlay(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
                                overlay.setFillColor(sf::Color(0, 0, 0, 128));
                                window.draw(overlay);
                                for (int x = 0; x < FIELD_WIDTH; x++) {
                                    for (int y = 0; y < FIELD_HEIGHT; y++) {
                                        int posX = x * CELL_SIZE;
                                        int posY = y * CELL_SIZE;
                                        if (field[x][y].hasMine) {
                                            explodedMineSprite.setPosition(posX, posY);
                                            window.draw(explodedMineSprite);
                                        }
                                    }
                                }
                                sf::Text message;
                                message.setFont(font);
                                message.setCharacterSize(COMPLEXITY / 40 * 3);
                                message.setFillColor(sf::Color::White);
                                message.setString("YOU LOST!");
                                message.setPosition(
                                        (WINDOW_WIDTH / 2) - (message.getLocalBounds().width / 2),
                                        (WINDOW_HEIGHT / 2) - (message.getLocalBounds().height / 2));
                                window.draw(message);
                                window.draw(restartText);
                            }

                            if (win) {
                                sf::RectangleShape overlay(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
                                overlay.setFillColor(sf::Color(0, 0, 0, 128));
                                window.draw(overlay);
                                for (int x = 0; x < FIELD_WIDTH; x++) {
                                    for (int y = 0; y < FIELD_HEIGHT; y++) {
                                        int posX = x * CELL_SIZE;
                                        int posY = y * CELL_SIZE;
                                        if (field[x][y].hasMine) {
                                            mineSprite.setPosition(posX, posY);
                                            window.draw(mineSprite);
                                        }
                                    }
                                }
                                sf::Text message;
                                message.setFont(font);
                                message.setCharacterSize(COMPLEXITY / 40 * 3);
                                message.setFillColor(sf::Color::White);
                                message.setString("YOU WIN!");
                                message.setPosition(
                                        (WINDOW_WIDTH / 2) - (message.getLocalBounds().width / 2),
                                        (WINDOW_HEIGHT / 2) - (message.getLocalBounds().height / 2));
                                window.draw(message);
                                window.draw(restartText);
                            }

                            window.display();

                            if (gameOver || win) {
                                restartRequested = true;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}