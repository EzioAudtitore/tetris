//include/ResourceManager.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <unordered_map>
#include <memory>

class ResourceManager {
public:
    static ResourceManager& instance();

    void loadTexture(const std::string& id, const std::string& filename);
    const sf::Texture& getTexture(const std::string& id) const;
    bool hasTexture(const std::string& id) const noexcept;

    bool loadSoundBuffer(const std::string& id, const std::string& filename);
    const sf::SoundBuffer& getSoundBuffer(const std::string& id) const;
    bool hasSoundBuffer(const std::string& id) const noexcept;

    void createMusic(const std::string& id, const std::string& filename);
    sf::Music& getMusic(const std::string& id);
    bool hasMusic(const std::string& id) const noexcept;

    void unloadAll() noexcept;

private:
    ResourceManager() = default;
    ~ResourceManager() = default;
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    std::unordered_map<std::string, sf::Texture> textures_;
    std::unordered_map<std::string, sf::SoundBuffer> soundBuffers_;
    std::unordered_map<std::string, std::unique_ptr<sf::Music>> music_;
};
