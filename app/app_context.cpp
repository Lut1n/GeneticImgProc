#include "app_context.hpp"

// --------------------------------------------------------------------------
AppContext::AppContext(int w, int h)
{
    window = new sf::RenderWindow(
        sf::VideoMode(w,h)
        , "Genetic algorithm"
        /*, sf::Style::Default
        , sf::ContextSettings(24)*/);

    window->setFramerateLimit(30);
}

// --------------------------------------------------------------------------
AppContext::~AppContext()
{
    delete window;
}

// --------------------------------------------------------------------------
bool AppContext::isOpen()
{
    return window->isOpen();
}

// --------------------------------------------------------------------------
void AppContext::update()
{
    sf::Event event;
    while (window->pollEvent(event))
    {
        /*if(event.type == sf::Event::KeyPressed)
        {
            window->close();
        }*/
        if (event.type == sf::Event::Closed)
        {
            window->close();
        }
    }
}

// --------------------------------------------------------------------------
void AppContext::draw( const sf::Texture& texture, sf::Vector2f p, float zoom )
{
    sprite.setTexture(texture,true);
    sprite.setPosition(p);
    sprite.setScale(zoom,zoom);
    window->draw(sprite);
}

// --------------------------------------------------------------------------
bool AppContext::operator()()
{
    if( !isOpen() ) return false;
    // display previous rendering
    window->display();

    // poll events and check close request
    update();
    if( !isOpen() ) return false;

    // start new rendering
    window->clear();
    return true;
}
