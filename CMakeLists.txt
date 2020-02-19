cmake_minimum_required(VERSION 3.0.0)
project(NonEuclideanSFML VERSION 0.1.0)

# include(CTest)
# enable_testing()


## DEPENDENCIES
set(SFML_DIR "/home/oopsvpwowlq/SFML-2.5.1-Compiled/lib/cmake/SFML")
find_package(SFML 2.5 COMPONENTS system window graphics audio REQUIRED)

## TARGETS
add_subdirectory(NonEuclidean)
target_include_directories(NonEuclideanSFMLSource PUBLIC
  ${SFML_INCLUDE_DIR}
)
target_compile_definitions(NonEuclideanSFMLSource PRIVATE SFML_STATIC)

add_executable(NonEuclideanSFML NonEuclidean/Main.cpp)

target_compile_definitions(NonEuclideanSFML PRIVATE SFML_STATIC)

target_link_libraries(NonEuclideanSFML
  NonEuclideanSFMLSource
  sfml-system
  sfml-window
  sfml-graphics
  sfml-audio
  GLEW
  GL
)

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
