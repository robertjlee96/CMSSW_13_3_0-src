import FWCore.ParameterSet.Config as cms

#  SiStripOfflineDQM (for Tier0 Harvesting Step) ####
siStripOfflineAnalyser = cms.EDProducer("SiStripOfflineDQM",
    GlobalStatusFilling      = cms.untracked.int32(2),
    CreateSummary            = cms.untracked.bool(False),
    SummaryConfigPath        = cms.untracked.string("DQM/SiStripMonitorClient/data/sistrip_monitorelement_config.xml"),
    UsedWithEDMtoMEConverter = cms.untracked.bool(True),
    PrintFaultyModuleList    = cms.untracked.bool(True),
    CreateTkMap              = cms.untracked.bool(False), 
    TrackRatePSet            = cms.PSet(
           Name     = cms.string("NumberOfTracks_"),
                  LowerCut = cms.double(0.0),
                  UpperCut = cms.double(1000.0),
               ),
    TrackChi2PSet            = cms.PSet(
           Name     = cms.string("Chi2oNDF_"),
                  LowerCut = cms.double(0.0),
                  UpperCut = cms.double(25.0),
               ),
    TrackHitPSet            = cms.PSet(
           Name     = cms.string("NumberOfRecHitsPerTrack_"),
                  LowerCut = cms.double(3.0),
                  UpperCut = cms.double(30.0),
               )
)

# clone and modify modules
from DQMServices.Core.DQMQualityTester import DQMQualityTester
siStripQTesterHI = DQMQualityTester(
    qtList = cms.untracked.FileInPath('DQM/SiStripMonitorClient/data/sistrip_qualitytest_config_tier0_heavyions.xml'),
    prescaleFactor = cms.untracked.int32(1),                               
    getQualityTestsFromFile = cms.untracked.bool(True)
)

from CalibTracker.SiStripESProducers.SiStripQualityESProducer_cfi import siStripQualityESProducer 
mergedSiStripQualityProducer = siStripQualityESProducer.clone(
    #names and desigantions
    ListOfRecordToMerge = cms.VPSet(
        cms.PSet(record = cms.string("SiStripDetVOffRcd"), tag = cms.string('')), # DCS information
        cms.PSet(record = cms.string('SiStripDetCablingRcd'), tag = cms.string('')), # Use Detector cabling information to exclude detectors not connected            
        cms.PSet(record = cms.string('SiStripBadChannelRcd'), tag = cms.string('')), # Online Bad components
        cms.PSet(record = cms.string('SiStripBadFiberRcd'), tag = cms.string('')),   # Bad Channel list from the selected IOV as done at PCL
        # BadChannel list from FED errors is included below
        cms.PSet(record = cms.string('RunInfoRcd'), tag = cms.string(''))            # List of FEDs exluded during data taking          
        ),
    ReduceGranularity = False,
    ThresholdForReducedGranularity = 0.3,
    appendToDataLabel = 'MergedBadComponent'
)

from DQM.SiStripMonitorClient.siStripBadComponentInfo_cfi import siStripBadComponentInfo
siStripBadComponentInfo.StripQualityLabel = 'MergedBadComponent'
siStripBadComponentInfo.BadComponentsFromFedErrors.Add = True

# define new HI sequence
#removed modules using TkDetMap
#SiStripOfflineDQMClientHI = cms.Sequence(siStripQTesterHI)
SiStripOfflineDQMClientHI = cms.Sequence(siStripQTesterHI*siStripOfflineAnalyser*siStripBadComponentInfo)

# Services needed for TkHistoMap
from CalibTracker.SiStripCommon.TkDetMapESProducer_cfi import *
