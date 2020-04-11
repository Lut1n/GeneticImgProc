#ifndef RANDOMIZER_HPP
#define RANDOMIZER_HPP

// --------------------------------------------------------------------------
// Helper class for random functions
class Randomizer
{
public:
    Randomizer();
    double gen();

    static Randomizer* instance();
    static double zo(); // Zero to One
    static double mp(); // Minus one to Plus one

private:
    static Randomizer* s_instance;
};

using RND = Randomizer;

#endif // RANDOMIZER_HPP
