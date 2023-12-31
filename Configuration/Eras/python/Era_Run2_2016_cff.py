import FWCore.ParameterSet.Config as cms

from Configuration.Eras.Modifier_run2_common_cff import run2_common
from Configuration.Eras.Modifier_run2_25ns_specific_cff import run2_25ns_specific
from Configuration.Eras.Modifier_stage2L1Trigger_cff import stage2L1Trigger
from Configuration.Eras.Modifier_ctpps_cff import ctpps
from Configuration.Eras.Modifier_ctpps_2016_cff import ctpps_2016
from Configuration.Eras.Modifier_run2_HLTconditions_2016_cff import run2_HLTconditions_2016
from Configuration.Eras.Modifier_run2_muon_2016_cff import run2_muon_2016
from Configuration.Eras.Modifier_run2_egamma_2016_cff import run2_egamma_2016
from Configuration.Eras.Modifier_run2_L1prefiring_cff import run2_L1prefiring
from Configuration.Eras.Modifier_pixel_2016_cff import pixel_2016
from Configuration.Eras.Modifier_run2_jme_2016_cff import run2_jme_2016
from Configuration.Eras.Modifier_strips_vfp30_2016_cff import strips_vfp30_2016
from Configuration.Eras.Modifier_run2_ECAL_2016_cff import run2_ECAL_2016
from Configuration.ProcessModifiers.trackingNoLoopers_cff import trackingNoLoopers

Run2_2016 = cms.ModifierChain(run2_common, run2_25ns_specific,
                              stage2L1Trigger, ctpps, ctpps_2016, run2_HLTconditions_2016, run2_ECAL_2016, run2_muon_2016, run2_egamma_2016, run2_L1prefiring, pixel_2016, run2_jme_2016, strips_vfp30_2016, trackingNoLoopers)

