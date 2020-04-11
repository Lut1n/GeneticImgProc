#ifndef TEXTURE_TILES_JOB_HPP
#define TEXTURE_TILES_JOB_HPP

#include <SFML/Graphics.hpp>

// --------------------------------------------------------------------------
// Helper class - give functions for doing job on texture tiles
class TextureTilesJob
{
public:

    // job structure
    struct Job
    {
        std::string glsl_code; // not used

        // must be implemented for setting shader uniforms
        virtual void prepareShaderUniforms(sf::Shader& shader, int offset=0, int count=-1) const = 0;
    };

    TextureTilesJob();
    virtual ~TextureTilesJob();

    void initialize();
    void cleanup();

    // execute a job on each tiles in the texture
    const sf::Texture& computeTiles( const sf::Vector2u& size, int rowSize, int colSize, const Job* job);

    // get result texture
    const sf::Texture& getResultAsTexture();

protected:

    // resize render target
    void resizeRenderTarget(const sf::Vector2u& size);

    // load shader from a given job
    void updateJob(const Job* job);

    sf::RenderTexture m_target;         // render target
    sf::VertexBuffer m_vertexBuffer;    // target area
    sf::Shader m_tilesJobShader;        // shader associated to a job
    sf::RenderStates m_states;          // render state
    sf::Vector2u m_rectSize;            // target size
    const Job* m_lastJob;               // job to use
};


// --------------------------------------------------------------------------
// Default job for this application
struct CircleJob : public TextureTilesJob::Job
{
    CircleJob();

    // setup shader uniforms
    void prepareShaderUniforms(sf::Shader& shader, int offset=0, int count=-1) const override;

    const sf::Texture* referenceTex;        // reference model texture
    const sf::Texture* previousState;       // previous state

    std::vector<sf::Vector2f> centers;      // centers of candidate circles
    std::vector<float> radius;              // radius of candidate circles
    std::vector<sf::Vector2f> grayalpha;    // grayscale value of candidate circles
};

#endif // TEXTURE_TILES_JOB_HPP
