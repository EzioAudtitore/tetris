// src/ResourceManager.cpp
#include "ResourceManager.hpp"
#include <stdexcept>

ResourceManager& ResourceManager::instance() {
    static ResourceManager mgr;
    return mgr;
}

void ResourceManager::loadTexture(const std::string& id,
    const std::string& filename)
{
    sf::Texture tex;
    if (!tex.loadFromFile(filename))
        throw std::runtime_error("Failed to load texture: " + filename);
    textures_.emplace(id, std::move(tex));
}

const sf::Texture& ResourceManager::getTexture(const std::string& id) const {
    auto it = textures_.find(id);
    if (it == textures_.end())
        throw std::runtime_error("Texture not found: " + id);
    return it->second;
}
