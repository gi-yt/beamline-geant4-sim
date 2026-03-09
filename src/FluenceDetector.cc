#include "FluenceDetector.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4ParticleDefinition.hh"
#include "G4Neutron.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include <cmath>
#include <algorithm>

FluenceDetector::FluenceDetector(G4String name)
    : G4VSensitiveDetector(name), 
      fTotalTrackLength(0.0),
      fDetectorVolume(1.0)
{
    InitializeEnergyBins();
    G4cout << "FluenceDetector created for NEUTRON-ONLY fluence measurement with " 
           << fEnergyBinEdges.size()-1 << " energy bins" << G4endl;
}

FluenceDetector::~FluenceDetector()
{
}

void FluenceDetector::InitializeEnergyBins()
{
    // 34 energy groups from thermal to 5 GeV
    G4double binEdges[] = {
        1e-9, 1e-8, 1e-7, 1e-6, 1e-5, 1e-4, 1e-3, 1e-2, 1e-1,
        0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0,
        2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0,
        20.0, 50.0, 100.0, 200.0, 500.0, 1000.0, 5000.0
    };
    
    G4int numBins = sizeof(binEdges) / sizeof(binEdges[0]) - 1;
    
    fEnergyBinEdges.assign(binEdges, binEdges + numBins + 1);
    fEnergyBinCenters.resize(numBins);
    fTrackLengthPerBin.assign(numBins, 0.0);
    
    for (G4int i = 0; i < numBins; i++) {
        fEnergyBinCenters[i] = std::sqrt(fEnergyBinEdges[i] * fEnergyBinEdges[i+1]);
    }
}

G4int FluenceDetector::FindEnergyBin(G4double energyMeV) const
{
    auto it = std::upper_bound(fEnergyBinEdges.begin(), fEnergyBinEdges.end(), energyMeV);
    G4int bin = std::distance(fEnergyBinEdges.begin(), it) - 1;
    if (bin < 0) bin = 0;
    if (bin >= (G4int)fTrackLengthPerBin.size()) bin = fTrackLengthPerBin.size() - 1;
    return bin;
}

void FluenceDetector::Initialize(G4HCofThisEvent*)
{
    // Called at start of each event
}

void FluenceDetector::ResetCounters()
{
    fTotalTrackLength = 0.0;
    std::fill(fTrackLengthPerBin.begin(), fTrackLengthPerBin.end(), 0.0);
    G4cout << "FluenceDetector counters reset" << G4endl;
}

G4bool FluenceDetector::ProcessHits(G4Step* step, G4TouchableHistory*)
{
    if (!step) return false;

    G4Track* track = step->GetTrack();
    if (!track) return false;

    // Print ALL particles entering fluence detector
  
    G4cout << track->GetDefinition()->GetParticleName() << G4endl;
    G4cout << "Energy: " << track->GetKineticEnergy() / MeV << " MeV" << G4endl;

    if (track->GetDefinition() != G4Neutron::NeutronDefinition()) {
        return false;
    }

    G4double stepLength = step->GetStepLength() / cm;
    if (stepLength <= 0) return false;

    fTotalTrackLength += stepLength;

    G4double neutronEnergy = track->GetKineticEnergy() / MeV;
    G4int bin = FindEnergyBin(neutronEnergy);
    fTrackLengthPerBin[bin] += stepLength;

    G4cout << "!!! NEUTRON in fluence detector: E=" << neutronEnergy
        << " MeV, Step=" << stepLength << " cm" << G4endl;

    return true;
}

void FluenceDetector::EndOfEvent(G4HCofThisEvent*)
{
    // Processing done in RunAction
}