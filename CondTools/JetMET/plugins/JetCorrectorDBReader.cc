// -*- C++ -*-
//
// Package:    JetCorrectorDBReader
// Class:
//
/**\class JetCorrectorDBReader

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Benedikt Hegner
//         Created:  Tue Mar 09 01:32:51 CET 2010
//
//

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "JetMETCorrections/Objects/interface/JetCorrectionsRecord.h"
//
// class declaration
//

class JetCorrectorDBReader : public edm::one::EDAnalyzer<> {
public:
  explicit JetCorrectorDBReader(const edm::ParameterSet&);
  ~JetCorrectorDBReader() override;

private:
  void beginJob() override;
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  void endJob() override;

  std::string mPayloadName, mGlobalTag;
  bool mCreateTextFile, mPrintScreen;
  edm::ESGetToken<JetCorrectorParametersCollection, JetCorrectionsRecord> mPayloadToken;
};

JetCorrectorDBReader::JetCorrectorDBReader(const edm::ParameterSet& iConfig) {
  mPayloadName = iConfig.getUntrackedParameter<std::string>("payloadName");
  mPayloadToken = esConsumes(edm::ESInputTag("", mPayloadName));
  mGlobalTag = iConfig.getUntrackedParameter<std::string>("globalTag");
  mPrintScreen = iConfig.getUntrackedParameter<bool>("printScreen");
  mCreateTextFile = iConfig.getUntrackedParameter<bool>("createTextFile");
}

JetCorrectorDBReader::~JetCorrectorDBReader() {}

void JetCorrectorDBReader::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  std::cout << "Inspecting JEC payload with label: " << mPayloadName << std::endl;
  auto const& JetCorParamsColl = iSetup.getData(mPayloadToken);
  std::vector<JetCorrectorParametersCollection::key_type> keys;
  JetCorParamsColl.validKeys(keys);
  for (std::vector<JetCorrectorParametersCollection::key_type>::const_iterator ibegin = keys.begin(),
                                                                               iend = keys.end(),
                                                                               ikey = ibegin;
       ikey != iend;
       ++ikey) {
    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "Processing key = " << *ikey << std::endl;
    std::cout << "object label: " << JetCorParamsColl.findLabel(*ikey) << std::endl;
    JetCorrectorParameters const& JetCorParams = JetCorParamsColl[*ikey];

    if (mCreateTextFile) {
      std::cout << "Creating txt file: "
                << mGlobalTag + "_" + mPayloadName + "_" + JetCorParamsColl.findLabel(*ikey) + ".txt" << std::endl;
      JetCorParams.printFile(mGlobalTag + "_" + JetCorParamsColl.findLabel(*ikey) + "_" + mPayloadName + ".txt");
    }
    if (mPrintScreen)
      JetCorParams.printScreen();
  }
}

void JetCorrectorDBReader::beginJob() {}

void JetCorrectorDBReader::endJob() {}

//define this as a plug-in
DEFINE_FWK_MODULE(JetCorrectorDBReader);
