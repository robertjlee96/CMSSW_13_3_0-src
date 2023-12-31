#ifndef Validation_MuonCSCDigis_CSCComparatorDigiValidation_H
#define Validation_MuonCSCDigis_CSCComparatorDigiValidation_H

#include "FWCore/Framework/interface/ConsumesCollector.h"

#include "DataFormats/CSCDigi/interface/CSCComparatorDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCStripDigiCollection.h"

#include "DQMServices/Core/interface/DQMStore.h"
#include "Validation/MuonCSCDigis/interface/CSCBaseValidation.h"

class CSCComparatorDigiValidation : public CSCBaseValidation {
public:
  CSCComparatorDigiValidation(const edm::ParameterSet &ps, edm::ConsumesCollector &&iC);
  ~CSCComparatorDigiValidation() override;
  void bookHistograms(DQMStore::IBooker &);
  void analyze(const edm::Event &, const edm::EventSetup &) override;
  void plotResolution(const PSimHit &hit, int strip, const CSCLayer *layer, int chamberType);

private:
  edm::EDGetTokenT<CSCStripDigiCollection> strips_Token_;
  edm::EDGetTokenT<CSCComparatorDigiCollection> comparators_Token_;
  edm::InputTag inputTagStrip_;
  edm::InputTag inputTagComp_;

  MonitorElement *theTimeBinPlots[10];
  MonitorElement *theNDigisPerLayerPlots[10];
  MonitorElement *theStripDigiPlots[10];
  MonitorElement *the3StripPlots[10];
  MonitorElement *theNDigisPerEventPlot;
  MonitorElement *theResolutionPlots[10];
};

#endif
