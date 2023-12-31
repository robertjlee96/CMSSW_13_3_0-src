import FWCore.ParameterSet.Config as cms

process = cms.Process("CONVERT")
process.source = cms.Source("EmptySource")
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(1))

process.load("Configuration.Geometry.GeometryIdeal_cff")
process.load("Geometry.MuonNumbering.muonNumberingInitialization_cfi")
process.load('Configuration.Geometry.GeometryExtended2021_cff')

process.DTGeometryAlInputDB = cms.ESProducer("DTGeometryESModule",
    appendToDataLabel = cms.string('idealForInputDB'),
    applyAlignment = cms.bool(False), 
    alignmentsLabel = cms.string(''),
    fromDDD = cms.bool(True)
)

process.CSCGeometryAlInputDB = cms.ESProducer("CSCGeometryESModule",
    appendToDataLabel = cms.string('idealForInputDB'),
    debugV = cms.untracked.bool(False),
    useGangedStripsInME1a = cms.bool(False),
    alignmentsLabel = cms.string(''),
    useOnlyWiresInME1a = cms.bool(False),
    useRealWireGeometry = cms.bool(True),
    useCentreTIOffsets = cms.bool(False),
    applyAlignment = cms.bool(False), 
    fromDDD = cms.bool(True),
    fromDD4hep = cms.bool(False)
)

process.GEMGeometryAlInputDB = cms.ESProducer("GEMGeometryESModule",
    appendToDataLabel = cms.string('idealForInputDB'),
    fromDDD = cms.bool(True),
    fromDD4hep = cms.bool(False),
    alignmentsLabel = cms.string(''),
    applyAlignment = cms.bool(False)
)

process.DTGeometryAlOutputXML = cms.ESProducer("DTGeometryESModule",
    appendToDataLabel = cms.string('idealForOutputXML'),
    applyAlignment = cms.bool(False), 
    alignmentsLabel = cms.string(''),
    fromDDD = cms.bool(True)
)

process.CSCGeometryAlOutputXML = cms.ESProducer("CSCGeometryESModule",
    appendToDataLabel = cms.string('idealForOutputXML'),
    debugV = cms.untracked.bool(False),
    useGangedStripsInME1a = cms.bool(False),
    alignmentsLabel = cms.string(''),
    useOnlyWiresInME1a = cms.bool(False),
    useRealWireGeometry = cms.bool(True),
    useCentreTIOffsets = cms.bool(False),
    applyAlignment = cms.bool(False), 
    fromDDD = cms.bool(True),
    fromDD4hep = cms.bool(False)
)

process.GEMGeometryAlOutputXML = cms.ESProducer("GEMGeometryESModule",
    appendToDataLabel = cms.string('idealForOutputXML'),
    fromDDD = cms.bool(True),
    fromDD4hep = cms.bool(False),
    alignmentsLabel = cms.string(''),
    applyAlignment = cms.bool(False)
)

process.load("CondCore.DBCommon.CondDBSetup_cfi")
process.PoolDBESSource = cms.ESSource("PoolDBESSource",
    process.CondDBSetup,
    connect = cms.string("sqlite_file:Alignments.db"),
    toGet = cms.VPSet(cms.PSet(
        record = cms.string('DTAlignmentRcd'),
        tag = cms.string('DT100InversepbScenario')
    ),
        cms.PSet(
            record = cms.string('DTAlignmentErrorExtendedRcd'),
            tag = cms.string('DT100InversepbScenarioErrors')
        ),
        cms.PSet(
            record = cms.string('CSCAlignmentRcd'),
            tag = cms.string('CSC100InversepbScenario')
        ),
        cms.PSet(
            record = cms.string('CSCAlignmentErrorExtendedRcd'),
            tag = cms.string('CSC100InversepbScenarioErrors')
        ),
        cms.PSet(
            record = cms.string('GEMAlignmentRcd'),
            tag = cms.string('GEM')
        ),
        cms.PSet(
            record = cms.string('GEMAlignmentErrorExtendedRcd'),
            tag = cms.string('test')
        )))
process.inertGlobalPositionRcd = cms.ESSource("PoolDBESSource",
    process.CondDBSetup,
    connect = cms.string("sqlite_file:inertGlobalPositionRcd.StdTags.746p3.DBv2.db"),
    toGet = cms.VPSet(cms.PSet(record = cms.string("GlobalPositionRcd"), tag = cms.string("inertGlobalPositionRcd"))))
process.MuonGeometryDBConverter = cms.EDAnalyzer("MuonGeometryDBConverter",
    input = cms.string("db"),
    dtLabel = cms.string(""),
    cscLabel = cms.string(""),
    gemLabel = cms.string(""),
    shiftErr = cms.double(1000.),
    angleErr = cms.double(6.28),
    getAPEs = cms.bool(True),
    output = cms.string("xml"),
    outputXML = cms.PSet(
        fileName = cms.string("REPLACEME.xml"),
        relativeto = cms.string("ideal"),
        survey = cms.bool(False),
        rawIds = cms.bool(False),
        eulerAngles = cms.bool(False),
        precision = cms.int32(10),
        suppressDTBarrel = cms.untracked.bool(True),
        suppressDTWheels = cms.untracked.bool(True),
        suppressDTStations = cms.untracked.bool(True),
        suppressDTChambers = cms.untracked.bool(False),
        suppressDTSuperLayers = cms.untracked.bool(False),
        suppressDTLayers = cms.untracked.bool(False),
        suppressCSCEndcaps = cms.untracked.bool(True),
        suppressCSCStations = cms.untracked.bool(True),
        suppressCSCRings = cms.untracked.bool(True),
        suppressCSCChambers = cms.untracked.bool(False),
        suppressCSCLayers = cms.untracked.bool(False)))

process.Path = cms.Path(process.MuonGeometryDBConverter)
