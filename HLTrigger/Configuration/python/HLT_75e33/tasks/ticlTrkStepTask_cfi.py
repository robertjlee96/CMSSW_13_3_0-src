import FWCore.ParameterSet.Config as cms

from ..modules.filteredLayerClustersTrk_cfi import *
from ..modules.ticlSeedingTrk_cfi import *
from ..modules.ticlTrackstersTrk_cfi import *

ticlTrkStepTask = cms.Task(filteredLayerClustersTrk, ticlSeedingTrk, ticlTrackstersTrk)
