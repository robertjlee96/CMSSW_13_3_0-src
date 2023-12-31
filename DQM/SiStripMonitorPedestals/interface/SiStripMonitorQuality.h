#ifndef SiStripMonitorQuality_SiStripMonitorQuality_h
#define SiStripMonitorQuality_SiStripMonitorQuality_h
// -*- C++ -*-
//
// Package:     SiStripMonitorQuality
// Class  :     SiStripMonitorQuality
//
/**\class SiStripMonitorQuality SiStripMonitorQuality.h

 Description: <one line class summary>

 Usage:
    <usage>

*/
//
// Original Author:  dutta
//         Created:  Fri Dec  7 20:49:51 CET 2007
//

// system include files
#include <memory>
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

// user include files
#include "FWCore/Framework/interface/ESWatcher.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DQMServices/Core/interface/DQMOneEDAnalyzer.h"
#include "DQMServices/Core/interface/DQMStore.h"

class SiStripDetCabling;
class SiStripQuality;
class TrackerTopology;

class SiStripMonitorQuality : public DQMOneEDAnalyzer<> {
public:
  explicit SiStripMonitorQuality(const edm::ParameterSet &);
  ~SiStripMonitorQuality() override;

  void bookHistograms(DQMStore::IBooker &, edm::Run const &, edm::EventSetup const &) override;
  void analyze(const edm::Event &, const edm::EventSetup &) override;
  void dqmEndRun(edm::Run const &run, edm::EventSetup const &eSetup) override;
  void endJob() override;

private:
  MonitorElement *getQualityME(uint32_t idet, const TrackerTopology *tTopo);

  DQMStore *dqmStore_;
  edm::ParameterSet conf_;
  const SiStripDetCabling *detCabling_;
  const SiStripQuality *stripQuality_;

  std::map<uint32_t, MonitorElement *> QualityMEs;
  std::string dataLabel_;

  edm::ESGetToken<TrackerTopology, TrackerTopologyRcd> tTopoToken_;
  edm::ESGetToken<SiStripDetCabling, SiStripDetCablingRcd> detCablingToken_;
  edm::ESGetToken<SiStripQuality, SiStripQualityRcd> qualityToken_;
  edm::ESWatcher<SiStripQualityRcd> qualityWatcher_;

  unsigned long long m_cacheID_;
};

#endif
