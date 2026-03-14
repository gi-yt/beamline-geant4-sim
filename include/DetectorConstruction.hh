#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include <map>
#include <vector>

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4VSolid;
class G4Element;

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    DetectorConstruction();
    virtual ~DetectorConstruction();

    virtual G4VPhysicalVolume* Construct() override;
    virtual void ConstructSDandField() override;

    void SetMaterial(G4int index);
    void RefreshPointers();
    G4String GetCurrentMaterialName() const;
    G4double GetCurrentTargetThickness() const;
    G4double GetTargetVolume() const;
    const G4VPhysicalVolume* GetTargetPV() const { return fTargetPhys; }
    G4int GetCurrentMaterialIndex() const { return fCurrentMaterialIndex; }
    const G4VPhysicalVolume* GetFluenceDetectorPV() const { return fFluenceDetectorPhys; }
    void PrintCurrentMaterial() const;
    G4bool IsGeometryInitialized() const { return fGeometryInitialized; }
    
    struct MaterialProperties {
        G4String name;
        G4double atomicNumber;
        G4double atomicMass;
        G4double density;
        G4double nuclearInteractionLength;
    };
    
private:
    void DefineMaterials();
    void BuildFluenceDetector(G4LogicalVolume* worldLog);
    G4Material* CreateMaterialFromProperties(const MaterialProperties& props);
    void UpdateTargetGeometry();
    void CleanupTarget();
    
    std::map<G4int, MaterialProperties> fMaterialDatabase;
    G4int fCurrentMaterialIndex;
    G4bool fGeometryInitialized;
    
    G4Material* fCurrentMaterial;
    G4double fCurrentThickness;
    G4VSolid* fCurrentTargetSolid;
    
    G4LogicalVolume* fTargetLog;
    G4VPhysicalVolume* fTargetPhys;
    G4LogicalVolume* fFluenceDetectorLog;
    G4VPhysicalVolume* fFluenceDetectorPhys;
    G4VPhysicalVolume* fWorldPhys;
    
    static constexpr G4double fTargetSizeX = 5.0 * cm;
    static constexpr G4double fTargetSizeY = 5.0 * cm;
    static constexpr G4double fDetectorRadius = 5.0 * cm;
    static constexpr G4double fDetectorDistance = 30.0 * cm;
};

#endif