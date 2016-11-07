cmake_minimum_required (VERSION 2.6)
project(dinotracks)


set(dino_source_files
  newmain.cpp
  pointmanager.cpp
  vrpoint.cpp
  shader.cpp
  animationcontroller.cpp
  filter.cpp
)



set(filter_source_files
  pathfilter.cpp
  pointmanager.cpp
  vrpoint.cpp
  shader.cpp
  animationcontroller.cpp
  filter.cpp
)

set(header_files
  pointmanager.h
  vrpoint.h
  shader.h
  animationcontroller.h
  filter.h
  pathfilter.h
  vertex.h
)

set(resource_files
  colormap.jpg
  pathmap.jpg
  desktop.xml
)

set (MINVR_ROOT /Users/jtveite/research/minvr/test CACHE STRING "Root
directory for MinVR")



file(COPY ${resource_files} DESTINATION ${CMAKE_BINARY_DIR}/bin)

set_source_files_properties(vertex.h GLUtil.hPROPERTIES HEADER_FILE_ONLY TRUE)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${MINVR_ROOT}/src)

link_directories(${MINVR_ROOT}/build/lib)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(dino ${dino_source_files})
add_executable(path-filter ${filter_source_files})
add_executable(sogl vrexample.cpp)

target_link_libraries(dino MinVR ${OPENGL_LIBRARIES}  ${GLEW_LIBRARIES} ${ALL_LIBS})
target_link_libraries(sogl MinVR ${OPENGL_LIBRARIES}  ${GLEW_LIBRARIES} ${ALL_LIBS})
target_link_libraries(path-filter ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES}
${ALL_LIBS})
