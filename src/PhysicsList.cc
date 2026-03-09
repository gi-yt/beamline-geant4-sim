#include "PhysicsList.hh"
#include "G4EmStandardPhysics.hh"
#include "G4HadronPhysicsFTFP_BERT_HP.hh"
#include "G4HadronElasticPhysicsHP.hh"
#include "G4DecayPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4NeutronTrackingCut.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"

PhysicsList::PhysicsList()
    : G4VModularPhysicsList()
{
    // Set default cut value - low enough for neutron cascade tracking
    defaultCutValue = 0.01 * mm;

    // Standard EM Physics
    RegisterPhysics(new G4EmStandardPhysics());

    // Hadronic Physics - FTFP_BERT_HP (High Precision for neutrons below 20 MeV)
    // The _HP version is essential for accurate neutron detection in ³He tubes
    RegisterPhysics(new G4HadronPhysicsFTFP_BERT_HP());

    // Elastic scattering for neutrons (HP version)
    RegisterPhysics(new G4HadronElasticPhysicsHP());

    // Decay physics for unstable particles
    RegisterPhysics(new G4DecayPhysics());

    // Radioactive decay for activation products
    RegisterPhysics(new G4RadioactiveDecayPhysics());

    // Optional: Kill neutrons below a certain energy to save CPU
    // RegisterPhysics(new G4NeutronTrackingCut());

    G4cout << "\n=== Physics List Initialized ===" << G4endl;
    G4cout << "Hadronic: FTFP_BERT_HP (High Precision for neutrons)" << G4endl;
    G4cout << "Elastic: G4HadronElasticPhysicsHP" << G4endl;
    G4cout << "Production threshold: " << defaultCutValue / mm << " mm" << G4endl;
    G4cout << "====================================\n" << G4endl;
}

PhysicsList::~PhysicsList()
{
    G4cout << "PhysicsList destroyed" << G4endl;
}

void PhysicsList::SetCuts()
{
    // Set default cuts first
    SetCutsWithDefault();

    // Override cuts for specific particles to ensure good neutron tracking
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