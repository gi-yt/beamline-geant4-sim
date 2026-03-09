#include "DetectorConstruction.hh"
#include "NeutronDoseSD.hh"
#include "FluenceDetector.hh"
#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include <cmath>
#include <iomanip>

// Constructor
DetectorConstruction::DetectorConstruction()
: G4VUserDetectorConstruction(),
  fCurrentMaterialIndex(-1),  // CHANGED: Start with invalid index
  fCurrentMaterial(nullptr),
  fCurrentThickness(0),
  fPolyethylene(nullptr),
  fTungstenLoadedPoly(nullptr),
  fHelium3Gas(nullptr),
  fStainlessSteel(nullptr),
  fAluminum(nullptr),
  fAir(nullptr),
  fTargetLog(nullptr),
  fTargetPhys(nullptr),
  fWENDIModeratorLog(nullptr),
  fWENDIModeratorPhys(nullptr),
  fWENDITungstenLayerLog(nullptr),
  fWENDITungstenLayerPhys(nullptr),
  fWENDIInnerModeratorLog(nullptr),
  fWENDIInnerModeratorPhys(nullptr),
  fWENDIHelium3Log(nullptr),
  fWENDIHelium3Phys(nullptr),
  fWENDICounterWallLog(nullptr),
  fWENDICounterWallPhys(nullptr),
  fWENDIHousingLog(nullptr),
  fWENDIHousingPhys(nullptr),
  fFluenceDetectorLog(nullptr),
  fFluenceDetectorPhys(nullptr),
  fWorldPhys(nullptr)
{
    // Define target material database (UPDATED WITH NEW METALS)
    fMaterialDatabase[0] = { "Lithium", 3.0, 6.94, 0.534, 154.5 };

    // Metals between Lithium and Aluminium
    fMaterialDatabase[1] = { "Beryllium", 4.0, 9.01, 1.85, 45.2 };
    fMaterialDatabase[2] = { "Sodium", 11.0, 22.99, 0.97, 65.8 };
    fMaterialDatabase[3] = { "Magnesium", 12.0, 24.31, 1.74, 42.1 };
    fMaterialDatabase[4] = { "Aluminium", 13.0, 26.98, 2.70, 39.4 };

    fMaterialDatabase[5] = { "Titanium", 22.0, 47.87, 4.51, 26.5 };
    fMaterialDatabase[6] = { "Iron", 26.0, 55.85, 7.87, 16.8 };
    fMaterialDatabase[7] = { "Cobalt", 27.0, 58.93, 8.90, 15.1 };

    // Metals between Cobalt and Lead (expanded selection)
    fMaterialDatabase[8] = { "Nickel", 28.0, 58.69, 8.91, 15.5 };
    fMaterialDatabase[9] = { "Zinc", 30.0, 65.38, 7.14, 18.2 };
    fMaterialDatabase[10] = { "Rubidium", 37.0, 85.47, 1.53, 89.3 };
    fMaterialDatabase[11] = { "Strontium", 38.0, 87.62, 2.64, 52.1 };
    fMaterialDatabase[12] = { "Yttrium", 39.0, 88.91, 4.47, 31.2 };
    fMaterialDatabase[13] = { "Niobium", 41.0, 92.91, 8.57, 16.9 };
    fMaterialDatabase[14] = { "Molybdenum", 42.0, 95.95, 10.2, 14.4 };
    fMaterialDatabase[15] = { "Ruthenium", 44.0, 101.07, 12.4, 12.7 };
    fMaterialDatabase[16] = { "Rhodium", 45.0, 102.91, 12.4, 12.9 };
    fMaterialDatabase[17] = { "Palladium", 46.0, 106.42, 12.0, 12.8 };
    fMaterialDatabase[18] = { "Silver", 47.0, 107.87, 10.5, 16.8 };
    fMaterialDatabase[19] = { "Cadmium", 48.0, 112.41, 8.65, 20.1 };
    fMaterialDatabase[20] = { "Tin", 50.0, 118.71, 7.29, 21.4 };
    fMaterialDatabase[21] = { "Cesium", 55.0, 132.91, 1.93, 107.2 };
    fMaterialDatabase[22] = { "Barium", 56.0, 137.33, 3.62, 46.3 };
    fMaterialDatabase[23] = { "Lanthanum", 57.0, 138.91, 6.15, 28.5 };
    fMaterialDatabase[24] = { "Hafnium", 72.0, 178.49, 13.3, 12.9 };
    fMaterialDatabase[25] = { "Tantalum", 73.0, 180.95, 16.7, 11.2 };
    fMaterialDatabase[26] = { "Tungsten", 74.0, 183.84, 19.3, 9.6 };
    fMaterialDatabase[27] = { "Rhenium", 75.0, 186.21, 21.0, 9.1 };
    fMaterialDatabase[28] = { "Osmium", 76.0, 190.23, 22.6, 8.7 };
    fMaterialDatabase[29] = { "Iridium", 77.0, 192.22, 22.6, 8.8 };
    fMaterialDatabase[30] = { "Platinum", 78.0, 195.08, 21.4, 8.3 };
    fMaterialDatabase[31] = { "Gold", 79.0, 196.97, 19.3, 9.7 };
    fMaterialDatabase[32] = { "Mercury", 80.0, 200.59, 13.5, 22.1 };
    fMaterialDatabase[33] = { "Lead", 82.0, 207.2, 11.3, 17.1 };

    G4cout << "\n=== Material Database Initialized (Target Metals) ===" << G4endl;
    for (const auto& pair : fMaterialDatabase) {
        G4cout << "Index " << pair.first << ": " << pair.second.name
            << " (Z=" << pair.second.atomicNumber
            << ", A=" << pair.second.atomicMass << " g/mol"
            << ", ρ=" << pair.second.density << " g/cm³"
            << ", λ=" << pair.second.nuclearInteractionLength << " cm)" << G4endl;
    }
    G4cout << "==================================================\n" << G4endl;
}
// Destructor
DetectorConstruction::~DetectorConstruction()
{
    if (fCurrentMaterial) delete fCurrentMaterial;
}

void DetectorConstruction::DefineMaterials()
{
    G4NistManager* nist = G4NistManager::Instance();
    
    fPolyethylene = nist->FindOrBuildMaterial("G4_POLYETHYLENE");
    fStainlessSteel = nist->FindOrBuildMaterial("G4_STAINLESS-STEEL");
    fAluminum = nist->FindOrBuildMaterial("G4_Al");
    fAir = nist->FindOrBuildMaterial("G4_AIR");
    
    G4Material* polyethylene = nist->FindOrBuildMaterial("G4_POLYETHYLENE");
    G4Material* tungsten = nist->FindOrBuildMaterial("G4_W");
    
    if (polyethylene && tungsten) {
        fTungstenLoadedPoly = new G4Material("TungstenLoadedPoly", 2.8*g/cm3, 2);
        fTungstenLoadedPoly->AddMaterial(polyethylene, 0.305);
        fTungstenLoadedPoly->AddMaterial(tungsten, 0.695);
    } else {
        G4cerr << "ERROR: Could not create tungsten-loaded polyethylene!" << G4endl;
        fTungstenLoadedPoly = fPolyethylene;
    }
    
    G4Element* elHe3 = new G4Element("Helium-3", "He3", 2.0, 3.016 * g/mole);
    fHelium3Gas = new G4Material("Helium3Gas", 5.01e-4 * g / cm3, 1);
    fHelium3Gas->AddElement(elHe3, 1.0);
    
    G4cout << "\n=== WENDI-II Materials Created ===" << G4endl;
    G4cout << "Polyethylene: " << fPolyethylene->GetName() << G4endl;
    G4cout << "Tungsten-loaded polyethylene: density " << fTungstenLoadedPoly->GetDensity()/(g/cm3) << " g/cm³" << G4endl;
    G4cout << "³He gas: " << fHelium3Gas->GetDensity()/(g/cm3) << " g/cm³" << G4endl;
    G4cout << "Air: " << fAir->GetDensity()/(g/cm3) << " g/cm³" << G4endl;
    G4cout << "==================================\n" << G4endl;
}

void DetectorConstruction::BuildWENDI(G4LogicalVolume* worldLog)
{
    G4cout << "\n=== Building WENDI-II Detector ===" << G4endl;

    G4ThreeVector wendiPos(0, 0, 100 * cm);

    // ============================================================
    // LAYER 1: Aluminum Housing
    // ============================================================
    G4Sphere* solidHousing = new G4Sphere("WENDI_Housing",
        fWENDIOuterRadius, fWENDIOuterRadius + 0.2 * cm,
        0, 2 * pi, 0, pi);
    fWENDIHousingLog = new G4LogicalVolume(solidHousing, fAluminum, "WENDI_Housing_Log");
    new G4PVPlacement(0, wendiPos, fWENDIHousingLog,
        "WENDI_Housing", worldLog, false, 0, true);

    // ============================================================
    // LAYER 2: Outer Polyethylene Moderator
    // ============================================================
    G4Sphere* solidOuterModerator = new G4Sphere("WENDI_OuterModerator",
        0, fWENDIOuterRadius, 0, 2 * pi, 0, pi);
    fWENDIModeratorLog = new G4LogicalVolume(solidOuterModerator, fPolyethylene, "WENDI_OuterModerator_Log");
    new G4PVPlacement(0, wendiPos, fWENDIModeratorLog,
        "WENDI_OuterModerator", worldLog, false, 0, true);

    // ============================================================
    // LAYER 3: Tungsten-Loaded Polyethylene Layer
    // ============================================================
    G4Sphere* solidTungstenLayer = new G4Sphere("WENDI_TungstenLayer",
        fWENDITungstenLayerRadius, fWENDIOuterRadius, 0, 2 * pi, 0, pi);
    fWENDITungstenLayerLog = new G4LogicalVolume(solidTungstenLayer, fTungstenLoadedPoly, "WENDI_TungstenLayer_Log");
    new G4PVPlacement(0, G4ThreeVector(0, 0, 0), fWENDITungstenLayerLog,
        "WENDI_TungstenLayer", fWENDIModeratorLog, false, 0, true);

    // ============================================================
    // LAYER 4: Inner Polyethylene Moderator
    // ============================================================
    G4Sphere* solidInnerModerator = new G4Sphere("WENDI_InnerModerator",
        0, fWENDITungstenLayerRadius, 0, 2 * pi, 0, pi);
    fWENDIInnerModeratorLog = new G4LogicalVolume(solidInnerModerator, fPolyethylene, "WENDI_InnerModerator_Log");
    new G4PVPlacement(0, G4ThreeVector(0, 0, 0), fWENDIInnerModeratorLog,
        "WENDI_InnerModerator", fWENDIModeratorLog, false, 0, true);

    // ============================================================
    // ³He ACTIVE VOLUME - DIRECTLY IN INNER MODERATOR, NO BARRIERS
    // ============================================================
    G4Tubs* solidHelium3 = new G4Tubs("WENDI_Helium3",
        0, fWENDIInnerRadius - 0.05 * cm,  // 2.49cm radius - almost full inner moderator
        fWENDIHelium3Length / 2, 0, 2 * pi);
    fWENDIHelium3Log = new G4LogicalVolume(solidHelium3, fHelium3Gas, "WENDI_Helium3_Log");
    fWENDIHelium3Phys = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), fWENDIHelium3Log,
        "WENDI_Helium3", fWENDIInnerModeratorLog, false, 0, true);

    G4cout << "=== WENDI-II Built Successfully ===" << G4endl;
    G4cout << "³He volume: radius=" << (fWENDIInnerRadius - 0.05 * cm) / cm
        << " cm, length=" << fWENDIHelium3Length / cm << " cm" << G4endl;

    // ============================================================
    // FLUENCE DETECTOR (offset position)
    // ============================================================
    G4double fluenceDetectorRadius = 5.0 * cm;
    G4Sphere* solidFluenceDetector = new G4Sphere("FluenceDetector",
        0, fluenceDetectorRadius, 0, 2 * pi, 0, pi);
    fFluenceDetectorLog = new G4LogicalVolume(solidFluenceDetector, fAir, "FluenceDetector_Log");
    new G4PVPlacement(0, G4ThreeVector(20 * cm, 0, 100 * cm), fFluenceDetectorLog,
        "FluenceDetector", worldLog, false, 0, true);

    G4double detectorVolume = (4.0 / 3.0) * 3.14159 * pow(fluenceDetectorRadius / cm, 3);
    G4cout << "Fluence detector: radius=5 cm, volume=" << detectorVolume
        << " cm³, position=(20,0,100 cm)" << G4endl;
}
G4Material* DetectorConstruction::CreateMaterialFromProperties(const MaterialProperties& props)
{
    G4String name = props.name;
    G4double density = props.density * g/cm3;
    
    G4NistManager* nist = G4NistManager::Instance();
    G4Element* element = nist->FindOrBuildElement(props.atomicNumber);
    
    G4Material* material = new G4Material(name, density, 1);
    material->AddElement(element, 1.0);
    
    return material;
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    DefineMaterials();
    
    G4double worldSize = 5 * m;
    G4Box* solidWorld = new G4Box("World", worldSize/2, worldSize/2, worldSize/2);
    
    G4Material* vacuum = G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
    G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, vacuum, "World");
    fWorldPhys = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World", 0, false, 0, true);
    
    G4cout << "\n=== World Created: " << worldSize/m << " m ===\n" << G4endl;
    
    // Build WENDI-II detector
    BuildWENDI(logicWorld);
    
    // REMOVED: No target created here! Target will be created by SetMaterial in main
    
    return fWorldPhys;
}

void DetectorConstruction::ConstructSDandField()
{
    // ³He detector
    NeutronDoseSD* neutronDoseSD = new NeutronDoseSD("NeutronDose");
    G4SDManager::GetSDMpointer()->AddNewDetector(neutronDoseSD);
    
    if (fWENDIHelium3Log) {
        fWENDIHelium3Log->SetSensitiveDetector(neutronDoseSD);
        G4cout << "NeutronDoseSD attached to ³He volume" << G4endl;
    }
    
    // Fluence detector
    FluenceDetector* fluenceDetectorSD = new FluenceDetector("FluenceDetector");
    G4SDManager::GetSDMpointer()->AddNewDetector(fluenceDetectorSD);
    
    if (fFluenceDetectorLog) {
        fFluenceDetectorLog->SetSensitiveDetector(fluenceDetectorSD);
        
        G4double radius = 5.0 * cm;
        G4double volume = (4.0/3.0) * 3.14159 * pow(radius/cm, 3);
        fluenceDetectorSD->SetDetectorVolume(volume);
        
        G4cout << "FluenceDetector attached to air sphere" << G4endl;
        G4cout << "Fluence detector volume set to: " << volume << " cm³" << G4endl;
    }
}

// ============================================================
// THE SETMATERIAL FUNCTION WITH DEBUGGING
// ============================================================
void DetectorConstruction::SetMaterial(G4int index)
{
    G4cout << "\n!!! DEBUG: SetMaterial START !!!" << G4endl;
    G4cout << "!!! DEBUG: Input index = " << index << G4endl;
    G4cout << "!!! DEBUG: Database size = " << fMaterialDatabase.size() << G4endl;

    if (index < 0 || index >= (G4int)fMaterialDatabase.size()) {
        G4cerr << "ERROR: Material index " << index << " out of range" << G4endl;
        return;
    }

    G4cout << "!!! DEBUG: Index is valid" << G4endl;
    G4cout << "!!! DEBUG: Material name = " << fMaterialDatabase[index].name << G4endl;

    G4cout << "\n=========================================" << G4endl;
    G4cout << "CHANGING MATERIAL TO: " << fMaterialDatabase[index].name << G4endl;
    G4cout << "=========================================\n" << G4endl;

    fCurrentMaterialIndex = index;
    G4cout << "!!! DEBUG: fCurrentMaterialIndex set to " << fCurrentMaterialIndex << G4endl;

    auto it = fMaterialDatabase.find(fCurrentMaterialIndex);
    if (it == fMaterialDatabase.end()) {
        G4cerr << "!!! DEBUG: Material not found in database!" << G4endl;
        return;
    }

    MaterialProperties& props = it->second;
    fCurrentThickness = props.nuclearInteractionLength * cm;
    G4cout << "!!! DEBUG: Thickness set to " << fCurrentThickness / cm << " cm" << G4endl;

    // Delete old material
    if (fCurrentMaterial) {
        delete fCurrentMaterial;
        fCurrentMaterial = nullptr;
        G4cout << "!!! DEBUG: Old material deleted" << G4endl;
    }

    // Create new material
    fCurrentMaterial = CreateMaterialFromProperties(props);
    G4cout << "!!! DEBUG: New material created" << G4endl;

    // Get world volume
    G4LogicalVolume* worldLog = fWorldPhys->GetLogicalVolume();
    G4cout << "!!! DEBUG: World volume obtained" << G4endl;

    // If target doesn't exist yet, create it
    if (!fTargetPhys) {
        G4cout << "!!! DEBUG: Creating new target" << G4endl;
        G4Box* solidTarget = new G4Box("Target",
            fTargetSizeX / 2,
            fTargetSizeY / 2,
            fCurrentThickness / 2);

        fTargetLog = new G4LogicalVolume(solidTarget, fCurrentMaterial, "TargetLog");
        fTargetPhys = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), fTargetLog,
            "Target", worldLog, false, 0, true);

        G4cout << "Target CREATED: " << props.name
            << ", thickness=" << fCurrentThickness / cm << " cm" << G4endl;
    }
    // If target exists, update its material
    else {
        G4cout << "!!! DEBUG: Updating existing target" << G4endl;
        fTargetLog->SetMaterial(fCurrentMaterial);
        G4cout << "Target UPDATED: " << props.name
            << ", thickness=" << fCurrentThickness / cm << " cm" << G4endl;
    }

    G4cout << "!!! DEBUG: SetMaterial COMPLETE !!!\n" << G4endl;
}

G4String DetectorConstruction::GetCurrentMaterialName() const
{
    if (fCurrentMaterialIndex < 0) return "None";
    auto it = fMaterialDatabase.find(fCurrentMaterialIndex);
    return (it != fMaterialDatabase.end()) ? it->second.name : "Unknown";
}

G4double DetectorConstruction::GetCurrentTargetThickness() const
{
    return fCurrentThickness;
}

G4double DetectorConstruction::GetTargetVolume() const
{
    if (!fTargetLog || !fTargetLog->GetSolid()) return 0;
    return fTargetLog->GetSolid()->GetCubicVolume() / cm3;
}

void DetectorConstruction::PrintCurrentMaterial() const
{
    G4cout << "\n=== CURRENT MATERIAL ===" << G4endl;
    G4cout << "Index: " << fCurrentMaterialIndex << G4endl;
    G4cout << "Name: " << GetCurrentMaterialName() << G4endl;
    G4cout << "Thickness: " << fCurrentThickness/cm << " cm" << G4endl;
    G4cout << "========================\n" << G4endl;
}