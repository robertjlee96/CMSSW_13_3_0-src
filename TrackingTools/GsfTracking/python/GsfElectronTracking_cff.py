import FWCore.ParameterSet.Config as cms

from RecoParticleFlow.PFTracking.trackerDrivenElectronSeeds_cff import *
from RecoEgamma.EgammaElectronProducers.ecalDrivenElectronSeeds_cff import *
from RecoParticleFlow.PFTracking.mergedElectronSeeds_cfi import *

electronSeedsTask = cms.Task(trackerDrivenElectronSeeds,ecalDrivenElectronSeeds,electronMergedSeeds) 
electronSeeds = cms.Sequence(electronSeedsTask)

from Configuration.ProcessModifiers.pp_on_AA_cff import pp_on_AA
pp_on_AA.toReplaceWith(electronSeedsTask, electronSeedsTask.copyAndExclude([trackerDrivenElectronSeeds]))

from Configuration.Eras.Modifier_fastSim_cff import fastSim
_fastSim_electronSeedsTask = electronSeedsTask.copy()
_fastSim_electronSeedsTask.replace(trackerDrivenElectronSeeds, cms.Task(trackerDrivenElectronSeedsTmp,trackerDrivenElectronSeeds))
fastSim.toReplaceWith(electronSeedsTask, _fastSim_electronSeedsTask)
# replace the ECAL driven electron track candidates with the FastSim emulated ones
import FastSimulation.Tracking.electronCkfTrackCandidates_cff
fastElectronCkfTrackCandidates = FastSimulation.Tracking.electronCkfTrackCandidates_cff.electronCkfTrackCandidates.clone()


from TrackingTools.GsfTracking.CkfElectronCandidateMaker_cff import *
from TrackingTools.GsfTracking.GsfElectronGsfFit_cff import *
electronGsfTrackingTask = cms.Task(electronSeedsTask,electronCkfTrackCandidates,electronGsfTracks)
electronGsfTracking = cms.Sequence(electronGsfTrackingTask)
_electronGsfTrackingTask = electronGsfTrackingTask.copy()
_fastSim_electronGsfTrackingTask = electronGsfTrackingTask.copy()
_fastSim_electronGsfTrackingTask.replace(electronCkfTrackCandidates,fastElectronCkfTrackCandidates)
fastSim.toReplaceWith(electronGsfTrackingTask,_fastSim_electronGsfTrackingTask)

from Configuration.Eras.Modifier_phase2_hgcal_cff import phase2_hgcal
phase2_hgcal.toReplaceWith(
  electronGsfTrackingTask, _electronGsfTrackingTask
)

from SimTracker.TrackAssociation.trackTimeValueMapProducer_cfi import trackTimeValueMapProducer
gsfTrackTimeValueMapProducer = trackTimeValueMapProducer.clone(trackSrc = 'electronGsfTracks')

electronGsfTrackingWithTimingTask = cms.Task(electronGsfTrackingTask.copy(),gsfTrackTimeValueMapProducer)
electronGsfTrackingWithTiming = cms.Sequence(electronGsfTrackingWithTimingTask)

from Configuration.Eras.Modifier_phase2_timing_cff import phase2_timing
phase2_timing.toReplaceWith(electronGsfTrackingTask, electronGsfTrackingWithTimingTask)
