// -*- C++ -*-
//
// Package:    EcalMIPRecHitFilter
// Class:      EcalMIPRecHitFilter
//
/**\class EcalMIPRecHitFilter EcalMIPRecHitFilter.cc Work/EcalMIPRecHitFilter/src/EcalMIPRecHitFilter.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Giovanni FRANZONI
//         Created:  Wed Sep 19 16:21:29 CEST 2007
//
//

// system include files
#include <memory>
#include <vector>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/global/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "DataFormats/EcalRecHit/interface/EcalUncalibratedRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"

#include "DataFormats/DetId/interface/DetId.h"
#include "Geometry/CaloTopology/interface/CaloTopology.h"
#include "Geometry/Records/interface/CaloTopologyRecord.h"

#include "CondFormats/EcalObjects/interface/EcalIntercalibConstants.h"
#include "CondFormats/DataRecord/interface/EcalIntercalibConstantsRcd.h"
#include "CalibCalorimetry/EcalLaserCorrection/interface/EcalLaserDbService.h"
#include "CalibCalorimetry/EcalLaserCorrection/interface/EcalLaserDbRecord.h"

#include "CondFormats/EcalObjects/interface/EcalADCToGeVConstant.h"
#include "CondFormats/DataRecord/interface/EcalADCToGeVConstantRcd.h"

//
// class declaration
//

class EcalMIPRecHitFilter : public edm::global::EDFilter<> {
public:
  explicit EcalMIPRecHitFilter(const edm::ParameterSet&);

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  bool filter(edm::StreamID, edm::Event&, edm::EventSetup const&) const override;

  // ----------member data ---------------------------
  const edm::ESGetToken<CaloTopology, CaloTopologyRecord> caloTopologyRecordToken_;
  const edm::ESGetToken<EcalIntercalibConstants, EcalIntercalibConstantsRcd> ecalIntercalibConstantsRcdToken_;
  const edm::ESGetToken<EcalLaserDbService, EcalLaserDbRecord> ecalLaserDbRecordToken_;
  const edm::ESGetToken<EcalADCToGeVConstant, EcalADCToGeVConstantRcd> ecalADCToGeVConstantRcdToken_;

  const edm::EDGetTokenT<EcalRecHitCollection> EcalRecHitToken_;
  const double minAmp1_;
  const double minAmp2_;
  const double minSingleAmp_;
  const std::vector<int> maskedList_;
  const int side_;
};

//
// constructors and destructor
//
EcalMIPRecHitFilter::EcalMIPRecHitFilter(const edm::ParameterSet& iConfig)
    : caloTopologyRecordToken_(esConsumes()),
      ecalIntercalibConstantsRcdToken_(esConsumes()),
      ecalLaserDbRecordToken_(esConsumes()),
      ecalADCToGeVConstantRcdToken_(esConsumes()),
      EcalRecHitToken_(consumes<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>("EcalRecHitCollection"))),
      minAmp1_(iConfig.getUntrackedParameter<double>("AmpMinSeed", 0.063)),
      minAmp2_(iConfig.getUntrackedParameter<double>("AmpMin2", 0.045)),
      minSingleAmp_(iConfig.getUntrackedParameter<double>("SingleAmpMin", 0.108)),
      maskedList_(iConfig.getUntrackedParameter<std::vector<int>>(
          "maskedChannels", std::vector<int>{})),  // this is using the ashed index
      side_(iConfig.getUntrackedParameter<int>("side", 3)) {
  // now do what ever initialization is needed
}

//
// member functions
//

// ------------ method called on each new Event  ------------
bool EcalMIPRecHitFilter::filter(edm::StreamID, edm::Event& iEvent, edm::EventSetup const& iSetup) const {
  using namespace edm;

  // getting very basic uncalRH
  Handle<EcalRecHitCollection> recHits;
  if (not iEvent.getByToken(EcalRecHitToken_, recHits)) {
    edm::EDConsumerBase::Labels labels;
    labelsForToken(EcalRecHitToken_, labels);
    LogWarning("EcalMIPRecHitFilter") << "InputTag:  label = \"" << labels.module << "\", instance = \""
                                      << labels.productInstance << "\", process = \"" << labels.process
                                      << "\" is not available";
    return false;
  }

  auto const& caloTopo = iSetup.getHandle(caloTopologyRecordToken_);

  // Intercalib constants
  auto const& ical = iSetup.getData(ecalIntercalibConstantsRcdToken_);
  const EcalIntercalibConstantMap& icalMap = ical.getMap();

  auto const& pLaser = iSetup.getHandle(ecalLaserDbRecordToken_);

  auto const& agc = iSetup.getData(ecalADCToGeVConstantRcdToken_);
  //std::cout << "Global EB ADC->GeV scale: " << agc.getEBValue() << " GeV/ADC count" ;
  float adcconst = agc.getEBValue();

  bool thereIsSignal = false;
  // loop on  rechits
  for (auto hitItr = recHits->begin(); hitItr != recHits->end(); ++hitItr) {
    EcalRecHit const& hit = *hitItr;

    // masking noisy channels //KEEP this for now, just in case a few show up
    auto result = std::find(maskedList_.begin(), maskedList_.end(), EBDetId(hit.id()).hashedIndex());
    if (result != maskedList_.end())
      // LogWarning("EcalFilter") << "skipping uncalRecHit for channel: " << ic << " with amplitude " << ampli_ ;
      continue;

    float ampli_ = hit.energy();
    EBDetId ebDet = hit.id();

    // find intercalib constant for this xtal
    auto icalit = icalMap.find(ebDet);
    EcalIntercalibConstant icalconst = 1.;
    if (icalit != icalMap.end()) {
      icalconst = (*icalit);
      //LogDebug("EcalRecHitDebug") << "Found intercalib for xtal " << EBDetId(it->id()).ic() << " " << icalconst ;
    } else {
      //edm::LogError("EcalRecHitError") << "No intercalib const found for xtal " << EBDetId(ebDet) << "! something wrong with EcalIntercalibConstants in your DB? " ;
    }
    float lasercalib = pLaser->getLaserCorrection(EBDetId(ebDet), iEvent.time());

    ampli_ /=
        (icalconst * lasercalib * adcconst);  ///LASER and CALIB constants from the DB //PUT THRESHOLDS IN ADC... AGAIN.
    // seeking channels with signal and displaced jitter
    if (ampli_ >= minSingleAmp_) {
      //std::cout << " THIS AMPLITUDE WORKS " << ampli_ << std::endl;
      thereIsSignal = true;
      // LogWarning("EcalFilter")  << "at evet: " << iEvent.id().event()
      // 				       << " and run: " << iEvent.id().run()
      // 				       << " there is OUT OF TIME signal at chanel: " << ic
      // 				       << " with amplitude " << ampli_  << " and max at: " << jitter_;
      break;
    }

    //Check for more robust selection other than just single crystal cosmics
    if (ampli_ >= minAmp1_) {
      //std::cout << " THIS AMPLITUDE WORKS " << ampli_ << std::endl;
      std::vector<DetId> neighbors = caloTopo->getWindow(ebDet, side_, side_);
      float secondMin = 0.;
      for (std::vector<DetId>::const_iterator detitr = neighbors.begin(); detitr != neighbors.end(); ++detitr) {
        auto thishit = recHits->find((*detitr));
        if (thishit == recHits->end()) {
          //LogWarning("EcalMIPRecHitFilter") << "No RecHit available, for "<< EBDetId(*detitr);
          continue;
        }
        if ((*thishit).id() != ebDet) {
          float thisamp = (*thishit).energy();
          // find intercalib constant for this xtal
          auto icalit2 = icalMap.find((*thishit).id());
          EcalIntercalibConstant icalconst2 = 1.;
          if (icalit2 != icalMap.end()) {
            icalconst2 = (*icalit2);
            //	   LogDebug("EcalRecHitDebug") << "Found intercalib for xtal " << EBDetId(it->id()).ic() << " " << icalconst ;
          } else {
            //edm::LogError("EcalRecHitError") << "No intercalib const found for xtal " << EBDetId(ebDet) << "! something wrong with EcalIntercalibConstants in your DB? " ;
          }
          float lasercalib2 = pLaser->getLaserCorrection(EBDetId((*thishit).id()), iEvent.time());
          thisamp /= (icalconst2 * lasercalib2 * adcconst);  ///LASER and CALIB constants from the DB
          if (thisamp > secondMin)
            secondMin = thisamp;
        }
      }

      if (secondMin > minAmp2_) {
        thereIsSignal = true;
        break;
      }
    }
  }
  //std::cout << " Ok is There one of THEM " << thereIsSignal << std::endl;
  return thereIsSignal;
}

void EcalMIPRecHitFilter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;

  desc.add<edm::InputTag>("EcalRecHitCollection", edm::InputTag("ecalRecHit", "EcalRecHitsEB"));
  desc.addUntracked<double>("AmpMinSeed", 0.045);
  desc.addUntracked<double>("AmpMin2", 0.045);
  desc.addUntracked<double>("SingleAmpMin", 0.108);
  desc.addUntracked<std::vector<int>>("maskedChannels", std::vector<int>{});
  desc.addUntracked<int>("side", 3);

  descriptions.add("ecalMIPRecHitFilter", desc);
}

// declare this class as a framework plugin
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(EcalMIPRecHitFilter);
