/* -*- C++ -*- */
#ifndef HcalTBSource_h_included
#define HcalTBSource_h_included 1

#include <map>
#include <string>
#include <vector>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Sources/interface/ProducerSourceFromFiles.h"

class TFile;
class TTree;
class CDFChunk;
class CDFEventInfo;

/** \class HcalTBSource
   \note Notice that there is a hack to renumber events from runs where the first event number was zero.
*/
class HcalTBSource : public edm::ProducerSourceFromFiles {
public:
  explicit HcalTBSource(const edm::ParameterSet& pset, edm::InputSourceDescription const& desc);
  ~HcalTBSource() override;

private:
  bool setRunAndEventInfo(edm::EventID& id, edm::TimeValue_t& time, edm::EventAuxiliary::ExperimentType&) override;
  void produce(edm::Event& e) override;
  void unpackSetup(const std::vector<std::string>& params);
  void openFile(const std::string& filename);
  TTree* m_tree;
  TFile* m_file;
  int m_i, m_fileCounter;
  bool m_quiet, m_onlyRemapped;
  int n_chunks;
  static const int CHUNK_COUNT = 64;  // MAX Chunks
  CDFChunk* m_chunks[CHUNK_COUNT];
  int m_chunkIds[CHUNK_COUNT];
  std::map<std::string, int> m_sourceIdRemap;
  CDFEventInfo* m_eventInfo;
  int m_eventNumberOffset, m_skip;
};

#endif  // HcalTBSource_h_included
