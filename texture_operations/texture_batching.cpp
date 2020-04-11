#include "texture_batching.hpp"

#include <iostream>
#include <cmath> // std::ceil

// --------------------------------------------------------------------------
TextureBatching::TextureBatching()
{
    initialize();
}

// --------------------------------------------------------------------------
TextureBatching::~TextureBatching()
{
    cleanup();
}

// --------------------------------------------------------------------------
void TextureBatching::initialize()
{
}

// --------------------------------------------------------------------------
void TextureBatching::cleanup()
{
}

// --------------------------------------------------------------------------
void TextureBatching::resizeRenderTarget(const sf::Vector2u& size)
{
    m_target.create(size.x,size.y);
}

// --------------------------------------------------------------------------
const sf::Texture& TextureBatching::batch( const std::vector<const sf::Texture*>& textures, int rowSize )
{
    int count = textures.size();
    if(count==0.0) return m_target.getTexture();

    int colSize = std::ceil(count / float(rowSize));
    // std::cout << "colSize = " << colSize << std::endl;

    sf::Vector2u currSize = m_target.getSize();
    sf::Vector2u tex_size = textures[0]->getSize();
    sf::Vector2u size(tex_size.x*rowSize, tex_size.y*colSize);
    if(currSize != size)
    {
        resizeRenderTarget(size);
        currSize = size;
    }

    auto it = textures.begin();
    // row major
    for(int c=0;c<colSize;++c) for(int r=0;r<rowSize;++r)
    {
        if(it == textures.end()) break;
        const sf::Texture* texture = *it; ++it;

        m_sprite.setTexture(*texture);
        m_sprite.setPosition(r*tex_size.x, c*tex_size.y);
        m_target.draw(m_sprite);
    }

    return m_target.getTexture();
}

// --------------------------------------------------------------------------
const sf::Texture& TextureBatching::batchFirstPixels( const std::vector<const sf::Texture*>& textures, int rowSize )
{
    int count = textures.size();
    if(count==0.0) return m_target.getTexture();

    int colSize = std::ceil(count / float(rowSize));
    // std::cout << "colSize = " << colSize << std::endl;

    sf::Vector2u currSize = m_target.getSize();
    sf::Vector2u size(rowSize, colSize);
    if(currSize != size)
    {
        resizeRenderTarget(size);
        currSize = size;
    }

    auto it = textures.begin();
    // row major
    for(int c=0;c<colSize;++c) for(int r=0;r<rowSize;++r)
    {
        if(it == textures.end()) break;
        const sf::Texture* texture = *it; ++it;

        m_sprite.setTexture(*texture);
        m_sprite.setPosition(r,c);
        m_target.draw(m_sprite);
    }

    return m_target.getTexture();
}

// --------------------------------------------------------------------------
const sf::Texture& TextureBatching::repeat( const sf::Texture& texture, int rowSize, int colSize )
{
    sf::Vector2u currSize = m_target.getSize();
    sf::Vector2u tex_size = texture.getSize();
    sf::Vector2u size(tex_size.x*rowSize, tex_size.y*colSize);
    if(currSize != size)
    {
        resizeRenderTarget(size);
        currSize = size;
    }

    m_sprite.setTexture(texture, true);

    if(texture.isRepeated())
    {
        sf::IntRect rect(0,0,currSize.x,currSize.y);
        m_sprite.setTextureRect(rect);
        m_sprite.setPosition(0,0);
        m_target.draw(m_sprite);
    }
    else
    {
        // row major
        for(int c=0;c<colSize;++c) for(int r=0;r<rowSize;++r)
        {
            m_sprite.setPosition(r*tex_size.x, c*tex_size.y);
            m_target.draw(m_sprite);
        }
    }


    return m_target.getTexture();
}

// --------------------------------------------------------------------------
const sf::Texture& TextureBatching::getResultAsTexture()
{
    return m_target.getTexture();
}
