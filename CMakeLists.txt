cmake_minimum_required (VERSION 2.6)
project(dinotracks) 

set(dino_source_files
  newmain.cpp
  cursor.cpp
  slide.cpp
  textboard.cpp
  pointmanager.cpp
  vrpoint.cpp
  shader.cpp
  animationcontroller.cpp
  filter.cpp
  movm.cpp
  footmeshviewer.cpp
  webupdatereader.cpp
  SimilarityEvaluator.cpp
  PathAlignmentSimilarityEvaluator.cpp

  Config.cpp

  GLUtil.cpp
)

set(test_source_files
  slide.cpp
  pointmanager.cpp
  vrpoint.cpp
  shader.cpp
  animationcontroller.cpp
  filter.cpp
  movm.cpp
  footmeshviewer.cpp
  webupdatereader.cpp
  SimilarityEvaluator.cpp
  PathAlignmentSimilarityEvaluator.cpp
  GLUtil.cpp
)



set(filter_source_files
  pathfilter.cpp
  pointmanager.cpp
  vrpoint.cpp
  shader.cpp
  animationcontroller.cpp
  filter.cpp
  movm.cpp
  GLUtil.cpp
)

set(header_files
  pointmanager.h
  vrpoint.h
  slide.h
  cursor.h
  textboard.h
  shader.h
  animationcontroller.h
  filter.h
  pathfilter.h
  vertex.h
  footmeshviewer.h
  webupdatereader.h
  SimilarityEvaluator.h
  PathAlignmentSimilarityEvaluator.h

  Config.h
  GLUtil.h
)

set(resource_files
  numbers.png
  colormap.jpg
  pathmap.jpg
  desktop.xml
)

set (MINVR_ROOT /users/jryoo/Desktop/ccv/MinVR CACHE STRING "Root
directory for MinVR")

#set(CMAKE_CXX_FLAGS -std=gnu++0x)
set(CMAKE_CXX_FLAGS -std=c++14)
#add_definitions(-std=c++14)

find_package(Doxygen)

option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)")

if(BUILD_DOCUMENTATION)
  if(NOT DOXYGEN_FOUND)
    message(FATAL_ERROR "Doxygen is needed to build the documentation.")
  endif()

  message("-- Configured to build documentation.  Use 'make doc'.")

  # Process the config file and put it in the build directory.
  set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/doc/dinotrackviewer.doxygen.in)
  set(doxyfile ${CMAKE_BINARY_DIR}/dinotrackviewer.doxygen)
  configure_file(${doxyfile_in} ${doxyfile} @ONLY)

  add_custom_target(doc
    COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM)

  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION share/doc)
endif()

file(COPY ${resource_files} DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY data/active-dataset.out DESTINATION ${CMAKE_BINARY_DIR}/bin)

set_source_files_properties(vertex.h GLUtil.h PROPERTIES HEADER_FILE_ONLY TRUE)

find_package(OpenGL REQUIRED)
#find_package(GLEW REQUIRED)
set (GLEW_INCLUDE_DIRS /gpfs/runtime/opt/glew/1.13.0/include)
set (GLEW_LIBRARIES /gpfs/runtime/opt/glew/1.13.0/lib64/libGLEW.so)

include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${MINVR_ROOT}/src)

link_directories(${MINVR_ROOT}/build/lib)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


add_executable(dino ${dino_source_files})
add_executable(path-filter ${filter_source_files})
add_executable(sim-test ${test_source_files})

#add_definitions(-std=c++14)

target_compile_definitions(sim-test PRIVATE TESTING)

target_link_libraries(dino MinVR ${OPENGL_LIBRARIES}  ${GLEW_LIBRARIES} ${ALL_LIBS})
target_link_libraries(sim-test MinVR ${OPENGL_LIBRARIES}  ${GLEW_LIBRARIES} ${ALL_LIBS})
target_link_libraries(path-filter ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES}
${ALL_LIBS})
