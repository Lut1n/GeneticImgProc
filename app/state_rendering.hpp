#ifndef STATE_RENDERING_HPP
#define STATE_RENDERING_HPP

#include <SFML/Graphics.hpp>

#include "candidate_solution.hpp"

// --------------------------------------------------------------------------
// Incremental renderer for one candidate solution
class StateRendering
{
public:
    StateRendering();
    virtual ~StateRendering();

    void initialize();
    void cleanup();

    // swap renderTexture and mix previous state with new circle
    void renderCandidate(const CandidateSolution& candidate);

    // get last rendered texture
    const sf::Texture& getResult();

protected:

    // resize target area
    void resizeRect(const sf::Vector2u& size);

    sf::RenderTexture m_targets[2];     // target renderTextures
    sf::VertexBuffer m_vertexBuffer;    // target area
    sf::Shader m_renderShader;          // used shader
    sf::RenderStates m_states;          // render state
    sf::Vector2u m_rectSize;            // area size
    int m_currTarget;                   // last rendered texture index
};

#endif // STATE_RENDERING_HPP
