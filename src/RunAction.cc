#include "RunAction.hh"
#include "FluenceDetector.hh"
#include "DetectorConstruction.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include <iomanip>
#include <sstream>
#include <cmath>
#include <cassert>

extern const EnergyCoefficient ICRP74_ISO[];
extern const G4int fNumCoeffs;

RunAction::RunAction(DetectorConstruction* detector)
: G4UserRunAction(), fDetector(detector), fFluenceSD(nullptr)
{
    if (!fDetector) {
        G4cerr << "ERROR: RunAction constructed with null detector" << G4endl;
    }
}

RunAction::~RunAction() {}

void RunAction::BeginOfRunAction(const G4Run*)
{
    G4SDManager* sdManager = G4SDManager::GetSDMpointer();
    if (!sdManager) {
        G4cerr << "ERROR: Cannot get SD Manager" << G4endl;
        return;
    }
    
    fFluenceSD = dynamic_cast<FluenceDetector*>(sdManager->FindSensitiveDetector("FluenceDetector"));
    
    if (!fFluenceSD) {
        G4cerr << "ERROR: Could not find FluenceDetector!" << G4endl;
        return;
    }
    
    fFluenceSD->ResetCounters();
}

G4double RunAction::GetInterpolatedCoefficient(G4double energyMeV) const
{
    if (!fNumCoeffs || !ICRP74_ISO) {
        G4cerr << "ERROR: ICRP coefficients not initialized" << G4endl;
        return 243.0;
    }
    
    #ifndef NDEBUG
    for (G4int i = 0; i < fNumCoeffs - 1; i++) {
        assert(ICRP74_ISO[i].energy < ICRP74_ISO[i+1].energy);
    }
    #endif
    
    if (energyMeV <= ICRP74_ISO[0].energy) return ICRP74_ISO[0].coeff;
    if (energyMeV >= ICRP74_ISO[fNumCoeffs - 1].energy) return ICRP74_ISO[fNumCoeffs - 1].coeff;
    
    G4int low = 0, high = fNumCoeffs - 1;
    while (high - low > 1) {
        G4int mid = (low + high) / 2;
        if (energyMeV < ICRP74_ISO[mid].energy) high = mid;
        else low = mid;
    }
    
    G4double fraction = (energyMeV - ICRP74_ISO[low].energy) / 
                        (ICRP74_ISO[high].energy - ICRP74_ISO[low].energy);
    return ICRP74_ISO[low].coeff + fraction * (ICRP74_ISO[high].coeff - ICRP74_ISO[low].coeff);
}

G4double RunAction::CalculateAverageCoefficient(const std::vector<G4double>& trackLengthPerBin,
                                                 const std::vector<G4double>& binCenters) const
{
    if (trackLengthPerBin.size() != binCenters.size()) {
        G4cerr << "ERROR: Track length and bin center size mismatch" << G4endl;
        return 243.0;
    }
    
    G4double totalWeightedCoeff = 0.0;
    G4double totalTrackLength = 0.0;
    
    for (size_t i = 0; i < trackLengthPerBin.size(); i++) {
        if (trackLengthPerBin[i] > 0) {
            G4double coeff = GetInterpolatedCoefficient(binCenters[i]);
            totalWeightedCoeff += trackLengthPerBin[i] * coeff;
            totalTrackLength += trackLengthPerBin[i];
        }
    }
    
    return (totalTrackLength > 0) ? totalWeightedCoeff / totalTrackLength : 243.0;
}

G4double RunAction::CalculateLogBinWidth(G4double lowEdge, G4double highEdge) const
{
    if (lowEdge <= 0 || highEdge <= lowEdge) return 1.0;
    return std::log10(highEdge) - std::log10(lowEdge);
}

bool RunAction::WriteCSVFile(const G4String& filename, const G4String& materialName,
                             G4int nPions, G4double trackLength, G4double volume,
                             G4double fluence, G4double avgCoeff, G4double totalDose,
                             G4double dosePerPion) const
{
    std::ofstream csvFile(filename);
    if (!csvFile.is_open()) {
        G4cerr << "ERROR: Cannot open file " << filename << " for writing" << G4endl;
        return false;
    }
    
    csvFile << "Material,Pions,TotalTrackLength_cm,Volume_cm3,Fluence_n_cm2,"
            << "AvgCoeff_pSv_cm2,TotalDose_pSv,DosePerPion_pSv" << std::endl;
    csvFile << materialName << ","
            << nPions << ","
            << std::fixed << std::setprecision(6)
            << trackLength << ","
            << volume << ","
            << fluence << ","
            << avgCoeff << ","
            << totalDose << ","
            << dosePerPion << std::endl;
    
    bool success = csvFile.good();
    csvFile.close();
    
    if (!success) {
        G4cerr << "ERROR: Failed to write to file " << filename << G4endl;
    }
    
    return success;
}

bool RunAction::WriteSpectrumFile(const G4String& filename,
                                  const std::vector<G4double>& binCenters,
                                  const std::vector<G4double>& binEdges,
                                  const std::vector<G4double>& trackLengthPerBin,
                                  G4double detectorVolume, G4int numBins) const
{
    assert(binEdges.size() == static_cast<size_t>(numBins + 1));
    assert(binCenters.size() == static_cast<size_t>(numBins));
    assert(trackLengthPerBin.size() == static_cast<size_t>(numBins));
    
    std::ofstream spectrumFile(filename);
    if (!spectrumFile.is_open()) {
        G4cerr << "ERROR: Cannot open file " << filename << " for writing" << G4endl;
        return false;
    }
    
    spectrumFile << "Energy_MeV,Fluence_per_MeV_n_cm2_MeV,Fluence_per_LogE_n_cm2" << std::endl;
    
    for (G4int i = 0; i < numBins; i++) {
        G4double lowEdge = binEdges[i];
        G4double highEdge = binEdges[i + 1];
        G4double linearWidth = highEdge - lowEdge;
        G4double logWidth = CalculateLogBinWidth(lowEdge, highEdge);
        
        G4double binFluence = trackLengthPerBin[i] / detectorVolume;
        G4double fluencePerLinearMeV = (linearWidth > 0) ? binFluence / linearWidth : 0;
        G4double fluencePerLogE = (logWidth > 0) ? binFluence / logWidth : 0;
        
        spectrumFile << std::scientific << std::setprecision(6)
                    << binCenters[i] << ","
                    << fluencePerLinearMeV << ","
                    << fluencePerLogE << std::endl;
    }
    
    bool success = spectrumFile.good();
    spectrumFile.close();
    
    if (!success) {
        G4cerr << "ERROR: Failed to write to file " << filename << G4endl;
    }
    
    return success;
}

void RunAction::EndOfRunAction(const G4Run* run)
{
    if (!fFluenceSD) {
        G4cerr << "ERROR: FluenceDetector not found!" << G4endl;
        return;
    }
    
    if (!fDetector) {
        G4cerr << "ERROR: DetectorConstruction not available!" << G4endl;
        return;
    }
    
    G4int numberOfPions = run->GetNumberOfEvent();
    G4double totalTrackLength = fFluenceSD->GetTotalTrackLength();
    G4double detectorVolume = fFluenceSD->GetDetectorVolume();
    
    const std::vector<G4double>& trackLengthPerBin = fFluenceSD->GetTrackLengthPerBin();
    const std::vector<G4double>& binCenters = fFluenceSD->GetEnergyBinCenters();
    const std::vector<G4double>& binEdges = fFluenceSD->GetEnergyBinEdges();
    G4int numBins = fFluenceSD->GetNumBins();
    
    G4cout << "\n=========================================================" << G4endl;
    G4cout << "MATERIAL: " << fDetector->GetCurrentMaterialName() << G4endl;
    G4cout << "=========================================================" << G4endl;
    G4cout << "Pions fired: " << numberOfPions << G4endl;
    G4cout << "Total track length in detector: " << totalTrackLength << " cm" << G4endl;
    G4cout << "Detector volume: " << detectorVolume << " cm³" << G4endl;
    
    if (totalTrackLength <= 0) {
        G4cout << "WARNING: No track length recorded!" << G4endl;
        G4cout << "=========================================================\n" << G4endl;
        return;
    }
    
    G4double fluenceAtPosition = totalTrackLength / detectorVolume;
    G4double avgDoseCoefficient = CalculateAverageCoefficient(trackLengthPerBin, binCenters);
    G4double totalDose = fluenceAtPosition * avgDoseCoefficient;
    G4double dosePerPion = totalDose / numberOfPions;
    
    G4cout << "\n--- Results ---" << G4endl;
    G4cout << "Fluence Φ = " << fluenceAtPosition << " n/cm²" << G4endl;
    G4cout << "Average dose coefficient h_avg = " << avgDoseCoefficient << " pSv*cm2" << G4endl;
    G4cout << "H*(10) = " << totalDose << " pSv" << G4endl;
    G4cout << "Dose per pion = " << dosePerPion << " pSv/pion" << G4endl;
    G4cout << "=========================================================\n" << G4endl;
    
    G4String materialName = fDetector->GetCurrentMaterialName();
    std::stringstream fileName;
    fileName << "neutron_dose_" << materialName << ".csv";
    
    if (WriteCSVFile(fileName.str(), materialName, numberOfPions, totalTrackLength,
                     detectorVolume, fluenceAtPosition, avgDoseCoefficient,
                     totalDose, dosePerPion)) {
        G4cout << "Results saved to: " << fileName.str() << G4endl;
    }
    
    std::stringstream spectrumFileName;
    spectrumFileName << "neutron_spectrum_" << materialName << ".csv";
    
    if (WriteSpectrumFile(spectrumFileName.str(), binCenters, binEdges,
                          trackLengthPerBin, detectorVolume, numBins)) {
        G4cout << "Neutron spectrum saved to: " << spectrumFileName.str() << G4endl;
    }
}