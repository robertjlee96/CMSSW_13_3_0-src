// -*- C++ -*-
//
// Package:    PlottingDevice
// Class:      PlottingDevice
//
/**\class PlottingDevice PlottingDevice.cc Workspace/PlottingDevice/src/PlottingDevice.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Jean-Roch Vlimant
//         Created:  Thu May 15 14:37:59 CEST 2008
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
//
// class decleration
//

#include "PhysicsTools/UtilAlgos/interface/Plotter.h"
#include "PhysicsTools/UtilAlgos/interface/VariableHelper.h"

class PlottingDevice : public edm::one::EDAnalyzer<> {
public:
  explicit PlottingDevice(const edm::ParameterSet&);

private:
  void analyze(const edm::Event&, const edm::EventSetup&) override;

  // ----------member data ---------------------------
  std::string vHelperInstance_;
  std::string plotDirectoryName_;
  std::unique_ptr<Plotter> plotter_;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
PlottingDevice::PlottingDevice(const edm::ParameterSet& iConfig) {
  vHelperInstance_ = iConfig.getParameter<std::string>("@module_label");
  plotDirectoryName_ = "PlottingDevice";

  //configure the inputtag distributor
  if (iConfig.exists("InputTags"))
    edm::Service<InputTagDistributorService>()->init(
        vHelperInstance_, iConfig.getParameter<edm::ParameterSet>("InputTags"), consumesCollector());

  //configure the variable helper
  edm::Service<VariableHelperService>()->init(
      vHelperInstance_, iConfig.getParameter<edm::ParameterSet>("Variables"), consumesCollector());

  //configure the plotting device
  edm::ParameterSet plotPset = iConfig.getParameter<edm::ParameterSet>("Plotter");
  std::string plotterName = plotPset.getParameter<std::string>("ComponentName");
  plotter_ = PlotterFactory::get()->create(plotterName, plotPset);
}

//
// member functions
//

// ------------ method called to for each event  ------------
void PlottingDevice::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  plotter_->setDir(plotDirectoryName_);

  plotter_->fill(plotDirectoryName_, iEvent);
}
//define this as a plug-in
DEFINE_FWK_MODULE(PlottingDevice);
