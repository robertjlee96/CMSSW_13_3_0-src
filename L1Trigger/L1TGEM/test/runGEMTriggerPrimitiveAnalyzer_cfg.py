import sys
import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing
from Configuration.Eras.Era_Run3_cff import Run3

options = VarParsing('analysis')
options.register ("dataVsEmulation", False, VarParsing.multiplicity.singleton, VarParsing.varType.bool)
options.register ("dataVsEmulationFile", "empty", VarParsing.multiplicity.singleton, VarParsing.varType.string)
"""
- For CMS runs, use the actual run number. Set B904Setup to False
- For B904 runs, set B904Setup to True and set runNumber >= 341761.
  Set B904RunNumber to when the data was taken, e.g. 210519_162820.
"""
options.register ("runNumber", 0, VarParsing.multiplicity.singleton, VarParsing.varType.int)
options.register ("B904Setup", False, VarParsing.multiplicity.singleton, VarParsing.varType.bool)
options.register ("B904RunNumber", "YYMMDD_HHMMSS", VarParsing.multiplicity.singleton, VarParsing.varType.string)
options.parseArguments()

if options.B904Setup and options.B904RunNumber == "YYMMDD_HHMMSS":
    sys.exit("B904 setup was selected. Please provide a valid Run Number")

process = cms.Process("ANALYSIS", Run3)
process.load("FWCore.MessageService.MessageLogger_cfi")

process.source = cms.Source(
      "PoolSource",
      fileNames = cms.untracked.vstring(options.inputFiles)
)

if options.dataVsEmulation:
    options.maxEvents = 1
    process.source = cms.Source("EmptySource")


process.maxEvents = cms.untracked.PSet(
      input = cms.untracked.int32(options.maxEvents)
)

## customize the data vs emulator module
from DQM.L1TMonitor.L1TdeGEMTPG_cfi import l1tdeGEMTPGCommon
process.gemTriggerPrimitivesAnalyzer = cms.EDAnalyzer(
    "GEMTriggerPrimitivesAnalyzer",
    l1tdeGEMTPGCommon,
    ## file of the form "DQM_V0001_L1TEMU_R000334393"
    rootFileName = cms.string(options.dataVsEmulationFile),
    ## e.g. 334393
    runNumber = cms.uint32(options.runNumber),
    dataVsEmulatorPlots = cms.bool(options.dataVsEmulation),
    B904RunNumber = cms.string(options.B904RunNumber)
)

# this needs to be set here, otherwise we duplicate the B904Setup parameter
process.gemTriggerPrimitivesAnalyzer.B904Setup = options.B904Setup

process.p = cms.Path(process.gemTriggerPrimitivesAnalyzer)
