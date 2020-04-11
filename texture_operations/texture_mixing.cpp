#include "texture_mixing.hpp"

#include <iostream>

// --------------------------------------------------------------------------
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
    uniform sampler2D u_input1;
    uniform sampler2D u_input2;
    uniform int u_flip;

    vec3 mixing(vec3 v1, vec3 v2);

    void main()
    {
        vec2 uv = gl_TexCoord[0].xy;
        if(u_flip==1)uv.y = 1.0 - uv.y;

        vec3 v1 = texture2D(u_input1, uv).xyz;
        vec3 v2 = texture2D(u_input2, uv).xyz;

        vec3 d = mixing(v1,v2);
        gl_FragColor = vec4(d,1.0);
    }
);

    // --------------------------------------------------------------------------
static const std::string s_glsl_dist = GLSL_CODE(
    vec3 mixing(vec3 v1, vec3 v2)
    {
        return vec3(distance(v1,v2));
    }
);

// --------------------------------------------------------------------------
TextureMixing::TextureMixing()
{
    initialize();
}

// --------------------------------------------------------------------------
TextureMixing::~TextureMixing()
{
    cleanup();
}

// --------------------------------------------------------------------------
void TextureMixing::initialize()
{
    m_vertexBuffer = sf::VertexBuffer(sf::Quads, sf::VertexBuffer::Static);
    m_vertexBuffer.create(4);

    if (!m_distanceShader.loadFromMemory(s_glsl_vertex, s_glsl_frag + s_glsl_dist))
    {
        std::cout << "err with distance shader..." << std::endl;
    }

    // resizeRenderTarget(sf::Vector2u(128,128));
}

void TextureMixing::cleanup()
{
}

// --------------------------------------------------------------------------
void TextureMixing::resizeRenderTarget(const sf::Vector2u& size)
{
    m_target.create(size.x,size.y);

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
const sf::Texture& TextureMixing::computeDistance(const sf::Texture &texture1, const sf::Texture &texture2)
{
    sf::Vector2u currSize = m_target.getSize();
    sf::Vector2u size = texture1.getSize(); // get max ?
    if(currSize != size)
    {
        resizeRenderTarget(size);
        currSize = size;
    }

    bool flip = true;

    m_distanceShader.setUniform("u_input1", texture1);
    m_distanceShader.setUniform("u_input2", texture2);
    m_distanceShader.setUniform("u_flip", flip?1:0);
    m_target.clear();
    m_target.draw(m_vertexBuffer, &m_distanceShader);

    return m_target.getTexture();
}

// --------------------------------------------------------------------------
const sf::Texture& TextureMixing::getResultAsTexture()
{
    return m_target.getTexture();
}

