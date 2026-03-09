#include "SteppingAction.hh"
#include "EventAction.hh"
#include "DetectorConstruction.hh"
#include "G4Step.hh"

SteppingAction::SteppingAction(EventAction* eventAction, DetectorConstruction* detector)
: G4UserSteppingAction(), fEventAction(eventAction), fDetector(detector)
{}

SteppingAction::~SteppingAction()
{}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    // Intentionally minimal: all physics scoring handled by sensitive detectors.
    if (!step) return;
}
