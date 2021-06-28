// -*- C++ -*-
//
// Package:    TreeMaker/Ntuplzr
// Class:      Validator
// 
/**\class Validator Validator.cc TreeMaker/Validator/plugins/Validator.cc
/
/Description: [one line class summary]
/
/Implementation:
/    [Notes on implementation]
*/
//
// Original Author:  Sandhya Jain
//         Created:  Wed, 29 Mar 2017 05:29:37 GMT
//
//


// system include files
#include <memory>
#include <iostream>
// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "RecoEgamma/EgammaTools/interface/ConversionTools.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "PhysicsTools/SelectorUtils/interface/PFJetIDSelectionFunctor.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexFitter.h"
#include "SimTracker/Records/interface/TrackAssociatorRecord.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "Geometry/GEMGeometry/interface/ME0EtaPartitionSpecs.h"
#include "Geometry/GEMGeometry/interface/ME0Geometry.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "RecoVertex/KinematicFitPrimitives/interface/ParticleMass.h"
#include <RecoVertex/KinematicFitPrimitives/interface/KinematicParticleFactoryFromTransientTrack.h>
#include "RecoVertex/KinematicFitPrimitives/interface/KinematicVertex.h"
#include "RecoVertex/KinematicFit/interface/KinematicParticleVertexFitter.h"
#include "RecoVertex/KinematicFit/interface/KinematicParticleFitter.h"
#include "CommonTools/CandUtils/interface/AddFourMomenta.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"

#include "SimDataFormats/Vertex/interface/SimVertex.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include "Math/GenVector/VectorUtil.h"

using namespace std;
using namespace reco;
//
/// class declaration
///
//
/// If the analyzer does not use TFileService, please remove
/// the template argument to the base class so the class inherits
/// from  edm::one::EDAnalyzer<> and also remove the line from
/// constructor "usesResource("TFileService");"
/// This will improve performance in multithreaded jobs.
const Int_t kMaxVertices = 300;
const Int_t kMaxWeights = 1500;
const Int_t kMaxParticle = 10000;
const Int_t kMaxProton = 10000;
const Int_t kMaxGenJet = 800;
const Int_t kMaxPhoton = 800;
const Int_t kMaxElectron = 200;
const Int_t kMaxMuonLoose = 200;
const Int_t kMaxJet = 800;
const Int_t kMaxTau = 200;
const Int_t kMaxMissingET = 1;

class Validator : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
public:
  explicit Validator(const edm::ParameterSet&);
  ~Validator();
  
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  
private:
  virtual void beginJob() override;
  virtual void beginRun(edm::Run const&, edm::EventSetup const&) ;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endRun(edm::Run const&, edm::EventSetup const&) ;
  virtual void endJob() override;
  
  bool isME0MuonSelNew(reco::Muon, double, double, double, edm::EventSetup const& );
  float calculate_demetraIsolation(const pat::Tau&) const;
  bool debug_, extendFormat_, isSignalMC_;
  edm::Service<TFileService> fs_;
  edm::EDGetTokenT<std::vector<reco::Vertex>>         verticesToken_     ;
  edm::EDGetTokenT<std::vector<reco::Vertex>>         vertices4DToken_   ;
  edm::EDGetTokenT<std::vector<pat::PackedCandidate>> pfCandidToken_     ;
  edm::EDGetTokenT<std::vector<PileupSummaryInfo>>    pileUpToken_       ;
  edm::EDGetTokenT<std::vector<reco::GenParticle>>    genPartsToken_     ;
  edm::EDGetTokenT<std::vector<reco::GenJet>>         genJetsToken_      ;
  edm::EDGetTokenT<std::vector<reco::GenMET>>         genMetToken_       ;
  edm::EDGetTokenT<std::vector<pat::Photon>>          photnsToken_       ; 
  edm::EDGetTokenT<std::vector<pat::Electron>>        elecsToken_        ;
  //edm::EDGetTokenT<std::vector<reco::GsfElectron>>  elecsToken_  ;
  edm::EDGetTokenT<std::vector<pat::Muon>>            muonsToken_        ;
  edm::EDGetTokenT<std::vector<pat::Tau>>             tausToken_         ;
  edm::EDGetTokenT<std::vector<pat::Jet>>             jetsToken_         ;
  edm::EDGetTokenT<std::vector<pat::Jet>>             jetschsToken_      ;
  edm::EDGetTokenT<std::vector<pat::Jet>>             fatjetsToken_      ;
  edm::EDGetTokenT<std::vector<pat::MET>>             metToken_          ;
  edm::EDGetTokenT<std::vector<pat::MET>>             metpfToken_        ;
  //edm::EDGetTokenT<std::vector<reco::Conversion>>  convToken_       ;
  edm::EDGetTokenT<std::vector<reco::GenParticle>>    gePUProtonToken_   ;
  edm::EDGetTokenT<float>                             genT0Token_        ;

  const ME0Geometry*      ME0Geometry_;
  
  TTree* mytree;
  int evt_size;
  
  int vtx_size;
  float vtx_x[kMaxVertices], vtx_y[kMaxVertices], vtx_z[kMaxVertices];
  float vtx_pt2[kMaxVertices];

  int vtx4D_size;
  float vtx4D_x[kMaxVertices], vtx4D_y[kMaxVertices], vtx4D_z[kMaxVertices], vtx4D_t[kMaxVertices], vtx4D_terr[kMaxVertices];
  float vtx4D_pt2[kMaxVertices];


  int pfcand_size,  pfcand_pid[kMaxParticle];
  float pfcand_pt[kMaxParticle],pfcand_eta[kMaxParticle],pfcand_phi[kMaxParticle],pfcand_mass[kMaxParticle],pfcand_vz[kMaxParticle];
  float pfcand_t[kMaxParticle], pfcand_terr[kMaxParticle];

  int npuVertices;
  float trueInteractions; 
  
  int genpart_size;
  float genpart_pt[kMaxParticle],genpart_eta[kMaxParticle],genpart_phi[kMaxParticle],genpart_mass[kMaxParticle];
  int genpart_pid[kMaxParticle], genpart_status[kMaxParticle], genpart_m1[kMaxParticle], genpart_m2[kMaxParticle], genpart_d1[kMaxParticle], genpart_d2[kMaxParticle] ;

  // JH
  int genproton_size;
  float genproton_xi[kMaxProton], genproton_pz[kMaxProton], genproton_vz[kMaxProton];
  int genproton_ispu[kMaxProton];
  
  // JH
  float gen_t0;

  int genjet_size;
  float genjet_pt[kMaxGenJet], genjet_eta[kMaxGenJet], genjet_phi[kMaxGenJet], genjet_mass[kMaxGenJet];
  
  int genmet_size;
  float genmet_pt[kMaxMissingET],genmet_phi[kMaxMissingET];
  
  int gamma_size;
  float MVAgamma_[kMaxPhoton],gamma_pt[kMaxPhoton], gamma_eta[kMaxPhoton], gamma_phi[kMaxPhoton],gamma_mass[kMaxPhoton],gamma_reliso[kMaxPhoton], gamma_idvar[kMaxPhoton];
  uint32_t gamma_isopass[kMaxPhoton], gamma_idpass[kMaxPhoton];
  
  int elec_size, elec_charge[kMaxElectron];
  float elec_pt[kMaxElectron], elec_eta[kMaxElectron], elec_phi[kMaxElectron], elec_reliso[kMaxElectron], elec_mass[kMaxElectron], elec_idvar[kMaxElectron];
  uint32_t elec_isopass[kMaxElectron],elec_idpass[kMaxElectron];
  
  int muon_size, muon_charge[kMaxMuonLoose];
  float muon_pt[kMaxMuonLoose], muon_eta[kMaxMuonLoose], muon_phi[kMaxMuonLoose], muon_reliso[kMaxMuonLoose], muon_mass[kMaxMuonLoose], muon_idvar[kMaxMuonLoose];
  uint32_t muon_isopass[kMaxMuonLoose], muon_idpass[kMaxMuonLoose] ;

  int tau_size, tau_charge[kMaxTau];
  float tau_decaymode[kMaxTau], tau_neutraliso[kMaxTau], tau_chargediso[kMaxTau], tau_combinediso[kMaxTau], tau_pt[kMaxTau], tau_eta[kMaxTau], tau_phi[kMaxTau], tau_mass[kMaxTau]; 
  uint32_t tau_isopass[kMaxTau];
  
  int jetpuppi_size; 
  float jetpuppi_pt[kMaxJet], jetpuppi_eta[kMaxJet], jetpuppi_phi[kMaxJet], jetpuppi_mass[kMaxJet];
  uint32_t jetpuppi_idpass[kMaxJet];
  float jetpuppi_DeepJET[kMaxJet];
  uint32_t jetpuppi_btag[kMaxJet];

  int jetchs_size; 
  float jetchs_pt[kMaxJet], jetchs_eta[kMaxJet], jetchs_phi[kMaxJet], jetchs_mass[kMaxJet];
  uint32_t jetchs_idpass[kMaxJet];
  float jetchs_DeepJET[kMaxJet];
  uint32_t jetchs_btag[kMaxJet];

  int fatjet_size;
  float fatjet_pt[kMaxJet], fatjet_eta[kMaxJet], fatjet_phi[kMaxJet], fatjet_mass[kMaxJet];
  float fatjet_tau1[kMaxJet], fatjet_tau2[kMaxJet], fatjet_tau3[kMaxJet], fatjet_tau4[kMaxJet];
  float fatjet_msoftdrop[kMaxJet];
  float fatjet_particleNet_TvsQCD[kMaxJet], fatjet_particleNet_WvsQCD[kMaxJet];
  float fatjet_particleNetMD_XbbvsQCD[kMaxJet];

  int metpuppi_size;
  float metpuppi_pt[kMaxMissingET],metpuppi_phi[kMaxMissingET];

  int metpf_size;
  float metpf_pt[kMaxMissingET],metpf_phi[kMaxMissingET];


};



Validator::Validator(const edm::ParameterSet& iConfig):
  debug_(iConfig.getParameter<bool>("debug")),
  extendFormat_(iConfig.getParameter<bool>("extendFormat")),
  isSignalMC_(iConfig.getParameter<bool>("isSignalMC")),
  verticesToken_(consumes<std::vector<reco::Vertex>>(iConfig.getParameter<edm::InputTag>("vertices"))),
  vertices4DToken_(consumes<std::vector<reco::Vertex>>(iConfig.getParameter<edm::InputTag>("vertices4D"))),
  pfCandidToken_(consumes<std::vector<pat::PackedCandidate>>(iConfig.getParameter<edm::InputTag>("pfCandid"))),
  pileUpToken_(consumes<std::vector<PileupSummaryInfo>>(iConfig.getParameter<edm::InputTag>("pileUp"))),
  genPartsToken_(consumes<std::vector<reco::GenParticle>>(iConfig.getParameter<edm::InputTag>("genParts"))),
  genJetsToken_(consumes<std::vector<reco::GenJet>>(iConfig.getParameter<edm::InputTag>("genJets"))),
  genMetToken_(consumes<std::vector<reco::GenMET>>(iConfig.getParameter<edm::InputTag>("genMet"))),
//photnsToken_(consumes<std::vector<pat::Photon>>(iConfig.getParameter<edm::InputTag>("photons"))),
  //elecsToken_(consumes<std::vector<pat::Electron>>(iConfig.getParameter<edm::InputTag>("electrons"))),
//elecsToken_(consumes<std::vector<reco::GsfElectron>>(iConfig.getParameter<edm::InputTag>("electrons"))),
  muonsToken_(consumes<std::vector<pat::Muon>>(iConfig.getParameter<edm::InputTag>("muons"))),
  tausToken_(consumes<std::vector<pat::Tau>>(iConfig.getParameter<edm::InputTag>("taus"))),
  jetsToken_(consumes<std::vector<pat::Jet>>(iConfig.getParameter<edm::InputTag>("jets"))),
  jetschsToken_(consumes<std::vector<pat::Jet>>(iConfig.getParameter<edm::InputTag>("jetschs"))),
  fatjetsToken_(consumes<std::vector<pat::Jet>>(iConfig.getParameter<edm::InputTag>("fatjets"))),
  metToken_(consumes<std::vector<pat::MET>>(iConfig.getParameter<edm::InputTag>("met"))),
  metpfToken_(consumes<std::vector<pat::MET>>(iConfig.getParameter<edm::InputTag>("metpf"))),
  gePUProtonToken_(consumes<std::vector<reco::GenParticle>>(iConfig.getParameter<edm::InputTag>("genPUProtons"))),
  genT0Token_(consumes<float>(edm::InputTag("genParticles:t0")))

  { 
    if(debug_)  std::cout<<"Here I am : in constructor "<<std::endl;

    ME0Geometry_ = 0;
    evt_size     = 0;
    usesResource("TFileService");
    mytree   = fs_->make<TTree>("mytree","TestTree");
    mytree->Branch("evt_size",&evt_size, "evt_size/I");

    mytree->Branch("vtx_size",&vtx_size, "vtx_size/I");
    mytree->Branch("vtx_x",vtx_x, "vtx_x[vtx_size]/F");
    mytree->Branch("vtx_y",vtx_y, "vtx_y[vtx_size]/F");
    mytree->Branch("vtx_z",vtx_z, "vtx_z[vtx_size]/F");
    mytree->Branch("vtx_pt2",vtx_pt2, "vtx_pt2[vtx_size]/F");

    if(extendFormat_)
      {
	mytree->Branch("vtx4D_size",&vtx4D_size, "vtx4D_size/I");
	mytree->Branch("vtx4D_x",vtx4D_x, "vtx4D_x[vtx4D_size]/F");
	mytree->Branch("vtx4D_y",vtx4D_y, "vtx4D_y[vtx4D_size]/F");
	mytree->Branch("vtx4D_z",vtx4D_z, "vtx4D_z[vtx4D_size]/F");
	mytree->Branch("vtx4D_t",vtx4D_t, "vtx4D_t[vtx4D_size]/F");
	mytree->Branch("vtx4D_terr",vtx4D_terr, "vtx4D_terr[vtx4D_size]/F");
	mytree->Branch("vtx4D_pt2",vtx4D_pt2, "vtx4D_pt2[vtx4D_size]/F");

	mytree->Branch("pfcand_size",&pfcand_size, "pfcand_size/I");
	mytree->Branch("pfcand_pid", pfcand_pid, "pfcand_pid[pfcand_size]/I");
	mytree->Branch("pfcand_pt",pfcand_pt, "pfcand_pt[pfcand_size]/F");
	mytree->Branch("pfcand_eta",pfcand_eta, "pfcand_eta[pfcand_size]/F");
	mytree->Branch("pfcand_phi",pfcand_phi, "pfcand_phi[pfcand_size]/F");
	mytree->Branch("pfcand_mass",pfcand_mass, "pfcand_mass[pfcand_size]/F");
	mytree->Branch("pfcand_t",pfcand_t, "pfcand_t[pfcand_size]/F");
	mytree->Branch("pfcand_terr",pfcand_terr, "pfcand_terr[pfcand_size]/F");
	mytree->Branch("pfcand_vz",pfcand_vz, "pfcand_vz[pfcand_size]/F");

      }

    mytree->Branch("npuVertices",&npuVertices, "npuVertices/I");
    mytree->Branch("trueInteractions",&trueInteractions, "trueInteractions/F");
    
    mytree->Branch("genpart_size",&genpart_size, "genpart_size/I");
    mytree->Branch("genpart_pid", genpart_pid, "genpart_pid[genpart_size]/I");
    mytree->Branch("genpart_status",genpart_status, "genpart_status[genpart_size]/I");
    mytree->Branch("genpart_pt",genpart_pt, "genpart_pt[genpart_size]/F");
    mytree->Branch("genpart_eta",genpart_eta, "genpart_eta[genpart_size]/F");
    mytree->Branch("genpart_phi",genpart_phi, "genpart_phi[genpart_size]/F");
    mytree->Branch("genpart_mass",genpart_mass, "genpart_mass[genpart_size]/F");
    mytree->Branch("genpart_m1",genpart_m1, "genpart_m1[genpart_size]/I");
    mytree->Branch("genpart_m2",genpart_m2, "genpart_m2[genpart_size]/I");
    mytree->Branch("genpart_d1",genpart_d1, "genpart_d1[genpart_size]/I");
    mytree->Branch("genpart_d2",genpart_d2, "genpart_d2[genpart_size]/I");
    
    mytree->Branch("genjet_size",&genjet_size, "genjet_size/I");
    mytree->Branch("genjet_pt",genjet_pt, "genjet_pt[genjet_size]/F");
    mytree->Branch("genjet_eta",genjet_eta, "genjet_eta[genjet_size]/F");
    mytree->Branch("genjet_phi",genjet_phi, "genjet_phi[genjet_size]/F");
    mytree->Branch("genjet_mass",genjet_mass, "genjet_mass[genjet_size]/F");

    mytree->Branch("genmet_size",&genmet_size, "genmet_size/I");
    mytree->Branch("genmet_pt", genmet_pt, "genmet_pt[genmet_size]/F");
    mytree->Branch("genmet_phi",genmet_phi, "genmet_phi[genmet_size]/F");

    mytree->Branch("genproton_size",&genproton_size, "genproton_size/I");
    mytree->Branch("genproton_xi",&genproton_xi,"genproton_xi[genproton_size]/F");
    mytree->Branch("genproton_pz",&genproton_pz,"genproton_pz[genproton_size]/F");
    mytree->Branch("genproton_vz",&genproton_vz,"genproton_vz[genproton_size]/F");
    mytree->Branch("genproton_ispu",&genproton_ispu,"genproton_ispu[genproton_size]/I");

    mytree->Branch("genvtx_t0",&gen_t0,"genvtx_t0/F");

    mytree->Branch("gamma_size",&gamma_size, "gamma_size/I");
    mytree->Branch("gamma_pt",gamma_pt, "gamma_pt[gamma_size]/F");
    mytree->Branch("gamma_eta",gamma_eta, "gamma_eta[gamma_size]/F");
    mytree->Branch("gamma_phi",gamma_phi, "gamma_phi[gamma_size]/F");
    mytree->Branch("gamma_mass",gamma_mass, "gamma_mass[gamma_size]/F");
    mytree->Branch("gamma_idvar", gamma_idvar, "gamma_idvar[gamma_size]/F");
    mytree->Branch("gamma_reliso",gamma_reliso, "gamma_reliso[gamma_size]/F");
    mytree->Branch("gamma_idpass", gamma_idpass, "gamma_idpass[gamma_size]/i");
    mytree->Branch("gamma_isopass", gamma_isopass, "gamma_isopass[gamma_size]/i");
    
    mytree->Branch("elec_size",&elec_size, "elec_size/I");
    mytree->Branch("elec_pt",elec_pt, "elec_pt[elec_size]/F");
    mytree->Branch("elec_eta",elec_eta, "elec_eta[elec_size]/F");
    mytree->Branch("elec_phi",elec_phi, "elec_phi[elec_size]/F");
    mytree->Branch("elec_mass",elec_mass, "elec_mass[elec_size]/F");
    mytree->Branch("elec_charge",elec_charge, "elec_charge[elec_size]/I");
    mytree->Branch("elec_idvar", elec_idvar, "elec_idvar[elec_size]/F");
    mytree->Branch("elec_reliso",elec_reliso, "elec_reliso[elec_size]/F");
    mytree->Branch("elec_idpass",elec_idpass, "elec_idpass[elec_size]/i");
    mytree->Branch("elec_isopass", elec_isopass, "elec_isopass[elec_size]/i");
    
    mytree->Branch("muon_size",&muon_size, "muon_size/I");
    mytree->Branch("muon_pt",muon_pt, "muon_pt[muon_size]/F");
    mytree->Branch("muon_eta",muon_eta, "muon_eta[muon_size]/F");
    mytree->Branch("muon_phi",muon_phi, "muon_phi[muon_size]/F");
    mytree->Branch("muon_mass",muon_mass, "muon_mass[muon_size]/F");
    mytree->Branch("muon_charge",muon_charge, "muon_charge[muon_size]/I");
    mytree->Branch("muon_idvar", muon_idvar, "muon_idvar[muon_size]/F");
    mytree->Branch("muon_reliso",muon_reliso, "muon_reliso[muon_size]/F");
    mytree->Branch("muon_idpass", muon_idpass, "muon_idpass[muon_size]/i");
    mytree->Branch("muon_isopass", muon_isopass, "muon_isopass[muon_size]/i");
    
    mytree->Branch("tau_size",&tau_size, "tau_size/I");
    mytree->Branch("tau_pt",tau_pt, "tau_pt[tau_size]/F");
    mytree->Branch("tau_eta",tau_eta, "tau_eta[tau_size]/F");
    mytree->Branch("tau_phi",tau_phi, "tau_phi[tau_size]/F");
    mytree->Branch("tau_mass",tau_mass, "tau_mass[tau_size]/F");
    mytree->Branch("tau_charge",tau_charge, "tau_charge[tau_size]/I");
    mytree->Branch("tau_decaymode",tau_decaymode, "tau_decaymode[tau_size]/F");
    mytree->Branch("tau_chargediso",tau_chargediso, "tau_chargediso[tau_size]/F");
    mytree->Branch("tau_neutraliso",tau_neutraliso, "tau_neutraliso[tau_size]/F");
    mytree->Branch("tau_combinediso",tau_combinediso, "tau_combinediso[tau_size]/F");
    mytree->Branch("tau_isopass", tau_isopass, "tau_isopass[tau_size]/i");
    
    mytree->Branch("jetpuppi_size",&jetpuppi_size, "jetpuppi_size/I");
    mytree->Branch("jetpuppi_pt",jetpuppi_pt, "jetpuppi_pt[jetpuppi_size]/F");
    mytree->Branch("jetpuppi_eta",jetpuppi_eta, "jetpuppi_eta[jetpuppi_size]/F");
    mytree->Branch("jetpuppi_phi",jetpuppi_phi, "jetpuppi_phi[jetpuppi_size]/F");
    mytree->Branch("jetpuppi_mass",jetpuppi_mass, "jetpuppi_mass[jetpuppi_size]/F");
    mytree->Branch("jetpuppi_idpass", jetpuppi_idpass, "jetpuppi_idpass[jetpuppi_size]/i");
    mytree->Branch("jetpuppi_DeepJET",jetpuppi_DeepJET,"jetpuppi_DeepJET[jetpuppi_size]/F");
    mytree->Branch("jetpuppi_btag",jetpuppi_btag,"jetpuppi_btag[jetpuppi_size]/i");

    mytree->Branch("jetchs_size",&jetchs_size, "jetchs_size/I");
    mytree->Branch("jetchs_pt",jetchs_pt, "jetchs_pt[jetchs_size]/F");
    mytree->Branch("jetchs_eta",jetchs_eta, "jetchs_eta[jetchs_size]/F");
    mytree->Branch("jetchs_phi",jetchs_phi, "jetchs_phi[jetchs_size]/F");
    mytree->Branch("jetchs_mass",jetchs_mass, "jetchs_mass[jetchs_size]/F");
    mytree->Branch("jetchs_idpass", jetchs_idpass, "jetchs_idpass[jetchs_size]/i");
    mytree->Branch("jetchs_DeepJET",jetchs_DeepJET,"jetchs_DeepJET[jetchs_size]/F");
    mytree->Branch("jetchs_btag",jetchs_btag,"jetchs_btag[jetchs_size]/i");

    mytree->Branch("fatjet_size", &fatjet_size, "fatjet_size/I");
    mytree->Branch("fatjet_pt", fatjet_pt, "fatjet_pt[fatjet_size]/F");
    mytree->Branch("fatjet_eta", fatjet_eta, "fatjet_eta[fatjet_size]/F");
    mytree->Branch("fatjet_phi", fatjet_phi, "fatjet_phi[fatjet_size]/F");
    mytree->Branch("fatjet_mass", fatjet_mass, "fatjet_mass[fatjet_size]/F");
    mytree->Branch("fatjet_tau1", fatjet_tau1, "fatjet_tau1[fatjet_size]/F");
    mytree->Branch("fatjet_tau2", fatjet_tau2, "fatjet_tau2[fatjet_size]/F");
    mytree->Branch("fatjet_tau3", fatjet_tau3, "fatjet_tau3[fatjet_size]/F");
    mytree->Branch("fatjet_tau4", fatjet_tau4, "fatjet_tau4[fatjet_size]/F");
    mytree->Branch("fatjet_msoftdrop", fatjet_msoftdrop, "fatjet_msoftdrop[fatjet_size]/F");
    mytree->Branch("fatjet_particleNet_TvsQCD", fatjet_particleNet_TvsQCD, "fatjet_particleNet_TvsQCD[fatjet_size]/F");
    mytree->Branch("fatjet_particleNet_WvsQCD", fatjet_particleNet_WvsQCD, "fatjet_particleNet_WvsQCD[fatjet_size]/F");
    mytree->Branch("fatjet_particleNetMD_XbbvsQCD", fatjet_particleNetMD_XbbvsQCD, "fatjet_particleNetMD_XbbvsQCD[fatjet_size]/F");

    mytree->Branch("metpuppi_size",&metpuppi_size, "metpuppi_size/I");
    mytree->Branch("metpuppi_pt", metpuppi_pt, "metpuppi_pt[metpuppi_size]/F");
    mytree->Branch("metpuppi_phi",metpuppi_phi, "metpuppi_phi[metpuppi_size]/F");

    mytree->Branch("metpf_size",&metpf_size, "metpf_size/I");
    mytree->Branch("metpf_pt", metpf_pt, "metpf_pt[metpf_size]/F");
    mytree->Branch("metpf_phi",metpf_phi, "metpf_phi[metpf_size]/F");


    if(debug_)      std::cout<<"Here I am : ending constructor "<<std::endl;
}
 

Validator::~Validator()
{}

//
// member functions
//

// ------------ method called for each event  ------------
void 
Validator::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  if(debug_) std::cout<<"here starts the event:"<<std::endl;
 
  Handle<std::vector<reco::Vertex>> vertices;
  iEvent.getByToken(verticesToken_, vertices);

  Handle<std::vector<reco::Vertex>> vertices4D;
  iEvent.getByToken(vertices4DToken_, vertices4D);

  Handle<std::vector<pat::PackedCandidate>> pfCandids;
  iEvent.getByToken(pfCandidToken_, pfCandids);

  edm::Handle<std::vector<PileupSummaryInfo>>  PupInfo;
  iEvent.getByToken(pileUpToken_, PupInfo);

  Handle<std::vector<reco::GenParticle>> genParts;
  iEvent.getByToken(genPartsToken_, genParts);

  Handle<std::vector<reco::GenParticle>> genPUProtons;
  iEvent.getByToken(gePUProtonToken_,genPUProtons);

  Handle<std::vector<reco::GenJet>> genJets;
  iEvent.getByToken(genJetsToken_, genJets);

  Handle<std::vector<reco::GenMET>> genMet;
  iEvent.getByToken(genMetToken_, genMet);
	
  //Handle<std::vector<pat::Photon>> photns;
  //iEvent.getByToken(photnsToken_, photns);
  //
  //Handle<std::vector<pat::Electron>> elecs;
  //iEvent.getByToken(elecsToken_, elecs);

  //Handle<std::vector<reco::GsfElectron>> elecs;
  //iEvent.getByToken(elecsToken_, elecs);

  Handle<std::vector<pat::Muon>> muons;
  iEvent.getByToken(muonsToken_, muons);

  Handle<std::vector<pat::Tau>> taus;
  iEvent.getByToken(tausToken_, taus);

  Handle<std::vector<pat::Jet>> jets;
  iEvent.getByToken(jetsToken_, jets);

  Handle<std::vector<pat::Jet>> jetschs;
  iEvent.getByToken(jetschsToken_, jetschs);

  Handle<std::vector<pat::Jet>> fatjets;
  iEvent.getByToken(fatjetsToken_, fatjets);

  Handle<std::vector<pat::MET>> met;
  iEvent.getByToken(metToken_, met);

  Handle<std::vector<pat::MET>> metpf;
  iEvent.getByToken(metpfToken_, metpf);

  const auto& gent0 = iEvent.get(genT0Token_);
  gen_t0 = gent0;


  if(debug_) std::cout<<"Here I am : got handles right "<<std::endl;  
  vtx_size         = 0;
  if(extendFormat_)
    {
      vtx4D_size   = 0;
      pfcand_size  = 0;
    }
  npuVertices      = 0.;
  trueInteractions = 0.;
  genpart_size     = 0;
  genproton_size   = 0;
  genjet_size      = 0;
  genmet_size      = 0;
  elec_size        = 0;
  jetpuppi_size    = 0;
  jetchs_size      = 0;
  fatjet_size      = 0;
  muon_size        = 0;
  metpuppi_size    = 0;
  metpf_size       = 0;
  gamma_size       = 0;
  tau_size         = 0;

  if(debug_) std::cout<<"Here I am : initalised number of particles=0 in the event "<<std::endl;   
  evt_size++;
  if(debug_) std::cout<<"Event:"<<evt_size<<std::endl;

  /////////////////////////////
  //////vertices info//////
  /////////////////////////////

  int prVtx = -1;
  for (size_t i = 0; i < vertices->size(); i++) {
    if (vertices->at(i).isFake()) continue;
    if (vertices->at(i).ndof() <= 4) continue;
    //if (fabs(vertices->at(i).position().rho()) > 2.) continue;
    //if (fabs(vertices->at(i).z()) > 24.) continue;
    if (prVtx < 0) prVtx = i;
    vtx_pt2[vtx_size] = vertices->at(i).p4().pt();
    vtx_x[vtx_size] = vertices->at(i).x();
    vtx_y[vtx_size] = vertices->at(i).y();
    vtx_z[vtx_size] = vertices->at(i).z();
    if(debug_)  std::cout<<"vertex info:"<< vtx_x[vtx_size]<< "," << vtx_y[vtx_size]<<","<< vtx_z[vtx_size]<<std::endl;
    vtx_size++;
  }
  if (prVtx < 0) return;
  if(debug_)  std::cout<<"Here I am : got vertex infor right "<<std::endl;

  /////////////////////////////
  //////4D vertices info//////
  /////////////////////////////

  if(extendFormat_)
    {
      int prVtx = -1;
      for (size_t i = 0; i < vertices4D->size(); i++) {
	if (vertices4D->at(i).isFake()) continue;
	if (vertices4D->at(i).ndof() <= 4) continue;
	//if (fabs(vertices->at(i).position().rho()) > 2.) continue;
	//if (fabs(vertices->at(i).z()) > 24.) continue;
	if (prVtx < 0) prVtx = i;
	vtx4D_pt2[vtx4D_size] = vertices4D->at(i).p4().pt();
	vtx4D_x[vtx4D_size]   = vertices4D->at(i).x();
	vtx4D_y[vtx4D_size]   = vertices4D->at(i).y();
	vtx4D_z[vtx4D_size]   = vertices4D->at(i).z();
	vtx4D_t[vtx4D_size]   = vertices4D->at(i).t();
	vtx4D_terr[vtx4D_size]   = vertices4D->at(i).tError();
	if(debug_) std::cout<<"4D vertex info:"<< vtx4D_x[vtx4D_size]<< "," << vtx4D_y[vtx4D_size]<<","<< vtx4D_z[vtx4D_size]<<","<< vtx4D_t[vtx4D_size] << ","<< vtx4D_pt2[vtx4D_size]<<std::endl;
	vtx4D_size++;
      }

      if (prVtx < 0) return;
      if(debug_)  std::cout<<"Here I am : got 4D vertex infor right "<<std::endl;
      for (size_t i = 0; i < pfCandids->size(); i++) {
	pfcand_pid[pfcand_size]       = pfCandids->at(i).pdgId();
	pfcand_pt[pfcand_size]        = pfCandids->at(i).pt();
	pfcand_phi[pfcand_size]       = pfCandids->at(i).phi();
	pfcand_eta[pfcand_size]       = pfCandids->at(i).eta();
	pfcand_mass[pfcand_size]      = pfCandids->at(i).mass();
	pfcand_vz[pfcand_size]        = pfCandids->at(i).vz();

	if( pfCandids->at(i).timeError() > 0)
	  {
	    pfcand_t[pfcand_size]         = pfCandids->at(i).time();
	    pfcand_terr[pfcand_size]      = pfCandids->at(i).timeError();
	  }
	else
	  {
	    pfcand_t[pfcand_size]         = -99.;
	    pfcand_terr[pfcand_size]      = -99.;
	  }
	      	  
	if(debug_) std::cout<<"PF cand info:"<< pfcand_pid[pfcand_size]
			    << "," << pfcand_pt[pfcand_size]<<","
			    << pfcand_phi[pfcand_size]<<","<< pfcand_eta[pfcand_size]  
			    << ","<< pfcand_mass[pfcand_size]
			    << ","<< pfcand_t[pfcand_size] << ","<< pfcand_terr[pfcand_size]
			    <<std::endl;
	
	pfcand_size++;  
	if(pfcand_size>kMaxParticle) break;       
      }
      if(debug_)  std::cout<<"Here I am : got PF candid info right "<<std::endl;
    }



  /////////////////////////////
  //////Pileup info////////////
  /////////////////////////////
  std::vector<PileupSummaryInfo>::const_iterator PVI;

  for(PVI = PupInfo->begin(); PVI != PupInfo->end(); ++PVI) {
    if(PVI->getBunchCrossing()==  0){ 
      npuVertices   += PVI->getPU_NumInteractions(); 
      trueInteractions = PVI->getTrueNumInteractions();  
    }  
  }
  if(debug_)  std::cout<<"Here I am : got pileup info right "<<std::endl;
  /////////////////////////////
  //////Gen Particle info//////
  /////////////////////////////
  vector<const reco::Candidate *> vectorCandidate;
  vector<const reco::Candidate *>::iterator itCandidate;

  for( vector<reco::GenParticle>::const_iterator itPackedParticle = genParts->begin(); itPackedParticle != genParts->end(); ++itPackedParticle)
    {
      vectorCandidate.push_back(&*itPackedParticle);
    }
  
  //GenParticle information
  for (size_t i = 0; i < genParts->size(); i++) {
    genpart_pid[genpart_size]       = genParts->at(i).pdgId();
    genpart_status[genpart_size]    = genParts->at(i).status();
    genpart_pt[genpart_size]        = genParts->at(i).pt();
    genpart_phi[genpart_size]       = genParts->at(i).phi();
    genpart_eta[genpart_size]       = genParts->at(i).eta();
    genpart_mass[genpart_size]      = genParts->at(i).mass();
    if(genParts->at(i).mother())
      {
	itCandidate = find(vectorCandidate.begin(), vectorCandidate.end(), genParts->at(i).mother());
	if(itCandidate != vectorCandidate.end())
	  {
	    genpart_m1[genpart_size] = distance(vectorCandidate.begin(), itCandidate);
	    genpart_m2[genpart_size] = distance(vectorCandidate.begin(), itCandidate);
	  }
      }
    else
      {
	genpart_m1[genpart_size] =-99;
	genpart_m2[genpart_size] =-99;
      }
    
    if(genParts->at(i).numberOfDaughters())
      {
	itCandidate = find(vectorCandidate.begin(), vectorCandidate.end(), genParts->at(i).daughter(0));
	if(itCandidate != vectorCandidate.end()) genpart_d1[genpart_size] = distance(vectorCandidate.begin(), itCandidate);
	itCandidate = find(vectorCandidate.begin(), vectorCandidate.end(), genParts->at(i).daughter(genParts->at(i).numberOfDaughters() - 1));
	if(itCandidate != vectorCandidate.end()) genpart_d2[genpart_size] = distance(vectorCandidate.begin(), itCandidate);
      }
    else {
	genpart_d1[genpart_size] =-99;
	genpart_d2[genpart_size] =-99;
    }

    // JH
    if(isSignalMC_)
      {
	if(genParts->at(i).pdgId() == 2212)
	  {
	    genproton_xi[genproton_size] = 1.0 - (genParts->at(i).energy()/7000.0);
	    genproton_pz[genproton_size] = genParts->at(i).pz();
	    genproton_vz[genproton_size] = genParts->at(i).vz();
	    genproton_ispu[genproton_size] = 0;
	    genproton_size++;
	  }
      }

    genpart_size++;
    if(genpart_size>kMaxParticle) break;       
  }
  if(debug_)   std::cout<<"Here I am : got genpart infor right "<<std::endl;
  
  // JH - PU protons
  for(size_t ipup = 0; ipup < genPUProtons->size(); ipup++)
    {
      genproton_xi[genproton_size] = 1.0 - (genPUProtons->at(ipup).energy()/7000.0);
      genproton_pz[genproton_size] = genPUProtons->at(ipup).pz();
      genproton_vz[genproton_size] = genPUProtons->at(ipup).vz();
      genproton_ispu[genproton_size] = 1;
      genproton_size++;
    }
  
  /////////////////////////////
  //GenJet information
  /////////////////////////////
  std::vector<size_t> jGenJets;                                            
  for(size_t ij= 0 ; ij < genJets->size(); ij++)
    {
      if (genJets->at(ij).pt() < 20.) continue;
      if (fabs(genJets->at(ij).eta()) > 5) continue;
      
      //bool overlaps = false;
      //for (size_t j = 0; j < genParts->size(); j++) {
      //	if (abs(genParts->at(j).pdgId()) != 11 && abs(genParts->at(j).pdgId()) != 13) continue;
      //	if (fabs(genJets->at(ij).pt()-genParts->at(j).pt()) < 0.01*genParts->at(j).pt() && ROOT::Math::VectorUtil::DeltaR(genParts->at(j).p4(),genJets->at(ij).p4()) < 0.01) {
      //	  overlaps = true;
      //	  break;
      //	}
      //}
      //if (overlaps) continue;
      jGenJets.push_back(ij);
      
      genjet_pt[genjet_size]  = genJets->at(ij).pt();
      genjet_eta[genjet_size] = genJets->at(ij).eta();
      genjet_phi[genjet_size] = genJets->at(ij).phi();
      genjet_mass[genjet_size]= genJets->at(ij).mass();
      genjet_size++;

      if(genjet_size>kMaxGenJet) break;       
    }
  if(debug_)   std::cout<<"Here I am : got genjet infor right "<<std::endl;

  /////////////////////////////
  //GenMET information
  /////////////////////////////
  for(size_t imet= 0 ; imet < genMet->size(); imet++)
     {
       genmet_pt[genmet_size]  = genMet->at(imet).pt();
       genmet_phi[genmet_size] = genMet->at(imet).phi();
       //if(debug_)   std::cout<<"genMet:"<< genmet_pt[genmet_size] << std::endl;
       genmet_size++;
      }


  /*
  /////////////////////////////
  //Photon information         
  /////////////////////////////                                                                                                
  for(size_t ip= 0 ; ip < photns->size(); ip++)
    {
      if(photns->at(ip).pt() < 10.) continue;
      if(fabs(photns->at(ip).eta()) > 3.) continue;
      float mvaValue = photns->at(ip).userFloat("mvaValue");
      bool isEB = photns->at(ip).isEB();
      bool isLoose(0), isMedium(0), isTight(0);
      
      if( isEB )
	{
	  isLoose  = (mvaValue > 0.00);
	  isMedium = (mvaValue > 0.2); 
	  isTight  = (mvaValue > 0.56);
	}     
      else
	{
	  isLoose = (mvaValue > 0.20);
	  isMedium = (mvaValue > 0.4); 
	  isTight = (mvaValue > 0.68);
	}          
      
      gamma_pt[gamma_size]        = photns->at(ip).pt();
      gamma_eta[gamma_size]       = photns->at(ip).eta();
      gamma_phi[gamma_size]       = photns->at(ip).phi();
      gamma_mass[gamma_size]      = photns->at(ip).mass();
      gamma_idvar[gamma_size]     = mvaValue; // MVA
      gamma_reliso[gamma_size]    = (photns->at(ip).puppiChargedHadronIso() + photns->at(ip).puppiNeutralHadronIso() + photns->at(ip).puppiPhotonIso()) / photns->at(ip).pt();
      gamma_idpass[gamma_size]    = 0;
      gamma_isopass[gamma_size]   = 0;
      
      if(isLoose)
	gamma_idpass[gamma_size] |= 1 << 0;
      
      if(isMedium)
	gamma_idpass[gamma_size] |= 1 << 1;
      
      if(isTight)
	gamma_idpass[gamma_size] |= 1 << 2;
      
      if(gamma_reliso[gamma_size] < 0.1)
	gamma_isopass[gamma_size] |= 1 << 2;
      
      if(gamma_reliso[gamma_size] < 0.2)
	 gamma_isopass[gamma_size] |= 1 << 1;
      
      if(gamma_reliso[gamma_size] < 0.3)
	gamma_isopass[gamma_size] |= 1 << 0;
      
      //if(gamma_reliso[gamma_size] < 0.4)
	//gamma_isopass[gamma_size] |= 1 << 3;
      
      gamma_size++;
      if(gamma_size>kMaxPhoton) break;
    }
  if(debug_)   std::cout<<"Here I am : got pho infor right "<<std::endl;
  
  /////////////////////////////
  //Electron information
  /////////////////////////////                       
  for(size_t ie= 0 ; ie < elecs->size(); ie++)  {
    if(elecs->at(ie).pt() < 10.) continue;
    if (fabs(elecs->at(ie).eta()) > 3.) continue;
    float mvaValue = elecs->at(ie).userFloat("mvaValue");
    //float mvaValue = 1.;
    elec_pt[elec_size]               = elecs->at(ie).pt();
    elec_eta[elec_size]              = elecs->at(ie).eta();
    elec_phi[elec_size]              = elecs->at(ie).phi();
    elec_mass[elec_size]             = elecs->at(ie).mass();
    elec_charge[elec_size]           = elecs->at(ie).charge();
    elec_idvar[elec_size]            = mvaValue; //MVA
    elec_idpass[elec_size]           = 0;
    elec_isopass[elec_size]          = 0;
    bool isEB                        = elecs->at(ie).isEB();
    if(isEB) 
      elec_reliso[elec_size] = (elecs->at(ie).puppiNoLeptonsChargedHadronIso() + elecs->at(ie).puppiNoLeptonsNeutralHadronIso() + elecs->at(ie).puppiNoLeptonsPhotonIso()) / elecs->at(ie).pt();
    else 
      elec_reliso[elec_size] = (elecs->at(ie).userFloat("hgcElectronID:caloIsoRing1") + elecs->at(ie).userFloat("hgcElectronID:caloIsoRing2") + elecs->at(ie).userFloat("hgcElectronID:caloIsoRing3") + elecs->at(ie).userFloat("hgcElectronID:caloIsoRing4")) / elecs->at(ie).energy();

    bool isLoose(0), isMedium(0), isTight(0);
    
    if( isEB ) {
      if (elecs->at(ie).pt() < 20.) {
	isLoose  = (mvaValue  > -0.661);
	isMedium = (mvaValue > 0.885);
	isTight  = (mvaValue  > 0.986);  	
      }
      else {
	isLoose  = (mvaValue  > -0.797);
	isMedium = (mvaValue > 0.723);
	isTight  = (mvaValue  > 0.988);
      }
    }
    else {
      if (not (elecs->at(ie).userFloat("hgcElectronID:ecEnergy") > 0)) continue;
      if (not (elecs->at(ie).userFloat("hgcElectronID:sigmaUU") > 0)) continue;
      if (not (elecs->at(ie).fbrem() > -1)) continue;
      if (not (elecs->at(ie).userFloat("hgcElectronID:measuredDepth") < 40)) continue;
      if (not (elecs->at(ie).userFloat("hgcElectronID:nLayers") > 20)) continue;
      if (elecs->at(ie).pt() < 20.) {
	isLoose = (mvaValue  > -0.320);
	isMedium = (mvaValue > 0.777);
	isTight = (mvaValue  > 0.969);
      }
      else {
	isLoose  = (mvaValue  > -0.919);
	isMedium = (mvaValue > 0.591);
	isTight  = (mvaValue  > 0.983);
      }
    }
    
    if(isLoose)
      //{
	elec_idpass[elec_size] |= 1 << 0;
	//std::cout<<"loose muon found"<<std::endl;
    // }
    if(isMedium)
      elec_idpass[elec_size] |= 1 << 1;
    
    if(isTight)
      elec_idpass[elec_size] |= 1 << 2;  
    
    if(elec_reliso[elec_size] < 0.1)
      elec_isopass[elec_size] |= 1 << 2;
    
    if(elec_reliso[elec_size] < 0.2)
      elec_isopass[elec_size] |= 1 << 1;
    
    if(elec_reliso[elec_size] < 0.3)
      elec_isopass[elec_size] |= 1 << 0;
    
    //if(elec_reliso[elec_size] < 0.4)
      //elec_isopass[elec_size] |= 1 << 3;
    
    elec_size++;
    if(elec_size>kMaxElectron) break;
  }
  
  //std::cout<<elec_size<<std::endl;
  if(debug_)   std::cout<<"Here I am : got elec infor right "<<std::endl;

  */
  /////////////////////////////
  // Muon information
  /////////////////////////////
  for(size_t im= 0 ; im < muons->size(); im++)
    {
      if (muons->at(im).pt() < 2.) continue;
      if (fabs(muons->at(im).eta()) > 2.8) continue;
      
      muon_pt[muon_size]               = muons->at(im).pt();
      muon_eta[muon_size]              = muons->at(im).eta();
      muon_phi[muon_size]              = muons->at(im).phi();
      muon_mass[muon_size]             = muons->at(im).mass();
      muon_charge[muon_size]           = muons->at(im).charge();
      muon_reliso[muon_size]           = muons->at(im).trackIso()/muons->at(im).pt();
      muon_idvar[muon_size]            = 1.0;
      muon_idpass[muon_size]           = 0;
      muon_isopass[muon_size]          = 0;
      double dPhiCut = std::min(std::max(1.2/muons->at(im).p(),1.2/100),0.056);
      double dPhiBendCut = std::min(std::max(0.2/muons->at(im).p(),0.2/100),0.0096);
      
      int isLoose = (int) (fabs(muons->at(im).eta()) < 2.4 && muon::isLooseMuon(muons->at(im))) || (fabs(muons->at(im).eta()) > 2.4 && isME0MuonSelNew(muons->at(im), 0.077, dPhiCut, dPhiBendCut, iSetup));

      if(isLoose)
	muon_idpass[muon_size] |= 1 << 0;
	
      int isMedium = isLoose;

      if(isMedium)
	muon_idpass[muon_size] |= 1 << 1;
	      
      
      bool ipxy = false, ipz = false, validPxlHit = false, highPurity = false;
      if (muons->at(im).innerTrack().isNonnull()){
	ipxy = std::abs(muons->at(im).muonBestTrack()->dxy(vertices->at(prVtx).position())) < 0.2;
	ipz = std::abs(muons->at(im).muonBestTrack()->dz(vertices->at(prVtx).position())) < 0.5;
	validPxlHit = muons->at(im).innerTrack()->hitPattern().numberOfValidPixelHits() > 0;
	highPurity = muons->at(im).innerTrack()->quality(reco::Track::highPurity);
      }      
      dPhiCut = std::min(std::max(1.2/muons->at(im).p(),1.2/100),0.032);
      dPhiBendCut = std::min(std::max(0.2/muons->at(im).p(),0.2/100),0.0041);
      int isTight = (int) (fabs(muons->at(im).eta()) < 2.4 && vertices->size() > 0 && muon::isTightMuon(muons->at(im),vertices->at(prVtx))) || (fabs(muons->at(im).eta()) > 2.4 && isME0MuonSelNew(muons->at(im), 0.048, dPhiCut, dPhiBendCut, iSetup) && ipxy && ipz && validPxlHit && highPurity);

      if(isTight)
	muon_idpass[muon_size] |= 1 << 2;
      //CutBasedIdLoosemuon[muon_size] = muons->at(im).passed(reco::Muon::CutBasedIdLoose);
      //CutBasedIdMediummuon[muon_size] = muons->at(im).passed(reco::Muon::CutBasedIdMedium);
      //CutBasedIdTightmuon[muon_size] = muons->at(im).passed(reco::Muon::CutBasedIdTight);
      //PFIsoLoosemuon[muon_size] = muons->at(im).passed(reco::Muon::PFIsoLoose);
      //PFIsoMediummuon[muon_size] = muons->at(im).passed(reco::Muon::PFIsoMedium);
      //
      //PFIsoTightmuon[muon_size] = muons->at(im).passed(reco::Muon::PFIsoTight);
         
      if(muon_reliso[muon_size] < 0.1)
	muon_isopass[muon_size] |= 1 << 2;
      
      if(muon_reliso[muon_size] < 0.2)
	muon_isopass[muon_size] |= 1 << 1;
      
      if(muon_reliso[muon_size] < 0.3)
	muon_isopass[muon_size] |= 1 << 0;
      
      //if(muon_reliso[muon_size] < 0.4)
      //muon_isopass[muon_size] |= 1 << 3;
      
      muon_size++;
      if(muon_size>kMaxMuonLoose) break;
    }
  
   if(debug_)   std::cout<<"Here I am : got muon infor right "<<std::endl;
   //std::cout<<"Muon"<<std::endl;
   
   /////////////////////////////
   // Taus info
   /////////////////////////////
   for (size_t it = 0; it < taus->size(); it++) {
     if (taus->at(it).pt()<20.) continue; 
     if (fabs(taus->at(it).eta()) > 3.0) continue;
     //if (taus->at(it).tauID("decayModeFinding")<0) continue;     
     if (!(taus->at(it).tauID("decayModeFindingNewDMs") > 0.5))// require to pass new DM finding algorithm
       continue;
     //if (!(taus->at(it).decayMode() < 5 || taus->at(it).decayMode() > 7)) // only 1- and 3-prongs
     //  continue;

     tau_pt[tau_size]          = taus->at(it).pt();
     tau_eta[tau_size]         = taus->at(it).eta();
     tau_phi[tau_size]         = taus->at(it).phi();
     tau_mass[tau_size]        = taus->at(it).mass();
     tau_charge[tau_size]      = taus->at(it).charge();
     tau_decaymode[tau_size]   = taus->at(it).decayMode();
     tau_chargediso[tau_size]  = taus->at(it).tauID("chargedIsoPtSum");
     //tau_chargediso[tau_size]  = taus->at(it).tauID("byLooseIsolationMVArun2017v2DBnewDMwLT2017");
     tau_neutraliso[tau_size]  = taus->at(it).tauID("neutralIsoPtSumdR03");
     tau_isopass[tau_size]     = 0;
     
     if (std::abs(tau_eta[tau_size])<1.4)
       tau_combinediso[tau_size]      = tau_chargediso[tau_size] + 0.2*max(0.,tau_neutraliso[tau_size] - 5.);
     else 
       tau_combinediso[tau_size]      = tau_chargediso[tau_size] + 0.2*max(0.,tau_neutraliso[tau_size] - 1.);
     
     
     /*if(fabs(taus->at(it).eta()) > 1.4) 
       {
	 if(tau_combinediso[tau_size] < 2.)
	   tau_isopass[tau_size] |= 1 << 2;
	 if(tau_combinediso[tau_size] < 4.)
	   tau_isopass[tau_size] |= 1 << 1;
	 
	 if(tau_combinediso[tau_size] < 5.)
	   tau_isopass[tau_size] |= 1 << 0;
	 //if(tau_combinediso[tau_size] < 1.2)
	 //tau_isopass[tau_size] |= 1 << 3;
	 	 
       }
    else
     {*/
      if(taus->at(it).tauID("byTightIsolationMVArun2017v2DBnewDMwLT2017"))
	tau_isopass[tau_size] |= 1 << 2;
      if(taus->at(it).tauID("byMediumIsolationMVArun2017v2DBnewDMwLT2017"))
	tau_isopass[tau_size] |= 1 << 1;
      if(taus->at(it).tauID("byLooseIsolationMVArun2017v2DBnewDMwLT2017"))
	tau_isopass[tau_size] |= 1 << 0;

      // }     
     tau_size++;
     if(tau_size>kMaxTau) break; 
   }
   
   if(debug_)   std::cout<<"Here I am : got tau infor right "<<std::endl;
   
   
   /////////////////////////////
   //Jet information
   /////////////////////////////

   if(debug_)
     {
       //std::cout<<"Puppi jets info:"<<std::endl;
       //std::cout<< " jet pt,eta, phi, mass, deepJET:"<<  std::endl;
     }
   for(size_t ij= 0 ; ij < jets->size(); ij++)
     {
       //if (jets->at(ij).pt() < 20.) continue;
       //if (fabs(jets->at(ij).eta()) > 5) continue;
       //

       jetpuppi_pt[jetpuppi_size]     = jets->at(ij).pt();
       jetpuppi_eta[jetpuppi_size]    = jets->at(ij).eta();
       jetpuppi_phi[jetpuppi_size]    = jets->at(ij).phi();
       jetpuppi_mass[jetpuppi_size]   = jets->at(ij).mass();
       jetpuppi_idpass[jetpuppi_size] = 0;
       jetpuppi_btag[jetpuppi_size]   = 0;

       
       bool isLoose(0), isMedium(0), isTight(0);
       if( (jets->at(ij).numberOfDaughters() > 1 ) && ( jets->at(ij).neutralEmEnergyFraction()< 0.99 ) && ( jets->at(ij).neutralHadronEnergyFraction() < 0.99 ))
       	 {
       	   isLoose = 1;
       	 }
       isMedium = isLoose;
       if( (jets->at(ij).numberOfDaughters() > 1 ) && ( jets->at(ij).neutralEmEnergyFraction()< 0.9 ) && ( jets->at(ij).neutralHadronEnergyFraction() < 0.9 ))
       	 {
	   isTight = 1;
	 }

       if(isLoose)
	 jetpuppi_idpass[jetpuppi_size] |= 1 << 0;
	   
       if(isMedium)
	 jetpuppi_idpass[jetpuppi_size] |= 1 << 1;

       if(isTight)
	 jetpuppi_idpass[jetpuppi_size] |= 1 << 2;
       
       float DeepJETb    = (float) jets->at(ij).bDiscriminator("pfDeepFlavourJetTags:probb");
       float DeepJETbb   = (float) jets->at(ij).bDiscriminator("pfDeepFlavourJetTags:probbb");
       float DeepJETlepb = (float) jets->at(ij).bDiscriminator("pfDeepFlavourJetTags:problepb");
       jetpuppi_DeepJET[jetpuppi_size]  = (DeepJETb > -5) ? DeepJETb + DeepJETbb + DeepJETlepb : -10;
             
       if(jetpuppi_DeepJET[jetpuppi_size] > 0.125 )
	 jetpuppi_btag[jetpuppi_size] |= 1 << 0; 

       if(jetpuppi_DeepJET[jetpuppi_size] > 0.563 )
	 jetpuppi_btag[jetpuppi_size] |= 1 << 1; 

       if(jetpuppi_DeepJET[jetpuppi_size] > 0.902 )
	 jetpuppi_btag[jetpuppi_size] |= 1 << 2; 
       
       if(debug_)
       std::cout<< jetpuppi_pt[jetpuppi_size] << ","<< jetpuppi_eta[jetpuppi_size] <<","<< jetpuppi_phi[jetpuppi_size] << ","<< jetpuppi_mass[jetpuppi_size]<< "," << jetpuppi_DeepJET[jetpuppi_size] << std::endl; 
       jetpuppi_size++;
       if(jetpuppi_size>kMaxJet) break;
     }
   if(debug_)   std::cout<<"Here I am : got jetpuppi infor right "<<std::endl;
  

   /////////////////////////////
   //CHSJet information
   /////////////////////////////

   if(debug_)
     {
       std::cout<<"CHS jets info:"<<std::endl;
       std::cout<< " jet pt,eta, phi, mass, deepJET:"<<  std::endl;
     }
   for(size_t ij= 0 ; ij < jetschs->size(); ij++)
     {
       //if (jetschs->at(ij).pt() < 20.) continue;
       //if (fabs(jetschs->at(ij).eta()) > 5) continue;
       //
       jetchs_pt[jetchs_size]     = jetschs->at(ij).pt();
       jetchs_eta[jetchs_size]    = jetschs->at(ij).eta();
       jetchs_phi[jetchs_size]    = jetschs->at(ij).phi();
       jetchs_mass[jetchs_size]   = jetschs->at(ij).mass();
       jetchs_idpass[jetchs_size] = 0;
       jetchs_btag[jetchs_size]   = 0;
	 
       bool isLoose(0), isMedium(0), isTight(0);
       if( (jetschs->at(ij).numberOfDaughters() > 1 ) && ( jetschs->at(ij).neutralEmEnergyFraction()< 0.99 ) && ( jetschs->at(ij).neutralHadronEnergyFraction() < 0.99 ))
       	 {
       	   isLoose = 1;
       	 }
       isMedium = isLoose;
       if( (jetschs->at(ij).numberOfDaughters() > 1 ) && ( jetschs->at(ij).neutralEmEnergyFraction()< 0.9 ) && ( jetschs->at(ij).neutralHadronEnergyFraction() < 0.9 ))
       	 {
	   isTight = 1;
	 }

       if(isLoose)
	 jetchs_idpass[jetchs_size] |= 1 << 0;
	   
       if(isMedium)
	 jetchs_idpass[jetchs_size] |= 1 << 1;

       if(isTight)
	 jetchs_idpass[jetchs_size] |= 1 << 2;
       
       float DeepJETb    = (float) jetschs->at(ij).bDiscriminator("pfDeepFlavourJetTags:probb");
       float DeepJETbb   = (float) jetschs->at(ij).bDiscriminator("pfDeepFlavourJetTags:probbb");
       float DeepJETlepb = (float) jetschs->at(ij).bDiscriminator("pfDeepFlavourJetTags:problepb");
       jetchs_DeepJET[jetchs_size]  = (DeepJETb > -5) ? DeepJETb + DeepJETbb + DeepJETlepb : -10;
       
       if(jetchs_DeepJET[jetchs_size] > 0.054 )
	 jetchs_btag[jetchs_size] |= 1 << 0; 

       if(jetchs_DeepJET[jetchs_size] > 0.283 )
	 jetchs_btag[jetchs_size] |= 1 << 1; 

       if(jetchs_DeepJET[jetchs_size] > 0.668 )
	 jetchs_btag[jetchs_size] |= 1 << 2; 
       
       if(debug_)
        std::cout<< jetchs_pt[jetchs_size] << ","<< jetchs_eta[jetchs_size]<<","<<jetchs_phi[jetchs_size] << ","<< jetchs_mass[jetchs_size]<< "," << jetchs_DeepJET[jetchs_size] << std::endl; 

       jetchs_size++;
       if(jetchs_size>kMaxJet) break;
     }
   if(debug_)   std::cout<<"Here I am : got jetchs infor right "<<std::endl;


   /////////////////////////////
   //FatJet information
   /////////////////////////////
   if (debug_)
   {
     std::cout << "AK8 jets info:" << std::endl;
     std::cout << " jet pt, eta, phi, mass:" << std::endl;
   }
   for (size_t ij = 0; ij < fatjets->size(); ij++)
   {
     const auto &fj = fatjets->at(ij);
     if (fj.pt() < 170)
       continue;

     fatjet_pt[fatjet_size] = fj.pt();
     fatjet_eta[fatjet_size] = fj.eta();
     fatjet_phi[fatjet_size] = fj.phi();
     fatjet_mass[fatjet_size] = fj.mass();
     fatjet_tau1[fatjet_size] = fj.userFloat("NjettinessAK8Puppi:tau1");
     fatjet_tau2[fatjet_size] = fj.userFloat("NjettinessAK8Puppi:tau2");
     fatjet_tau3[fatjet_size] = fj.userFloat("NjettinessAK8Puppi:tau3");
     fatjet_tau4[fatjet_size] = fj.userFloat("NjettinessAK8Puppi:tau4");
     fatjet_msoftdrop[fatjet_size] = fj.groomedMass("SoftDropPuppi");
     fatjet_particleNet_TvsQCD[fatjet_size] = fj.bDiscriminator("pfParticleNetDiscriminatorsJetTags:TvsQCD");
     fatjet_particleNet_WvsQCD[fatjet_size] = fj.bDiscriminator("pfParticleNetDiscriminatorsJetTags:WvsQCD");
     fatjet_particleNetMD_XbbvsQCD[fatjet_size] = fj.bDiscriminator("pfMassDecorrelatedParticleNetDiscriminatorsJetTags:XbbvsQCD");

     if (debug_)
     {
       std::cout << fatjet_pt[fatjet_size] << "," << fatjet_eta[fatjet_size] << "," << fatjet_phi[fatjet_size] << "," << fatjet_mass[fatjet_size] << std::endl;
     }
     fatjet_size++;
     if (fatjet_size > kMaxJet)
       break;
   }
   if (debug_)
     std::cout << "Here I am : got fatjet infor right " << std::endl;


   /////////////////////////////
   //Met information
   /////////////////////////////
   //if(debug_)
   //  std::cout<<"calo, CHS , track, uncorrected, corrected METs: " <<std::endl;
   for(size_t imet= 0 ; imet < met->size(); imet++)
     {
       metpuppi_pt[metpuppi_size]  = met->at(imet).pt();
       metpuppi_phi[metpuppi_size] = met->at(imet).phi();
       //if(debug_)
       //std::cout<< met->at(imet).caloMETPt() << ", " << met->at(imet).corPt(pat::MET::RawChs)<< ","<< met->at(imet).corPt(pat::MET::RawTrk)<< ","
       //	  << met->at(imet).uncorPt()<< "," << met->at(imet).pt() <<std::endl;
       metpuppi_size++;
      }
   
   
   if(debug_)   std::cout<<"Here I am : got metpuppi infor right "<<std::endl;
   //if(debug_)
     //std::cout<<"genMET, calo, CHS , track, uncorrected, corrected METs: " <<std::endl;
     //std::cout<<"calomet, CHS, track corrected, uncorrected, corrected METs: " <<std::endl;
   
   for(size_t imet= 0 ; imet < metpf->size(); imet++)
     {
       metpf_pt[metpf_size]  = metpf->at(imet).uncorPt();
       metpf_phi[metpf_size] = metpf->at(imet).phi();
       //if(debug_)
       //std::cout  <<  metpf->at(imet).caloMETPt() << ", " << metpf->at(imet).corPt(pat::MET::RawChs)<< ","<< metpf->at(imet).corPt(pat::MET::RawTrk)<< ","
       //	      << metpf->at(imet).uncorPt()<< "," << metpf->at(imet).pt() << std::endl;
       metpf_size++;
      }
   
   if(debug_)   std::cout<<"Here I am : got metpf infor right "<<std::endl;

   if(debug_) std::cout<<"beforeend"<<std::endl;
  
   mytree->Fill();
   if(debug_) std::cout<<"end"<<std::endl;
  
}


// ------------ method to improve ME0 muon ID ----------------
bool 
Validator::isME0MuonSelNew(reco::Muon muon, double dEtaCut, double dPhiCut, double dPhiBendCut, edm::EventSetup const& iSetup)
{

  bool result = false;
  bool isME0 = muon.isME0Muon();

  if(isME0){

    double deltaEta = 999;
    double deltaPhi = 999;
    double deltaPhiBend = 999;


    if(!ME0Geometry_)
      //throw std::runtime_error("Validator::isME0MuonSelNew: muon geometry not loaded");
      {
	edm::ESHandle<ME0Geometry> hGeom;
	iSetup.get<MuonGeometryRecord>().get(hGeom);
	ME0Geometry_ =( &*hGeom);
	if(!ME0Geometry_) return result;
      }

    const std::vector<reco::MuonChamberMatch>& chambers = muon.matches();
    for( std::vector<reco::MuonChamberMatch>::const_iterator chamber = chambers.begin(); chamber != chambers.end(); ++chamber ){

      if (chamber->detector() == 5){

        for ( std::vector<reco::MuonSegmentMatch>::const_iterator segment = chamber->me0Matches.begin(); segment != chamber->me0Matches.end(); ++segment ){

          LocalPoint trk_loc_coord(chamber->x, chamber->y, 0);
          LocalPoint seg_loc_coord(segment->x, segment->y, 0);
          LocalVector trk_loc_vec(chamber->dXdZ, chamber->dYdZ, 1);
          LocalVector seg_loc_vec(segment->dXdZ, segment->dYdZ, 1);

          const ME0Chamber * me0chamber = ME0Geometry_->chamber(chamber->id);
	  if(!me0chamber)continue;

          GlobalPoint trk_glb_coord = me0chamber->toGlobal(trk_loc_coord);
          GlobalPoint seg_glb_coord = me0chamber->toGlobal(seg_loc_coord);

          //double segDPhi = segment->me0SegmentRef->deltaPhi();
          // need to check if this works
          double segDPhi = me0chamber->computeDeltaPhi(seg_loc_coord, seg_loc_vec);
          double trackDPhi = me0chamber->computeDeltaPhi(trk_loc_coord, trk_loc_vec);

          deltaEta = std::abs(trk_glb_coord.eta() - seg_glb_coord.eta() );
          deltaPhi = std::abs(trk_glb_coord.phi() - seg_glb_coord.phi() );
          deltaPhiBend = std::abs(segDPhi - trackDPhi);

          if (deltaEta < dEtaCut && deltaPhi < dPhiCut && deltaPhiBend < dPhiBendCut) result = true;

        }
      }
    }

  }

  return result;

}


////Jan's code//
float Validator::calculate_demetraIsolation(const pat::Tau& tau)const{
  /*  unsigned int tau_vertex_idxpf=-1;
  pat::PackedCandidate const* packedLeadTauCand = dynamic_cast<pat::PackedCandidate const*>(tau.leadChargedHadrCand().get());
  tau_vertex_idxpf = packedLeadTauCand->vertexRef().key();
  
  float isoDR03pt08dz015=0;
  float gamma_DR03sum=0;
  
  
  for(const auto& IsoCand: tau.isolationChargedHadrCands()){
    pat::PackedCandidate const* cand = dynamic_cast<pat::PackedCandidate const*>(IsoCand.get());
    if (! cand->charge() )continue;
    //WATCH OUT WHICH VERTICES THESE ARE
    /*if(!vertices())continue;
    const auto& tau_vertex = (*vertices())[tau_vertex_idxpf];
    
    if ((cand->pt()<=0.8) || (fabs(cand->dxy(tau_vertex.position()))>=0.05))continue;
    if (cand->hasTrackDetails()){
      const auto &tt = cand->pseudoTrack();
      if (tt.normalizedChi2()>=100. || cand->numberOfHits()<3)continue;
    }
    
    if (reco::deltaR2(tau,*cand)<0.3*0.3
	&& fabs(cand->dz(tau_vertex.position()))<0.15){
	   
	   isoDR03pt08dz015+=cand->pt();
    }
  }
  for(const auto&  IsoCand: tau.isolationGammaCands()){
    pat::PackedCandidate const* cand = dynamic_cast<pat::PackedCandidate const*>(IsoCand.get());
    if ( cand->pt() < 0.5 ) continue;
    if (reco::deltaR2(tau,*cand)<0.3*0.3 && cand->pt()>1.){
      gamma_DR03sum+=cand->pt();
      }
  }
  */
  // if ( std::abs(cand->eta()) < 1.4) return (isoDR03pt08dz015 + 0.2 * std::max(0., gamma_DR03sum - 5.));
  // else return (isoDR03pt08dz015 + 0.2 * std::max(0., gamma_DR03sum - 1.));
  return 1.;
    
  
}




// ------------ method called once each job just before starting event loop  ------------
void 
Validator::beginJob()
{

}


void
Validator::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup)
{
  edm::ESHandle<ME0Geometry> hGeom;
  iSetup.get<MuonGeometryRecord>().get(hGeom);
  ME0Geometry_ =( &*hGeom);
  /*  edm::Handle<LHERunInfoProduct> run; 
  typedef std::vector<LHERunInfoProduct::Header>::const_iterator headers_const_iterator;
 
  iRun.getByToken( "externalLHEProducer", abc );
  LHERunInfoProduct myLHERunInfoProduct = *(abc.product());
 
  for (headers_const_iterator iter=myLHERunInfoProduct.headers_begin(); iter!=myLHERunInfoProduct.headers_end(); iter++){
    std::cout << iter->tag() << std::endl;
    std::vector<std::string> lines = iter->lines();
    for (unsigned int iLine = 0; iLine<lines.size(); iLine++) {
      std::cout << lines.at(iLine);
    }
    }*/
}

// ------------ method called when ending the processing of a run  ------------
  void
  Validator::endRun(edm::Run const&, edm::EventSetup const&)
  {
  }


// ------------ method called once each job just after ending the event loop  ------------
void 
Validator::endJob() 
{

}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
Validator::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(Validator);

