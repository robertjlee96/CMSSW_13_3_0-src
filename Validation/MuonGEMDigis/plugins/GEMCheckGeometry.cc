#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "Geometry/CommonTopologies/interface/StripTopology.h"
#include "Geometry/GEMGeometry/interface/GEMEtaPartitionSpecs.h"
#include "Validation/MuonGEMDigis/plugins/GEMCheckGeometry.h"

#include <iomanip>

GEMCheckGeometry::GEMCheckGeometry(const edm::ParameterSet &gc) {
  GE11PhiBegin_ = gc.getUntrackedParameter<double>("GE11PhiBegin", -5.);
  GE11PhiStep_ = gc.getUntrackedParameter<double>("GE11PhiStep", 10);
  minPhi_ = gc.getUntrackedParameter<double>("minPhi", -180.);
  maxPhi_ = gc.getUntrackedParameter<double>("maxPhi", +180.);
  detailPlot_ = gc.getParameter<bool>("detailPlot");
  geomToken_ = esConsumes<GEMGeometry, MuonGeometryRecord, edm::Transition::BeginRun>();
}

void GEMCheckGeometry::bookHistograms(DQMStore::IBooker &ibooker, edm::Run const &Run, edm::EventSetup const &iSetup) {
  if (!detailPlot_)
    return;

  const auto gemH = iSetup.getHandle(geomToken_);
  if (!gemH.isValid()) {
    edm::LogError("GEMCheckGeometry") << "Failed to initialize GEM geometry.";
    return;
  }
  const GEMGeometry *geometry = gemH.product();

  ibooker.setCurrentFolder("MuonGEMDigisV/GEMDigisTask");
  LogDebug("GEMCheckGeometry") << "ibooker set current folder\n";

  for (auto region : geometry->regions()) {
    TString title = TString::Format("Geometry's phi distribution on Region %d ; #phi(degree); ;", region->region());
    TString name = TString::Format("geo_phi_r%d", region->region());
    auto temp_me = ibooker.book2D(name.Data(), title.Data(), 360000, -180., 180, 12, 1, 13);
    temp_me->setBinLabel(1, "St1,La1_even", 2);
    temp_me->setBinLabel(2, "St1,La1_odd", 2);
    temp_me->setBinLabel(3, "St1,La2_even", 2);
    temp_me->setBinLabel(4, "St1,La2_odd", 2);
    temp_me->setBinLabel(5, "St2,La1_even", 2);
    temp_me->setBinLabel(6, "St2,La1_odd", 2);
    temp_me->setBinLabel(7, "St2,La2_even", 2);
    temp_me->setBinLabel(8, "St2,La2_odd", 2);
    theStdPlots.insert(std::map<UInt_t, MonitorElement *>::value_type(name.Hash(), temp_me));
  }

  for (auto region : geometry->regions()) {
    for (auto station : region->stations()) {
      for (auto ring : station->rings()) {
        for (auto sch : ring->superChambers()) {
          for (auto ch : sch->chambers()) {
            for (auto roll : ch->etaPartitions()) {
              const StripTopology *topology(&(roll->specificTopology()));
              auto parameters(roll->specs()->parameters());
              float nStrips(parameters[3]);
              for (int strip = 0; strip <= nStrips; strip++) {
                LocalPoint lEdge(topology->localPosition(strip));

                double phi = roll->toGlobal(lEdge).phi().degrees();

                GEMDetId id(roll->id());
                int region_idx = id.region();
                int station_idx = id.station();
                int chamber_idx = id.chamber();
                int layer_idx = id.layer();
                int value = (station_idx - 1) * 4 + (layer_idx - 1) * 2 + (chamber_idx % 2) + 1;

                if (region_idx == 1) {
                  UInt_t hash = TString("geo_phi_r1").Hash();
                  theStdPlots[hash]->Fill(phi, value);
                } else {
                  UInt_t hash = TString("geo_phi_r-1").Hash();
                  theStdPlots[hash]->Fill(phi, value);
                }
              }
            }
          }
        }
      }
    }
  }
}

GEMCheckGeometry::~GEMCheckGeometry() {}

void GEMCheckGeometry::analyze(const edm::Event &e, const edm::EventSetup &) {}
