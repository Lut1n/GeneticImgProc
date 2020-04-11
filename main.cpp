#include <iostream>
#include <fstream>

#include "texture_operations/texture_reduction.hpp"
#include "texture_operations/texture_mixing.hpp"
#include "texture_operations/texture_conversion.hpp"
#include "texture_operations/texture_batching.hpp"
#include "texture_operations/texture_tiles_job.hpp"

#include "genetic_routine/genetic_population.hpp"
#include "genetic_routine/genetic_individual.hpp"
#include "genetic_routine/randomizer.hpp"

#include "app/candidate_solution.hpp"
#include "app/scoring_process.hpp"
#include "app/app_context.hpp"
#include "app/state_rendering.hpp"

void print_help()
{
    std::cout << "- Help with GeneticImgProc" << std::endl;
    std::cout << "Syntax :" << std::endl;
    std::cout << "./GeneticImgProc.exe [-i <input_filename>] [-o <output_filename>] [-f <json|csv|tsv>] [-g] [-h]" << std::endl;
    std::cout << std::endl;
    std::cout << "Warning :" << std::endl;
    std::cout << "Current version only accepts input grayscale images of size 128x128" << std::endl;
    std::cout << std::endl;
    std::cout << "Options :" << std::endl;
    std::cout << "-i : input image filename. Default is \"input.png\"" << std::endl;
    std::cout << "-o : output text filename. Default is \"output.txt\"" << std::endl;
    std::cout << "-f : output text format. Possible value must be json, csv or tsv. Default is json" << std::endl;
    std::cout << "-g : Display for debugging. Add 4 bests errors values and average values of the 1024 individuals." << std::endl;
    std::cout << "-h : Print help" << std::endl;
}

void saveAsJson(const std::string& ofilename, const std::vector<CandidateSolution>& recording)
{
    std::ofstream os(ofilename);
    os << "[" << std::endl;
    for(auto it : recording)
    {
        os << "\t{\"x\":" << it.center.x << ", \"y\":" << it.center.y;
        os << ", \"r\":" << it.radius << ", \"g\":" << it.grayscale << "}," << std::endl;
    }
    os << "]" << std::endl;
}

void saveAsSeparatedValues(const std::string& ofilename, const std::vector<CandidateSolution>& recording, const std::string& sv)
{
    std::ofstream os(ofilename);
    for(auto it : recording)
    {
        os << it.center.x << sv << it.center.y << sv << it.radius << sv << it.grayscale << sv << std::endl;
    }
}

int main(int argc, char* argv[])
{
    // arguments
    std::vector<std::string> args;
    if(argc>1) args = std::vector<std::string>(argv+1, argv+argc);

    // argument parsing variables
    std::vector<std::string> ext({"json","csv","tsv"});
    std::string filename_ref = "input.png",  ofilename = "output.txt";
    int w = 512, h = 256;
    bool debug_img = false;
    int ext_index = 0;


    // arguments interpretation
    for(auto it=args.begin();it!=args.end();++it)
    {
        if(*it=="-i" && ++it!=args.end()) filename_ref = *it;
        if(*it=="-o" && ++it!=args.end()) ofilename = *it;
        if(*it=="-f" && ++it!=args.end()){ auto fnd=std::find(ext.begin(),ext.end(),*it); if(fnd!=ext.end())ext_index=(fnd-ext.begin()); }
        if(*it=="-g"){ debug_img=true; w=1024; }
        if(*it=="-h"){ print_help(); return 0; }
    }

    // App stuff
    AppContext context(w,h);                        // context
    sf::Texture ref_model;                          // reference model texture
    ScoringProcess scoring;                         // scoring procedure
    StateRendering stateRendering;                  // result renderer
    GeneticPopulation<CandidateSolution> pop(1024); // genetic population
    std::vector<CandidateSolution> recording;       // result recording for output
    int iteration=0, max_it=300, lastScore=-1;      // incremental variables

    // reference model loading
    ref_model.loadFromFile(filename_ref);

    // initial scoring
    scoring.execute(ref_model, stateRendering.getResult(), pop.getIndividuals());
    // selection (sort by score value)
    pop.selection();
    std::cout << "generation #" << recording.size() << " [";

    while(context())
    {
        CandidateSolution& solution = pop.best();

        if(++iteration>=max_it)
        {
            std::cout << "] finished" << std::endl;
            if(lastScore==-1 || solution.score<lastScore)
            {
                // update state and record
                stateRendering.renderCandidate(solution);
                recording.push_back(solution);
                lastScore = solution.score;

                std::cout << "> new score =" << solution.score << std::endl;
            }
            else
            {
                std::cout << "bad solution. retry" << std::endl;
            }

            // new genesis of the population
            pop = GeneticPopulation<CandidateSolution>(1024);
            std::cout << "generation #" << recording.size() << " [";
            iteration = 0;
        }
        else
        {
            int progress_size = 20;
            if(iteration % (max_it/progress_size) == 0) {std::cout << "|"; std::cout.flush();}
            // next generation of the population
            pop.nextGen(0.01);
        }

        // give a score to candidate solutions
        scoring.execute(ref_model, stateRendering.getResult(), pop.getIndividuals());
        // selection (sort by score value)
        pop.selection();

        // display graphic result
        context.draw(ref_model, sf::Vector2f(0.0,0.0), 2.0);
        context.draw(stateRendering.getResult(), sf::Vector2f(256,0.0), 2.0);
        if(debug_img)
        {
            // debug graphic info
            context.draw(scoring.getResult(), sf::Vector2f(512.0,0.0), 1.0);
            context.draw(scoring.getScores(), sf::Vector2f(768.0,0.0), 8.0);
        }
    }

    // output result
    if(ext_index == 0) saveAsJson(ofilename, recording);
    else if(ext_index == 1) saveAsSeparatedValues(ofilename, recording, ",");
    else if(ext_index == 2) saveAsSeparatedValues(ofilename, recording, "\t");

    return EXIT_SUCCESS;
}
