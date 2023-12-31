import FWCore.ParameterSet.Config as cms

process = cms.Process("DumpECDD4hep")

process.load('FWCore.MessageService.MessageLogger_cfi')
process.source = cms.Source("EmptySource")
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
    )

process.MessageLogger.cerr.FwkReport.reportEvery = 5
if hasattr(process,'MessageLogger'):
    process.MessageLogger.EcalGeom=dict()

process.DDDetectorESProducer = cms.ESSource("DDDetectorESProducer",
                                            confGeomXMLFiles = cms.FileInPath('Geometry/EcalCommonData/data/dd4hep/cms-ecal-geometry-dump.xml'),
                                            appendToDataLabel = cms.string('DDEcal')
                                            )

process.testDump = cms.EDAnalyzer("DDTestDumpFile",
                                  outputFileName = cms.untracked.string('ecalDD4hep.root'),
                                  DDDetector = cms.ESInputTag('','DDEcal')
                                  )

process.p = cms.Path(process.testDump)
