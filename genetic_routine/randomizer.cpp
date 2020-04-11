#include "randomizer.hpp"

#include <cstdlib> // stdlib for rand

// --------------------------------------------------------------------------
Randomizer* Randomizer::s_instance = nullptr;

// --------------------------------------------------------------------------
Randomizer::Randomizer()
{
    std::srand(666666);
}

// --------------------------------------------------------------------------
double Randomizer::gen()
{
    return std::rand()/(double)RAND_MAX;
}

// --------------------------------------------------------------------------
Randomizer* Randomizer::instance()
{
    if(s_instance==nullptr) s_instance = new Randomizer();
    return s_instance;
}

// --------------------------------------------------------------------------
double Randomizer::zo()
{
    return instance()->gen();
}

// --------------------------------------------------------------------------
double Randomizer::mp()
{
    return zo()*2.0-1.0;
}
