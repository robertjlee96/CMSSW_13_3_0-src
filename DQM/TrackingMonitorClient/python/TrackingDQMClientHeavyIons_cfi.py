import FWCore.ParameterSet.Config as cms
from DQMServices.Core.DQMEDHarvester import DQMEDHarvester

hiTrackingDqmClientHI = DQMEDHarvester("TrackingDQMClientHeavyIons",
                                              FolderName = cms.string('Tracking/TrackParameters/GeneralProperties')
                                              )

from DQM.TrackingMonitor.TrackFoldedOccupancyClient_cfi import TrackerMapFoldedClient

TrackerMapFoldedClient_heavyionTk = TrackerMapFoldedClient.clone(
    AlgoName = 'HeavyIonTk',
    TrackQuality = ''
)
hiTrackingDqmClientHeavyIons=cms.Sequence(hiTrackingDqmClientHI*TrackerMapFoldedClient_heavyionTk)
