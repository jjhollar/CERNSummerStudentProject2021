import FWCore.ParameterSet.Config as cms

from FWCore.ParameterSet.VarParsing import VarParsing
from Configuration.Eras.Era_Phase2C9_cff import Phase2C9
from RecoTauTag.RecoTau.tools import runTauIdMVA

options = VarParsing ('python')

#$$
#options.register('pileup', 200,
#                 VarParsing.multiplicity.singleton,
#                 VarParsing.varType.int,
#                 "Specify the pileup in the sample (used for choosing B tag MVA thresholds)"
#)
#$$

options.register('debug', False,
                 VarParsing.multiplicity.singleton,
                 VarParsing.varType.bool,
                 "For Debug purposes"
)

options.register('rerunBtag', True,
                 VarParsing.multiplicity.singleton,
                 VarParsing.varType.bool,
                 "Rerun the B tagging algorithms using new training"
)


options.register('GlobalTag', '112X_mcRun4_realistic_v4',
                 VarParsing.multiplicity.singleton,
                 VarParsing.varType.string,
                 "Specify the global tag for the release "
)


options.register('Analyzr', 'Validator',
                 VarParsing.multiplicity.singleton,
                 VarParsing.varType.string,
                 "Specify which ntuplzer you want to run "
)

options.parseArguments()

process = cms.Process("MyAna", Phase2C9)


# Geometry, GT, and other standard sequences                                                                                                                       

process.load("Configuration.StandardSequences.MagneticField_cff")
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
#process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')                                                                                                               
process.load("TrackingTools/TransientTrack/TransientTrackBuilder_cfi")
#process.GlobalTag.globaltag = "auto:phase2_realistic_T15"                                                                                                                                      

#process.load('Configuration.StandardSequences.RawToDigi_cff')                                                                                                                                  
#process.load('Configuration.StandardSequences.L1Reco_cff')                                                                                                                                     
#process.load('Configuration.StandardSequences.Reconstruction_cff')                                                                                                                             
#process.load('Configuration.StandardSequences.RecoSim_cff')                                                                                                                                    
#process.load('Configuration.StandardSequences.EndOfProcess_cff')                                                                                                                               

process.load('Configuration.Geometry.GeometryExtended2026D49Reco_cff')
process.load('Configuration.Geometry.GeometryExtended2026D49_cff')

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, "auto:phase2_realistic_T15", "")


# Log settings
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100
process.MessageLogger.cerr.threshold = 'INFO'
process.MessageLogger.categories.append('MyAna')
process.MessageLogger.cerr.INFO = cms.untracked.PSet(
        limit = cms.untracked.int32(0)
)


# Input

process.options   = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True),
    allowUnscheduled = cms.untracked.bool(True)
)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvents) ) 

#options.inputFiles = ['file:gammagammaMuMu_FPMC_pT25_PU140_MINIAOD_1.root']
#options.inputFiles = ['file:step3.root']
options.inputFiles = [
#    '/store/relval/CMSSW_11_2_0_pre7/RelValZMM_14/MINIAODSIM/PU25ns_112X_mcRun4_realistic_v2_2026D49PU200_gcc900-v1/20000/745F7F94-571B-2C4A-84C8-E5AF869948E3.root',
#    '/store/relval/CMSSW_11_2_0_pre7/RelValZMM_14/MINIAODSIM/PU25ns_112X_mcRun4_realistic_v2_2026D49PU200_gcc900-v1/20000/AD490F52-F287-1748-B9EE-AFA70A694C9E.root',
#    '/store/relval/CMSSW_11_2_0_pre7/RelValZMM_14/MINIAODSIM/PU25ns_112X_mcRun4_realistic_v2_2026D49PU200_gcc900-v1/20000/034A8B16-E68C-8A42-A05B-207F52F0C189.root',
#    '/store/relval/CMSSW_11_2_0_pre7/RelValZMM_14/MINIAODSIM/PU25ns_112X_mcRun4_realistic_v2_2026D49PU200_gcc900-v1/20000/74A5C82F-29AD-064E-9DE8-0FADA430FEDE.root'
'file:/tmp/jjhollar/CMSSW_11_2_0_pre9/src/production_tests/gammagammamumu_FPMC_pT25_MINIAOD_2_version3.root'

]


process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(
                                options.inputFiles 
        ),
                            
)

process.source.inputCommands = cms.untracked.vstring("keep *")

# HGCAL EGamma ID
#process.load("RecoEgamma.Phase2InterimID.phase2EgammaPAT_cff")
#process.load("RecoEgamma.Phase2InterimID.phase2EgammaRECO_cff")
# analysis
moduleName = options.Analyzr  
process.myana = cms.EDAnalyzer(moduleName)
process.load("TreeMaker.Ntuplzr."+moduleName+"_cfi")

process.myana.debug = options.debug
process.myana.extendFormat = True
process.myana.isSignalMC = True

#JH
process.myana.genPUProtons = cms.InputTag("genPUProtons")

#$$
postfix='WithNewTraining'
patJetSource = 'selectedUpdatedPatJets'+postfix
#$$
from PhysicsTools.PatAlgos.tools.jetTools import updateJetCollection
# The updateJetCollection function uncorrect the jets from MiniAOD and 
# then recorrect them using the curren set of JEC in the event setup, recalculates
# btag discriminators from new training
# And the new name of the updated jet collection becomes selectedUpdatedPatJets+postfix
if options.rerunBtag:
    updateJetCollection(
        process,
        jetSource      = cms.InputTag('slimmedJetsPuppi'),
        pvSource       = cms.InputTag('offlineSlimmedPrimaryVertices'),
        svSource       = cms.InputTag('slimmedSecondaryVertices'),
        pfCandidates	= cms.InputTag('packedPFCandidates'),
        jetCorrections = ('AK4PFPuppi', cms.vstring(['L1FastJet', 'L2Relative', 'L3Absolute']), 'None'),
        btagDiscriminators = [
            'pfDeepCSVJetTags:probb', 
            'pfDeepCSVJetTags:probbb',
            'pfDeepFlavourJetTags:probb',
            'pfDeepFlavourJetTags:probbb',
            'pfDeepFlavourJetTags:problepb'
        ],
        postfix = postfix
    )
    process.myana.jets = cms.InputTag(patJetSource)
    #print patJetSource

    postfix='CHSWithNewTraining'
    patJetSource = 'selectedUpdatedPatJets'+postfix
    updateJetCollection(
	process,
	jetSource      = cms.InputTag('slimmedJets'),
	pvSource       = cms.InputTag('offlineSlimmedPrimaryVertices'),
	svSource       = cms.InputTag('slimmedSecondaryVertices'),
	pfCandidates	= cms.InputTag('packedPFCandidates'),
	jetCorrections = ('AK4PFchs', cms.vstring(['L1FastJet', 'L2Relative', 'L3Absolute']), 'None'),
	btagDiscriminators = [
	    'pfDeepCSVJetTags:probb', 
	    'pfDeepCSVJetTags:probbb',
	    'pfDeepFlavourJetTags:probb',
	    'pfDeepFlavourJetTags:probbb',
	    'pfDeepFlavourJetTags:problepb'
	],
	postfix = postfix
    )
    #print patJetSource
    process.myana.jetschs = cms.InputTag(patJetSource)


tauIdEmbedder = runTauIdMVA.TauIDEmbedder(
    process, cms, updatedTauName = "slimmedTausNewID",
    toKeep = ["2017v2", "newDM2017v2", "newDMPhase2v1", "deepTau2017v2p1",  "againstEle2018", "againstElePhase2v1"]
)
tauIdEmbedder.runTauID()
tauSrc_InputTag = cms.InputTag('slimmedTausNewID')# to be taken for any n-tuplizer

# EB photon ID
#from MyTools.EDProducers.photonIDProducerEB_cfi import *
#process.photonPhaseIImvaIdEB = photonMVAIDProducerEB.clone(
#    debug = False,
#)
#process.photonID_seq = cms.Sequence(process.photonPhaseIImvaIdEB)


for key in options._register.keys():
    print "{:<20} : {}".format(key, getattr(options, key))


process.TFileService = cms.Service("TFileService",
                                   #fileName = cms.string(options.outputFile)
                                   fileName = cms.string("gammagammaMuMu_FPMC_pT25_PU140_NTUPLE_1_version3.root")
#                                   fileName = cms.string("DYMuMu_PU200_NTUPLE_1_v2.root")
                                   )

#$$
#Trick to make it work with rerunBtag
process.tsk = cms.Task()
for mod in process.producers_().itervalues():
    process.tsk.add(mod)
for mod in process.filters_().itervalues():
    process.tsk.add(mod)
#$$

# run
#process.out = cms.OutputModule("PoolOutputModule",
#                               fileName = cms.untracked.string("output.root"),
#                               outputCommands = cms.untracked.vstring("drop *", "keep *_slimmedTausNewID_*_*"))    
#process.check  = cms.OutputModule("PoolOutputModule",
#                               compressionAlgorithm = cms.untracked.string('LZMA'),
#                               compressionLevel = cms.untracked.int32(4),
#                               eventAutoFlushCompressedSize = cms.untracked.int32(15728640),
#                               dataset = cms.untracked.PSet(                                                                                                                                                #dataTier = cms.untracked.string('AODSIM'),                                                                                                                      #filterName = cms.untracked.string('')                                                                                                                                  #),
#                               fileName = cms.untracked.string("out.root"),
#                               SelectEvents = cms.untracked.PSet(                                                                                              
#                                                 SelectEvents = cms.vstring("p")                                                                                #                                            ) 
#                              )
#


#$$

#process.p = cms.Path(process.photonID_seq*process.rerunMvaIsolationSequence*process.slimmedTausNewID*process.myana, process.tsk)
process.p = cms.Path(process.rerunMvaIsolationSequence*process.slimmedTausNewID*process.myana, process.tsk)
#process.e = cms.EndPath(process.out)#process.check)

#open('ntupleFileDump.py','w').write(process.dumpPython())


