#ifndef TEXTURE_REDUCTION_HPP
#define TEXTURE_REDUCTION_HPP

#include <SFML/Graphics.hpp>

// --------------------------------------------------------------------------
// Helper class - give functions for texture reduction
class TextureReduction
{
public:

    // max iteration count
    static const int MaxItCount;

    TextureReduction();
    virtual ~TextureReduction();

    void initialize();
    void cleanup();

    // reduces texture to its average
    const sf::Texture& getMean( const sf::Texture& texture, int iteration_count = MaxItCount );

    // reduce texture to its max value
    const sf::Texture& getMax( const sf::Texture& texture, int iteration_count = MaxItCount );

    // reduce texture to its min value
    const sf::Texture& getMin( const sf::Texture& texture, int iteration_count = MaxItCount );

    // get result texture
    const sf::Texture& getResultAsTexture();

    // download image from gpu and return pixel color value
    sf::Color copyResultAsColor();


protected:

    // resize render target
    void resizeRenderTarget(const sf::Vector2u& size);

    // compute a reduction by using a given shader
    void compute(const sf::Texture& input, sf::Shader& shader, int iteration_count = MaxItCount);

    // iterate reduction
    void renderNext(int targetIndex, const sf::Texture& input, const sf::Vector2f& resolution, sf::Shader& shader);

    sf::RenderTexture m_targets[2];                     // render targets (ping pong approach)
    sf::VertexBuffer m_vertexBuffer;                    // target area
    sf::Shader m_meanShader, m_maxShader, m_minShader;  // shaders for average, min and max
    int m_resultIndex;                                  // result target index
    int m_iterationCount;                               // iteration count
};

#endif // TEXTURE_REDUCTION_HPP
