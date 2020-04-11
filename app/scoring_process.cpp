#include "scoring_process.hpp"

#include <iostream>

// --------------------------------------------------------------------------
ScoringProcess::ScoringProcess()
{
    initialize();
}

// --------------------------------------------------------------------------
ScoringProcess::~ScoringProcess()
{
    cleanup();
}

// --------------------------------------------------------------------------
void ScoringProcess::initialize()
{
    job.glsl_code = "";
    job.centers.resize(1024, sf::Vector2f(0.5,0.5));
    job.radius.resize(1024, 0.3);
    job.grayalpha.resize(1024, sf::Vector2f(0.5,0.5));
}

// --------------------------------------------------------------------------
void ScoringProcess::cleanup()
{
}

// --------------------------------------------------------------------------
void ScoringProcess::execute(const sf::Texture& refModel, const sf::Texture& previousState, std::vector<CandidateSolution>& candidates)
{
    // setup global parameters
    job.referenceTex = &refModel;
    job.previousState = &previousState;

    // setup indivual properties
    for(int i=0;i<(int)candidates.size();++i)
    {
        job.centers[i] = candidates[i].center;
        job.radius[i] = candidates[i].radius;
        job.grayalpha[i].x = candidates[i].grayscale;
        // job.grayalpha[i].y = candidates[i].alpha;
    }

    // render all candidates solution into one 4k textures
    const sf::Texture& tex_tiles = tilejob.computeTiles(sf::Vector2u(4096,4096), 32, 32, &job);
    // reduce each solutions to his mean value
    const sf::Texture& tex_means = tiled_mean.getMean(tex_tiles, 7);
    // resizing texture : 1 pixel for 1 solution
    const sf::Texture& tex_result = resizing.computeResizing(tex_means, sf::Vector2u(32,32));

    // download result
    errorImage = tex_result.copyToImage();

    // update scores
    for(int y=0;y<32;++y) for(int x=0;x<32;++x)
    {
        int index = y*32+x;
        sf::Color pixel = errorImage.getPixel(x,32-1-y);
        // candidates[index].score = pixel.r;
        candidates[index].score = pixel.r*65536 + pixel.g*256 + pixel.b;
    }
}

// --------------------------------------------------------------------------
const sf::Texture& ScoringProcess::getResult()
{
    return tilejob.getResultAsTexture();
}

// --------------------------------------------------------------------------
const sf::Texture& ScoringProcess::getScores()
{
    return resizing.getResultAsTexture();
}
