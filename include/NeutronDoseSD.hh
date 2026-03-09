#ifndef NeutronDoseSD_h
#define NeutronDoseSD_h 1

#include <set>
#include "G4VSensitiveDetector.hh"
#include "globals.hh"
#include <vector>

class G4Step;
class G4HCofThisEvent;

class NeutronDoseSD : public G4VSensitiveDetector
{
public:
    NeutronDoseSD(G4String name);
    virtual ~NeutronDoseSD();
    
    virtual void Initialize(G4HCofThisEvent* hce);
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
    virtual void EndOfEvent(G4HCofThisEvent* hce);
    
    // Accessors
    G4double GetTotalTrackLength() const { return fTotalTrackLength; }
    G4double GetTotalDose() const { return fTotalDose; }
    G4int GetNeutronCount() const { return fNeutronCount; }  // This exists
    void ResetCounters();
    
    const std::vector<G4double>& GetEnergyBinCenters() const { return fEnergyBinCenters; }
    const std::vector<G4double>& GetTrackLengthPerBin() const { return fTrackLengthPerBin; }
    const std::vector<G4double>& GetDoseCoeffPerBin() const { return fDoseCoeffPerBin; }
    
private:
    std::set<G4int> fCountedTrackIDs;
    void InitializeEnergyBins();
    G4double GetDoseConversionCoefficient(G4double energyMeV) const;
    G4int FindEnergyBin(G4double energyMeV) const;
    
    std::vector<G4double> fEnergyBinEdges;
    std::vector<G4double> fEnergyBinCenters;
    std::vector<G4double> fDoseCoeffPerBin;
    std::vector<G4double> fTrackLengthPerBin;
    
    G4double fTotalTrackLength;
    G4double fTotalDose;
    G4int fNeutronCount;
};

#endif