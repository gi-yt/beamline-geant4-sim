#include "RunAction.hh"
#include "NeutronDoseSD.hh"
#include "FluenceDetector.hh"
#include "DetectorConstruction.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cmath>

RunAction::RunAction(DetectorConstruction* detector)
: G4UserRunAction(), 
  fDetector(detector), 
  fNeutronSD(nullptr),
  fFluenceSD(nullptr),  // FIXED: Correct spelling
  fCSVFile(nullptr)
{
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetDefaultFileType("csv");
    analysisManager->SetFileName("neutron_dose_results");
}

RunAction::~RunAction()
{
    if (fCSVFile) {
        fCSVFile->close();
        delete fCSVFile;
    }
}

void RunAction::BeginOfRunAction(const G4Run* run)
{
    G4SDManager* sdManager = G4SDManager::GetSDMpointer();
    fNeutronSD = dynamic_cast<NeutronDoseSD*>(sdManager->FindSensitiveDetector("NeutronDose"));
    fFluenceSD = dynamic_cast<FluenceDetector*>(sdManager->FindSensitiveDetector("FluenceDetector"));  // FIXED
    
    if (!fNeutronSD || !fFluenceSD) {
        G4cerr << "ERROR: Could not find sensitive detectors!" << G4endl;
        G4cerr << "  Looked for: 'NeutronDose' and 'FluenceDetector'" << G4endl;
        return;
    }
    
    fNeutronSD->ResetCounters();
    fFluenceSD->ResetCounters();
    
    G4String materialName = fDetector->GetCurrentMaterialName();
    
    std::stringstream fileName;
    fileName << "neutron_dose_" << materialName << ".csv";
    
    if (fCSVFile) {
        fCSVFile->close();
        delete fCSVFile;
    }
    fCSVFile = new std::ofstream(fileName.str());
    
    *fCSVFile << "Material,Pion_Count,Neutron_Reactions_He3,"
              << "Fluence_Detector_Track_Length_cm,Fluence_Detector_Volume_cm3,"
              << "Fluence_at_WENDI_n_cm2,Avg_Dose_Coefficient_pSv_cm2,"
              << "Total_Dose_pSv,Dose_per_Pion_pSv" << std::endl;
    
    G4cout << "\n=========================================================" << G4endl;
    G4cout << "Material: " << materialName << G4endl;
    G4cout << "Target thickness: " << fDetector->GetCurrentTargetThickness()/cm << " cm" << G4endl;
    G4cout << "Output: " << fileName.str() << G4endl;
    G4cout << "=========================================================\n" << G4endl;
}

void RunAction::EndOfRunAction(const G4Run* run)
{
    if (!fNeutronSD || !fFluenceSD) {  // FIXED
        G4cerr << "ERROR: Sensitive detectors not found in EndOfRunAction!" << G4endl;
        return;
    }
    
    G4int numberOfPions = run->GetNumberOfEvent();
    G4int neutronReactions = fNeutronSD->GetNeutronCount();  // This exists
    G4double totalTrackLength = fFluenceSD->GetTotalTrackLength();  // FIXED
    G4double fluenceDetectorVolume = fFluenceSD->GetDetectorVolume();  // FIXED
    
    const std::vector<G4double>& trackLengthPerBin = fNeutronSD->GetTrackLengthPerBin();
    const std::vector<G4double>& doseCoeffPerBin = fNeutronSD->GetDoseCoeffPerBin();
    
    G4cout << "\n=========================================================" << G4endl;
    G4cout << "MATERIAL: " << fDetector->GetCurrentMaterialName() << G4endl;
    G4cout << "=========================================================" << G4endl;
    G4cout << "Pions fired: " << numberOfPions << G4endl;
    G4cout << "Neutron reactions in ³He: " << neutronReactions << G4endl;
    G4cout << "Total track length in fluence detector: " << totalTrackLength << " cm" << G4endl;
    G4cout << "Fluence detector volume: " << fluenceDetectorVolume << " cm³" << G4endl;
    
    if (totalTrackLength <= 0 || fluenceDetectorVolume <= 0) {
        G4cout << "WARNING: No track length or invalid volume!" << G4endl;
        G4cout << "=========================================================\n" << G4endl;
        return;
    }
    
    // Calculate fluence at WENDI position
    G4double fluenceAtPosition = totalTrackLength / fluenceDetectorVolume;
    
    // Calculate average dose coefficient (track-length-weighted)
    G4double totalWeightedCoeff = 0.0;
    G4double totalBinLength = 0.0;
    
    for (size_t i = 0; i < trackLengthPerBin.size(); i++) {
        totalWeightedCoeff += trackLengthPerBin[i] * doseCoeffPerBin[i];
        totalBinLength += trackLengthPerBin[i];
    }
    
    G4double avgDoseCoefficient = (totalBinLength > 0) ? totalWeightedCoeff / totalBinLength : 243.0;
    
    // Total dose
    G4double totalDose = fluenceAtPosition * avgDoseCoefficient;
    G4double dosePerPion = totalDose / numberOfPions;
    
    G4cout << "\n--- Fluence at WENDI Position ---" << G4endl;
    G4cout << "Φ = " << fluenceAtPosition*100 << " n/cm²" << G4endl;
    
    G4cout << "\n--- ICRP 74 H*(10) Calculation ---" << G4endl;
    G4cout << "Average dose coefficient: " << avgDoseCoefficient << " pSv·cm²" << G4endl;
    G4cout << "H*(10) = " << totalDose*100 << " pSv" << G4endl;
    
    G4cout << "\n--- Dose Per Pion ---" << G4endl;
    G4cout << "Dose/Pion = " << dosePerPion*100 << " pSv" << G4endl;
    G4cout << "=========================================================\n" << G4endl;
    
    if (fCSVFile && fCSVFile->is_open()) {
        *fCSVFile << fDetector->GetCurrentMaterialName() << ","
                  << numberOfPions << ","
                  << neutronReactions << ","
                  << std::fixed << std::setprecision(6)
                  << totalTrackLength << ","
                  << fluenceDetectorVolume << ","
                  << fluenceAtPosition << ","
                  << avgDoseCoefficient << ","
                  << totalDose << ","
                  << dosePerPion << std::endl;
        fCSVFile->flush();
    }
}