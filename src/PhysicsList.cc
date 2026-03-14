#include "PhysicsList.hh"
#include "G4EmStandardPhysics.hh"
#include "G4HadronPhysicsFTFP_BERT_HP.hh"
#include "G4HadronElasticPhysicsHP.hh"
#include "G4DecayPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"

PhysicsList::PhysicsList() : G4VModularPhysicsList()
{
    defaultCutValue = 0.01 * mm;

    RegisterPhysics(new G4EmStandardPhysics());
    RegisterPhysics(new G4HadronPhysicsFTFP_BERT_HP("FTFP_BERT_HP"));
    RegisterPhysics(new G4HadronElasticPhysicsHP());
    RegisterPhysics(new G4DecayPhysics());
    RegisterPhysics(new G4RadioactiveDecayPhysics());
    
    G4cout << "\n=== Physics List Initialized ===" << G4endl;
    G4cout << "Hadronic: FTFP_BERT_HP (High Precision for neutrons)" << G4endl;
    G4cout << "Production threshold: " << defaultCutValue / mm << " mm" << G4endl;
    G4cout << "====================================\n" << G4endl;
}

PhysicsList::~PhysicsList() {}

void PhysicsList::SetCuts()
{
    SetCutsWithDefault();
    
    SetCutValue(0.01 * mm, "proton");
    SetCutValue(0.01 * mm, "neutron");
    SetCutValue(0.01 * mm, "pi-");
    SetCutValue(0.01 * mm, "pi+");
    SetCutValue(0.01 * mm, "e-");
    SetCutValue(0.01 * mm, "e+");
    SetCutValue(0.01 * mm, "gamma");
    
    if (verboseLevel > 0) {
        G4cout << "PhysicsList::SetCuts: cuts set to " << defaultCutValue / mm << " mm" << G4endl;
    }
}