#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include <map>
#include <vector>

// Forward declarations
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

    virtual G4VPhysicalVolume* Construct();
    virtual void ConstructSDandField();

    // Material selection methods
    void SetMaterial(G4int index);
    G4String GetCurrentMaterialName() const;
    G4double GetCurrentTargetThickness() const;
    G4double GetTargetVolume() const;
    const G4VPhysicalVolume* GetTargetPV() const { return fTargetPhys; }

    G4int GetCurrentMaterialIndex() const { return fCurrentMaterialIndex; }

    // WENDI-II detector access
    const G4VPhysicalVolume* GetWENDIModeratorPV() const { return fWENDIModeratorPhys; }
    const G4VPhysicalVolume* GetWENDIHelium3PV() const { return fWENDIHelium3Phys; }

    // Fluence detector access
    const G4VPhysicalVolume* GetFluenceDetectorPV() const { return fFluenceDetectorPhys; }

    void PrintCurrentMaterial() const;

    // Material properties structure
    struct MaterialProperties {
        G4String name;
        G4double atomicNumber;
        G4double atomicMass;
        G4double density;
        G4double nuclearInteractionLength;
    };

private:
    void DefineMaterials();
    void BuildWENDI(G4LogicalVolume* worldLog);
    G4Material* CreateMaterialFromProperties(const MaterialProperties& props);

    // Material database for target metals
    std::map<G4int, MaterialProperties> fMaterialDatabase;
    G4int fCurrentMaterialIndex;

    // Target materials
    G4Material* fCurrentMaterial;
    G4double fCurrentThickness;

    // WENDI-II detector materials
    G4Material* fPolyethylene;
    G4Material* fTungstenLoadedPoly;
    G4Material* fHelium3Gas;
    G4Material* fStainlessSteel;
    G4Material* fAluminum;
    G4Material* fAir;

    // Geometry volumes
    G4LogicalVolume* fTargetLog;
    G4VPhysicalVolume* fTargetPhys;

    // WENDI-II components
    G4LogicalVolume* fWENDIModeratorLog;
    G4VPhysicalVolume* fWENDIModeratorPhys;
    G4LogicalVolume* fWENDITungstenLayerLog;
    G4VPhysicalVolume* fWENDITungstenLayerPhys;
    G4LogicalVolume* fWENDIInnerModeratorLog;
    G4VPhysicalVolume* fWENDIInnerModeratorPhys;
    G4LogicalVolume* fWENDIHelium3Log;
    G4VPhysicalVolume* fWENDIHelium3Phys;
    G4LogicalVolume* fWENDICounterWallLog;
    G4VPhysicalVolume* fWENDICounterWallPhys;
    G4LogicalVolume* fWENDIHousingLog;
    G4VPhysicalVolume* fWENDIHousingPhys;

    // Fluence detector
    G4LogicalVolume* fFluenceDetectorLog;
    G4VPhysicalVolume* fFluenceDetectorPhys;

    // World
    G4VPhysicalVolume* fWorldPhys;

    // Target dimensions
    static constexpr G4double fTargetSizeX = 5.0 * cm;
    static constexpr G4double fTargetSizeY = 5.0 * cm;

    // WENDI-II dimensions
    static constexpr G4double fWENDIOuterRadius = 11.5 * cm;
    static constexpr G4double fWENDITungstenLayerRadius = 9.5 * cm;
    static constexpr G4double fWENDIInnerRadius = 2.54 * cm;
    static constexpr G4double fWENDIHelium3Length = 15.0 * cm;
    static constexpr G4double fWENDIWallThickness = 0.1 * cm;
};

#endif