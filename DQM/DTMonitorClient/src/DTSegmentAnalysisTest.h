#ifndef DTSegmentAnalysisTest_H
#define DTSegmentAnalysisTest_H

/** \class DTSegmentAnalysisTest
 * *
 *  DQM Test Client
 *
 *  \author  G. Mila - INFN Torino
 *
 *  threadsafe version (//-) oct/nov 2014 - WATWanAbdullah ncpp-um-my
 *
 *   
 */

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"

#include "DQMServices/Core/interface/DQMStore.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "DQMServices/Core/interface/DQMEDHarvester.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"

#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

class DTGeometry;
class DTChamberId;
class DTSuperLayerId;

class DTSegmentAnalysisTest : public DQMEDHarvester {
public:
  /// Constructor
  DTSegmentAnalysisTest(const edm::ParameterSet &ps);

  /// Destructor
  ~DTSegmentAnalysisTest() override;

  void beginRun(const edm::Run &, const edm::EventSetup &) override;

  void bookHistos(DQMStore::IBooker &);

  /// Get the ME name
  std::string getMEName(const DTChamberId &chID, std::string histoTag);

  /// Perform client diagnostic operations
  void performClientDiagnostic(DQMStore::IGetter &);

protected:
  void dqmEndJob(DQMStore::IBooker &, DQMStore::IGetter &) override;
  void dqmEndLuminosityBlock(DQMStore::IBooker &,
                             DQMStore::IGetter &,
                             edm::LuminosityBlock const &,
                             edm::EventSetup const &) override;

  void dqmBeginLuminosityBlock(edm::LuminosityBlock const &lumiSeg, edm::EventSetup const &);

private:
  int nLSs;
  unsigned int nLumiSegs;
  // switch on for detailed analysis
  bool detailedAnalysis;
  int nMinEvts;

  int maxPhiHit;
  int maxPhiZHit;

  bool runOnline;

  bool bookingdone;

  edm::ParameterSet parameters;
  edm::ESGetToken<DTGeometry, MuonGeometryRecord> muonGeomToken_;
  const DTGeometry *muonGeom;

  // the histograms
  std::map<std::pair<int, int>, MonitorElement *> chi2Histos;
  std::map<std::pair<int, int>, MonitorElement *> segmRecHitHistos;
  std::map<int, MonitorElement *> summaryHistos;
  // top folder for the histograms in DQMStore
  std::string topHistoFolder;
};

#endif
