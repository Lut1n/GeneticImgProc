#include "candidate_solution.hpp"

#include "../genetic_routine/randomizer.hpp"

// --------------------------------------------------------------------------
float clamp(float f, float mn=0.0, float mx=1.0)
{
    return std::max(mn,std::min(mx,f));
}

// --------------------------------------------------------------------------
template<typename T>
T rndChose(const T& a, const T& b)
{
    return RND::zo()<0.5 ? a : b;
}

// --------------------------------------------------------------------------
CandidateSolution::CandidateSolution()
    : GeneticIndividual()
{
    reset();
}

// --------------------------------------------------------------------------
CandidateSolution::~CandidateSolution()
{
}

// --------------------------------------------------------------------------
void CandidateSolution::reset()
{
    center = sf::Vector2f(RND::zo(),RND::zo());
    radius = RND::zo();
    grayscale = RND::zo();
    // alpha = RND::zo();
    score = 0;
}

// --------------------------------------------------------------------------
void CandidateSolution::validate()
{
    center.x = clamp(center.x);
    center.y = clamp(center.y);
    radius = clamp(radius);
    grayscale = clamp(grayscale);
    // alpha = clamp(alpha);
}

// --------------------------------------------------------------------------
void CandidateSolution::crossover(const GeneticIndividual& s, GeneticIndividual& schild) const
{
    const CandidateSolution& c = static_cast<const CandidateSolution&>(s);
    CandidateSolution& child = static_cast<CandidateSolution&>(schild);

    child.center.x = rndChose(center.x, c.center.x);
    child.center.y = rndChose(center.y, c.center.y);
    child.radius = rndChose(radius, c.radius);
    child.grayscale = rndChose(grayscale, c.grayscale);
    // child.alpha = rndChose(alpha, c.alpha);
}

// --------------------------------------------------------------------------
void CandidateSolution::mutation(float mutlvl)
{
    center.x += RND::mp()*mutlvl;
    center.y += RND::mp()*mutlvl;
    radius += RND::mp()*mutlvl;
    grayscale += RND::mp()*mutlvl;
    // alpha += RND::mp()*mutlvl;
}
 
