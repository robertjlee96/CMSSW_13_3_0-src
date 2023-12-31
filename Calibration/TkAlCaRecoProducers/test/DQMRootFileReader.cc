// -*- C++ -*-
//
// Class:      DQMRootFileReader
//
/**\class DQMRootFileReader

Description: Simple example showing how to read MonitorElements from a DQM plain
ROOT file

Implementation:
<Notes on implementation>
*/
//
//
//

// system include files
#include <iostream>
#include <string>
#include <vector>

// user include files
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DQMServices/Core/interface/DQMStore.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

using std::cout;
using std::endl;

//
// class declaration
//
class DQMRootFileReader : public edm::one::EDAnalyzer<> {
public:
  typedef dqm::legacy::DQMStore DQMStore;
  explicit DQMRootFileReader(const edm::ParameterSet &);
  ~DQMRootFileReader() override = default;

  void analyze(const edm::Event &, const edm::EventSetup &) override;

private:
  // ----------member data ---------------------------
  // back-end interface
  DQMStore *dbe;
  std::string filename;
};

//
// constructors and destructor
//
DQMRootFileReader::DQMRootFileReader(const edm::ParameterSet &iConfig) {
  // get hold of back-end interface
  dbe = edm::Service<DQMStore>().operator->();
  filename = iConfig.getUntrackedParameter<std::string>("RootFileName", "test_playback.root");
}

//
// member functions
//

// ------------ method called to produce the data  ------------
void DQMRootFileReader::analyze(const edm::Event &iEvent, const edm::EventSetup &iSetup) {
  // NOTE: this is here just because we need it after the beginRun of
  // MEtoEDMCoverter which calls a Reset on all MEs.
  dbe->open(filename, false, "", "", DQMStore::OpenRunDirs::StripRunDirs);
}

// define this as a plug-in
DEFINE_FWK_MODULE(DQMRootFileReader);
