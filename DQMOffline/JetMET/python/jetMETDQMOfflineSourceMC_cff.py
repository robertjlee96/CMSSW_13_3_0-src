import FWCore.ParameterSet.Config as cms

from DQMOffline.JetMET.metDQMConfig_cff     import *
from DQMOffline.JetMET.jetAnalyzer_cff   import *
from DQMOffline.JetMET.SUSYDQMAnalyzer_cfi  import *
from DQMOffline.JetMET.goodOfflinePrimaryVerticesDQM_cfi import *
from RecoJets.JetProducers.PileupJetID_cfi  import *
from RecoJets.JetProducers.QGTagger_cfi  import *
from RecoMET.METFilters.metFilters_cff  import *

pileupJetIdCalculatorDQM=pileupJetIdCalculator.clone(
    jets = "ak4PFJets",
    jec = "AK4PF",
    applyJec = True,
    inputIsCorrected = False
)

pileupJetIdEvaluatorDQM=pileupJetIdEvaluator.clone(
    jets = "ak4PFJets",
    jetids = "pileupJetIdCalculatorDQM",
    jec = "AK4PF",
    applyJec = True,
    inputIsCorrected = False
)

pileupJetIdCalculatorCHSDQM=pileupJetIdCalculator.clone(
    applyJec = True,
    inputIsCorrected = False,
)

pileupJetIdEvaluatorCHSDQM=pileupJetIdEvaluator.clone(
    jetids = "pileupJetIdCalculatorCHSDQM",
    applyJec = True,
    inputIsCorrected = False
    )

from JetMETCorrections.Configuration.JetCorrectors_cff import ak4CaloL2L3CorrectorChain,ak4CaloResidualCorrector,ak4CaloL2L3Corrector,ak4CaloL3AbsoluteCorrector,ak4CaloL2RelativeCorrector

dqmAk4CaloL2L3Corrector = ak4CaloL2L3Corrector.clone()
dqmAk4CaloL2L3CorrectorChain = cms.Sequence(
    #ak4CaloL2RelativeCorrector*ak4CaloL3AbsoluteCorrector*
    dqmAk4CaloL2L3Corrector
)

from JetMETCorrections.Configuration.JetCorrectors_cff import ak4PFL1FastL2L3CorrectorChain,ak4PFL1FastL2L3Corrector,ak4PFL3AbsoluteCorrector,ak4PFL2RelativeCorrector,ak4PFL1FastjetCorrector

dqmAk4PFL1FastL2L3Corrector = ak4PFL1FastL2L3Corrector.clone()
dqmAk4PFL1FastL2L3CorrectorChain = cms.Sequence(
    #ak4PFL1FastjetCorrector*ak4PFL2RelativeCorrector*ak4PFL3AbsoluteCorrector*
    dqmAk4PFL1FastL2L3Corrector
)

from JetMETCorrections.Configuration.JetCorrectors_cff import ak4PFCHSL1FastL2L3CorrectorChain,ak4PFCHSL1FastL2L3Corrector,ak4PFCHSL3AbsoluteCorrector,ak4PFCHSL2RelativeCorrector,ak4PFCHSL1FastjetCorrector

dqmAk4PFCHSL1FastL2L3Corrector = ak4PFCHSL1FastL2L3Corrector.clone()
dqmAk4PFCHSL1FastL2L3CorrectorChain = cms.Sequence(
    #ak4PFCHSL1FastjetCorrector*ak4PFCHSL2RelativeCorrector*ak4PFCHSL3AbsoluteCorrector
    dqmAk4PFCHSL1FastL2L3Corrector
)

jetPreDQMTask = cms.Task(ak4CaloL2RelativeCorrector,
                         ak4CaloL3AbsoluteCorrector,
                         ak4PFL1FastjetCorrector,
                         ak4PFL2RelativeCorrector,
                         ak4PFL3AbsoluteCorrector,
                         ak4PFCHSL1FastjetCorrector,
                         ak4PFCHSL2RelativeCorrector,
                         ak4PFCHSL3AbsoluteCorrector
)
jetPreDQMSeq = cms.Sequence(jetPreDQMTask)

from JetMETCorrections.Type1MET.correctedMet_cff import pfMetT1
from JetMETCorrections.Type1MET.correctionTermsPfMetType0PFCandidate_cff import *
from JetMETCorrections.Type1MET.correctionTermsPfMetType1Type2_cff import corrPfMetType1

dqmCorrPfMetType1=corrPfMetType1.clone(jetCorrLabel = 'dqmAk4PFCHSL1FastL2L3Corrector',
                                       jetCorrLabelRes = 'dqmAk4PFCHSL1FastL2L3Corrector'
                                       )
pfMETT1=pfMetT1.clone(srcCorrections = (
        'dqmCorrPfMetType1:type1',
    ))

jetDQMAnalyzerAk4CaloUncleanedMC=jetDQMAnalyzerAk4CaloUncleaned.clone(JetCorrections  = "dqmAk4CaloL2L3Corrector")
jetDQMAnalyzerAk4CaloCleanedMC=jetDQMAnalyzerAk4CaloCleaned.clone(JetCorrections    = "dqmAk4CaloL2L3Corrector")
jetDQMAnalyzerAk4PFUncleanedMC=jetDQMAnalyzerAk4PFUncleaned.clone(JetCorrections    = "dqmAk4PFL1FastL2L3Corrector")
jetDQMAnalyzerAk4PFCleanedMC=jetDQMAnalyzerAk4PFCleaned.clone(JetCorrections      = "dqmAk4PFL1FastL2L3Corrector")
jetDQMAnalyzerAk4PFCHSCleanedMC=jetDQMAnalyzerAk4PFCHSCleaned.clone(JetCorrections   = "dqmAk4PFCHSL1FastL2L3Corrector")

caloMetDQMAnalyzerMC=caloMetDQMAnalyzer.clone(JetCorrections    = "dqmAk4CaloL2L3Corrector")
pfMetDQMAnalyzerMC=pfMetDQMAnalyzer.clone(JetCorrections      = "dqmAk4PFL1FastL2L3Corrector")
pfMetT1DQMAnalyzerMC=pfMetT1DQMAnalyzer.clone(JetCorrections    = "dqmAk4PFCHSL1FastL2L3Corrector")

jetMETDQMOfflineSource = cms.Sequence(cms.ignore(goodOfflinePrimaryVerticesDQM)*AnalyzeSUSYDQM*QGTagger*
                                      pileupJetIdCalculatorCHSDQM*pileupJetIdEvaluatorCHSDQM*
                                      pileupJetIdCalculatorDQM*pileupJetIdEvaluatorDQM*
                                      jetPreDQMSeq*
                                      dqmAk4CaloL2L3CorrectorChain*dqmAk4PFL1FastL2L3CorrectorChain*dqmAk4PFCHSL1FastL2L3CorrectorChain*
                                      dqmCorrPfMetType1*pfMETT1*
                                      jetDQMAnalyzerAk4CaloCleanedMC*jetDQMAnalyzerAk4PFUncleanedMC*jetDQMAnalyzerAk4PFCleanedMC*jetDQMAnalyzerAk4PFCHSCleanedMC*
                                      HBHENoiseFilterResultProducer*caloMetDQMAnalyzerMC*pfMetDQMAnalyzerMC*pfMetT1DQMAnalyzerMC)

jetMETDQMOfflineRedoProductsMiniAOD = cms.Sequence(goodOfflinePrimaryVerticesDQMforMiniAOD)
jetMETDQMOfflineSourceMiniAOD = cms.Sequence(jetDQMAnalyzerSequenceMiniAOD*METDQMAnalyzerSequenceMiniAOD)
