#include "texture_reduction.hpp"

#include <cmath>    // std::log
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
static const std::string s_glsl_mean = GLSL_CODE(

    vec3 encodeError(float e)
    {
        float X = 256.0;
        float Y = X*X;
        float Z = X*Y;

        // max 24b = 16 777 215
        e = e*Z;
        // e = e*65536.0;
        // e = 65536*r + 256*g + b

        vec3 v3;
        v3.z = mod(e,X);
        float t = (e-v3.z)/X;
        v3.y = mod(t,X);
        t = (e-v3.z);
        v3.x = t/Y - v3.y/X;

        return v3 / X;
    }

    float decodeError(vec3 v3)
    {
        float X = 256.0;
        float Y = X*X;
        float Z = X*Y;

        v3 = v3 * X;

        float e = Y*v3.r + X*v3.y + v3.z;
        return e / Z;
    }

    vec3 reduce(vec3 p00, vec3 p01, vec3 p10, vec3 p11)
    {
        float e00 = decodeError(p00);
        float e01 = decodeError(p01);
        float e10 = decodeError(p10);
        float e11 = decodeError(p11);

        float e = (e00+e01+e10+e11) * 0.25;
        return encodeError(e);

        // return (p00+p01+p10+p11) * 0.25;
    }
);

// --------------------------------------------------------------------------
static const std::string s_glsl_max = GLSL_CODE(
    vec3 reduce(vec3 p00, vec3 p01, vec3 p10, vec3 p11)
    {
        return max( max(p00,p01), max(p10,p11) );
    }
);

// --------------------------------------------------------------------------
static const std::string s_glsl_min = GLSL_CODE(
    vec3 reduce(vec3 p00, vec3 p01, vec3 p10, vec3 p11)
    {
        return min( min(p00,p01), min(p10,p11) );
    }
);

// --------------------------------------------------------------------------
static const std::string s_glsl_frag = GLSL_CODE(
    uniform sampler2D u_input;
    uniform int u_flip;
    uniform vec2 u_resolution;

    vec3 reduce(vec3 p00, vec3 p01, vec3 p10, vec3 p11);

    vec3 valueAt(vec2 uv) { return texture2D(u_input, uv).xyz; }

    void main()
    {
        vec2 uv = gl_TexCoord[0].xy;
        if(u_flip==1)uv.y = 1.0 - uv.y;

        vec2 output_res = u_resolution * 0.5;
        vec2 zo = vec2(0.0,1.0);

        vec2 tx_step = vec2(1.0) / u_resolution;
        vec2 o_cd = floor(uv * output_res) / output_res;

        vec3 p00 = valueAt(o_cd + tx_step*zo.xx ).xyz;
        vec3 p01 = valueAt(o_cd + tx_step*zo.xy ).xyz;
        vec3 p10 = valueAt(o_cd + tx_step*zo.yx ).xyz;
        vec3 p11 = valueAt(o_cd + tx_step*zo.yy ).xyz;

        vec3 reduced = reduce(p00,p01,p10,p11);
        gl_FragColor = vec4(reduced,1.0);
    }
);

// --------------------------------------------------------------------------
const int TextureReduction::MaxItCount = 12;


// --------------------------------------------------------------------------
TextureReduction::TextureReduction()
{
    initialize();
}

// --------------------------------------------------------------------------
TextureReduction::~TextureReduction()
{
    cleanup();
}

// --------------------------------------------------------------------------
void TextureReduction::initialize()
{
    m_vertexBuffer = sf::VertexBuffer(sf::Quads, sf::VertexBuffer::Static);
    m_vertexBuffer.create(4);

    if (!m_meanShader.loadFromMemory(s_glsl_vertex, s_glsl_frag + s_glsl_mean))
    {
        std::cout << "err with mean shader..." << std::endl;
    }
    if (!m_maxShader.loadFromMemory(s_glsl_vertex, s_glsl_frag + s_glsl_max))
    {
        std::cout << "err with min shader..." << std::endl;
    }
    if (!m_minShader.loadFromMemory(s_glsl_vertex, s_glsl_frag + s_glsl_min))
    {
        std::cout << "err with max shader..." << std::endl;
    }

    // resizeRenderTarget(sf::Vector2u(128,128));
    m_resultIndex = 0;
}

// --------------------------------------------------------------------------
void TextureReduction::cleanup()
{
}

// --------------------------------------------------------------------------
void TextureReduction::renderNext(int targetIndex, const sf::Texture& input, const sf::Vector2f& resolution, sf::Shader& shader)
{
    bool flip = true;

    shader.setUniform("u_input", input);
    shader.setUniform("u_flip", flip?1:0);
    shader.setUniform("u_resolution", resolution);
    m_targets[targetIndex].clear();
    m_targets[targetIndex].draw(m_vertexBuffer, &shader);
}

// --------------------------------------------------------------------------
void TextureReduction::resizeRenderTarget(const sf::Vector2u& size)
{
    for(auto& t : m_targets) t.create(size.x,size.y);


    sf::Vertex vertices[] =
    {
        sf::Vertex(sf::Vector2f(     0,      0), sf::Color::White, sf::Vector2f(0,0)),
        sf::Vertex(sf::Vector2f(     0, size.y), sf::Color::White, sf::Vector2f(0,1)),
        sf::Vertex(sf::Vector2f(size.x, size.y), sf::Color::White, sf::Vector2f(1,1)),
        sf::Vertex(sf::Vector2f(size.x,      0), sf::Color::White, sf::Vector2f(1,0))
    };
    m_vertexBuffer.update(vertices);


    const int dim = std::min(size.x,size.y);
    m_iterationCount = std::log(dim) / std::log(2);
    // something, iteration_count is less than the expected value
    if( std::pow(2, m_iterationCount) < dim ) m_iterationCount++;

    m_iterationCount = std::min(MaxItCount, m_iterationCount);

    // std::cout << "ite count = " << m_iterationCount << std::endl;
}

// --------------------------------------------------------------------------
void TextureReduction::compute(const sf::Texture& input, sf::Shader& shader, int iteration_count)
{
    sf::Vector2u currSize = m_targets[0].getSize();
    sf::Vector2u size = input.getSize();
    if(currSize != size)
    {
        resizeRenderTarget(size);
        currSize = size;
    }

    if(iteration_count != MaxItCount) m_iterationCount = std::min(MaxItCount, iteration_count);

    int curr_target = 0, prev_target = 1;
    sf::Vector2f resolution(currSize.x,currSize.y);

    if(m_iterationCount > 0) renderNext(curr_target, input, resolution, shader);
    for(int i=1; i<m_iterationCount; ++i)
    {
        std::swap(curr_target,prev_target);

        resolution *= 0.5f;
        const sf::Texture& tex_used = m_targets[prev_target].getTexture();
        renderNext(curr_target, tex_used, resolution, shader);
    }

    m_resultIndex = curr_target;
}

// --------------------------------------------------------------------------
const sf::Texture& TextureReduction::getMean(const sf::Texture &texture, int iteration_count)
{
    compute(texture, m_meanShader,iteration_count);
    return m_targets[m_resultIndex].getTexture();
}

// --------------------------------------------------------------------------
const sf::Texture& TextureReduction::getMax( const sf::Texture& texture, int iteration_count )
{
    compute(texture, m_maxShader,iteration_count);
    return m_targets[m_resultIndex].getTexture();
}

// --------------------------------------------------------------------------
const sf::Texture& TextureReduction::getMin( const sf::Texture& texture, int iteration_count )
{
    compute(texture, m_minShader,iteration_count);
    return m_targets[m_resultIndex].getTexture();
}

// --------------------------------------------------------------------------
const sf::Texture& TextureReduction::getResultAsTexture()
{
    return m_targets[m_resultIndex].getTexture();
}

// --------------------------------------------------------------------------
sf::Color TextureReduction::copyResultAsColor()
{
    const sf::Texture& texture = m_targets[m_resultIndex].getTexture();
    sf::Image copy = texture.copyToImage();
    return copy.getPixel(0,0);
}
