//src/ResourceManager.cpp
#include "ResourceManager.hpp"
#include <stdexcept>
#include <iostream>

ResourceManager& ResourceManager::instance() {
    static ResourceManager mgr;
    return mgr;
}

void ResourceManager::loadTexture(const std::string& id, const std::string& filename) {
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

bool ResourceManager::hasTexture(const std::string& id) const noexcept {
    return textures_.find(id) != textures_.end();
}

bool ResourceManager::loadSoundBuffer(const std::string& id, const std::string& filename) {
    sf::SoundBuffer buf;
    if (!buf.loadFromFile(filename)) {
        std::cerr << "Failed to load sound buffer: " << filename << std::endl;
        return false;
    }
    soundBuffers_.emplace(id, std::move(buf));
    return true;
}

const sf::SoundBuffer& ResourceManager::getSoundBuffer(const std::string& id) const {
    auto it = soundBuffers_.find(id);
    if (it == soundBuffers_.end())
        throw std::runtime_error("SoundBuffer not found: " + id);
    return it->second;
}

bool ResourceManager::hasSoundBuffer(const std::string& id) const noexcept {
    return soundBuffers_.find(id) != soundBuffers_.end();
}

void ResourceManager::createMusic(const std::string& id, const std::string& filename) {
    auto m = std::make_unique<sf::Music>();
    if (!m->openFromFile(filename))
        throw std::runtime_error("Failed to open music: " + filename);
    music_.emplace(id, std::move(m));
}

sf::Music& ResourceManager::getMusic(const std::string& id) {
    auto it = music_.find(id);
    if (it == music_.end())
        throw std::runtime_error("Music not found: " + id);
    return *(it->second);
}

bool ResourceManager::hasMusic(const std::string& id) const noexcept {
    return music_.find(id) != music_.end();
}

void ResourceManager::unloadAll() noexcept {
    textures_.clear();
    soundBuffers_.clear();
    music_.clear();
}