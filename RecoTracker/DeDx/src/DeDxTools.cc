#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "RecoTracker/DeDx/interface/DeDxTools.h"

#include <vector>
#include <numeric>

namespace deDxTools {
  using namespace std;
  using namespace reco;

  bool shapeSelection(const SiStripCluster& clus) {
    // ----------------  Counting the number of maxima   --------------------------
    //----------------------------------------------------------------------------
    auto const& ampls = clus.amplitudes();
    Int_t NofMax = 0;
    Int_t recur255 = 1;
    Int_t recur254 = 1;
    bool MaxOnStart = false;
    bool MaxInMiddle = false, MaxOnEnd = false;
    Int_t MaxPos = 0;

    // Start with max
    if (ampls.size() != 1 &&
        ((ampls[0] > ampls[1]) ||
         (ampls.size() > 2 && ampls[0] == ampls[1] && ampls[1] > ampls[2] && ampls[0] != 254 && ampls[0] != 255) ||
         (ampls.size() == 2 && ampls[0] == ampls[1] && ampls[0] != 254 && ampls[0] != 255))) {
      NofMax = NofMax + 1;
      MaxOnStart = true;
    }

    // Maximum reached
    if (ampls.size() > 2) {
      for (unsigned int i = 1; i < ampls.size() - 1U; i++) {
        if ((ampls[i] > ampls[i - 1] && ampls[i] > ampls[i + 1]) ||
            (ampls.size() > 3 && i > 0 && i < ampls.size() - 2U && ampls[i] == ampls[i + 1] &&
             ampls[i] > ampls[i - 1] && ampls[i] > ampls[i + 2] && ampls[i] != 254 && ampls[i] != 255)) {
          NofMax = NofMax + 1;
          MaxInMiddle = true;
          MaxPos = i;
        }
        if (ampls[i] == 255 && ampls[i] == ampls[i - 1]) {
          recur255 = recur255 + 1;
          MaxPos = i - (recur255 / 2);
          if (ampls[i] > ampls[i + 1]) {
            NofMax = NofMax + 1;
            MaxInMiddle = true;
          }
        }
        if (ampls[i] == 254 && ampls[i] == ampls[i - 1]) {
          recur254 = recur254 + 1;
          MaxPos = i - (recur254 / 2);
          if (ampls[i] > ampls[i + 1]) {
            NofMax = NofMax + 1;
            MaxInMiddle = true;
          }
        }
      }
    }

    // End with a max
    if (ampls.size() > 1) {
      if (ampls[ampls.size() - 1] > ampls[ampls.size() - 2] ||
          (ampls.size() > 2 && ampls[ampls.size() - 1] == ampls[ampls.size() - 2] &&
           ampls[ampls.size() - 2] > ampls[ampls.size() - 3]) ||
          ampls[ampls.size() - 1] == 255) {
        NofMax = NofMax + 1;
        MaxOnEnd = true;
      }
    }

    // If only one strip touched
    if (ampls.size() == 1) {
      NofMax = 1;
    }

    // --------------  SHAPE-BASED SELECTION FOR UNIQUE MAXIMAS --------------
    //------------------------------------------------------------------------
    /*
                 ____
                |    |____
            ____|    |    |
           |    |    |    |____
       ____|    |    |    |    |
      |    |    |    |    |    |____
    __|____|____|____|____|____|____|__
    C_Mnn C_Mn C_M  C_D  C_Dn C_Dnn
    */
    bool shapecdtn = false;

    Float_t C_M = 0.0;
    Float_t C_D = 0.0;
    Float_t C_Mn = 10000;
    Float_t C_Dn = 10000;
    Float_t C_Mnn = 10000;
    Float_t C_Dnn = 10000;
    Int_t CDPos;
    Float_t coeff1 = 1.7;
    Float_t coeff2 = 2.0;
    Float_t coeffn = 0.10;
    Float_t coeffnn = 0.02;
    Float_t noise = 4.0;

    if (NofMax == 1) {
      if (MaxOnStart == true) {
        C_M = (Float_t)ampls[0];
        C_D = (Float_t)ampls[1];
        if (ampls.size() < 3)
          shapecdtn = true;
        else if (ampls.size() == 3) {
          C_Dn = (Float_t)ampls[2];
          if (C_Dn <= coeff1 * coeffn * C_D + coeff2 * coeffnn * C_M + 2 * noise || C_D == 255)
            shapecdtn = true;
        } else if (ampls.size() > 3) {
          C_Dn = (Float_t)ampls[2];
          C_Dnn = (Float_t)ampls[3];
          if ((C_Dn <= coeff1 * coeffn * C_D + coeff2 * coeffnn * C_M + 2 * noise || C_D == 255) &&
              C_Dnn <= coeff1 * coeffn * C_Dn + coeff2 * coeffnn * C_D + 2 * noise) {
            shapecdtn = true;
          }
        }
      }

      if (MaxOnEnd == true) {
        C_M = (Float_t)ampls[ampls.size() - 1];
        C_D = (Float_t)ampls[ampls.size() - 2];
        if (ampls.size() < 3)
          shapecdtn = true;
        else if (ampls.size() == 3) {
          C_Dn = (Float_t)ampls[0];
          if (C_Dn <= coeff1 * coeffn * C_D + coeff2 * coeffnn * C_M + 2 * noise || C_D == 255)
            shapecdtn = true;
        } else if (ampls.size() > 3) {
          C_Dn = (Float_t)ampls[ampls.size() - 3];
          C_Dnn = (Float_t)ampls[ampls.size() - 4];
          if ((C_Dn <= coeff1 * coeffn * C_D + coeff2 * coeffnn * C_M + 2 * noise || C_D == 255) &&
              C_Dnn <= coeff1 * coeffn * C_Dn + coeff2 * coeffnn * C_D + 2 * noise) {
            shapecdtn = true;
          }
        }
      }

      if (MaxInMiddle == true) {
        C_M = (Float_t)ampls[MaxPos];
        int LeftOfMaxPos = MaxPos - 1;
        if (LeftOfMaxPos <= 0)
          LeftOfMaxPos = 0;
        int RightOfMaxPos = MaxPos + 1;
        if (RightOfMaxPos >= (int)ampls.size())
          RightOfMaxPos = ampls.size() - 1;
        if (ampls[LeftOfMaxPos] < ampls[RightOfMaxPos]) {
          C_D = (Float_t)ampls[RightOfMaxPos];
          C_Mn = (Float_t)ampls[LeftOfMaxPos];
          CDPos = RightOfMaxPos;
        } else {
          C_D = (Float_t)ampls[LeftOfMaxPos];
          C_Mn = (Float_t)ampls[RightOfMaxPos];
          CDPos = LeftOfMaxPos;
        }
        if (C_Mn < coeff1 * coeffn * C_M + coeff2 * coeffnn * C_D + 2 * noise || C_M == 255) {
          if (ampls.size() == 3)
            shapecdtn = true;
          else if (ampls.size() > 3) {
            if (CDPos > MaxPos) {
              if (ampls.size() - CDPos - 1 == 0) {
                C_Dn = 0;
                C_Dnn = 0;
              }
              if (ampls.size() - CDPos - 1 == 1) {
                C_Dn = (Float_t)ampls[CDPos + 1];
                C_Dnn = 0;
              }
              if (ampls.size() - CDPos - 1 > 1) {
                C_Dn = (Float_t)ampls[CDPos + 1];
                C_Dnn = (Float_t)ampls[CDPos + 2];
              }
              if (MaxPos >= 2) {
                C_Mnn = (Float_t)ampls[MaxPos - 2];
              } else if (MaxPos < 2)
                C_Mnn = 0;
            }
            if (CDPos < MaxPos) {
              if (CDPos == 0) {
                C_Dn = 0;
                C_Dnn = 0;
              }
              if (CDPos == 1) {
                C_Dn = (Float_t)ampls[0];
                C_Dnn = 0;
              }
              if (CDPos > 1) {
                C_Dn = (Float_t)ampls[CDPos - 1];
                C_Dnn = (Float_t)ampls[CDPos - 2];
              }
              if (ampls.size() - LeftOfMaxPos > 1 && MaxPos + 2 < (int)(ampls.size()) - 1) {
                C_Mnn = (Float_t)ampls[MaxPos + 2];
              } else
                C_Mnn = 0;
            }
            if ((C_Dn <= coeff1 * coeffn * C_D + coeff2 * coeffnn * C_M + 2 * noise || C_D == 255) &&
                C_Mnn <= coeff1 * coeffn * C_Mn + coeff2 * coeffnn * C_M + 2 * noise &&
                C_Dnn <= coeff1 * coeffn * C_Dn + coeff2 * coeffnn * C_D + 2 * noise) {
              shapecdtn = true;
            }
          }
        }
      }
    }
    if (ampls.size() == 1) {
      shapecdtn = true;
    }

    return shapecdtn;
  }

  int getCharge(const SiStripCluster* cluster,
                int& nSatStrip,
                const GeomDetUnit& detUnit,
                const std::vector<std::vector<float> >& calibGains,
                const unsigned int& offsetDU_) {
    const auto& Ampls = cluster->amplitudes();

    nSatStrip = 0;
    int charge = 0;

    if (calibGains.empty()) {
      for (unsigned int i = 0; i < Ampls.size(); i++) {
        int calibratedCharge = Ampls[i];
        charge += calibratedCharge;
        if (calibratedCharge >= 254)
          nSatStrip++;
      }
    } else {
      for (unsigned int i = 0; i < Ampls.size(); i++) {
        int calibratedCharge = Ampls[i];

        auto& gains = calibGains[detUnit.index() - offsetDU_];
        calibratedCharge = (int)(calibratedCharge / gains[(cluster->firstStrip() + i) / 128]);
        if (calibratedCharge >= 255) {
          if (calibratedCharge >= 1025)
            calibratedCharge = 255;
          else
            calibratedCharge = 254;
        }

        charge += calibratedCharge;
        if (calibratedCharge >= 254)
          nSatStrip++;
      }
    }
    return charge;
  }

  void makeCalibrationMap(const std::string& m_calibrationPath,
                          const TrackerGeometry& tkGeom,
                          std::vector<std::vector<float> >& calibGains,
                          const unsigned int& offsetDU_) {
    auto const& dus = tkGeom.detUnits();
    calibGains.resize(dus.size() - offsetDU_);

    TChain* t1 = new TChain("SiStripCalib/APVGain");
    t1->Add(m_calibrationPath.c_str());

    unsigned int tree_DetId;
    unsigned char tree_APVId;
    double tree_Gain;
    t1->SetBranchAddress("DetId", &tree_DetId);
    t1->SetBranchAddress("APVId", &tree_APVId);
    t1->SetBranchAddress("Gain", &tree_Gain);

    for (unsigned int ientry = 0; ientry < t1->GetEntries(); ientry++) {
      t1->GetEntry(ientry);
      auto& gains = calibGains[tkGeom.idToDetUnit(DetId(tree_DetId))->index() - offsetDU_];
      if (gains.size() < (size_t)(tree_APVId + 1)) {
        gains.resize(tree_APVId + 1);
      }
      gains[tree_APVId] = tree_Gain;
    }
    t1->Delete();
  }

  ESGetTokenH3DDVariant esConsumes(std::string const& Record, edm::ConsumesCollector& iCC) {
    if (Record == "SiStripDeDxMip_3D_Rcd") {
      return iCC.esConsumes<H3DD, SiStripDeDxMip_3D_Rcd, edm::Transition::BeginRun>();
    }
    if (Record == "SiStripDeDxPion_3D_Rcd") {
      return iCC.esConsumes<H3DD, SiStripDeDxPion_3D_Rcd, edm::Transition::BeginRun>();
    }
    if (Record == "SiStripDeDxKaon_3D_Rcd") {
      return iCC.esConsumes<H3DD, SiStripDeDxKaon_3D_Rcd, edm::Transition::BeginRun>();
    }
    if (Record == "SiStripDeDxProton_3D_Rcd") {
      return iCC.esConsumes<H3DD, SiStripDeDxProton_3D_Rcd, edm::Transition::BeginRun>();
    }
    if (Record == "SiStripDeDxElectron_3D_Rcd") {
      return iCC.esConsumes<H3DD, SiStripDeDxElectron_3D_Rcd, edm::Transition::BeginRun>();
    }
    throw cms::Exception("WrongRecord for dEdx") << "The record : " << Record << "is unknown\n";
  }

  PhysicsTools::Calibration::HistogramD3D const& getHistogramD3D(edm::EventSetup const& iES,
                                                                 ESGetTokenH3DDVariant const& iToken) {
    switch (iToken.index()) {
      case 0:
        return iES.getData(std::get<0>(iToken));
      case 1:
        return iES.getData(std::get<1>(iToken));
      case 2:
        return iES.getData(std::get<2>(iToken));
      case 3:
        return iES.getData(std::get<3>(iToken));
      case 4:
        return iES.getData(std::get<4>(iToken));
    }
    throw cms::Exception("HistogramD3DTokenUnset");
  }

  void buildDiscrimMap(PhysicsTools::Calibration::HistogramD3D const& deDxMap,
                       std::string const& ProbabilityMode,
                       TH3F*& Prob_ChargePath) {
    float xmin = deDxMap.rangeX().min;
    float xmax = deDxMap.rangeX().max;
    float ymin = deDxMap.rangeY().min;
    float ymax = deDxMap.rangeY().max;
    float zmin = deDxMap.rangeZ().min;
    float zmax = deDxMap.rangeZ().max;

    if (Prob_ChargePath)
      delete Prob_ChargePath;
    Prob_ChargePath = new TH3F("Prob_ChargePath",
                               "Prob_ChargePath",
                               deDxMap.numberOfBinsX(),
                               xmin,
                               xmax,
                               deDxMap.numberOfBinsY(),
                               ymin,
                               ymax,
                               deDxMap.numberOfBinsZ(),
                               zmin,
                               zmax);

    if (strcmp(ProbabilityMode.c_str(), "Accumulation") == 0) {
      for (int i = 0; i <= Prob_ChargePath->GetXaxis()->GetNbins() + 1; i++) {
        for (int j = 0; j <= Prob_ChargePath->GetYaxis()->GetNbins() + 1; j++) {
          float Ni = 0;
          for (int k = 0; k <= Prob_ChargePath->GetZaxis()->GetNbins() + 1; k++) {
            Ni += deDxMap.binContent(i, j, k);
          }
          for (int k = 0; k <= Prob_ChargePath->GetZaxis()->GetNbins() + 1; k++) {
            float tmp = 0;
            for (int l = 0; l <= k; l++) {
              tmp += deDxMap.binContent(i, j, l);
            }
            if (Ni > 0) {
              Prob_ChargePath->SetBinContent(i, j, k, tmp / Ni);
            } else {
              Prob_ChargePath->SetBinContent(i, j, k, 0);
            }
          }
        }
      }
    } else if (strcmp(ProbabilityMode.c_str(), "Integral") == 0) {
      for (int i = 0; i <= Prob_ChargePath->GetXaxis()->GetNbins() + 1; i++) {
        for (int j = 0; j <= Prob_ChargePath->GetYaxis()->GetNbins() + 1; j++) {
          float Ni = 0;
          for (int k = 0; k <= Prob_ChargePath->GetZaxis()->GetNbins() + 1; k++) {
            Ni += deDxMap.binContent(i, j, k);
          }
          for (int k = 0; k <= Prob_ChargePath->GetZaxis()->GetNbins() + 1; k++) {
            float tmp = deDxMap.binContent(i, j, k);
            if (Ni > 0) {
              Prob_ChargePath->SetBinContent(i, j, k, tmp / Ni);
            } else {
              Prob_ChargePath->SetBinContent(i, j, k, 0);
            }
          }
        }
      }
    } else {
      throw cms::Exception("WrongConfig for dEdx") << "The ProbabilityMode: " << ProbabilityMode << "is unknown\n";
    }
  }

  bool isSpanningOver2APV(unsigned int FirstStrip, unsigned int ClusterSize) {
    if (FirstStrip == 0)
      return true;
    if (FirstStrip == 128)
      return true;
    if (FirstStrip == 256)
      return true;
    if (FirstStrip == 384)
      return true;
    if (FirstStrip == 512)
      return true;
    if (FirstStrip == 640)
      return true;

    if (FirstStrip <= 127 && FirstStrip + ClusterSize > 127)
      return true;
    if (FirstStrip <= 255 && FirstStrip + ClusterSize > 255)
      return true;
    if (FirstStrip <= 383 && FirstStrip + ClusterSize > 383)
      return true;
    if (FirstStrip <= 511 && FirstStrip + ClusterSize > 511)
      return true;
    if (FirstStrip <= 639 && FirstStrip + ClusterSize > 639)
      return true;

    if (FirstStrip + ClusterSize == 127)
      return true;
    if (FirstStrip + ClusterSize == 255)
      return true;
    if (FirstStrip + ClusterSize == 383)
      return true;
    if (FirstStrip + ClusterSize == 511)
      return true;
    if (FirstStrip + ClusterSize == 639)
      return true;
    if (FirstStrip + ClusterSize == 767)
      return true;

    return false;
  }

  bool isFarFromBorder(const TrajectoryStateOnSurface& trajState, const GeomDetUnit* it) {
    if (dynamic_cast<const StripGeomDetUnit*>(it) == nullptr && dynamic_cast<const PixelGeomDetUnit*>(it) == nullptr) {
      edm::LogInfo("deDxTools") << "this detID doesn't seem to belong to the Tracker" << std::endl;
      return false;
    }

    LocalPoint HitLocalPos = trajState.localPosition();
    LocalError HitLocalError = trajState.localError().positionError();

    const BoundPlane plane = it->surface();
    const TrapezoidalPlaneBounds* trapezoidalBounds(dynamic_cast<const TrapezoidalPlaneBounds*>(&(plane.bounds())));
    const RectangularPlaneBounds* rectangularBounds(dynamic_cast<const RectangularPlaneBounds*>(&(plane.bounds())));

    if (!trapezoidalBounds && !rectangularBounds)
      return false;

    double DistFromBorder = 1.0;
    double HalfLength =
        trapezoidalBounds ? (*trapezoidalBounds).parameters()[3] : it->surface().bounds().length() / 2.0;

    if (fabs(HitLocalPos.y()) + HitLocalError.yy() >= (HalfLength - DistFromBorder))
      return false;

    return true;
  }

}  // namespace deDxTools
