// MyPionNeutronLab.cc
// Main function for the Windows Geant4 application
// This is the entry point of your program.

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

// --- Your custom classes (we will create these next) ---
#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "ActionInitialization.hh"

int main(int argc, char** argv)
{
    // 1. Construct the run manager (controls the simulation)
    G4RunManager* runManager = new G4RunManager();

    // 2. Set Mandatory User Classes
    // DetectorConstruction: Builds the world, targets, and detectors
    runManager->SetUserInitialization(new DetectorConstruction());
    // PhysicsList: Defines which particles and physics processes (hadronic, EM) are active
    runManager->SetUserInitialization(new PhysicsList());
    // ActionInitialization: Defines what happens at the start of each run/event
    runManager->SetUserInitialization(new ActionInitialization());

    // 3. Initialize G4 kernel
    runManager->Initialize();

    // 4. Get the pointer to the User Interface manager
    G4UImanager* UImanager = G4UImanager::GetUIpointer();

    // 5. Visualization: Check if we are running in interactive mode with arguments
    G4UIExecutive* ui = nullptr;
    if (argc == 1) {
        ui = new G4UIExecutive(argc, argv);
    }

    G4VisManager* visManager = new G4VisExecutive();
    visManager->Initialize();

    if (ui) {
        // Interactive mode: Open the viewer
        UImanager->ApplyCommand("/control/execute mac/init_vis.mac");
        ui->SessionStart();
        delete ui;
    } else {
        // Batch mode: Execute a macro file if provided as argument
        G4String command = "/control/execute ";
        G4String fileName = argv[1];
        UImanager->ApplyCommand(command + fileName);
    }

    // 6. Job termination
    delete visManager;
    delete runManager;

    return 0;
}