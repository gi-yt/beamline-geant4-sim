#include "NeutronDoseSD.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4ParticleDefinition.hh"
#include "G4Neutron.hh"
#include "G4SystemOfUnits.hh"
#include "G4LogicalVolume.hh"
#include "G4VSolid.hh"
#include "G4ios.hh"
#include <cmath>
#include <iomanip>
#include <algorithm>
#include "G4TrackStatus.hh"

NeutronDoseSD::NeutronDoseSD(G4String name)
    : G4VSensitiveDetector(name), 
      fTotalTrackLength(0.0),
      fTotalDose(0.0),
      fNeutronCount(0)
{
    InitializeEnergyBins();
    
    G4cout << "\n=== NeutronDoseSD Initialized ===" << G4endl;
    G4cout << "Detector: WENDI-II ³He active volume" << G4endl;
    G4cout << "Energy bins: " << fDoseCoeffPerBin.size() << " groups" << G4endl;
    G4cout << "====================================\n" << G4endl;
}

NeutronDoseSD::~NeutronDoseSD()
{
}

void NeutronDoseSD::InitializeEnergyBins()
{
    // 34 energy groups from thermal to 5 GeV
    G4double binEdges[] = {
        1e-9, 1e-8, 1e-7, 1e-6, 1e-5, 1e-4, 1e-3, 1e-2, 1e-1,
        0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0,
        2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0,
        20.0, 50.0, 100.0, 200.0, 500.0, 1000.0, 5000.0
    };
    
    // ICRP 74 conversion coefficients for each bin
    G4double coeffs[] = {
        6.73, 7.30, 8.52, 9.92, 11.9, 18.9, 59.5, 105.0, 165.0,
        210.0, 230.0, 240.0, 245.0, 250.0, 255.0, 260.0, 265.0,
        270.0, 285.0, 300.0, 310.0, 320.0, 330.0, 340.0, 350.0,
        375.0, 380.0, 390.0, 400.0, 410.0, 530.0, 580.0, 440.0
    };
    
    G4int numBins = sizeof(binEdges) / sizeof(binEdges[0]) - 1;
    
    fEnergyBinEdges.assign(binEdges, binEdges + numBins + 1);
    fDoseCoeffPerBin.assign(coeffs, coeffs + numBins);
    
    fEnergyBinCenters.resize(numBins);
    fTrackLengthPerBin.assign(numBins, 0.0);
    
    for (G4int i = 0; i < numBins; i++) {
        fEnergyBinCenters[i] = std::sqrt(fEnergyBinEdges[i] * fEnergyBinEdges[i+1]);
    }
}

G4int NeutronDoseSD::FindEnergyBin(G4double energyMeV) const
{
    auto it = std::upper_bound(fEnergyBinEdges.begin(), fEnergyBinEdges.end(), energyMeV);
    G4int bin = std::distance(fEnergyBinEdges.begin(), it) - 1;
    if (bin < 0) bin = 0;
    if (bin >= (G4int)fDoseCoeffPerBin.size()) bin = fDoseCoeffPerBin.size() - 1;
    return bin;
}

void NeutronDoseSD::Initialize(G4HCofThisEvent*)
{
    fCountedTrackIDs.clear();    // Called at start of each event
}

void NeutronDoseSD::ResetCounters()
{
    fCountedTrackIDs.clear();
    fTotalTrackLength = 0.0;
    fTotalDose = 0.0;
    fNeutronCount = 0;
    std::fill(fTrackLengthPerBin.begin(), fTrackLengthPerBin.end(), 0.0);
    G4cout << "NeutronDoseSD counters reset" << G4endl;
}
G4bool NeutronDoseSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
    if (!step) return false;

    G4Track* track = step->GetTrack();
    if (!track) return false;

    // CRITICAL DEBUG - REMOVE AFTER TESTING
    G4cout << "\n*** ³He DETECTOR HIT BY: " << track->GetDefinition()->GetParticleName() << " ***" << G4endl;
    G4cout << "Energy: " << track->GetKineticEnergy() / MeV << " MeV" << G4endl;
    G4cout << "Step length: " << step->GetStepLength() / cm << " cm" << G4endl;
    G4cout << "Track status: " << track->GetTrackStatus() << G4endl;

    if (track->GetDefinition() != G4Neutron::NeutronDefinition()) {
        G4cout << "Not a neutron - ignoring" << G4endl;
        return false;
    }

    G4cout << "*** NEUTRON DETECTED IN ³He! ***" << G4endl;

    G4double stepLength = step->GetStepLength() / cm;
    if (stepLength <= 0) return false;

    G4double neutronEnergy = track->GetKineticEnergy() / MeV;
    G4int bin = FindEnergyBin(neutronEnergy);
    G4double doseCoeff = fDoseCoeffPerBin[bin];

    fTrackLengthPerBin[bin] += stepLength;
    fTotalTrackLength += stepLength;
    fTotalDose += doseCoeff * stepLength;

    if (track->GetTrackStatus() == fStopAndKill) {
        G4int trackID = track->GetTrackID();
        if (fCountedTrackIDs.find(trackID) == fCountedTrackIDs.end()) {
            fCountedTrackIDs.insert(trackID);
            fNeutronCount++;
            G4cout << "*** ABSORPTION #" << fNeutronCount << " ***" << G4endl;
        }
    }

    return true;
}
void NeutronDoseSD::EndOfEvent(G4HCofThisEvent*)
{
    // Processing done in RunAction
}

G4double NeutronDoseSD::GetDoseConversionCoefficient(G4double energyMeV) const
{
    // This method is kept for compatibility
    // But the binning approach in ProcessHits is more accurate
    G4int bin = FindEnergyBin(energyMeV);
    return fDoseCoeffPerBin[bin];
}