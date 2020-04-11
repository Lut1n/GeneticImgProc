#ifndef SCORING_PROCESS_HPP
#define SCORING_PROCESS_HPP

#include <SFML/Graphics.hpp>

#include "candidate_solution.hpp"

#include "../texture_operations/texture_tiles_job.hpp"
#include "../texture_operations/texture_reduction.hpp"
#include "../texture_operations/texture_conversion.hpp"

// --------------------------------------------------------------------------
// Rendering stuff for scoring 1024 candidates in one time
class ScoringProcess
{
public:
    ScoringProcess();
    virtual ~ScoringProcess();

    void initialize();
    void cleanup();

    // do a render pass of 1024 tiles in one big 4k texture. use candidates with a previous state and a reference model.
    // The resulting 4k texture is reduced into 32x32 texture where each pixel is an average
    // 32x32 textures is download and interpreted to update candidate's score value.
    void execute(const sf::Texture& refModel, const sf::Texture& previousState, std::vector<CandidateSolution>& candidates);

    // get last rendered 4k texture
    const sf::Texture& getResult();

    // get last 32x32 texture of averages
    const sf::Texture& getScores();

protected:
    CircleJob job;                  // job with data for tiles computing
    TextureTilesJob tilejob;        // texture tiling procedure
    TextureReduction tiled_mean;    // texture reduction procedure
    TextureConversion resizing;     // texture resizing procedure
    sf::Image errorImage;           // downloaded image with scores
};

#endif // SCORING_PROCESS_HPP
 
