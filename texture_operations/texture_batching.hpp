#ifndef TEXTURE_BATCHING_HPP
#define TEXTURE_BATCHING_HPP

#include <SFML/Graphics.hpp>

// --------------------------------------------------------------------------
// Helper class - give functions for textures alignment and tiling
class TextureBatching
{
public:

    TextureBatching();
    virtual ~TextureBatching();

    void initialize();
    void cleanup();

    // tiles a set of textures into one texture
    const sf::Texture& batch( const std::vector<const sf::Texture*>& textures, int rowSize );

    // create a texture with first pixel of each given texture
    const sf::Texture& batchFirstPixels( const std::vector<const sf::Texture*>& textures, int rowSize );

    // repeat a texture in a big texture
    const sf::Texture& repeat( const sf::Texture& texture, int rowSize, int colSize = 1 );

    // get result texture
    const sf::Texture& getResultAsTexture();

protected:

    // resize result texture
    void resizeRenderTarget(const sf::Vector2u& size);

    sf::RenderTexture m_target;     // target render texture
    sf::Sprite m_sprite;            // used sprite
};

#endif // TEXTURE_BATCHING_HPP
