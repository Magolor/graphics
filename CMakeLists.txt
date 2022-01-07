CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(FINAL CXX)

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -w -O0 -g")

IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
ENDIF()

ADD_SUBDIRECTORY(deps/vecmath)
ADD_SUBDIRECTORY(deps/tinyobjloader)

SET(FINAL_SOURCES
        src/main.cpp
        src/material.cpp
        src/mesh.cpp
        src/scene_parser.cpp
        src/utils.cpp
)

SET(FINAL_INCLUDES
        include/curve.hpp
        include/camera.hpp
        include/group.hpp
        include/hit.hpp
        include/image.hpp
        include/kdtree.hpp
        include/light.hpp
        include/material.hpp
        include/mesh.hpp
        include/object3d.hpp
        include/plane.hpp
        include/ray.hpp
        include/scene_parser.hpp
        include/sphere.hpp
        include/tqdm.h
        include/transform.hpp
        include/triangle.hpp
        include/utils.hpp
)

SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

ADD_EXECUTABLE(${PROJECT_NAME} ${FINAL_SOURCES} ${FINAL_INCLUDES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} vecmath tinyobjloader)
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE include)
