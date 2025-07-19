// include/ResourceManager.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>

/// Singleton to load & cache SFML textures exactly once.
class ResourceManager {
public:
    static ResourceManager& instance();

    /// Loads texture from disk and stores it under `id`. Throws on failure.
    void loadTexture(const std::string& id, const std::string& filename);

    /// Returns a previously loaded texture. Throws if `id` not found.
    const sf::Texture& getTexture(const std::string& id) const;

private:
    ResourceManager() = default;
    ~ResourceManager() = default;
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    std::unordered_map<std::string, sf::Texture> textures_;
};
