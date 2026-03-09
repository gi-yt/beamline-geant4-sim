#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"
#include "G4ios.hh"
#include "Randomize.hh"
#include "G4String.hh"

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"

int main(int argc, char** argv)
{
    G4Random::setTheEngine(new CLHEP::RanecuEngine);
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

    runManager->Initialize();

    G4int eventsPerMaterial = 10000;

    // ============================================================
    // DIRECT MATERIAL SET - NO FUNCTIONS
    // ============================================================

    G4cout << "\n=== SETTING MATERIAL TO ALUMINIUM (INDEX 1) ===" << G4endl;

    // CALL THE FUNCTION DIRECTLY
    detector->SetMaterial(8);

    // Don't use any getters - just trust it worked
    G4cout << "Material set complete. Running simulation..." << G4endl;
    G4cout << "==============================================\n" << G4endl;

    // Run events
    runManager->BeamOn(eventsPerMaterial);

    G4cout << "\n=========================================================" << G4endl;
    G4cout << "SIMULATION COMPLETED" << G4endl;
    G4cout << "=========================================================\n" << G4endl;

    delete runManager;
    return 0;
}