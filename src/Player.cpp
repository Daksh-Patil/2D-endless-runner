#include "Player.hpp"

Player::Player(Animation &idleAnim) {
  animator.setAnimation(idleAnim);
  sprite.setPosition(200, 100);
  sprite.setScale(0.28f, 0.28f);
  // origin at bottom center
  animator.update(0.f, sprite);
  sprite.setOrigin(sprite.getLocalBounds().width / 2.f,
                   sprite.getLocalBounds().height);
}

void Player::reset(sf::Vector2f pos) {
  sprite.setPosition(pos);
  velocityY = 0.f;
  isGrounded = false;
  isDead = false;
}

void Player::jump() {
  if (isGrounded && !isDead) {
    velocityY = JUMP_FORCE_CONST;
    isGrounded = false;
  }
}

void Player::setAnimationState(Animation &idle, Animation &walk,
                               Animation &jump) {
  if (isDead) {
    animator.setAnimation(idle);
  } else if (!isGrounded) {
    animator.setAnimation(jump);
  } else {
    animator.setAnimation(walk);
  }
}

void Player::update(float dt, Animation &idle, Animation &walk,
                    Animation &jump) {
  animator.update(dt, sprite);
  setAnimationState(idle, walk, jump);

  if (!isGrounded) {
    velocityY += GRAVITY_CONST * dt;
  }
  sprite.move(0, velocityY * dt);
}

sf::FloatRect Player::getBounds() const {
  sf::FloatRect bounds = sprite.getGlobalBounds();
  bounds.left += bounds.width * 0.3f; // tighter hitbox
  bounds.width *= 0.4f;
  return bounds;
}
