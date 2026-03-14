#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"
#include "G4ios.hh"
#include "Randomize.hh"
#include "G4String.hh"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <climits>

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"

int main(int argc, char** argv)
{
    G4int materialIndex = 9; // Default: Lead
    
    for (G4int i = 1; i < argc; i++) {
        G4String arg = argv[i];
        if (arg == "-m" || arg == "--material") {
            if (i + 1 < argc) {
                char* endptr;
                long val = std::strtol(argv[++i], &endptr, 10);
                // Updated range: 0-10 (now includes Tungsten)
                if (*endptr != '\0' || val < 0 || val > 10) {
                    G4cerr << "ERROR: Invalid material index. Must be integer 0-10" << G4endl;
                    return 1;
                }
                materialIndex = static_cast<G4int>(val);
            } else {
                G4cerr << "ERROR: -m option requires an argument" << G4endl;
                return 1;
            }
        }
        else if (arg == "-h" || arg == "--help") {
            G4cout << "Usage: " << argv[0] << " [options]" << G4endl;
            G4cout << "Options:" << G4endl;
            G4cout << "  -m, --material INDEX   Set material index (0-10)" << G4endl;
            G4cout << "  -h, --help            Show this help" << G4endl;
            G4cout << "\nMaterial indices:" << G4endl;
            G4cout << "  0: Lithium" << G4endl;
            G4cout << "  1: Aluminium" << G4endl;
            G4cout << "  2: Titanium" << G4endl;
            G4cout << "  3: Iron" << G4endl;
            G4cout << "  4: Cobalt" << G4endl;
            G4cout << "  5: Molybdenum" << G4endl;
            G4cout << "  6: Tin" << G4endl;
            G4cout << "  7: Hafnium" << G4endl;
            G4cout << "  8: Platinum" << G4endl;
            G4cout << "  9: Lead (default)" << G4endl;
            G4cout << " 10: Tungsten" << G4endl;
            return 0;
        }
    }
    
    G4Random::setTheEngine(new CLHEP::RanecuEngine);
    
    std::time_t rawTime = std::time(nullptr);
    G4long seed;
    if (rawTime > static_cast<std::time_t>(INT_MAX)) {
        seed = static_cast<G4long>(rawTime % INT_MAX);
    } else {
        seed = static_cast<G4long>(rawTime);
    }
    G4Random::setTheSeed(seed);
    
    G4cout << "\n=== INITIALIZING GEANT4 ===" << G4endl;
    
    G4RunManager* runManager = new G4RunManager;

    DetectorConstruction* detector = new DetectorConstruction();
    runManager->SetUserInitialization(detector);

    PhysicsList* physicsList = new PhysicsList();
    runManager->SetUserInitialization(physicsList);

    PrimaryGeneratorAction* genAction = new PrimaryGeneratorAction(detector);
    runManager->SetUserAction(genAction);

    RunAction* runAction = new RunAction(detector);
    runManager->SetUserAction(runAction);

    EventAction* eventAction = new EventAction(runAction);
    runManager->SetUserAction(eventAction);

    SteppingAction* steppingAction = new SteppingAction(eventAction, detector);
    runManager->SetUserAction(steppingAction);

    // Set material BEFORE Initialize
    G4cout << "Setting material to index: " << materialIndex << G4endl;
    detector->SetMaterial(materialIndex);
    
    G4cout << "Calling Initialize()..." << G4endl;
    runManager->Initialize();
    
    // Refresh pointers after geometry is built
    detector->RefreshPointers();
    
    G4cout << "\n=== SIMULATION CONFIGURATION ===" << G4endl;
    G4cout << "Material: " << detector->GetCurrentMaterialName() 
           << " (index " << materialIndex << ")" << G4endl;
    G4cout << "Target thickness: " << detector->GetCurrentTargetThickness()/cm << " cm" << G4endl;
    G4cout << "Events: 10000" << G4endl;
    G4cout << "Random seed: " << seed << G4endl;
    G4cout << "================================\n" << G4endl;

    G4cout << "\n=== STARTING SIMULATION ===" << G4endl;
    G4cout << "Processing 10000 events... (progress every 1000 events)" << G4endl;
    
    runManager->BeamOn(10000);

    G4cout << "\n=== SIMULATION COMPLETED ===" << G4endl;
    G4cout << "Material: " << detector->GetCurrentMaterialName() << G4endl;
    G4cout << "============================\n" << G4endl;

    delete runManager;
    
    G4cout << "Press any key to exit..." << G4endl;
    std::cin.get();
    
    return 0;
}