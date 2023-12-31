#ifndef CalibTracker_SiStripLorentzAngle_SiStripLAProfileBooker_h
#define CalibTracker_SiStripLorentzAngle_SiStripLAProfileBooker_h

#include <map>

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"

#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"

#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "Geometry/Records/interface/TrackerTopologyRcd.h"

#include "CalibTracker/Records/interface/SiStripDetCablingRcd.h"
#include "CalibFormats/SiStripObjects/interface/SiStripDetCabling.h"

#include "DQMServices/Core/interface/DQMStore.h"

#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/GeometryVector/interface/LocalVector.h"

#include <TTree.h>
#include <TFile.h>
#include <TH1D.h>
#include <TDirectory.h>

class TrackerTopology;

class SiStripLAProfileBooker : public edm::one::EDAnalyzer<edm::one::WatchRuns> {
public:
  typedef dqm::legacy::MonitorElement MonitorElement;
  typedef dqm::legacy::DQMStore DQMStore;
  explicit SiStripLAProfileBooker(const edm::ParameterSet& conf);

  ~SiStripLAProfileBooker() override;

  void beginRun(edm::Run const&, const edm::EventSetup& c) override;

  void endRun(edm::Run const&, const edm::EventSetup& c) override;

  void endJob() override;

  void analyze(const edm::Event& e, const edm::EventSetup& c) override;

  void getlayer(const DetId& detid, const TrackerTopology* tTopo, std::string& name, unsigned int& layerid);

private:
  typedef struct {
    float thickness;
    float pitch;
    LocalVector magfield;
  } detparameters;
  typedef std::map<unsigned int, detparameters*> detparmap;
  typedef std::map<unsigned int, MonitorElement*> histomap;

  int trackcollsize, trajsize, RunNumber, EventNumber, ClSize, HitCharge, Type, Layer, Wheel, bw_fw, Ext_Int,
      MonoStereo, ParticleCharge;
  float sumx, hit_std_dev, barycenter, TanTrackAngle, TanTrackAngleParallel, SignCorrection, MagField, XGlobal, YGlobal,
      ZGlobal, Momentum, pt, chi2norm, EtaTrack, PhiTrack;
  int nstrip, eventcounter, size, HitNr, hitcounter, hitcounter_2ndloop, worse_double_hit, better_double_hit,
      HitPerTrack;
  int id_detector, TrackCounter, EventCounter;
  float thick_detector, pitch_detector;
  uint8_t Amplitudes[100];

  TTree *HitsTree, *TrackTree, *EventTree;
  TFile* hFile;

  TDirectory *Hit_Tree, *Track_Tree, *Event_Tree;

  histomap histos;
  histomap summaryhisto;

  DQMStore* dbe_;

  detparmap detmap;
  detparmap summarydetmap;
  edm::ParameterSet conf_;
  std::string treename_;

  const TrackerGeometry* tkGeom_ = nullptr;
  edm::ESGetToken<TrackerTopology, TrackerTopologyRcd> tTopoToken_;
  edm::ESGetToken<TrackerGeometry, TrackerDigiGeometryRecord> tkGeomToken_;
  edm::ESGetToken<MagneticField, IdealMagneticFieldRecord> magFieldToken_;
  edm::ESGetToken<SiStripDetCabling, SiStripDetCablingRcd> detCablingToken_;
};

#endif
