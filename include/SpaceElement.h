#ifndef SPACEELEMENT_H
#define SPACEELEMENT_H

#include <SFML/Graphics.hpp>
#include "Vecteur.h"
#include "Coordinate.h"
#include "ResourceManager.h"
#include "Resource.h"

#include <iostream>

enum class ElementType {PLAYER, BULLET, OTHER, BONUS, WEAPON, ENNEMY, ENNEMYBULLET};

class SpaceElement: public Resource
{
    public:
        explicit SpaceElement(std::string_view const& pathImage);
        SpaceElement(SpaceElement const& other) = delete;
        virtual ~SpaceElement() = default;

        float getRadius() const;
        void actualize(float time);
        void operator=(SpaceElement const& other) = delete;
        void screenLimit(void);
        void destructOutOfScreen(void);
        bool isBossFullInScreen(void);

        virtual void display(sf::RenderWindow& window) const;
        virtual void crashTest(SpaceElement& other) = 0;

        static inline bool isDestruct(std::unique_ptr<SpaceElement>& element) {return element->destruct;};

        Coordinate position{};
        ElementType type{ElementType::OTHER};

    protected:
        bool destruct{false};
        float ACCELERATION{400.f};
        float marge{50.f};

        Resource resource{};
        Vecteur speed{0.f, 0.f};

        virtual void update(float time) = 0;
        void changeSprite(std::string_view const& pathImage);
};

#endif // SPACEELEMENT_H
