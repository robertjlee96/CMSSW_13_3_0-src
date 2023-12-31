// -*- C++ -*-
//
// Package:    L1GctTest
// Class:      L1GctTest
//
/**\class L1GctTest L1GctTest.cc L1Trigger/GlobalCaloTrigger/test/L1GctTest.cc

 Description: a collection of GCT tests

*/
//
// Original Author:  Gregory Heath
//         Created:  Mon Mar 12 16:36:35 CET 2007
//
//

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "L1Trigger/GlobalCaloTrigger/interface/L1GlobalCaloTrigger.h"

class gctTestFunctions;

//
// class declaration
//
class L1GctJetFinderParamsRcd;
class L1GctChannelMaskRcd;
class L1JetEtScaleRcd;
class L1HtMissScaleRcd;
class L1HfRingEtScaleRcd;

class L1GctTest : public edm::one::EDAnalyzer<> {
public:
  /// typedefs
  typedef L1GlobalCaloTrigger::lutPtr lutPtr;
  typedef L1GlobalCaloTrigger::lutPtrVector lutPtrVector;

  explicit L1GctTest(const edm::ParameterSet&);
  ~L1GctTest();

private:
  virtual void beginJob();
  virtual void analyze(const edm::Event& e, const edm::EventSetup& c);
  virtual void endJob();

  void configureGct(const edm::EventSetup& c);
  void configParamsPrint(std::ostream& out);

  // ----------member data ---------------------------

  L1GlobalCaloTrigger* m_gct;
  lutPtrVector m_jetEtCalibLuts;

  std::unique_ptr<gctTestFunctions> m_tester;

  edm::ESGetToken<L1GctJetFinderParams, L1GctJetFinderParamsRcd> m_jfParsToken;
  edm::ESGetToken<L1GctChannelMask, L1GctChannelMaskRcd> m_chanMaskToken;
  edm::ESGetToken<L1CaloEtScale, L1JetEtScaleRcd> m_etScaleToken;
  edm::ESGetToken<L1CaloEtScale, L1HtMissScaleRcd> m_htMissScaleToken;
  edm::ESGetToken<L1CaloEtScale, L1HfRingEtScaleRcd> m_hfRingEtScaleToken;

  bool theElectronTestIsEnabled;
  bool theSingleEventTestIsEnabled;
  bool theEnergyAlgosTestIsEnabled;
  bool theFirmwareTestIsEnabled;
  bool theRealDataTestIsEnabled;

  bool theUseNewTauAlgoFlag;
  bool theConfigParamsPrintFlag;

  std::string theInputDataFileName;
  std::string theReferenceDataFileName;
  std::string theEnergySumsDataFileName;

  int m_firstBx;
  int m_lastBx;

  unsigned m_eventNo;
  bool m_allGood;
};
