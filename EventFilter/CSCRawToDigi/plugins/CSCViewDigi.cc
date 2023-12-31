// Original Author:  Alexandre Sakharov
//         Created:  Sun May 10 15:43:28 CEST 2009

#include <memory>
#include <iostream>

#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "EventFilter/CSCRawToDigi/interface/CSCALCTHeader.h"
#include "DataFormats/CSCDigi/interface/CSCWireDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCStripDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCComparatorDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCRPCDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCALCTDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCCLCTDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCCorrelatedLCTDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCDCCFormatStatusDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCDDUStatusDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCDCCStatusDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCShowerDigiCollection.h"
#include "DataFormats/GEMDigi/interface/GEMPadDigiClusterCollection.h"

class CSCViewDigi : public edm::one::EDAnalyzer<> {
public:
  explicit CSCViewDigi(const edm::ParameterSet&);
  ~CSCViewDigi() override;

private:
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  void endJob() override;

  bool WiresDigiDump, AlctDigiDump, ClctDigiDump, CorrClctDigiDump;
  bool StripDigiDump, ComparatorDigiDump, RpcDigiDump, StatusDigiDump;
  bool DDUStatusDigiDump, DCCStatusDigiDump;
  bool gemPadsDigiDump, showerDigiDump;

  edm::EDGetTokenT<CSCWireDigiCollection> wd_token;
  edm::EDGetTokenT<CSCStripDigiCollection> sd_token;
  edm::EDGetTokenT<CSCComparatorDigiCollection> cd_token;
  edm::EDGetTokenT<CSCRPCDigiCollection> rd_token;
  edm::EDGetTokenT<CSCALCTDigiCollection> al_token;
  edm::EDGetTokenT<CSCCLCTDigiCollection> cl_token;
  edm::EDGetTokenT<CSCCorrelatedLCTDigiCollection> co_token;
  edm::EDGetTokenT<CSCDCCFormatStatusDigiCollection> st_token;
  edm::EDGetTokenT<CSCDDUStatusDigiCollection> dd_token;
  edm::EDGetTokenT<CSCDCCStatusDigiCollection> dc_token;
  edm::EDGetTokenT<GEMPadDigiClusterCollection> gem_token;
  edm::EDGetTokenT<CSCShowerDigiCollection> lct_shwr_token;
  edm::EDGetTokenT<CSCShowerDigiCollection> anode_shwr_token;
  edm::EDGetTokenT<CSCShowerDigiCollection> cathode_shwr_token;
  edm::EDGetTokenT<CSCShowerDigiCollection> alct_anode_shwr_token;
};

CSCViewDigi::CSCViewDigi(const edm::ParameterSet& conf) {
  wd_token = consumes<CSCWireDigiCollection>(conf.getParameter<edm::InputTag>("wireDigiTag"));
  sd_token = consumes<CSCStripDigiCollection>(conf.getParameter<edm::InputTag>("stripDigiTag"));
  cd_token = consumes<CSCComparatorDigiCollection>(conf.getParameter<edm::InputTag>("comparatorDigiTag"));
  rd_token = consumes<CSCRPCDigiCollection>(conf.getParameter<edm::InputTag>("rpcDigiTag"));
  al_token = consumes<CSCALCTDigiCollection>(conf.getParameter<edm::InputTag>("alctDigiTag"));
  cl_token = consumes<CSCCLCTDigiCollection>(conf.getParameter<edm::InputTag>("clctDigiTag"));
  co_token = consumes<CSCCorrelatedLCTDigiCollection>(conf.getParameter<edm::InputTag>("corrclctDigiTag"));
  st_token = consumes<CSCDCCFormatStatusDigiCollection>(conf.getParameter<edm::InputTag>("statusDigiTag"));
  dd_token = consumes<CSCDDUStatusDigiCollection>(conf.getParameter<edm::InputTag>("DDUstatusDigiTag"));
  dc_token = consumes<CSCDCCStatusDigiCollection>(conf.getParameter<edm::InputTag>("DCCstatusDigiTag"));
  gem_token = consumes<GEMPadDigiClusterCollection>(conf.getParameter<edm::InputTag>("gemPadsDigiTag"));
  lct_shwr_token = consumes<CSCShowerDigiCollection>(conf.getParameter<edm::InputTag>("lctShowerDigiTag"));
  anode_shwr_token = consumes<CSCShowerDigiCollection>(conf.getParameter<edm::InputTag>("anodeShowerDigiTag"));
  cathode_shwr_token = consumes<CSCShowerDigiCollection>(conf.getParameter<edm::InputTag>("cathodeShowerDigiTag"));
  alct_anode_shwr_token =
      consumes<CSCShowerDigiCollection>(conf.getParameter<edm::InputTag>("alct_anodeShowerDigiTag"));

  WiresDigiDump = conf.getUntrackedParameter<bool>("WiresDigiDump", false);
  StripDigiDump = conf.getUntrackedParameter<bool>("StripDigiDump", false);
  ComparatorDigiDump = conf.getUntrackedParameter<bool>("ComparatorDigiDump", false);
  RpcDigiDump = conf.getUntrackedParameter<bool>("RpcDigiDump", false);
  AlctDigiDump = conf.getUntrackedParameter<bool>("AlctDigiDump", false);
  ClctDigiDump = conf.getUntrackedParameter<bool>("ClctDigiDump", false);
  CorrClctDigiDump = conf.getUntrackedParameter<bool>("CorrClctDigiDump", false);
  StatusDigiDump = conf.getUntrackedParameter<bool>("StatusDigiDump", false);
  DDUStatusDigiDump = conf.getUntrackedParameter<bool>("DDUStatus", false);
  DCCStatusDigiDump = conf.getUntrackedParameter<bool>("DCCStatus", false);
  gemPadsDigiDump = conf.getUntrackedParameter<bool>("GEMPadsDigiDump", false);
  showerDigiDump = conf.getUntrackedParameter<bool>("ShowerDigiDump", false);
}

CSCViewDigi::~CSCViewDigi() {}

void CSCViewDigi::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  using namespace edm;

  edm::Handle<CSCWireDigiCollection> wires;
  edm::Handle<CSCStripDigiCollection> strips;
  edm::Handle<CSCComparatorDigiCollection> comparators;
  edm::Handle<CSCRPCDigiCollection> rpcs;
  edm::Handle<CSCALCTDigiCollection> alcts;
  edm::Handle<CSCCLCTDigiCollection> clcts;
  edm::Handle<CSCCorrelatedLCTDigiCollection> correlatedlcts;
  edm::Handle<CSCDCCFormatStatusDigiCollection> statusdigis;
  edm::Handle<CSCDDUStatusDigiCollection> DDUstatusdigi;
  edm::Handle<CSCDCCStatusDigiCollection> DCCstatusdigi;
  edm::Handle<GEMPadDigiClusterCollection> gemPadsClusters;
  edm::Handle<CSCShowerDigiCollection> lctShower;
  edm::Handle<CSCShowerDigiCollection> anodeShower;
  edm::Handle<CSCShowerDigiCollection> cathodeShower;
  edm::Handle<CSCShowerDigiCollection> alct_anodeShower;

  iEvent.getByToken(wd_token, wires);
  iEvent.getByToken(sd_token, strips);
  iEvent.getByToken(cd_token, comparators);
  iEvent.getByToken(rd_token, rpcs);
  iEvent.getByToken(al_token, alcts);
  iEvent.getByToken(cl_token, clcts);
  iEvent.getByToken(co_token, correlatedlcts);

  if (StatusDigiDump)
    iEvent.getByToken(st_token, statusdigis);

  if (DDUStatusDigiDump)
    iEvent.getByToken(dd_token, DDUstatusdigi);

  if (DCCStatusDigiDump)
    iEvent.getByToken(dc_token, DCCstatusdigi);

  if (gemPadsDigiDump)
    iEvent.getByToken(gem_token, gemPadsClusters);

  if (showerDigiDump) {
    iEvent.getByToken(lct_shwr_token, lctShower);
    iEvent.getByToken(anode_shwr_token, anodeShower);
    iEvent.getByToken(cathode_shwr_token, cathodeShower);
    iEvent.getByToken(alct_anode_shwr_token, alct_anodeShower);
  }

  if (WiresDigiDump) {
    edm::LogVerbatim("ViewDigi") << std::endl;
    edm::LogVerbatim("ViewDigi") << "Event " << iEvent.id() << std::endl;
    edm::LogVerbatim("ViewDigi") << std::endl;
    edm::LogVerbatim("ViewDigi") << "********WIRES Digis********" << std::endl;
    for (CSCWireDigiCollection::DigiRangeIterator j = wires->begin(); j != wires->end(); j++) {
      edm::LogVerbatim("ViewDigi") << "Wire digis from " << CSCDetId((*j).first) << std::endl;
      std::vector<CSCWireDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCWireDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        digiItr->print();
      }
    }
  }

  if (StripDigiDump) {
    edm::LogVerbatim("ViewDigi") << std::endl;
    edm::LogVerbatim("ViewDigi") << "Event " << iEvent.id() << std::endl;
    edm::LogVerbatim("ViewDigi") << std::endl;
    edm::LogVerbatim("ViewDigi") << "********STRIPS Digis********" << std::endl;
    for (CSCStripDigiCollection::DigiRangeIterator j = strips->begin(); j != strips->end(); j++) {
      edm::LogVerbatim("ViewDigi") << "Strip digis from " << CSCDetId((*j).first) << std::endl;
      std::vector<CSCStripDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCStripDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        digiItr->print();
      }
    }
  }

  if (ComparatorDigiDump) {
    edm::LogVerbatim("ViewDigi") << std::endl;
    edm::LogVerbatim("ViewDigi") << "Event " << iEvent.id() << std::endl;
    edm::LogVerbatim("ViewDigi") << std::endl;
    edm::LogVerbatim("ViewDigi") << "********COMPARATOR Digis********" << std::endl;
    for (CSCComparatorDigiCollection::DigiRangeIterator j = comparators->begin(); j != comparators->end(); j++) {
      edm::LogVerbatim("ViewDigi") << "Comparator digis from " << CSCDetId((*j).first) << std::endl;
      std::vector<CSCComparatorDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCComparatorDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        digiItr->print();
      }
    }
  }

  if (RpcDigiDump) {
    edm::LogVerbatim("ViewDigi") << std::endl;
    edm::LogVerbatim("ViewDigi") << "Event " << iEvent.id() << std::endl;
    edm::LogVerbatim("ViewDigi") << std::endl;
    edm::LogVerbatim("ViewDigi") << "********RPC Digis********" << std::endl;
    for (CSCRPCDigiCollection::DigiRangeIterator j = rpcs->begin(); j != rpcs->end(); j++) {
      edm::LogVerbatim("ViewDigi") << "RPC digis from " << CSCDetId((*j).first) << std::endl;
      std::vector<CSCRPCDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCRPCDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        digiItr->print();
      }
    }
  }

  if (AlctDigiDump) {
    edm::LogVerbatim("ViewDigi") << std::endl;
    edm::LogVerbatim("ViewDigi") << "Event " << iEvent.id() << std::endl;
    edm::LogVerbatim("ViewDigi") << std::endl;
    edm::LogVerbatim("ViewDigi") << "********ALCT Digis********" << std::endl;
    for (CSCALCTDigiCollection::DigiRangeIterator j = alcts->begin(); j != alcts->end(); j++) {
      std::vector<CSCALCTDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCALCTDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        digiItr->print();
      }
    }
  }

  if (ClctDigiDump) {
    edm::LogVerbatim("ViewDigi") << std::endl;
    edm::LogVerbatim("ViewDigi") << "Event " << iEvent.id() << std::endl;
    edm::LogVerbatim("ViewDigi") << std::endl;
    edm::LogVerbatim("ViewDigi") << "********CLCT Digis********" << std::endl;
    for (CSCCLCTDigiCollection::DigiRangeIterator j = clcts->begin(); j != clcts->end(); j++) {
      std::vector<CSCCLCTDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCCLCTDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        digiItr->print();
      }
    }
  }

  if (CorrClctDigiDump) {
    edm::LogVerbatim("ViewDigi") << std::endl;
    edm::LogVerbatim("ViewDigi") << "Event " << iEvent.id() << std::endl;
    edm::LogVerbatim("ViewDigi") << std::endl;
    edm::LogVerbatim("ViewDigi") << "********CorrelatedLCT Digis********" << std::endl;
    for (CSCCorrelatedLCTDigiCollection::DigiRangeIterator j = correlatedlcts->begin(); j != correlatedlcts->end();
         j++) {
      std::vector<CSCCorrelatedLCTDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCCorrelatedLCTDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        digiItr->print();
      }
    }
  }

  if (StatusDigiDump) {
    edm::LogVerbatim("ViewDigi") << std::endl;
    edm::LogVerbatim("ViewDigi") << "Event " << iEvent.id() << std::endl;
    edm::LogVerbatim("ViewDigi") << std::endl;
    edm::LogVerbatim("ViewDigi") << "********STATUS Digis********" << std::endl;
    for (CSCDCCFormatStatusDigiCollection::DigiRangeIterator j = statusdigis->begin(); j != statusdigis->end(); j++) {
      std::vector<CSCDCCFormatStatusDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCDCCFormatStatusDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        digiItr->print();
      }
    }
  }

  if (DDUStatusDigiDump) {
    edm::LogVerbatim("ViewDigi") << std::endl;
    edm::LogVerbatim("ViewDigi") << "Event " << iEvent.id() << std::endl;
    edm::LogVerbatim("ViewDigi") << std::endl;
    edm::LogVerbatim("ViewDigi") << "********DDU STATUS Digis********" << std::endl;
    for (CSCDDUStatusDigiCollection::DigiRangeIterator j = DDUstatusdigi->begin(); j != DDUstatusdigi->end(); j++) {
      std::vector<CSCDDUStatusDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCDDUStatusDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        digiItr->print();
      }
    }
  }

  if (DCCStatusDigiDump) {
    edm::LogVerbatim("ViewDigi") << std::endl;
    edm::LogVerbatim("ViewDigi") << "Event " << iEvent.id() << std::endl;
    edm::LogVerbatim("ViewDigi") << std::endl;
    edm::LogVerbatim("ViewDigi") << "********DCC STATUS Digis********" << std::endl;
    for (CSCDCCStatusDigiCollection::DigiRangeIterator j = DCCstatusdigi->begin(); j != DCCstatusdigi->end(); j++) {
      std::vector<CSCDCCStatusDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCDCCStatusDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        digiItr->print();
      }
    }
  }

  if (showerDigiDump) {
    edm::LogVerbatim("ViewDigi") << std::endl;
    edm::LogVerbatim("ViewDigi") << "Event " << iEvent.id() << std::endl;
    edm::LogVerbatim("ViewDigi") << std::endl;
    edm::LogVerbatim("ViewDigi") << "********lct Shower Digis********" << std::endl;
    for (CSCShowerDigiCollection::DigiRangeIterator j = lctShower->begin(); j != lctShower->end(); j++) {
      std::vector<CSCShowerDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCShowerDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        edm::LogVerbatim("ViewDigi") << *digiItr << std::endl;
      }
    }

    edm::LogVerbatim("ViewDigi") << "********cathode Shower Digis********" << std::endl;
    for (CSCShowerDigiCollection::DigiRangeIterator j = cathodeShower->begin(); j != cathodeShower->end(); j++) {
      std::vector<CSCShowerDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCShowerDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        edm::LogVerbatim("ViewDigi") << *digiItr << std::endl;
      }
    }

    edm::LogVerbatim("ViewDigi") << "********anode Shower Digis********" << std::endl;
    for (CSCShowerDigiCollection::DigiRangeIterator j = anodeShower->begin(); j != anodeShower->end(); j++) {
      std::vector<CSCShowerDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCShowerDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        edm::LogVerbatim("ViewDigi") << *digiItr << std::endl;
      }
    }

    edm::LogVerbatim("ViewDigi") << "********ALCT anode Shower Digis********" << std::endl;
    for (CSCShowerDigiCollection::DigiRangeIterator j = alct_anodeShower->begin(); j != alct_anodeShower->end(); j++) {
      std::vector<CSCShowerDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCShowerDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        if (digiItr->isValid())
          edm::LogVerbatim("ViewDigi") << *digiItr << " bx: " << digiItr->getCSCID() << std::endl;
      }
    }
  }

  if (gemPadsDigiDump) {
    edm::LogVerbatim("ViewDigi") << std::endl;
    edm::LogVerbatim("ViewDigi") << "Event " << iEvent.id() << std::endl;
    edm::LogVerbatim("ViewDigi") << std::endl;
    edm::LogVerbatim("ViewDigi") << "********GEMPadDigiCluster Digis********" << std::endl;
    for (GEMPadDigiClusterCollection::DigiRangeIterator j = gemPadsClusters->begin(); j != gemPadsClusters->end();
         j++) {
      GEMDetId gemdetid = (*j).first;
      std::vector<GEMPadDigiCluster>::const_iterator digiItr = (*j).second.first;
      std::vector<GEMPadDigiCluster>::const_iterator last = (*j).second.second;
      edm::LogVerbatim("ViewDigi") << "GEM" << gemdetid << std::endl;
      for (; digiItr != last; ++digiItr) {
        digiItr->print();
      }
    }
  }

#ifdef THIS_IS_AN_EVENT_EXAMPLE
  Handle<ExampleData> pIn;
  iEvent.getByLabel("example", pIn);
#endif

#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
  ESHandle<SetupData> pSetup;
  iSetup.get<SetupRecord>().get(pSetup);
#endif
}

// ------------ method called once each job just after ending the event loop  ------------
void CSCViewDigi::endJob() {}

//define this as a plug-in
DEFINE_FWK_MODULE(CSCViewDigi);
