#ifndef Validation_EventGenerator_DQMHelper
#define Validation_EventGenerator_DQMHelper

/* class DQMHelper
 *
 * Simple  class to configure dqm histograms
 *
 */

#include <iostream>
#include "DQMServices/Core/interface/DQMStore.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "DQMServices/Core/interface/DQMEDAnalyzer.h"

class DQMHelper {
public:
  typedef DQMEDAnalyzer::DQMStore DQMStore;
  typedef DQMEDAnalyzer::MonitorElement MonitorElement;

  DQMHelper(DQMStore::IBooker *i);
  virtual ~DQMHelper();

  MonitorElement *book1dHisto(const std::string &name,
                              const std::string &title,
                              int n,
                              double xmin,
                              double xmax,
                              const std::string &xaxis,
                              const std::string &yaxis);
  MonitorElement *book2dHisto(const std::string &name,
                              const std::string &title,
                              int nx,
                              double xmin,
                              double xmax,
                              int ny,
                              double ymin,
                              double ymax,
                              const std::string &xaxis,
                              const std::string &yaxis);

  MonitorElement *book1dHisto(const std::string &name, const std::string &title, int n, double xmin, double xmax);
  MonitorElement *book2dHisto(const std::string &name,
                              const std::string &title,
                              int nx,
                              double xmin,
                              double xmax,
                              int ny,
                              double ymin,
                              double ymax);

private:
  DQMStore::IBooker *ibooker;
};

#endif
