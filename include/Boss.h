#ifndef BOSS_H
#define BOSS_H

#include "Ennemy.h"

class Boss: public Ennemy
{
    public:
        explicit Boss(Game &p_game, Space& p_space, float x, float y);
        virtual void update(float time) override;
        virtual void crashReaction(SpaceElement& other) override;
    private:
        int direction = 1;
        sf::Color bossColor[3] = {
            sf::Color::Magenta,
            sf::Color::Cyan,
            sf::Color::Green
        };
};

#endif