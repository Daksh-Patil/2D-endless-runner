#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Animation.hpp"
#include "Constants.hpp"
#include <SFML/Graphics.hpp>


class Player {
public:
  sf::Sprite sprite;
  Animator animator;
  float velocityY = 0.f;
  bool isGrounded = false;
  bool isDead = false;

  Player(Animation &idleAnim);
  void reset(sf::Vector2f pos);
  void jump();
  void setAnimationState(Animation &idle, Animation &walk, Animation &jump);
  void update(float dt, Animation &idle, Animation &walk, Animation &jump);
  sf::FloatRect getBounds() const;
};

#endif // PLAYER_HPP
