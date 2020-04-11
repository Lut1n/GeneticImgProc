cmake_minimum_required(VERSION 3.5)

project(GeneticImgProc_Test)

set(SRCS
    main.cpp
    app/app_context.cpp
    app/candidate_solution.cpp
    app/scoring_process.cpp
    app/state_rendering.cpp

    genetic_routine/genetic_population.cpp
    genetic_routine/genetic_individual.cpp
    genetic_routine/randomizer.cpp

    texture_operations/texture_batching.cpp
    texture_operations/texture_conversion.cpp
    texture_operations/texture_mixing.cpp
    texture_operations/texture_tiles_job.cpp
    texture_operations/texture_reduction.cpp
    )

set(HEADERS
    app/app_context.hpp
    app/candidate_solution.hpp
    app/scoring_process.hpp
    app/state_rendering.hpp

    genetic_routine/genetic_population.hpp
    genetic_routine/genetic_individual.hpp
    genetic_routine/randomizer.hpp

    texture_operations/texture_batching.hpp
    texture_operations/texture_conversion.hpp
    texture_operations/texture_mixing.hpp
    texture_operations/texture_tiles_job.hpp
    texture_operations/texture_reduction.hpp
    )

add_executable(GeneticImgProc ${SRCS} ${HEADERS})

## If you want to link SFML statically
# set(SFML_STATIC_LIBRARIES TRUE)

## In most cases better set in the CMake cache
# set(SFML_DIR "<sfml root prefix>/lib/cmake/SFML")
set(SFML_DIR "C:/SFML-2.5.1/lib/cmake/SFML")

find_package(SFML 2.5 COMPONENTS graphics REQUIRED)

target_link_libraries(GeneticImgProc sfml-graphics)
