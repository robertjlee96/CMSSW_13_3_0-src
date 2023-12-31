import FWCore.ParameterSet.Config as cms
from Validation.RecoTau.RecoTauValidation_cfi import *
import copy

from PhysicsTools.HepMCCandAlgos.genParticles_cfi import *
from RecoJets.Configuration.RecoGenJets_cff import *
from RecoJets.Configuration.GenJetParticles_cff import *
import PhysicsTools.PatAlgos.tools.helpers as helpers

kinematicSelectedTauValDenominatorQCD = cms.EDFilter(
    "GenJetSelector",
    src = cms.InputTag('slimmedGenJets'),
    cut = kinematicSelectedTauValDenominatorCut,#cms.string('pt > 5. && abs(eta) < 2.5'), #Defined: Validation.RecoTau.RecoTauValidation_cfi 
    filter = cms.bool(False)
)

procAttributes = dir(proc) #Takes a snapshot of what there in the process
helpers.cloneProcessingSnippet( proc, proc.TauValNumeratorAndDenominator, 'QCD') #clones the sequence inside the process with QCD postfix
helpers.cloneProcessingSnippet( proc, proc.TauEfficiencies, 'QCD') #clones the sequence inside the process with QCD postfix
helpers.massSearchReplaceAnyInputTag(proc.TauValNumeratorAndDenominatorQCD, 'kinematicSelectedTauValDenominator', 'kinematicSelectedTauValDenominatorQCD') #sets the correct input tag

#adds to TauValNumeratorAndDenominator modules in the sequence QCD to the extention name
zttLabeler = lambda module : SetValidationExtention(module, 'QCD')
zttModifier = ApplyFunctionToSequence(zttLabeler)
proc.TauValNumeratorAndDenominatorQCD.visit(zttModifier)

#Set discriminators
discs_to_retain = ['ByDecayModeFinding', 'CombinedIsolationDBSumPtCorr3Hits', 'IsolationMVArun2v1DBoldDMwLT', 'IsolationMVArun2v1DBnewDMwLT']
proc.RunHPSValidationQCD.discriminators = cms.VPSet([p for p in proc.RunHPSValidationQCD.discriminators if any(disc in p.discriminator.value() for disc in discs_to_retain) ])

#Sets the correct naming to efficiency histograms
proc.efficienciesQCD.plots = Utils.SetPlotSequence(proc.TauValNumeratorAndDenominatorQCD)
proc.efficienciesQCDSummary = cms.EDProducer("TauDQMHistEffProducer",
    plots = cms.PSet(
        Summary = cms.PSet(
            denominator = cms.string('RecoTauV/standardValidation/hpsPFTauProducerQCD_Summary/#PAR#PlotDen'),
            efficiency = cms.string('RecoTauV/standardValidation/hpsPFTauProducerQCD_Summary/#PAR#Plot'),
            numerator = cms.string('RecoTauV/standardValidation/hpsPFTauProducerQCD_Summary/#PAR#PlotNum'),
            parameter = cms.vstring('summary'),
            stepByStep = cms.bool(True)
        ),
    )
)

#checks what's new in the process (the cloned sequences and modules in them)
newProcAttributes = [x for x in dir(proc) if (x not in procAttributes) and (x.find('QCD') != -1)]

#spawns a local variable with the same name as the proc attribute, needed for future process.load
for newAttr in newProcAttributes:
    locals()[newAttr] = getattr(proc,newAttr)

produceDenominatorQCD = cms.Sequence(
    cms.ignore(kinematicSelectedTauValDenominatorQCD)
)

produceDenominator = cms.Sequence(produceDenominatorQCD)

runTauValidationBatchMode = cms.Sequence(
    produceDenominatorQCD
    +TauValNumeratorAndDenominatorQCD
    )

runTauValidation = cms.Sequence(
    runTauValidationBatchMode
    +TauEfficienciesQCD
    )
