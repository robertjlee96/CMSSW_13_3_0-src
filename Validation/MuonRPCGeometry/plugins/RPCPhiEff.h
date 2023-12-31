#ifndef MuonRPCGeometry_RPCPhiEff_h
#define MuonRPCGeometry_RPCPhiEff_h

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "SimDataFormats/Track/interface/SimTrackContainer.h"

#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/RPCDigi/interface/RPCDigiCollection.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuRegionalCand.h"

#include <L1Trigger/RPCTrigger/interface/RPCLogCone.h>
#include <L1Trigger/RPCTrigger/interface/RPCConst.h>

#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuRegionalCand.h"

#include "DataFormats/Math/interface/LorentzVector.h"

#include <iostream>
#include <set>
#include <fstream>
#include <sstream>

#include "L1Trigger/RPCTrigger/interface/RPCConst.h"

//
// class decleration
//

class RPCPhiEff : public edm::one::EDAnalyzer<> {
public:
  explicit RPCPhiEff(const edm::ParameterSet &);
  ~RPCPhiEff() override;

private:
  void beginJob() override;
  void analyze(const edm::Event &, const edm::EventSetup &) override;
  std::string fromCones(const edm::Event &iEvent);
  std::string fromRaw(const edm::Event &iEvent);
  void endJob() override;
  std::ofstream m_outfileC;
  std::ofstream m_outfileR;

  // ----------member data ---------------------------

  RPCConst rpcconst;
  //L1MuTriggerScales m_scales;
  edm::EDGetTokenT<std::vector<L1MuRegionalCand> > m_rpcbToken;
  edm::EDGetTokenT<std::vector<L1MuRegionalCand> > m_rpcfToken;
  edm::EDGetTokenT<edm::SimTrackContainer> m_g4Token;
  edm::EDGetTokenT<RPCDigiCollection> m_rpcdigiToken;

  RPCConst m_const;
};

#endif
