#include "Background.hpp"
#include <cmath>

void Background::load(const std::string &folder,
                      const std::vector<std::string> &filenames,
                      const std::vector<float> &speeds) {
  for (size_t i = 0; i < filenames.size(); ++i) {
    auto layer = std::make_unique<Layer>();
    if (layer->texture.loadFromFile(folder + "/" + filenames[i])) {
      layer->texture.setRepeated(true);
      layer->texture.setSmooth(true);
      layer->sprite1.setTexture(layer->texture);
      layer->sprite2.setTexture(layer->texture);
      float scale = 720.f / layer->texture.getSize().y;
      layer->sprite1.setScale(scale, scale);
      layer->sprite2.setScale(scale, scale);
      layer->speedMultiplier = speeds[i];
      layers.push_back(std::move(layer));
    }
  }
}

void Background::update(float dt, float gameSpeed) {
  for (auto &layer : layers) {
    layer->offset += gameSpeed * layer->speedMultiplier * dt;
    float width = layer->texture.getSize().x * layer->sprite1.getScale().x;

    while (layer->offset >= width) {
      layer->offset -= width;
    }

    float x1 = std::floor(-layer->offset);
    float x2 = std::floor(-layer->offset + width) - 0.5f;

    layer->sprite1.setPosition(x1, 0);
    layer->sprite2.setPosition(x2, 0);
  }
}

void Background::draw(sf::RenderWindow &window) {
  for (auto &layer : layers) {
    window.draw(layer->sprite1);
    window.draw(layer->sprite2);
  }
}
