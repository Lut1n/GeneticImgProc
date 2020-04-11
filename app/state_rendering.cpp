#include "state_rendering.hpp"

#include <cmath>    // std::log
#include <iostream>

#define GLSL_CODE( src ) #src

// --------------------------------------------------------------------------
static const std::string s_glsl_vertex = GLSL_CODE(
    varying vec4 vertex;
    void main()
    {
        vertex = gl_ModelViewProjectionMatrix * gl_Vertex;
        gl_Position = vertex;
        gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
    }
);

// --------------------------------------------------------------------------
static const std::string s_glsl_frag = GLSL_CODE(
    uniform sampler2D u_previousState;
    uniform vec2 u_center;
    uniform float u_radius;
    uniform vec2 u_grayalpha;

    float circle(vec2 uv, vec2 c, float r)
    {
        return distance(uv,c) / r;
    }

    vec3 blend(vec2 uv, float q, vec2 grayalpha)
    {
        vec3 prev_c = texture2D(u_previousState, uv).xyz;
        float alpha = q>1.0 ? 0.0 : 1.0-q; // grayalpha.y;
        return vec3( clamp(mix(prev_c.x,grayalpha.x, alpha),0.0,1.0) );
    }

    void main()
    {
        vec2 abs_uv = gl_TexCoord[0].xy;
        abs_uv.y = 1.0 - abs_uv.y;

        float r = u_radius * 0.5;
        vec2 c = u_center;
        vec2 grayalpha = u_grayalpha;

        float cd = circle(abs_uv,c,r);

        vec3 mixed = blend(abs_uv, cd, grayalpha);
        mixed = clamp(mixed,0.0,1.0);

        gl_FragColor = vec4(mixed, 1.0);
    }
);


// --------------------------------------------------------------------------
StateRendering::StateRendering()
{
    initialize();
}

// --------------------------------------------------------------------------
StateRendering::~StateRendering()
{
    cleanup();
}

// --------------------------------------------------------------------------
void StateRendering::initialize()
{
    m_vertexBuffer = sf::VertexBuffer(sf::Quads, sf::VertexBuffer::Static);
    m_vertexBuffer.create(4);
    m_currTarget = 0;

    if (!m_renderShader.loadFromMemory(s_glsl_vertex, s_glsl_frag))
    {
        std::cout << "err with render shader..." << std::endl;
    }

    resizeRect( sf::Vector2u(128,128) );
}

// --------------------------------------------------------------------------
void StateRendering::cleanup()
{
}

// --------------------------------------------------------------------------
void StateRendering::resizeRect(const sf::Vector2u &size)
{
    m_targets[0].create(size.x,size.y);
    m_targets[1].create(size.x,size.y);

    m_rectSize = size;

    sf::Vertex vertices[] =
    {
        sf::Vertex(sf::Vector2f(     0,      0), sf::Color::White, sf::Vector2f(0,0)),
        sf::Vertex(sf::Vector2f(     0, size.y), sf::Color::White, sf::Vector2f(0,1)),
        sf::Vertex(sf::Vector2f(size.x, size.y), sf::Color::White, sf::Vector2f(1,1)),
        sf::Vertex(sf::Vector2f(size.x,      0), sf::Color::White, sf::Vector2f(1,0))
    };
    m_vertexBuffer.update(vertices);
}

// --------------------------------------------------------------------------
void StateRendering::renderCandidate(const CandidateSolution& candidate)
{
    if(m_rectSize.x != 128) resizeRect(sf::Vector2u(128,128));

    // swap target
    int previousTarget = m_currTarget;
    m_currTarget = (m_currTarget+1) % 2;
    sf::Vector2f grayalpha;
    grayalpha.x = candidate.grayscale;
    // grayalpha.y = candidate.alpha;

    m_renderShader.setUniform("u_previousState", m_targets[previousTarget].getTexture());
    m_renderShader.setUniform("u_center", candidate.center);
    m_renderShader.setUniform("u_radius", candidate.radius);
    m_renderShader.setUniform("u_grayalpha", grayalpha);

    // m_states.blendMode = sf::BlendAlpha;
    m_states.shader = &m_renderShader;

    m_targets[m_currTarget].clear();
    m_targets[m_currTarget].draw(m_vertexBuffer, m_states);
}

// --------------------------------------------------------------------------
const sf::Texture& StateRendering::getResult()
{
    return m_targets[m_currTarget].getTexture();
}
