cmake_minimum_required(VERSION 3.16)
project(MyPionNeutronLab)

find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

set(SOURCES
    src/DetectorConstruction.cc
    src/EventAction.cc
    src/FluenceDetector.cc
    src/main.cc
    src/PhysicsList.cc
    src/PrimaryGeneratorAction.cc
    src/RunAction.cc
    src/SteppingAction.cc
)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(MyPionNeutronLab ${SOURCES})

target_link_libraries(MyPionNeutronLab ${Geant4_LIBRARIES})

set_target_properties(MyPionNeutronLab PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

set_target_properties(MyPionNeutronLab PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

message(STATUS "==========================================")
message(STATUS "MyPionNeutronLab Configuration:")
message(STATUS "  Geant4 version: ${Geant4_VERSION}")
message(STATUS "  Source files: ${SOURCES}")
message(STATUS "  Include path: ${CMAKE_SOURCE_DIR}/include")
message(STATUS "  Detector at 30 cm from target")
message(STATUS "==========================================")