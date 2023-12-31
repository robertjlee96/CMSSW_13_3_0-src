/*----------------------------------------------------------------------

Toy EDProducers and EDProducts for testing purposes only.

----------------------------------------------------------------------*/

#include <stdexcept>
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include "FWCore/Framework/interface/global/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Utilities/interface/ESGetToken.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "CondFormats/CSCObjects/interface/CSCPedestals.h"
#include "CondFormats/DataRecord/interface/CSCPedestalsRcd.h"

using namespace std;

namespace edmtest {
  class CSCPedestalReadAnalyzer : public edm::global::EDAnalyzer<> {
  public:
    explicit CSCPedestalReadAnalyzer(edm::ParameterSet const& p) : token_{esConsumes()} {}
    ~CSCPedestalReadAnalyzer() override {}
    void analyze(edm::StreamID, const edm::Event& e, const edm::EventSetup& c) const override;

  private:
    edm::ESGetToken<CSCPedestals, CSCPedestalsRcd> token_;
  };

  void CSCPedestalReadAnalyzer::analyze(edm::StreamID, const edm::Event& e, const edm::EventSetup& context) const {
    using namespace edm::eventsetup;

    edm::LogSystem log("CSCPedestals");

    log << " I AM IN RUN NUMBER " << e.id().run() << std::endl;
    log << " ---EVENT NUMBER " << e.id().event() << std::endl;

    //call tracker code
    //
    /*
    int layerID=4;
    //CSCPedestals* myped=const_cast<CSCPedestals*>(pPeds.product());
    const CSCPedestals* myped=pPeds.product();
    std::map<int,std::vector<CSCPedestals::Item> >::const_iterator it=myped->pedestals.find(layerID);
    log << "looking for CSC layer: " << layerID<<std::endl;
    if( it!=myped->pedestals.end() ){
      log<<"layer id found "<<it->first<<std::endl;
      std::vector<CSCPedestals::Item>::const_iterator pedit;
      for( pedit=it->second.begin(); pedit!=it->second.end(); ++pedit ){
	log << "  ped:  " <<pedit->ped << " rms: " << pedit->rms
		  << std::endl;
      }
    }
    */
    const CSCPedestals* myped = &context.getData(token_);
    std::map<int, std::vector<CSCPedestals::Item> >::const_iterator it;
    for (it = myped->pedestals.begin(); it != myped->pedestals.end(); ++it) {
      log << "layer id found " << it->first << std::endl;
      std::vector<CSCPedestals::Item>::const_iterator pedit;
      for (pedit = it->second.begin(); pedit != it->second.end(); ++pedit) {
        log << "  ped:  " << pedit->ped << " rms: " << pedit->rms << std::endl;
      }
    }
  }
  DEFINE_FWK_MODULE(CSCPedestalReadAnalyzer);
}  // namespace edmtest
