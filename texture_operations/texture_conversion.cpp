#include "texture_conversion.hpp"

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
    uniform sampler2D u_input;
    uniform int u_flip;

    vec3 conversion(vec3 color);

    void main()
    {
        vec2 uv = gl_TexCoord[0].xy;
        if(u_flip==1)uv.y = 1.0 - uv.y;

        vec3 color = texture2D(u_input, uv).xyz;

        vec3 converted = conversion(color);
        gl_FragColor = vec4(converted,1.0);
    }
);

    // --------------------------------------------------------------------------
static const std::string s_glsl_grayscale = GLSL_CODE(
    vec3 conversion(vec3 f3)
    {
        // arithmetic mean
        float f1 = (f3.x+f3.y+f3.z) / 3.0;
        return vec3(f1);
    }
);

// --------------------------------------------------------------------------
static const std::string s_glsl_grayscale2 = GLSL_CODE(
    vec3 conversion(vec3 f3)
    {
        float f1 = length(f3);
        return vec3(f1);
    }
);

// --------------------------------------------------------------------------
static const std::string s_glsl_grayscale3 = GLSL_CODE(
    vec3 conversion(vec3 f3)
    {
        // quadratic mean
        float f1 = sqrt(dot(f3,f3) / 3);
        return vec3(f1);
    }
);

// --------------------------------------------------------------------------
static const std::string s_glsl_donothing = GLSL_CODE(
    vec3 conversion(vec3 f3)
    {
        return f3;
    }
);

// --------------------------------------------------------------------------
TextureConversion::TextureConversion()
{
    initialize();
}

// --------------------------------------------------------------------------
TextureConversion::~TextureConversion()
{
    cleanup();
}

// --------------------------------------------------------------------------
void TextureConversion::initialize()
{
    m_vertexBuffer = sf::VertexBuffer(sf::Quads, sf::VertexBuffer::Static);
    m_vertexBuffer.create(4);

    if (!m_grayscaleShader.loadFromMemory(s_glsl_vertex, s_glsl_frag + s_glsl_grayscale))
    {
        std::cout << "err with greyscale shader..." << std::endl;
    }
    if (!m_resizeShader.loadFromMemory(s_glsl_vertex, s_glsl_frag + s_glsl_donothing))
    {
        std::cout << "err with greyscale shader..." << std::endl;
    }

    // resizeRenderTarget(sf::Vector2u(128,128));
}

// --------------------------------------------------------------------------
void TextureConversion::cleanup()
{
}

// --------------------------------------------------------------------------
void TextureConversion::resizeRenderTarget(const sf::Vector2u& size)
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
const sf::Texture& TextureConversion::computeGrayscale(const sf::Texture &texture)
{
    sf::Vector2u currSize = m_target.getSize();
    sf::Vector2u size = texture.getSize();
    if(currSize != size)
    {
        resizeRenderTarget(size);
        currSize = size;
    }

    bool flip = true;

    m_grayscaleShader.setUniform("u_input", texture);
    m_grayscaleShader.setUniform("u_flip", flip?1:0);
    m_target.clear();
    m_target.draw(m_vertexBuffer, &m_grayscaleShader);

    return m_target.getTexture();
}

// --------------------------------------------------------------------------
const sf::Texture& TextureConversion::computeResizing( const sf::Texture& texture, const sf::Vector2u& newsize )
{
    sf::Vector2u currSize = m_target.getSize();
    if(currSize != newsize)
    {
        resizeRenderTarget(newsize);
        currSize = newsize;
    }

    bool flip = true;

    m_resizeShader.setUniform("u_input", texture);
    m_resizeShader.setUniform("u_flip", flip?1:0);
    m_target.clear();
    m_target.draw(m_vertexBuffer, &m_resizeShader);

    return m_target.getTexture();
}

// --------------------------------------------------------------------------
const sf::Texture& TextureConversion::getResultAsTexture()
{
    return m_target.getTexture();
}
