#include "EventAction.hh"
#include "RunAction.hh"
#include "G4Event.hh"

EventAction::EventAction(RunAction* runAction)
: G4UserEventAction(), fRunAction(runAction)
{}

EventAction::~EventAction()
{}

void EventAction::BeginOfEventAction(const G4Event*)
{
    // No per-event initialization needed
}

void EventAction::EndOfEventAction(const G4Event*)
{
    // No per-event processing needed
}
