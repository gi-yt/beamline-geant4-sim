#ifndef FluenceDetector_h
#define FluenceDetector_h 1

#include "G4VSensitiveDetector.hh"
#include "globals.hh"
#include <vector>

struct EnergyCoefficient {
    G4double energy;
    G4double coeff;
};

class FluenceDetector : public G4VSensitiveDetector
{
public:
    FluenceDetector(G4String name);
    virtual ~FluenceDetector();
    
    virtual void Initialize(G4HCofThisEvent* hce) override;
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;
    virtual void EndOfEvent(G4HCofThisEvent* hce) override;
    
    G4double GetTotalTrackLength() const { return fTotalTrackLength; }
    G4double GetDetectorVolume() const { return 523.5987756; }
    const std::vector<G4double>& GetEnergyBinCenters() const { return fEnergyBinCenters; }
    const std::vector<G4double>& GetTrackLengthPerBin() const { return fTrackLengthPerBin; }
    const std::vector<G4double>& GetEnergyBinEdges() const { return fEnergyBinEdges; }
    G4int GetNumBins() const { return fNumBins; }
    
    void ResetCounters();
    
private:
    void InitializeEnergyBins();
    G4int FindEnergyBin(G4double energyMeV) const;
    
    static const G4int fNumBins = 34;
    std::vector<G4double> fEnergyBinEdges;
    std::vector<G4double> fEnergyBinCenters;
    std::vector<G4double> fTrackLengthPerBin;
    
    G4double fTotalTrackLength;
};

extern const EnergyCoefficient ICRP74_ISO[];
extern const G4int fNumCoeffs;

#endif