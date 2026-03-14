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
    if (!fDetector) {
        G4cerr << "ERROR: PrimaryGeneratorAction constructed with null detector" << G4endl;
    }
    
    fParticleGun = new G4ParticleGun(1);
    G4ParticleDefinition* pion = G4PionMinus::PionMinus();
    if (!pion) {
        G4cerr << "ERROR: Cannot create pion minus particle" << G4endl;
        return;
    }
    
    fParticleGun->SetParticleDefinition(pion);
    fParticleGun->SetParticleEnergy(fBeamEnergy);
    
    G4cout << "Primary generator: pi- beam at " << fBeamEnergy/GeV << " GeV" << G4endl;
    G4cout << "Beam spot radius: 1 cm, divergence: 1 mrad" << G4endl;
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete fParticleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
    if (!fParticleGun) {
        G4cerr << "ERROR: Particle gun not initialized" << G4endl;
        return;
    }
    
    const G4double beamSpotRadius = 1.0 * cm;
    const G4double beamDivergence = 0.001 * rad;
    const G4double zPosition = -200.0 * cm;
    const G4double tolerance = 1.0e-10;

    G4double r = beamSpotRadius * std::sqrt(G4UniformRand());
    G4double phi = 2.0 * pi * G4UniformRand();
    G4double x0 = r * std::cos(phi);
    G4double y0 = r * std::sin(phi);
    G4double z0 = zPosition;

    G4double theta = beamDivergence * G4UniformRand();
    G4double phiDir = 2.0 * pi * G4UniformRand();
    
    G4double dx = std::sin(theta) * std::cos(phiDir);
    G4double dy = std::sin(theta) * std::sin(phiDir);
    G4double dz = std::cos(theta);
    
    G4double norm = std::sqrt(dx*dx + dy*dy + dz*dz);
    if (norm > tolerance) {
        dx /= norm;
        dy /= norm;
        dz /= norm;
    } else {
        dx = 0.0;
        dy = 0.0;
        dz = 1.0;
    }
    
    fParticleGun->SetParticlePosition(G4ThreeVector(x0, y0, z0));
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(dx, dy, dz));
    fParticleGun->GeneratePrimaryVertex(event);
}