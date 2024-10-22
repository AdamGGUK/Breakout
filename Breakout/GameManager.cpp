#include "GameManager.h"
#include "Ball.h"
#include "PowerupManager.h"
#include <iostream>

GameManager::GameManager(sf::RenderWindow* window)
    : _window(window), _paddle(nullptr), _ball(nullptr), _brickManager(nullptr), _powerupManager(nullptr),
    _messagingSystem(nullptr), _ui(nullptr), _pause(false), _time(0.f), _lives(3), _pauseHold(0.f), _levelComplete(false),
    _powerupInEffect({ none,0.f }), _timeLastPowerupSpawned(0.f)
{
    _font.loadFromFile("font/montS.ttf");
    _masterText.setFont(_font);
    _masterText.setPosition(230, 250);
    _masterText.setCharacterSize(48);
    _masterText.setFillColor(sf::Color::Yellow);

    _font.loadFromFile("font/montS.ttf");
    _scoreText.setFont(_font);
    _scoreText.setPosition(200, 350);
    _scoreText.setCharacterSize(48);
    _scoreText.setFillColor(sf::Color::Yellow);

    _score = 0;
    _gameState = GameManager::_startMenu;
    startMenu();
}

void GameManager::initialize()
{
    _paddle = new Paddle(_window);
    _brickManager = new BrickManager(_window, this);
    _messagingSystem = new MessagingSystem(_window);
    _ball = new Ball(_window, 400.0f, this); 
    _powerupManager = new PowerupManager(_window, _paddle, _ball);
    _ui = new UI(_window, _lives, this);

    // Create bricks
    _brickManager->createBricks(5, 10, 80.0f, 30.0f, 5.0f);
}

void GameManager::startNewGame()
{
    _score = 0;
    _lives = 3;
    _paddle = new Paddle(_window);
    _brickManager = new BrickManager(_window, this);
    _messagingSystem = new MessagingSystem(_window);
    _ball = new Ball(_window, 400.0f, this);
    _powerupManager = new PowerupManager(_window, _paddle, _ball);
    _ui = new UI(_window, _lives, this);
    _brickManager->createBricks(5, 10, 80.0f, 30.0f, 5.0f);
    _scoreText.setPosition(850, 700);
    _masterText.setString("");
}

void GameManager::update(float dt)
{
    _powerupInEffect = _powerupManager->getPowerupInEffect();
    _ui->updatePowerupText(_powerupInEffect);
    _powerupInEffect.second -= dt;

    if (_gameState == GameManager::_startMenu)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
        {
            _gameState = GameState::_inGame;
            startNewGame();
        }
    }

    if (_gameState == GameManager::_gameOver)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            startMenu();
        }
    }

    if (_gameState == GameManager::_inGame)
    {
    _scoreText.setString(std::to_string(_score));
    }
    

    if (_lives <= 0 && _gameState == GameState::_inGame)
    {
        _gameState = GameState::_gameOver;
        _finalScore = _score;
        gameOver();
        return;
    }
    if (_levelComplete)
    {
        _masterText.setString("Level completed.");
        return;
    }
    // pause and pause handling
    if (_pauseHold > 0.f) _pauseHold -= dt;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::P) && _gameState == GameState::_inGame)
    {
        if (!_pause && _pauseHold <= 0.f)
        {
            _pause = true;
            _masterText.setString("paused.");
            _pauseHold = PAUSE_TIME_BUFFER;

        }
        if (_pause && _pauseHold <= 0.f && _gameState != GameState::_startMenu)
        {
            _pause = false;
            _masterText.setString("");
            _pauseHold = PAUSE_TIME_BUFFER;
        }
    }
    if (_pause)
    {
        return;
    }

    // timer.
    _time += dt;


    if (_time > _timeLastPowerupSpawned + POWERUP_FREQUENCY && rand()%700 == 0)      // TODO parameterise
    {
        _powerupManager->spawnPowerup();
        _timeLastPowerupSpawned = _time;
    }

    // move paddle
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) _paddle->moveRight(dt);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) _paddle->moveLeft(dt);

    // update everything 
    if (_gameState == GameState::_inGame)
    {
    _paddle->update(dt);
    _ball->update(dt);
    _powerupManager->update(dt);
    }
}

void GameManager::loseLife()
{
    _lives--;
    _ui->lifeLost(_lives);

    // TODO screen shake.
}

void GameManager::increaseScore()
{
    _score += 10;
}

void GameManager::render()
{
    if (_gameState == GameState::_inGame)
    {
    _paddle->render();
    _ball->render();
    _brickManager->render();
    _powerupManager->render();
    _ui->render();
    }
    _window->draw(_masterText);
    _window->draw(_scoreText);
}

void GameManager::startMenu()
{
    _gameState = GameState::_startMenu;
    _masterText.setString("Welcome to Breakout!");
    _scoreText.setString("Press 'Backspace' to start.");
}

void GameManager::levelComplete()
{
    nextLevel();
}

void GameManager::nextLevel()
{
    GameManager::startNewGame();
}

void GameManager::gameOver()
{
    _masterText.setString("Game over :(");
    _scoreText.setString("Your score was : " + std::to_string(_finalScore));
    _masterText.setPosition(230, 250);
    _scoreText.setPosition(200, 350);
}

sf::RenderWindow* GameManager::getWindow() const { return _window; }
UI* GameManager::getUI() const { return _ui; }
Paddle* GameManager::getPaddle() const { return _paddle; }
BrickManager* GameManager::getBrickManager() const { return _brickManager; }
PowerupManager* GameManager::getPowerupManager() const { return _powerupManager; }
