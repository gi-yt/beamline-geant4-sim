#include "SteppingAction.hh"
#include "G4Step.hh"

SteppingAction::SteppingAction(EventAction*, DetectorConstruction*)
: G4UserSteppingAction(), fEventAction(nullptr), fDetector(nullptr)
{}

SteppingAction::~SteppingAction()
{}

void SteppingAction::UserSteppingAction(const G4Step*)
{}