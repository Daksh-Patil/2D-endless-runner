#include "Enemy.hpp"

Enemy::Enemy(Animation &slimeAnim, sf::Vector2f position,
             float platformHeight) {
  animator.setAnimation(slimeAnim);
  sprite.setScale(0.18f, 0.18f);
  sprite.setPosition(position);

  // Setup proper origin so it sits correctly
  animator.update(0.f, sprite);
  // Hitbox offset downwards roughly 35% of platform height
  sprite.setOrigin(0, sprite.getLocalBounds().height); // bottom aligned
  sprite.setPosition(position.x, position.y + platformHeight * 0.35f);
}

void Enemy::update(float dt, float speed) {
  if (!active)
    return;
  animator.update(dt, sprite);
  sprite.move(-speed * dt, 0);
}

sf::FloatRect Enemy::getBounds() const {
  // give a slightly smaller, more forgiving hitbox
  sf::FloatRect bounds = sprite.getGlobalBounds();
  bounds.left += bounds.width * 0.2f;
  bounds.width *= 0.6f;
  bounds.top += bounds.height * 0.2f;
  bounds.height *= 0.6f;
  return bounds;
}
