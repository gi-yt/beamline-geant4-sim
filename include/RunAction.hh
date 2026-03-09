#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include <vector>
#include <fstream>

class G4Run;
class DetectorConstruction;
class NeutronDoseSD;
class FluenceDetector;

class RunAction : public G4UserRunAction
{
public:
    RunAction(DetectorConstruction* detector);
    virtual ~RunAction();

    virtual void BeginOfRunAction(const G4Run* run);
    virtual void EndOfRunAction(const G4Run* run);
    
private:
    DetectorConstruction* fDetector;
    NeutronDoseSD* fNeutronSD;
    FluenceDetector* fFluenceSD;  // FIXED: Correct spelling
    std::ofstream* fCSVFile;
};

#endif