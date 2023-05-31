#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

const int COMPLEXITY = 160;
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

    sf::Sprite hiddenSprite(hiddenTexture);
    sf::Sprite revealedSprite(revealedTexture);
    sf::Sprite mineSprite(mineTexture);
    sf::Sprite explodedMineSprite(explodedMineTexture);

    std::vector<std::vector<Cell>> field(FIELD_WIDTH, std::vector<Cell>(FIELD_HEIGHT));

    srand(time(NULL));
    int mineCount = MINES;
    while (mineCount > 0) {
        int x = rand() % FIELD_WIDTH;
        int y = rand() % FIELD_HEIGHT;
        if (!field[x][y].hasMine) {
            field[x][y].hasMine = true;
            mineCount--;
        }
    }

    int startX = 0; // Начальная позиция игрока
    int startY = 0;
    int endX = FIELD_WIDTH - 1; // Конечная позиция
    int endY = FIELD_HEIGHT - 1;

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
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::MouseButtonReleased) {
                if (!gameOver && !win) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        int mouseX = event.mouseButton.x / CELL_SIZE;
                        int mouseY = event.mouseButton.y / CELL_SIZE;

                        if (mouseX >= 0 && mouseX < FIELD_WIDTH && mouseY >= 0 && mouseY < FIELD_HEIGHT) {
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
                            if (!field[x][y].hasMine && field[x][y].state != CellState::Revealed) {
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
            sf::Font font;
            if (!font.loadFromFile("Pixeled.ttf")) {
                std::cout << "Error arial.ttf" << std::endl;
                return 1;
            }
            message.setFont(font);
            message.setCharacterSize(COMPLEXITY/40*3);
            message.setFillColor(sf::Color::White);
            message.setString("YOU LOST!");
            message.setPosition((COMPLEXITY/2)-(COMPLEXITY/40*12), COMPLEXITY/2-30);
            window.draw(message);
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
            sf::Font font;
            if (!font.loadFromFile("Pixeled.ttf")) {
                std::cout << "Error arial.ttf" << std::endl;
                return 1;
            }
            message.setFont(font);
            message.setCharacterSize(COMPLEXITY/40*3);
            message.setFillColor(sf::Color::White);
            message.setString("YOU WIN!");
            message.setPosition((COMPLEXITY/2)-(COMPLEXITY/40*10), COMPLEXITY/2-30);
            window.draw(message);
        }

        window.display();
    }

    return 0;
}