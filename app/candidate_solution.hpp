#ifndef CANDIDATE_SOLUTION_HPP
#define CANDIDATE_SOLUTION_HPP

#include <SFML/Graphics.hpp>

#include "../genetic_routine/genetic_individual.hpp"

// --------------------------------------------------------------------------
// Define an genetic individual with circle properties
struct CandidateSolution : public GeneticIndividual
{
    // center of the circle
    sf::Vector2f center;
    
    // radius of the circle
    float radius;
    
    // level of grayscale
    float grayscale;
    
    // unused
    // alpha blending of the circle
    // float alpha;
    
    // constructor
    CandidateSolution();
    
    // destructor
    virtual ~CandidateSolution();

    // give new random values and reset score
    void reset() override;

    // clamp all values to the limits
    void validate() override;

    // create a child individual from two parent individuals
    void crossover(const GeneticIndividual& s, GeneticIndividual& child) const override;

    // modify all values randomly under a maximum percentage of mutation
    void mutation(float mutlvl) override;
};

#endif // CANDIDATE_SOLUTION_HPP
 
