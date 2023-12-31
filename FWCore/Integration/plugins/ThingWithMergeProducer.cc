
//
// Puts some simple test objects in the event, run, and lumi
// principals.  The values put into these objects are just
// arbitrary and meaningless.  Later we check that what get
// out when reading the file after merging is what is expected.
//
// Original Author: David Dagenhart, Fermilab, February 2008

#include "ThingWithMergeProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/TestObjects/interface/Thing.h"
#include "DataFormats/TestObjects/interface/ThingWithMerge.h"
#include "DataFormats/TestObjects/interface/ThingWithIsEqual.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

namespace edmtest {
  ThingWithMergeProducer::ThingWithMergeProducer(edm::ParameterSet const& pset)
      : changeIsEqualValue_(pset.getUntrackedParameter<bool>("changeIsEqualValue", false)),
        labelsToGet_(pset.getUntrackedParameter<std::vector<std::string>>("labelsToGet", std::vector<std::string>())),
        noPut_(pset.getUntrackedParameter<bool>("noPut", false)) {
    produces<Thing>("event");
    produces<Thing, edm::Transition::BeginLuminosityBlock>("beginLumi");
    produces<Thing, edm::Transition::EndLuminosityBlock>("endLumi");
    produces<Thing, edm::Transition::BeginRun>("beginRun");
    produces<Thing, edm::Transition::EndRun>("endRun");

    produces<ThingWithMerge>("event");
    produces<ThingWithMerge, edm::Transition::BeginLuminosityBlock>("beginLumi");
    produces<ThingWithMerge, edm::Transition::EndLuminosityBlock>("endLumi");
    produces<ThingWithMerge, edm::Transition::BeginRun>("beginRun");
    produces<ThingWithMerge, edm::Transition::EndRun>("endRun");

    produces<ThingWithIsEqual>("event");
    produces<ThingWithIsEqual, edm::Transition::BeginLuminosityBlock>("beginLumi");
    produces<ThingWithIsEqual, edm::Transition::EndLuminosityBlock>("endLumi");
    produces<ThingWithIsEqual, edm::Transition::BeginRun>("beginRun");
    produces<ThingWithIsEqual, edm::Transition::EndRun>("endRun");

    const std::string s_prod("PROD");
    for (auto const& label : labelsToGet_) {
      consumes<Thing>(edm::InputTag(label, "event", s_prod));
      consumes<Thing, edm::InLumi>(edm::InputTag(label, "beginLumi", s_prod));
      consumes<Thing, edm::InLumi>(edm::InputTag(label, "endLumi", s_prod));
      consumes<Thing, edm::InRun>(edm::InputTag(label, "beginRun", s_prod));
      consumes<Thing, edm::InRun>(edm::InputTag(label, "endRun", s_prod));
    }
  }

  ThingWithMergeProducer::~ThingWithMergeProducer() {}

  void ThingWithMergeProducer::produce(edm::Event& e, edm::EventSetup const&) {
    // The purpose of this first getByLabel call is to cause the products
    // that are "put" below to have a parent so we can do tests with the
    // parentage provenance.
    for (Iter iter = labelsToGet_.begin(), ie = labelsToGet_.end(); iter != ie; ++iter) {
      edm::Handle<Thing> h;
      edm::InputTag tag(*iter, "event", "PROD");
      e.getByLabel(tag, h);
    }

    auto result = std::make_unique<Thing>();
    result->a = 11;
    if (!noPut_)
      e.put(std::move(result), std::string("event"));

    auto result2 = std::make_unique<ThingWithMerge>();
    result2->a = 12;
    if (!noPut_)
      e.put(std::move(result2), std::string("event"));

    auto result3 = std::make_unique<ThingWithIsEqual>();
    result3->a = 13;
    if (changeIsEqualValue_)
      result3->a = 14;
    if (!noPut_)
      e.put(std::move(result3), std::string("event"));
  }

  void ThingWithMergeProducer::beginLuminosityBlockProduce(edm::LuminosityBlock& lb, edm::EventSetup const&) {
    auto result = std::make_unique<Thing>();
    result->a = 101;
    if (!noPut_)
      lb.put(std::move(result), "beginLumi");

    auto result2 = std::make_unique<ThingWithMerge>();
    result2->a = 102;
    if (!noPut_)
      lb.put(std::move(result2), "beginLumi");

    auto result3 = std::make_unique<ThingWithIsEqual>();
    result3->a = 103;
    if (changeIsEqualValue_)
      result3->a = 104;
    if (!noPut_)
      lb.put(std::move(result3), "beginLumi");
  }

  void ThingWithMergeProducer::endLuminosityBlockProduce(edm::LuminosityBlock& lb, edm::EventSetup const&) {
    auto result = std::make_unique<Thing>();
    result->a = 1001;
    if (!noPut_)
      lb.put(std::move(result), "endLumi");

    auto result2 = std::make_unique<ThingWithMerge>();
    result2->a = 1002;
    if (!noPut_)
      lb.put(std::move(result2), "endLumi");

    auto result3 = std::make_unique<ThingWithIsEqual>();
    result3->a = 1003;
    if (changeIsEqualValue_)
      result3->a = 1004;
    if (!noPut_)
      lb.put(std::move(result3), "endLumi");
  }

  // Functions that gets called by framework every run
  void ThingWithMergeProducer::beginRunProduce(edm::Run& r, edm::EventSetup const&) {
    auto result = std::make_unique<Thing>();
    result->a = 10001;
    if (!noPut_)
      r.put(std::move(result), "beginRun");

    auto result2 = std::make_unique<ThingWithMerge>();
    result2->a = 10002;
    if (!noPut_)
      r.put(std::move(result2), "beginRun");

    auto result3 = std::make_unique<ThingWithIsEqual>();
    result3->a = 10003;
    if (changeIsEqualValue_)
      result3->a = 10004;
    if (!noPut_)
      r.put(std::move(result3), "beginRun");
  }

  void ThingWithMergeProducer::endRunProduce(edm::Run& r, edm::EventSetup const&) {
    auto result = std::make_unique<Thing>();
    result->a = 100001;
    if (!noPut_)
      r.put(std::move(result), "endRun");

    auto result2 = std::make_unique<ThingWithMerge>();
    result2->a = 100002;
    if (!noPut_)
      r.put(std::move(result2), "endRun");

    auto result3 = std::make_unique<ThingWithIsEqual>();
    result3->a = 100003;
    if (changeIsEqualValue_)
      result3->a = 100004;
    if (!noPut_)
      r.put(std::move(result3), "endRun");
  }

  void ThingWithMergeProducer::beginLuminosityBlock(edm::LuminosityBlock const& lb, edm::EventSetup const&) {
    for (Iter iter = labelsToGet_.begin(), ie = labelsToGet_.end(); iter != ie; ++iter) {
      edm::Handle<Thing> h;
      edm::InputTag tag(*iter, "beginLumi", "PROD");
      lb.getByLabel(tag, h);
    }
  }

  void ThingWithMergeProducer::endLuminosityBlock(edm::LuminosityBlock const& lb, edm::EventSetup const&) {
    for (Iter iter = labelsToGet_.begin(), ie = labelsToGet_.end(); iter != ie; ++iter) {
      edm::Handle<Thing> h;
      edm::InputTag tag(*iter, "endLumi", "PROD");
      lb.getByLabel(tag, h);
    }
  }

  // Functions that gets called by framework every run
  void ThingWithMergeProducer::beginRun(edm::Run const& r, edm::EventSetup const&) {
    for (Iter iter = labelsToGet_.begin(), ie = labelsToGet_.end(); iter != ie; ++iter) {
      edm::Handle<Thing> h;
      edm::InputTag tag(*iter, "beginRun", "PROD");
      r.getByLabel(tag, h);
    }
  }

  void ThingWithMergeProducer::endRun(edm::Run const& r, edm::EventSetup const&) {
    for (Iter iter = labelsToGet_.begin(), ie = labelsToGet_.end(); iter != ie; ++iter) {
      edm::Handle<Thing> h;
      edm::InputTag tag(*iter, "endRun", "PROD");
      r.getByLabel(tag, h);
    }
  }

  void ThingWithMergeProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.addUntracked<bool>("changeIsEqualValue", false);
    desc.addUntracked<std::vector<std::string>>("labelsToGet", std::vector<std::string>());
    desc.addUntracked<bool>("noPut", false);
    descriptions.add("thingWithMergeProducer", desc);
  }

}  // namespace edmtest
using edmtest::ThingWithMergeProducer;
DEFINE_FWK_MODULE(ThingWithMergeProducer);
