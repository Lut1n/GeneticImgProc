#include "texture_tiles_job.hpp"

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
static const std::string s_glsl_frag = GLSL_CODE(
    uniform sampler2D u_referenceTex;
    uniform sampler2D u_previousState;
    uniform vec2 u_size;
    uniform vec2 u_center[256];
    uniform float u_radius[256];
    uniform vec2 u_grayalpha[256];
    // uniform int u_count;

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

    vec3 error(vec2 uv, vec3 c)
    {
        vec3 ref_c = texture2D(u_referenceTex, uv).xyz;
        float error = distance(c, ref_c);

        return encodeError(error);
        // return vec3(error);
    }

    int getIndex(vec2 idx2d)
    {
        // row major
        int row_id = int(idx2d.y * u_size.x + idx2d.x);
        return row_id;
    }

    void main()
    {
        vec2 abs_uv = gl_TexCoord[0].xy;
        abs_uv.y = 1.0 - abs_uv.y;

        vec2 tilesize = vec2(1.0) / u_size;
        vec2 idx2d = floor(abs_uv * u_size);
        vec2  o = idx2d  * tilesize;
        vec2 tile_uv = (abs_uv-o) / tilesize;

        int index = getIndex(idx2d);

        float r = u_radius[index] * 0.5;
        vec2 c = u_center[index];
        vec2 grayalpha = u_grayalpha[index];

        float cd = circle(tile_uv,c,r);

        vec3 mixed = blend(tile_uv, cd, grayalpha);
        mixed = clamp(mixed,0.0,1.0);

        vec3 err = error(tile_uv, mixed);
        err = clamp(err,0.0,1.0);

        gl_FragColor = vec4(err, 1.0);
    }
);

// --------------------------------------------------------------------------
TextureTilesJob::TextureTilesJob()
{
    initialize();
}

// --------------------------------------------------------------------------
TextureTilesJob::~TextureTilesJob()
{
    cleanup();
}

// --------------------------------------------------------------------------
void TextureTilesJob::initialize()
{
    m_vertexBuffer = sf::VertexBuffer(sf::Quads, sf::VertexBuffer::Static);
    m_vertexBuffer.create(4);

    // resizeRenderTarget(sf::Vector2u(128,128));
}

// --------------------------------------------------------------------------
void TextureTilesJob::cleanup()
{
}

// --------------------------------------------------------------------------
void TextureTilesJob::resizeRenderTarget(const sf::Vector2u& size)
{
    m_target.create(size.x,size.y);
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
void TextureTilesJob::updateJob(const Job* job)
{
    m_lastJob = job;

    if (!m_tilesJobShader.loadFromMemory(s_glsl_vertex, s_glsl_frag + m_lastJob->glsl_code))
    {
        std::cout << "err with tilesJob shader..." << std::endl;
    }
}

// --------------------------------------------------------------------------
const sf::Texture& TextureTilesJob::computeTiles( const sf::Vector2u& size, int rowSize, int colSize, const Job* job)
{
    sf::Vector2u currSize = m_target.getSize();
    if(currSize != size)
    {
        resizeRenderTarget(size);
        currSize = size;
    }

    // because of the opengl shader limits on uniforms, we need to divise the rendering into multiple sub-rendering
    int meta_rowsize = 1;
    int meta_colsize = 1;
    int count = rowSize*colSize;
    while(count > 256)
    {
        rowSize *= 0.5;
        colSize *= 0.5;
        ++meta_rowsize;
        ++meta_colsize;
        count = rowSize*colSize;
    }

    // in case of multiple sub-rendering, redefine the size of the draw area
    if(meta_rowsize > 1)
    {
        m_rectSize = sf::Vector2u( size.x/meta_rowsize, size.y/meta_colsize );
        sf::Vertex vertices[] =
        {
            sf::Vertex(sf::Vector2f(           0,            0), sf::Color::White, sf::Vector2f(0,0)),
            sf::Vertex(sf::Vector2f(           0, m_rectSize.y), sf::Color::White, sf::Vector2f(0,1)),
            sf::Vertex(sf::Vector2f(m_rectSize.x, m_rectSize.y), sf::Color::White, sf::Vector2f(1,1)),
            sf::Vertex(sf::Vector2f(m_rectSize.x,            0), sf::Color::White, sf::Vector2f(1,0))
        };
        m_vertexBuffer.update(vertices);
    }

    if(m_lastJob != job) updateJob(job);


    sf::Vector2f tilecount(rowSize,colSize);
    m_tilesJobShader.setUniform("u_size", tilecount);


    // rendering
    m_target.clear();
    for(int c=0;c<meta_colsize;++c) for(int r=0;r<meta_rowsize;++r)
    {
        int meta_index = c*meta_rowsize + r;
        int offset = meta_index * count;

        job->prepareShaderUniforms(m_tilesJobShader, offset, count);

        m_states.shader = &m_tilesJobShader;
        m_states.transform = sf::Transform::Identity;
        m_states.transform.translate(r*m_rectSize.x, c*m_rectSize.y);

        m_target.draw(m_vertexBuffer, m_states);
    }

    return m_target.getTexture();
}

// --------------------------------------------------------------------------
const sf::Texture& TextureTilesJob::getResultAsTexture()
{
    return m_target.getTexture();
}





// --------------------------------------------------------------------------
CircleJob::CircleJob()
{
}

// --------------------------------------------------------------------------
void CircleJob::prepareShaderUniforms(sf::Shader& shader, int offset, int count) const
{
    if(count==-1) count=radius.size();
    std::uint32_t end = offset + count;

    if(end>centers.size() || end>radius.size() || end>grayalpha.size())
    {
        std::cout << "error with job offset" << std::endl;
        return;
    }

    shader.setUniform("u_referenceTex", *referenceTex);
    shader.setUniform("u_previousState", *previousState);
    shader.setUniformArray("u_center", centers.data()+offset, count);
    shader.setUniformArray("u_radius", radius.data()+offset, count);
    shader.setUniformArray("u_grayalpha", grayalpha.data()+offset, count);
}
