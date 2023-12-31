import FWCore.ParameterSet.Config as cms

from ..modules.filteredLayerClustersHAD_cfi import *
from ..modules.ticlSeedingGlobal_cfi import *
from ..modules.ticlTrackstersHAD_cfi import *

ticlHADStepTask = cms.Task(filteredLayerClustersHAD, ticlSeedingGlobal, ticlTrackstersHAD)
