#ifndef TEXTURE_MIXING_HPP
#define TEXTURE_MIXING_HPP

#include <SFML/Graphics.hpp>

// --------------------------------------------------------------------------
// Helper class - give functions for texture mixing
class TextureMixing
{
public:
    TextureMixing();
    virtual ~TextureMixing();

    void initialize();
    void cleanup();

    // computes texture of error distances between two textures
    const sf::Texture& computeDistance( const sf::Texture& texture1, const sf::Texture& texture2 );

    // get result texture
    const sf::Texture& getResultAsTexture();

protected:
    // resize render target
    void resizeRenderTarget(const sf::Vector2u& size);

    sf::RenderTexture m_target;         // render target
    sf::VertexBuffer m_vertexBuffer;    // target area
    sf::Shader m_distanceShader;        // shader for error distances
};

#endif // TEXTURE_MIXING_HPP
