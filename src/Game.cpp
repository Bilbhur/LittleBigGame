#include "Game.h"
#include <memory>
#include "ResourceManager.h"
#include "Player.h"
#include "font.h"
#include "Ennemy.h"
#include "Bonus.h"
#include "Background.h"
#include "EnnemyGeneration.h"
#include <string>
#include <fstream>

using namespace std::string_literals;

Game::Game(Space &p_space, int widthScreen):
space{p_space}
{
    if (!font.loadFromMemory(Air_Americana_ttf, Air_Americana_ttf_size))
        throw std::runtime_error("Police introuvable");

    textScore.setFont(font);
    textBestScore.setFont(font);
    textBestScore.move(150,0);

    textFPS.setFont(font);
    displayFps(widthScreen);

    textLife.setFont(font);
    textShield.setFont(font);
    displayShipState(widthScreen);
    loadBestScoreFromFile();

    refreshBestScore();

    try {
        homeSprite.setTexture(ResourceManager<sf::Texture>::getResource("ressources/accueil.png"));
        setupMusic("ressources/swtheme.wav");
    } catch(std::exception const& exception) {
        initException(exception);
    } 
}

void Game::startGame()
{
    running = true;
    
    try {
        setupMusic("ressources/imperial_march.wav");
    } catch (std::exception const& exception) {
        initException(exception);
    }
    
    _clock.restart();
    if (level == 1)
        score = 0;
    refreshScore();
    space.add(std::make_unique<Background>(level));
    space.add(std::make_unique<Player>(*this, space));
    
    try {
        enm = EnnemyGeneration::GetEnnemiesToGenerate(level);
    } catch (std::exception const& exception) {
        initException(exception);
        endGame("ressources/end_win.png");
    }
}

void Game::generateEnnemies()
{
    if (isRunning()) {
        // for each second, we generate the ennemies
        int now = _clock.getElapsedTime().asSeconds();
        if (lastGeneration != now) {
            for (std::list<EnnemyToGenerate>::const_iterator i = enm[now].begin(), end = enm[now].end(); i != end; ++i) {
                space.add(EnnemyFactory::GetInstance().Create(*this, space, 1050, i->ypos, i->ennemyType));
            }
        }
        lastGeneration = now;
    }
}

void Game::endGame(std::string_view path)
{
    running = false;
    space.purge();
    music.stop();
    setupMusic("ressources/swtheme.wav");
    homeSprite.setTexture(ResourceManager<sf::Texture>::getResource(path.data()));
    recordBestScoreInFile();
    if (level == 4)
        restartGameLevel();
}

void Game::display(sf::RenderWindow& window) const
{
    if (textException)
        window.draw(*textException);
    else {
        if (!running && space.isEmpty())
            window.draw(homeSprite);
        window.draw(textScore);
        window.draw(textFPS);

        window.draw(imgLife.sprite);
        window.draw(textLife);
        window.draw(imgShield.sprite);
        window.draw(textShield);
        
        window.draw(textBestScore);
    }
}

void Game::initException(std::exception const& exception)
{
    textException = std::make_unique<sf::Text>();
    textException->setFont(font);
    textException->setString(exception.what());
    textException->setFillColor(sf::Color::Red);
}

void Game::setupMusic(std::string_view path, int volume, bool loop)
{
    music.openFromFile(path.data());
    music.play();
    music.setVolume(volume);
    music.setLoop(loop);
}

// FPS Management
void Game::updateFps()
{
    fpsCount++;
    if (fpsInterval.getElapsedTime().asMilliseconds() > 1000) {
        fps = fpsCount;
        fpsCount = 0;
        fpsInterval.restart().asMilliseconds();
    }
}

void Game::displayFps(int widthScreen)
{
    textFPS.setString("FPS : "s + std::to_string(fps));
    textFPS.move(widthScreen - (textFPS.getLocalBounds().width + 15), 0);
}

void Game::refreshFps()
{
    textFPS.setString("Fps: "s + std::to_string(fps));
}



// Score Management
void Game::addPoints(int points)
{
    score += points;
    if (score > bestScore)
        bestScore = score;
    refreshScore();
    refreshBestScore();
}

void Game::refreshScore()
{
    textScore.setString("Score : "s + std::to_string(score));
}

void Game::refreshBestScore()
{
    textBestScore.setString("Best score : "s + std::to_string(bestScore));
}

void Game::loadBestScoreFromFile()
{
    auto file = std::ifstream("bestScores.txt");
    if (file.is_open())
        file >> bestScore;
    else
        bestScore = 0;
    file.close();
}

void Game::recordBestScoreInFile()
{
    if (score == bestScore) {
        bestScore = score;
        auto file = std::ofstream{"bestScores.txt"};
        if (file.is_open())
            file << bestScore;
        else 
            throw std::runtime_error("Impossible. No bestScores file opened.");
        file.close();
    }
}

void Game::displayShipState(int widthScreen)
{
    // Generate img life
    imgLife.sprite.setTexture(ResourceManager<sf::Texture>::getResource("ressources/bonus_life.png"));
    imgLife.sprite.setScale(0.5f, 0.5f);
    imgLife.sprite.setPosition((widthScreen / 2), 0);

    // display nbLife
    displayValues(textLife, imgLife.sprite, shipLife);

    // Generate img Shield
    imgShield.sprite.setTexture(ResourceManager<sf::Texture>::getResource("ressources/bonus_shield.png"));
    imgShield.sprite.setScale(0.5f, 0.5f);
    imgShield.sprite.setPosition((widthScreen / 2) + imgLife.sprite.getLocalBounds().width, 0);

    // display nbShield
    displayValues(textShield, imgShield.sprite, shipShield);
}

void Game::displayValues(sf::Text &text, sf::Sprite &sprite, int value)
{
    text.move(sprite.getPosition().x + (sprite.getLocalBounds().width / 2) + 5, 0);
    text.setString(std::to_string(value));
}

void Game::refreshLife(int life)
{
    textLife.setString(std::to_string(life));
}

void Game::refreshShield(int shield)
{
    textShield.setString(std::to_string(shield));
}

void Game::setShipState(int life, int shield)
{
    shipLife = life;
    shipShield = shield;
    refreshLife(life);
    refreshShield(shield);
}

void Game::setPlayerPosition(Coordinate &p_coord)
{
    playerCoord = p_coord;
}

void Game::nextLevel(void)
{
    level += 1;
}

void Game::restartGameLevel(void)
{
    level = 1;
}

int Game::getLevel(void) const
{
    return level;
}