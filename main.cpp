#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

// Taille de la fenetre et des cellules du joeure
const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;
const int CELL_SIZE = 20;

// direction que le joueur peu avoir
enum Direction
{
    Up,
    Down,
    Left,
    Right
};

// structure du serpent
struct SnakeSegment
{
    int x, y;

    // Surcharge de l'opérateur d'égalitée pour la comparaison
    bool operator==(const SnakeSegment &other) const
    {
        return x == other.x && y == other.y;
    }
};

// classe principale du jeu
class SnakeGame
{
private:
    sf::RenderWindow window;
    std::vector<SnakeSegment> snake;
    Direction direction;
    bool paused;
    bool inMenu;
    sf::Vector2i food;
    sf::Font font;
    sf::Text scoreText, resumeText, restartText, quitText;
    int score;

    // réinitialisation du jeu
    void resetGame()
    {
        snake.clear();
        snake.push_back({ WINDOW_WIDTH / 2 / CELL_SIZE * CELL_SIZE,
                          WINDOW_HEIGHT / 2 / CELL_SIZE * CELL_SIZE });
        direction = Right;
        paused = false;
        inMenu = false;
        score = 0;
        spawnFood();
    }

    // génération de la nouriture
    void spawnFood()
    {
        food.x = (std::rand() % (WINDOW_WIDTH / CELL_SIZE)) * CELL_SIZE;
        food.y = (std::rand() % (WINDOW_HEIGHT / CELL_SIZE)) * CELL_SIZE;
    }

    // mise à jour de la position du serpent
    void updateSnake()
    {
        if (paused || inMenu)
            return;

        SnakeSegment head = snake.front();
        switch (direction)
        {
        case Up:
            head.y -= CELL_SIZE;
            break;
        case Down:
            head.y += CELL_SIZE;
            break;
        case Left:
            head.x -= CELL_SIZE;
            break;
        case Right:
            head.x += CELL_SIZE;
            break;
        }

        // vérification des limites de la fenêtre et des collision avec le
        // corps du serpent
        if (head.x < 0 || head.x >= WINDOW_WIDTH || head.y < 0
            || head.y >= WINDOW_HEIGHT
            || std::find(snake.begin(), snake.end(), head) != snake.end())
        {
            inMenu = true; // ouvrir le menu en cas de collisions
        }
        else
        {
            snake.insert(snake.begin(), head);

            // si le serpent mange la nourriture
            if (head.x == food.x && head.y == food.y)
            {
                score += 1;
                spawnFood();
            }
            else
            {
                snake.pop_back();
            }
        }
    }

    // dessin du serpent
    void drawSnake()
    {
        sf::RectangleShape segmentShape(
            sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));
        segmentShape.setFillColor(sf::Color::Green);

        for (const auto &segment : snake)
        {
            segmentShape.setPosition(segment.x, segment.y);
            window.draw(segmentShape);
        }
    }

    // dessin de la nourriture
    void drawFood()
    {
        sf::RectangleShape foodShape(
            sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));
        foodShape.setFillColor(sf::Color::Red);
        foodShape.setPosition(food.x, food.y);
        window.draw(foodShape);
    }

    // dessin du score
    void drawScore()
    {
        scoreText.setString("Score: " + std::to_string(score));
        window.draw(scoreText);
    }

    // dessin du menu
    void drawMenu()
    {
        resumeText.setPosition(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 50);
        restartText.setPosition(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2);
        quitText.setPosition(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 + 50);

        window.draw(resumeText);
        window.draw(restartText);
        window.draw(quitText);
    }

// variable publique
public:
    SnakeGame()
        : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Snake Game")
        , paused(false)
        , inMenu(false)
        , score(0)
    {
        std::srand(static_cast<unsigned>(std::time(0)));

        // Chargement de la police
        if (!font.loadFromFile("arial.TTF"))
        {
            throw std::runtime_error("Impossible de charger la police");
        }

        scoreText.setFont(font);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(10, 10);

        resumeText.setFont(font);
        resumeText.setCharacterSize(24);
        resumeText.setFillColor(sf::Color::White);
        resumeText.setString("Reprendre");

        restartText.setFont(font);
        restartText.setCharacterSize(24);
        restartText.setFillColor(sf::Color::White);
        restartText.setString("Recommencer");

        quitText.setFont(font);
        quitText.setCharacterSize(24);
        quitText.setFillColor(sf::Color::White);
        quitText.setString("Quitter");

        resetGame();
    }

    void run()
    {
        sf::Clock clock;

        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                {
                    window.close();
                }
                else if (event.type == sf::Event::KeyPressed)
                {
                    if (event.key.code == sf::Keyboard::Escape)
                    {
                        inMenu = !inMenu;
                    }
                    else if (!paused && !inMenu)
                    {
                        if (event.key.code == sf::Keyboard::Z
                            && direction != Down)
                        {
                            direction = Up;
                        }
                        else if (event.key.code == sf::Keyboard::S
                                 && direction != Up)
                        {
                            direction = Down;
                        }
                        else if (event.key.code == sf::Keyboard::Q
                                 && direction != Right)
                        {
                            direction = Left;
                        }
                        else if (event.key.code == sf::Keyboard::D
                                 && direction != Left)
                        {
                            direction = Right;
                        }
                    }
                }
                else if (event.type == sf::Event::MouseButtonPressed && inMenu)
                {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::FloatRect resumeBounds = resumeText.getGlobalBounds();
                    sf::FloatRect restartBounds = restartText.getGlobalBounds();
                    sf::FloatRect quitBounds = quitText.getGlobalBounds();

                    if (resumeBounds.contains(static_cast<float>(mousePos.x),
                                              static_cast<float>(mousePos.y)))
                    {
                        inMenu = false;
                    }
                    else if (restartBounds.contains(
                                 static_cast<float>(mousePos.x),
                                 static_cast<float>(mousePos.y)))
                    {
                        resetGame();
                    }
                    else if (quitBounds.contains(
                                 static_cast<float>(mousePos.x),
                                 static_cast<float>(mousePos.y)))
                    {
                        window.close();
                    }
                }
            }

            if (clock.getElapsedTime().asMilliseconds() > 100)
            {
                updateSnake();
                clock.restart();
            }

            window.clear();
            if (inMenu)
            {
                drawMenu();
            }
            else
            {
                drawSnake();
                drawFood();
                drawScore();
            }
            window.display();
        }
    }
};

int main()
{
    try
    {
        SnakeGame game;
        game.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Erreur: " << e.what() << std::endl;
    }

    return 0;
}
