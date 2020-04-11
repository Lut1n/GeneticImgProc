#ifndef APP_CONTEXT_HPP
#define APP_CONTEXT_HPP

#include <SFML/Graphics.hpp>

// --------------------------------------------------------------------------
// Define the SFML context of the application
class AppContext
{
public:
    AppContext(int w=1024, int h=256);
    virtual ~AppContext();

    // helper operator that check window state, clear, update and  display
    // to be used as a condition of a while loop around draw calls :
    //      while(context()){draw(...);}
    bool operator()();

    // draw a texture in the window
    void draw( const sf::Texture& texture, sf::Vector2f p = sf::Vector2f(0.0,0.0), float zoom = 1.0 );


protected:
    // True if the SFML window is open
    bool isOpen();

    // poll SFML event. Listens close button event
    void update();

    sf::RenderWindow* window;   // SFML window
    sf::Sprite sprite;          // sprite for drawing
};

#endif // APP_CONTEXT_HPP
 
