#ifndef GENETIC_INDIVIDUAL_HPP
#define GENETIC_INDIVIDUAL_HPP

// --------------------------------------------------------------------------
// Base structure for genetic individual
struct GeneticIndividual
{
    // score value of the individual
    int score;
    
    // constructor
    GeneticIndividual();
    
    // destructor
    virtual ~GeneticIndividual();

    // compare two individuals
    bool operator<(const GeneticIndividual& other) const;

    // give new random values and reset score
    virtual void reset() = 0;
    
    // clamp all values to the limits
    virtual void validate() = 0;

    // create a child individual from two parent individuals
    virtual void crossover(const GeneticIndividual& s, GeneticIndividual& child) const = 0;

    // modify all values randomly under a maximum percentage of mutation
    virtual void mutation(float mutlvl) = 0;
};

#endif // GENETIC_INDIVIDUAL_HPP
