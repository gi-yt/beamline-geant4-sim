#ifndef FluenceDetector_h
#define FluenceDetector_h 1

#include "G4VSensitiveDetector.hh"
#include "globals.hh"
#include <vector>

class G4Step;
class G4HCofThisEvent;

class FluenceDetector : public G4VSensitiveDetector
{
public:
    FluenceDetector(G4String name);
    virtual ~FluenceDetector();
    
    virtual void Initialize(G4HCofThisEvent* hce);
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
    virtual void EndOfEvent(G4HCofThisEvent* hce);
    
    G4double GetTotalTrackLength() const { return fTotalTrackLength; }
    G4double GetDetectorVolume() const { return fDetectorVolume; }
    void ResetCounters();
    void SetDetectorVolume(G4double vol) { fDetectorVolume = vol; }
    
    // Energy bin results for neutron spectrum
    const std::vector<G4double>& GetEnergyBinCenters() const { return fEnergyBinCenters; }
    const std::vector<G4double>& GetTrackLengthPerBin() const { return fTrackLengthPerBin; }
    
private:
    void InitializeEnergyBins();
    G4int FindEnergyBin(G4double energyMeV) const;
    
    std::vector<G4double> fEnergyBinEdges;
    std::vector<G4double> fEnergyBinCenters;
    std::vector<G4double> fTrackLengthPerBin;
    
    G4double fTotalTrackLength;  // Only neutron track length
    G4double fDetectorVolume;
};

#endif