import FWCore.ParameterSet.Config as cms
from Configuration.ProcessModifiers.dd4hep_cff import dd4hep

process = cms.Process("G4PrintGeometry",dd4hep)

process.DDDetectorESProducer = cms.ESSource("DDDetectorESProducer",
                                            confGeomXMLFiles = cms.FileInPath('Geometry/ForwardCommonData/data/dd4hep/testForwardGeometry.xml'),
                                            appendToDataLabel = cms.string('')
)
process.DDSpecParRegistryESProducer = cms.ESProducer("DDSpecParRegistryESProducer",
                                                     appendToDataLabel = cms.string('')
)
process.DDVectorRegistryESProducer = cms.ESProducer("DDVectorRegistryESProducer",
                                                    appendToDataLabel = cms.string(''))
process.DDCompactViewESProducer = cms.ESProducer("DDCompactViewESProducer",
                                                 appendToDataLabel = cms.string('')
)

process.load('FWCore.MessageService.MessageLogger_cfi')

if 'MessageLogger' in process.__dict__:
    process.MessageLogger.SimG4CoreApplication=dict()
    process.MessageLogger.G4cout=dict()
    process.MessageLogger.ForwardGeom=dict()

from SimG4Core.PrintGeomInfo.g4TestGeometry_cfi import *
process = checkOverlap(process)

# enable Geant4 overlap check 
process.g4SimHits.CheckGeometry = True

# Geant4 geometry check 
process.g4SimHits.G4CheckOverlap.OutputBaseName = cms.string("forward")
process.g4SimHits.G4CheckOverlap.OverlapFlag = cms.bool(True)
process.g4SimHits.G4CheckOverlap.Tolerance  = cms.double(0.0)
process.g4SimHits.G4CheckOverlap.Resolution = cms.int32(10000)
# tells if NodeName is G4Region or G4PhysicalVolume
process.g4SimHits.G4CheckOverlap.RegionFlag = cms.bool(False)
# list of names
process.g4SimHits.G4CheckOverlap.NodeNames  = cms.vstring('cms:CMSE_1')
# enable dump gdml file 
process.g4SimHits.G4CheckOverlap.gdmlFlag   = cms.bool(False)
# if defined a G4PhysicsVolume info is printed
process.g4SimHits.G4CheckOverlap.PVname     = ''
# if defined a list of daughter volumes is printed
process.g4SimHits.G4CheckOverlap.LVname     = ''

# extra output files, created if a name is not empty
process.g4SimHits.FileNameField   = ''
process.g4SimHits.FileNameGDML    = ''
process.g4SimHits.FileNameRegions = ''
#
process.g4SimHits.OnlySDs = cms.vstring('ZdcSensitiveDetector', 'TotemT2ScintSensitiveDetector', 'TotemSensitiveDetector', 'BCM1FSensitiveDetector', 'BHMSensitiveDetector', 'PLTSensitiveDetector')
