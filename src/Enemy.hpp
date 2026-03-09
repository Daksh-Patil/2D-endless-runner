#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "Animation.hpp"
#include <SFML/Graphics.hpp>


class Enemy {
public:
  sf::Sprite sprite;
  Animator animator;
  bool active = true;

  Enemy(Animation &slimeAnim, sf::Vector2f position, float platformHeight);
  void update(float dt, float speed);
  sf::FloatRect getBounds() const;
};

#endif // ENEMY_HPP
