#include "genetic_individual.hpp"

// --------------------------------------------------------------------------
// constructor
GeneticIndividual::GeneticIndividual()
    : score(0)
{
}

// --------------------------------------------------------------------------
// destructor
GeneticIndividual::~GeneticIndividual()
{
}

// --------------------------------------------------------------------------
// compare two individuals
bool GeneticIndividual::operator<(const GeneticIndividual& other) const
{
    return score < other.score;
}
