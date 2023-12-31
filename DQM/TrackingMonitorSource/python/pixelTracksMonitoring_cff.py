import FWCore.ParameterSet.Config as cms

from DQM.TrackingMonitor.TrackerCollisionTrackingMonitor_cfi import *
pixelTracksMonitor = TrackerCollisionTrackMon.clone(
    FolderName = 'Tracking/PixelTrackParameters/pixelTracks',
    TrackProducer = 'pixelTracks',
    allTrackProducer = 'pixelTracks',
    beamSpot = 'offlineBeamSpot',
    primaryVertex = 'pixelVertices',
    pvNDOF = 1,
    doAllPlots = False,
    doLumiAnalysis = True,
    doProfilesVsLS = True,
    doDCAPlots = True,
    doEffFromHitPatternVsPU = False,
    doEffFromHitPatternVsBX = False,
    doEffFromHitPatternVsLUMI = False,
    doPlotsVsGoodPVtx = True,
    doPlotsVsLUMI = True,
    doPlotsVsBX = True
)

_trackSelector = cms.EDFilter('TrackSelector',
    src = cms.InputTag('pixelTracks'),
    cut = cms.string("")
)

quality = {
    "L"  : "loose",
    "T"  : "tight",
    "HP" : "highPurity",
}

for key,value in quality.items():
    label = "pixelTrks"+key
#    print label
    cutstring = "quality('" + value + "')" 
#    print cutstring
    if label not in globals():
        locals()[label] = _trackSelector.clone( cut = cutstring )
        locals()[label].setLabel(label)
    else :
        print(label,"already configured")

for key,value in quality.items():
    label = "pixelTrksMonitor"+key
    locals()[label] = pixelTracksMonitor.clone(
        TrackProducer = "pixelTrks"+key,
        FolderName    = "Tracking/PixelTrackParameters/"+value
    )
    locals()[label].setLabel(label)

ntuplet = {
    '3' : "3Hits", # ==3
    '4' : "4Hits"  # >=4 
}
for kN,vN in ntuplet.items():
    for key,value in quality.items():
        label = "pixelTrks" + vN + key
#        print label

        cutstring = "numberOfValidHits == " + kN + " & quality('" + value + "')" 
#        print cutstring
        locals()[label] = _trackSelector.clone( cut = cutstring )
        locals()[label].setLabel(label)

for kN,vN in ntuplet.items():
    for key,value in quality.items():
        label = "pixelTrks" + vN + "Monitor" + key
#        print label
        locals()[label] = pixelTracksMonitor.clone(
            TrackProducer = "pixelTrks" + vN + key,
            FolderName    = "Tracking/PixelTrackParameters/" + vN + "/" + value
        )
        locals()[label].setLabel(label)


from CommonTools.ParticleFlow.goodOfflinePrimaryVertices_cfi import goodOfflinePrimaryVertices as _goodOfflinePrimaryVertices
goodPixelVertices = _goodOfflinePrimaryVertices.clone(
    src = "pixelVertices"
)

from DQM.TrackingMonitor.primaryVertexResolution_cfi import primaryVertexResolution as _primaryVertexResolution
pixelVertexResolution = _primaryVertexResolution.clone(
    vertexSrc = "goodPixelVertices",
    rootFolder = "OfflinePixelPV/Resolution"
)

pixelTracksMonitoringTask = cms.Task(
    goodPixelVertices,
)

for category in ["pixelTrks", "pixelTrks3Hits", "pixelTrks4Hits"]:
    for key in quality:
        label = category+key
#        print label
        pixelTracksMonitoringTask.add(locals()[label])

allPixelTracksMonitoring = cms.Sequence()
for category in ["pixelTrksMonitor", "pixelTrks3HitsMonitor", "pixelTrks4HitsMonitor" ]:
    for key in quality:
        label = category+key
#        print label
        allPixelTracksMonitoring += locals()[label]

pixelTracksMonitoring = cms.Sequence(
    allPixelTracksMonitoring +
    pixelVertexResolution,
    pixelTracksMonitoringTask
)




