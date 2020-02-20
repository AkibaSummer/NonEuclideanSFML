cmake_minimum_required(VERSION 3.0.0)
project(NonEuclideanSFML VERSION 0.1.0)

# include(CTest)
# enable_testing()


## DEPENDENCIES
# set(SFML_DIR "/home/oopsvpwowlq/SFML-2.5.1-Compiled/lib/cmake/SFML")
set(SFML_DIR "C:/env/SFML-2.5.1/lib/cmake/SFML")
find_package(SFML 2.5 COMPONENTS system window graphics audio REQUIRED)
set(GLEW_INCLUDE_DIRS "C:/env/glew-2.1.0/include")
set(GLEW_LIBRARIES "C:/env/glew-2.1.0/lib/Release/Win32")
set(SFML_INCLUDE_DIR "C:/env/SFML-2.5.1/include")

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

## TARGETS
add_subdirectory(NonEuclidean)
target_include_directories(NonEuclideanSFMLSource PUBLIC
  ${SFML_INCLUDE_DIR}
  ${GLEW_INCLUDE_DIRS}
)
target_compile_definitions(NonEuclideanSFMLSource PRIVATE SFML_STATIC)

add_executable(NonEuclideanSFML NonEuclidean/Main.cpp)

target_compile_definitions(NonEuclideanSFML PRIVATE SFML_STATIC)

link_directories("C:/env/glew-2.1.0/lib/Release/Win32")

target_link_libraries(NonEuclideanSFML
  NonEuclideanSFMLSource
  sfml-system
  sfml-window
  sfml-graphics
  sfml-audio
  opengl32
  "C:/env/glew-2.1.0/lib/Release/Win32/glew32.lib"
)

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
