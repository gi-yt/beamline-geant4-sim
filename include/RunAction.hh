#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include <vector>
#include <fstream>
#include <string>

class G4Run;
class DetectorConstruction;
class FluenceDetector;

class RunAction : public G4UserRunAction
{
public:
    explicit RunAction(DetectorConstruction* detector);
    virtual ~RunAction();

    virtual void BeginOfRunAction(const G4Run* run) override;
    virtual void EndOfRunAction(const G4Run* run) override;
    
private:
    G4double GetInterpolatedCoefficient(G4double energyMeV) const;
    G4double CalculateAverageCoefficient(const std::vector<G4double>& trackLengthPerBin,
                                         const std::vector<G4double>& binCenters) const;
    G4double CalculateLogBinWidth(G4double lowEdge, G4double highEdge) const;
    bool WriteCSVFile(const G4String& filename, const G4String& materialName,
                      G4int nPions, G4double trackLength, G4double volume,
                      G4double fluence, G4double avgCoeff, G4double totalDose,
                      G4double dosePerPion) const;
    bool WriteSpectrumFile(const G4String& filename,
                           const std::vector<G4double>& binCenters,
                           const std::vector<G4double>& binEdges,
                           const std::vector<G4double>& trackLengthPerBin,
                           G4double detectorVolume, G4int numBins) const;
    
    DetectorConstruction* fDetector;
    FluenceDetector* fFluenceSD;
};

#endif