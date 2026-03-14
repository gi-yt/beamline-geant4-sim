#include "DetectorConstruction.hh"
#include "FluenceDetector.hh"
#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4ios.hh"
#include <cmath>
#include <iomanip>
#include <stdexcept>

DetectorConstruction::DetectorConstruction()
: G4VUserDetectorConstruction(),
  fCurrentMaterialIndex(-1),
  fGeometryInitialized(false),
  fCurrentMaterial(nullptr),
  fCurrentThickness(0),
  fCurrentTargetSolid(nullptr),
  fTargetLog(nullptr),
  fTargetPhys(nullptr),
  fFluenceDetectorLog(nullptr),
  fFluenceDetectorPhys(nullptr),
  fWorldPhys(nullptr)
{
    fMaterialDatabase[0] = {"Lithium", 3.0, 6.94, 0.534, 154.5};
    fMaterialDatabase[1] = {"Aluminium", 13.0, 26.98, 2.70, 39.4};
    fMaterialDatabase[2] = {"Titanium", 22.0, 47.87, 4.51, 26.5};
    fMaterialDatabase[3] = {"Iron", 26.0, 55.85, 7.87, 16.8};
    fMaterialDatabase[4] = {"Cobalt", 27.0, 58.93, 8.90, 15.1};
    fMaterialDatabase[5] = {"Molybdenum", 42.0, 95.95, 10.2, 14.4};
    fMaterialDatabase[6] = {"Tin", 50.0, 118.71, 7.29, 21.4};
    fMaterialDatabase[7] = {"Hafnium", 72.0, 178.49, 13.3, 12.9};
    fMaterialDatabase[8] = {"Platinum", 78.0, 195.08, 21.4, 8.3};
    fMaterialDatabase[9] = {"Lead", 82.0, 207.2, 11.3, 17.1};
    fMaterialDatabase[10] = { "Tungsten", 74.0, 183.84, 19.3, 9.59 };
}

DetectorConstruction::~DetectorConstruction()
{
    fTargetPhys = nullptr;
    fTargetLog = nullptr;
    fCurrentTargetSolid = nullptr;
    fFluenceDetectorPhys = nullptr;
    fFluenceDetectorLog = nullptr;
    fWorldPhys = nullptr;
}

void DetectorConstruction::DefineMaterials()
{
    G4NistManager* nist = G4NistManager::Instance();
    G4Material* air = nist->FindOrBuildMaterial("G4_AIR");
    if (!air) {
        throw std::runtime_error("FATAL: Air material not found");
    }
}

void DetectorConstruction::BuildFluenceDetector(G4LogicalVolume* worldLog)
{
    G4Sphere* solidDetector = new G4Sphere("FluenceDetector",
        0, fDetectorRadius,
        0, 2 * pi,
        0, pi);
    
    G4Material* air = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
    fFluenceDetectorLog = new G4LogicalVolume(solidDetector, air, "FluenceDetector_Log");
    
    G4ThreeVector detectorPos(0, 0, fDetectorDistance);
    fFluenceDetectorPhys = new G4PVPlacement(0, detectorPos, fFluenceDetectorLog,
        "FluenceDetector", worldLog, false, 0, true);
}

// FIXED: Check for existing material before creating new one
G4Material* DetectorConstruction::CreateMaterialFromProperties(const MaterialProperties& props)
{
    // Check if material already exists in global table
    G4Material* existing = G4Material::GetMaterial(props.name, false);
    if (existing) {
        G4cout << "Reusing existing material: " << props.name << G4endl;
        return existing;
    }
    
    G4NistManager* nist = G4NistManager::Instance();
    G4Element* element = nist->FindOrBuildElement(props.atomicNumber);
    if (!element) {
        G4cerr << "ERROR: Element Z=" << props.atomicNumber << " not found" << G4endl;
        return nullptr;
    }
    
    G4Material* material = new G4Material(props.name, props.density * g/cm3, 1);
    material->AddElement(element, 1.0);
    G4cout << "Created new material: " << props.name << G4endl;
    return material;
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    try {
        DefineMaterials();
    } catch (const std::runtime_error& e) {
        G4cerr << e.what() << G4endl;
        exit(1);
    }
    
    G4double worldSize = 5 * m;
    G4Box* solidWorld = new G4Box("World", worldSize/2, worldSize/2, worldSize/2);
    
    G4Material* vacuum = G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
    G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, vacuum, "World");
    fWorldPhys = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World", 0, false, 0, true);
    
    BuildFluenceDetector(logicWorld);
    
    // FIXED: Build target directly without calling SetMaterial() which would trigger GeometryHasBeenModified
    if (fCurrentMaterialIndex >= 0 && fCurrentMaterial) {
        fCurrentTargetSolid = new G4Box("Target",
            fTargetSizeX/2, fTargetSizeY/2, fCurrentThickness/2);
        fTargetLog = new G4LogicalVolume(fCurrentTargetSolid, fCurrentMaterial, "TargetLog");
        fTargetPhys = new G4PVPlacement(0, G4ThreeVector(0,0,0), fTargetLog,
            "Target", logicWorld, false, 0, true);
        G4cout << "Target built during Construct() with material: " 
               << GetCurrentMaterialName() << G4endl;
    }
    
    fGeometryInitialized = true;
    
    return fWorldPhys;
}

void DetectorConstruction::RefreshPointers()
{
    if (!fWorldPhys) {
        G4cerr << "WARNING: Cannot refresh pointers - world not initialized" << G4endl;
        return;
    }
    
    G4LogicalVolume* worldLog = fWorldPhys->GetLogicalVolume();
    if (!worldLog) {
        G4cerr << "WARNING: Cannot refresh pointers - world logical volume not found" << G4endl;
        return;
    }
    
    bool targetFound = false;
    bool detectorFound = false;
    
    for (G4int i = 0; i < worldLog->GetNoDaughters(); i++) {
        G4VPhysicalVolume* pv = worldLog->GetDaughter(i);
        G4String name = pv->GetName();
        
        if (name == "Target") {
            fTargetPhys = pv;
            fTargetLog = pv->GetLogicalVolume();
            fCurrentTargetSolid = fTargetLog->GetSolid();
            targetFound = true;
        }
        else if (name == "FluenceDetector") {
            fFluenceDetectorPhys = pv;
            fFluenceDetectorLog = pv->GetLogicalVolume();
            detectorFound = true;
        }
    }
    
    if (!targetFound) {
        G4cerr << "WARNING: Target volume not found during pointer refresh" << G4endl;
    }
    if (!detectorFound) {
        G4cerr << "WARNING: FluenceDetector volume not found during pointer refresh" << G4endl;
    }
}

// FIXED: Check for existing SD before creating new one
void DetectorConstruction::ConstructSDandField()
{
    G4SDManager* sdManager = G4SDManager::GetSDMpointer();
    
    FluenceDetector* fluenceDetectorSD = dynamic_cast<FluenceDetector*>(
        sdManager->FindSensitiveDetector("FluenceDetector", false));
    
    if (!fluenceDetectorSD) {
        fluenceDetectorSD = new FluenceDetector("FluenceDetector");
        sdManager->AddNewDetector(fluenceDetectorSD);
        G4cout << "Created new FluenceDetector" << G4endl;
    } else {
        G4cout << "Reusing existing FluenceDetector" << G4endl;
    }
    
    if (fFluenceDetectorLog) {
        fFluenceDetectorLog->SetSensitiveDetector(fluenceDetectorSD);
    }
}

void DetectorConstruction::CleanupTarget()
{
    fTargetPhys = nullptr;
    fTargetLog = nullptr;
    fCurrentTargetSolid = nullptr;
}

void DetectorConstruction::UpdateTargetGeometry()
{
    if (!fWorldPhys) {
        G4cerr << "ERROR: World not initialized in UpdateTargetGeometry" << G4endl;
        return;
    }
    
    G4LogicalVolume* worldLog = fWorldPhys->GetLogicalVolume();
    if (!worldLog) {
        G4cerr << "ERROR: World logical volume not found" << G4endl;
        return;
    }
    
    CleanupTarget();
    
    fCurrentTargetSolid = new G4Box("Target",
        fTargetSizeX / 2,
        fTargetSizeY / 2,
        fCurrentThickness / 2);
    
    fTargetLog = new G4LogicalVolume(fCurrentTargetSolid, fCurrentMaterial, "TargetLog");
    
    fTargetPhys = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), fTargetLog,
        "Target", worldLog, false, 0, true);
    
    G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

void DetectorConstruction::SetMaterial(G4int index)
{
    if (index < 0 || index >= (G4int)fMaterialDatabase.size()) {
        G4cerr << "ERROR: Material index " << index << " out of range" << G4endl;
        return;
    }

    fCurrentMaterialIndex = index;
    MaterialProperties props = fMaterialDatabase.at(index);
    fCurrentThickness = props.nuclearInteractionLength * cm;

    fCurrentMaterial = CreateMaterialFromProperties(props);
    if (!fCurrentMaterial) {
        G4cerr << "ERROR: Failed to create material " << props.name << G4endl;
        return;
    }

    if (fGeometryInitialized) {
        UpdateTargetGeometry();
    } else {
        G4cout << "Material " << props.name << " selected (will be created during Construct())" << G4endl;
    }
}

G4String DetectorConstruction::GetCurrentMaterialName() const
{
    if (fCurrentMaterialIndex < 0) return "None";
    return fMaterialDatabase.at(fCurrentMaterialIndex).name;
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