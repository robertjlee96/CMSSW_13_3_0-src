// Tests effect of LogFlush by cfg-configurable choices of how many
// messages to use to clog the queue and whether or not FlushMessageLog
// is invoked.  Under normal testing, it will invoke FlushMessageLog in
// a situation where its absence would result in different output.

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/global/EDAnalyzer.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <string>

namespace edmtest {

  class UnitTestClient_P : public edm::global::EDAnalyzer<> {
  public:
    explicit UnitTestClient_P(edm::ParameterSet const& p) : useLogFlush(true), queueFillers(1) {
      useLogFlush = p.getUntrackedParameter<bool>("useLogFlush", useLogFlush);
      queueFillers = p.getUntrackedParameter<int>("queueFillers", queueFillers);
    }

    void analyze(edm::StreamID, edm::Event const&, edm::EventSetup const&) const override;

  private:
    bool useLogFlush;
    int queueFillers;
  };

  void UnitTestClient_P::analyze(edm::StreamID, edm::Event const&, edm::EventSetup const&) const {
    edm::LogWarning("configuration") << "useLogFlush = " << useLogFlush << " queueFillers = " << queueFillers;
    std::string longMessage;
    for (int k = 0; k < 100; k++) {
      longMessage += "Line in long message\n";
    }
    for (int i = 0; i < queueFillers; ++i) {
      edm::LogInfo("cat") << "message " << i << "\n" << longMessage;
    }

    edm::LogError("keyMessage") << "This message is issued just before abort";
    if (useLogFlush)
      edm::FlushMessageLog();
    _exit(0);
  }

}  // namespace edmtest

using edmtest::UnitTestClient_P;
DEFINE_FWK_MODULE(UnitTestClient_P);
