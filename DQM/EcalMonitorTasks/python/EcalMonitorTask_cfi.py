import FWCore.ParameterSet.Config as cms

from DQM.EcalCommon.CommonParams_cfi import ecalCommonParams

from DQM.EcalMonitorTasks.CollectionTags_cfi import ecalDQMCollectionTags

from DQM.EcalMonitorTasks.ClusterTask_cfi import ecalClusterTask
from DQM.EcalMonitorTasks.EnergyTask_cfi import ecalEnergyTask
from DQM.EcalMonitorTasks.IntegrityTask_cfi import ecalIntegrityTask
from DQM.EcalMonitorTasks.OccupancyTask_cfi import ecalOccupancyTask
from DQM.EcalMonitorTasks.PresampleTask_cfi import ecalPresampleTask
from DQM.EcalMonitorTasks.RawDataTask_cfi import ecalRawDataTask
from DQM.EcalMonitorTasks.RecoSummaryTask_cfi import ecalRecoSummaryTask
from DQM.EcalMonitorTasks.SelectiveReadoutTask_cfi import ecalSelectiveReadoutTask
from DQM.EcalMonitorTasks.TimingTask_cfi import ecalTimingTask
from DQM.EcalMonitorTasks.TrigPrimTask_cfi import ecalTrigPrimTask
from DQM.EcalMonitorTasks.ecalPiZeroTask_cfi import ecalPiZeroTask

from DQMServices.Core.DQMEDAnalyzer import DQMEDAnalyzer
ecalMonitorTask = DQMEDAnalyzer('EcalDQMonitorTask',
    moduleName = cms.untracked.string("Ecal Monitor Source"),
    # tasks to be turned on
    workers = cms.untracked.vstring(
        "ClusterTask",
        "EnergyTask",
        "IntegrityTask",
        "OccupancyTask",
        "PresampleTask",
        "RawDataTask",
        "RecoSummaryTask",
        "TimingTask",
        "TrigPrimTask",
        "PiZeroTask"
    ),
    # task parameters (included from indivitual cfis)
    workerParameters =  cms.untracked.PSet(
        ClusterTask = ecalClusterTask,
        EnergyTask = ecalEnergyTask,
        IntegrityTask = ecalIntegrityTask,
        OccupancyTask = ecalOccupancyTask,
        PresampleTask = ecalPresampleTask,
        RawDataTask = ecalRawDataTask,
        RecoSummaryTask = ecalRecoSummaryTask,
        SelectiveReadoutTask = ecalSelectiveReadoutTask,
        TimingTask = ecalTimingTask,
        TrigPrimTask = ecalTrigPrimTask,
        PiZeroTask = ecalPiZeroTask
    ),
    commonParameters = ecalCommonParams,
    collectionTags = ecalDQMCollectionTags,
    skipCollections = cms.untracked.vstring(),
    allowMissingCollections = cms.untracked.bool(True),
    verbosity = cms.untracked.int32(0),
    resetInterval = cms.untracked.double(2.)
)
