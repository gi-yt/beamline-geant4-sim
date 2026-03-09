#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;
class DetectorConstruction;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
    PrimaryGeneratorAction(DetectorConstruction* detector);
    ~PrimaryGeneratorAction() override;

    void GeneratePrimaries(G4Event* event) override;
    
private:
    G4ParticleGun* fParticleGun;
    DetectorConstruction* fDetector;
    G4double fBeamEnergy;
};

#endif