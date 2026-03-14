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

const EnergyCoefficient ICRP74_ISO[] = {
    {1e-9, 1.29}, {1e-8, 1.56}, {2.5e-8, 1.76}, {1e-7, 2.26},
    {2e-7, 2.54}, {5e-7, 2.92}, {1e-6, 3.15}, {2e-6, 3.32},
    {5e-6, 3.47}, {1e-5, 3.52}, {2e-5, 3.54}, {5e-5, 3.55},
    {1e-4, 3.54}, {2e-4, 3.52}, {5e-4, 3.47}, {0.001, 3.46},
    {0.002, 3.48}, {0.005, 3.66}, {0.01, 4.19}, {0.02, 5.61},
    {0.03, 7.18}, {0.05, 10.4}, {0.07, 13.7}, {0.1, 18.6},
    {0.15, 26.6}, {0.2, 34.4}, {0.3, 49.4}, {0.5, 77.1},
    {0.7, 102}, {0.9, 126}, {1.0, 137}, {1.2, 153},
    {1.5, 174}, {2.0, 203}, {3.0, 244}, {4.0, 271},
    {5.0, 290}, {6.0, 303}, {7.0, 313}, {8.0, 321},
    {9.0, 327}, {10.0, 332}, {12.0, 339}, {14.0, 344},
    {15.0, 346}, {16.0, 347}, {18.0, 350}, {20.0, 352},
    {21.0, 353}, {30.0, 358}, {50.0, 371}, {75.0, 387},
    {100, 397}, {130, 407}, {150, 412}, {180, 421},
    {200, 426}, {300, 455}, {400, 488}, {500, 521},
    {600, 553}, {700, 580}, {800, 602}, {900, 620},
    {1000, 635}, {2000, 750}, {3000, 820}, {4000, 860},
    {5000, 890}
};

const G4int fNumCoeffs = sizeof(ICRP74_ISO) / sizeof(ICRP74_ISO[0]);

FluenceDetector::FluenceDetector(G4String name)
    : G4VSensitiveDetector(name), fTotalTrackLength(0.0)
{
    InitializeEnergyBins();
}

FluenceDetector::~FluenceDetector() {}

void FluenceDetector::InitializeEnergyBins()
{
    G4double binEdges[fNumBins+1] = {
        1e-9, 1e-8, 1e-7, 1e-6, 1e-5, 1e-4, 1e-3, 1e-2, 1e-1,
        0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0,
        2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0,
        20.0, 50.0, 100.0, 200.0, 500.0, 1000.0, 5000.0, 10000.0
    };
    
    fEnergyBinEdges.assign(binEdges, binEdges + fNumBins + 1);
    fEnergyBinCenters.resize(fNumBins);
    fTrackLengthPerBin.assign(fNumBins, 0.0);
    
    for (G4int i = 0; i < fNumBins; i++) {
        fEnergyBinCenters[i] = std::sqrt(fEnergyBinEdges[i] * fEnergyBinEdges[i+1]);
    }
}

G4int FluenceDetector::FindEnergyBin(G4double energyMeV) const
{
    auto it = std::upper_bound(fEnergyBinEdges.begin(), fEnergyBinEdges.end(), energyMeV);
    G4int bin = std::distance(fEnergyBinEdges.begin(), it) - 1;
    if (bin < 0) bin = 0;
    if (bin >= fNumBins) bin = fNumBins - 1;
    return bin;
}

void FluenceDetector::Initialize(G4HCofThisEvent*) {}

void FluenceDetector::ResetCounters()
{
    fTotalTrackLength = 0.0;
    std::fill(fTrackLengthPerBin.begin(), fTrackLengthPerBin.end(), 0.0);
}

G4bool FluenceDetector::ProcessHits(G4Step* step, G4TouchableHistory*)
{
    if (!step) return false;

    G4Track* track = step->GetTrack();
    if (!track) return false;

    if (track->GetDefinition() != G4Neutron::NeutronDefinition()) {
        return false;
    }

    G4double stepLength = step->GetStepLength() / cm;
    if (stepLength <= 0) return false;

    G4double neutronEnergy = track->GetKineticEnergy() / MeV;
    
    // SIMPLE: Direct accumulation
    fTotalTrackLength += stepLength;
    
    G4int bin = FindEnergyBin(neutronEnergy);
    fTrackLengthPerBin[bin] += stepLength;

    return true;
}

void FluenceDetector::EndOfEvent(G4HCofThisEvent*) {}