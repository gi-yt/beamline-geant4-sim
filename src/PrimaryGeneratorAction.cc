#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4PionMinus.hh"
#include "Randomize.hh"
#include "G4PhysicalConstants.hh"
#include "G4ios.hh"
#include <cmath>

PrimaryGeneratorAction::PrimaryGeneratorAction(DetectorConstruction* detector)
: G4VUserPrimaryGeneratorAction(), fParticleGun(nullptr), fDetector(detector),
  fBeamEnergy(10*GeV)
{
    fParticleGun = new G4ParticleGun(1);

    G4ParticleDefinition* pionMinus = G4PionMinus::PionMinus();
    fParticleGun->SetParticleDefinition(pionMinus);
    fParticleGun->SetParticleEnergy(fBeamEnergy);

    G4cout << "Primary generator: pi- beam at " << fBeamEnergy/GeV << " GeV" << G4endl;
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete fParticleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
    // Beam parameters
    const G4double beamSpotRadius = 1.0*cm;
    const G4double beamDivergence = 0.001*rad;

    // Circular beam spot (uniform in area)
    G4double r   = beamSpotRadius * std::sqrt(G4Random::getTheEngine()->flat());
    G4double phi = 2.0*pi * G4Random::getTheEngine()->flat();
    G4double x0  = r * std::cos(phi);
    G4double y0  = r * std::sin(phi);
    G4double z0  = -2.0*m;   // Inside 5 m world (half-length 2.5 m)

    // Small-angle beam divergence
    G4double theta  = beamDivergence * std::sqrt(G4Random::getTheEngine()->flat());
    G4double phiDir = 2.0*pi * G4Random::getTheEngine()->flat();
    G4double dx = std::sin(theta) * std::cos(phiDir);
    G4double dy = std::sin(theta) * std::sin(phiDir);
    G4double dz = std::cos(theta);

    fParticleGun->SetParticlePosition(G4ThreeVector(x0, y0, z0));
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(dx, dy, dz));
    fParticleGun->GeneratePrimaryVertex(event);
}
