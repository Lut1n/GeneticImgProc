#ifndef GENETIC_POPULATION_HPP
#define GENETIC_POPULATION_HPP

#include <vector>
#include <algorithm>

#include "randomizer.hpp"

// --------------------------------------------------------------------------
// Define a population of individuals
template<typename IndividualTy>
struct GeneticPopulation
{
public:
    GeneticPopulation(int size);

    // apply a genesis of the population
    void initialize();

    // apply a selection (sort individuals by score value)
    // Individuals must have been scored
    // Second half of the population is considered dead
    void selection();

    // Pass to the next generation of the population
    // Individuals need to be scored and selected ( see selection() )
    // Procedural approach :
    // - hybridation : apply a cross-over on each dead individual with 2 alive to create a new individual
    // - mutation : randomly modify properties value of individuals, according to a mutation rate
    void nextGen(double mut_rate);
    
    // get best individual of the population
    IndividualTy& best();
    
    // get individual at a given index
    IndividualTy& at(int i);

    // get all individuals
    std::vector<IndividualTy>& getIndividuals();
    
protected:
    // find a potential individual parent
    unsigned int findParent();
    
    // list of individual
    std::vector<IndividualTy> individuals;
};

// --------------------------------------------------------------------------
template<typename IndividualTy>
GeneticPopulation<IndividualTy>::GeneticPopulation(int size)
{
    individuals.resize(size);
}

// --------------------------------------------------------------------------
template<typename IndividualTy>
void GeneticPopulation<IndividualTy>::initialize()
{
    for(auto ind : individuals)
    {
        ind.reset();
        // ind.validate();
    }
}

// --------------------------------------------------------------------------
template<typename IndividualTy>
unsigned int GeneticPopulation<IndividualTy>::findParent()
{
    int hsize = individuals.size()/2;
    unsigned int res = RND::zo() * hsize;
    return res;
}

// --------------------------------------------------------------------------
template<typename IndividualTy>
void GeneticPopulation<IndividualTy>::nextGen(double mut)
{
    auto mid = individuals.begin() + individuals.size()/2;
    auto end = individuals.end();

    // hybridation
    for(auto it = mid; it!= end; it++)
    {
        // find two parent
        unsigned int p1 = findParent();
        unsigned int p2 = findParent();
        while(p2 == p1) p2 = findParent();

        individuals[p1].crossover(individuals[p2],*it);
        it->score = 0.0;
    }

    // mutation
    for(auto ind : individuals)
    {
        ind.mutation(mut);
        ind.validate();
    }
}

// --------------------------------------------------------------------------
template<typename IndividualTy>
void GeneticPopulation<IndividualTy>::selection()
{
    std::sort(individuals.begin(),individuals.end());
}

// --------------------------------------------------------------------------
template<typename IndividualTy>
IndividualTy& GeneticPopulation<IndividualTy>::best()
{
    return individuals[0];
}

// --------------------------------------------------------------------------
template<typename IndividualTy>
IndividualTy& GeneticPopulation<IndividualTy>::at(int i)
{
    return individuals[i];
}

// --------------------------------------------------------------------------
template<typename IndividualTy>
std::vector<IndividualTy>& GeneticPopulation<IndividualTy>::getIndividuals()
{
    return individuals;
}

#endif // GENETIC_POPULATION_HPP
