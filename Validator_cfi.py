import FWCore.ParameterSet.Config as cms

myana = cms.EDAnalyzer('Validator',
                       debug          = cms.bool(False),
                       extendFormat   = cms.bool(False),
                       isSignalMC     = cms.bool(True),
                       vertices       = cms.InputTag("offlineSlimmedPrimaryVertices"),
                       vertices4D     = cms.InputTag("offlineSlimmedPrimaryVertices4D"),
                       pfCandid       = cms.InputTag("packedPFCandidates"),
                       pileUp         = cms.InputTag("slimmedAddPileupInfo"),
                       genParts       = cms.InputTag("prunedGenParticles"),
                       genJets        = cms.InputTag("slimmedGenJets"),
                       genMet         = cms.InputTag("genMetTrue"),
                       photons        = cms.InputTag("slimmedPhotons"),
                       electrons      = cms.InputTag("ecalDrivenGsfElectronsFromMultiCl"),
                       #photons        = cms.InputTag("phase2Photons"),
                       #electrons      = cms.InputTag("phase2Electrons"),
                       muons          = cms.InputTag("slimmedMuons" ),
                       taus           = cms.InputTag("slimmedTausNewID"),
                       jets           = cms.InputTag("slimmedJetsPuppi"),
                       jetschs        = cms.InputTag("slimmedJets"),
                       fatjets        = cms.InputTag("slimmedJetsAK8"),
                       met            = cms.InputTag("slimmedMETsPuppi"),
                       metpf          = cms.InputTag("slimmedMETs"),
)
