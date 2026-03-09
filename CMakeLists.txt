cmake_minimum_required(VERSION 3.16)
project(MyPionNeutronLab)

# Find Geant4 with required components
find_package(Geant4 REQUIRED ui_all vis_all)

# Include Geant4 macros
include(${Geant4_USE_FILE})

# Set source files
set(SOURCES
    src/DetectorConstruction.cc
    src/EventAction.cc
    src/FluenceDetector.cc
    src/main.cc
    src/NeutronDoseSD.cc
    src/PhysicsList.cc
    src/PrimaryGeneratorAction.cc
    src/RunAction.cc
    src/SteppingAction.cc
)

# Set include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add executable
add_executable(MyPionNeutronLab ${SOURCES})

# Link Geant4 libraries
target_link_libraries(MyPionNeutronLab ${Geant4_LIBRARIES})

# Set properties for C++ standard
set_target_properties(MyPionNeutronLab PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Optional: Set output directory for binaries
set_target_properties(MyPionNeutronLab PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

message(STATUS "==========================================")
message(STATUS "MyPionNeutronLab v6 Configuration:")
message(STATUS "  Geant4 version: ${Geant4_VERSION}")
message(STATUS "  C++ Standard: 17")
message(STATUS "  World size: 5 m (beam at z=-2 m contained)")
message(STATUS "  Source files: ${SOURCES}")
message(STATUS "==========================================")