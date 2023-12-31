// -*- C++ -*-
//
// Package:     Calo
// Class  :     FWMETProxyBuilder
//
// Implementation:
//     <Notes on implementation>
//
// Original Author:
//         Created:  Sun Jan  6 23:57:00 EST 2008
//

// system include files
#include "TEveGeoNode.h"
#include "TEveGeoShape.h"
#include "TEveScalableStraightLineSet.h"
#include "TGeoTube.h"
#include "TMath.h"

// user include files
#include "Fireworks/Core/interface/FWSimpleProxyBuilderTemplate.h"
#include "Fireworks/Core/interface/FWViewType.h"
#include "Fireworks/Core/interface/BuilderUtils.h"
#include "Fireworks/Core/interface/Context.h"
#include "Fireworks/Core/interface/FWViewEnergyScale.h"
#include "Fireworks/Core/interface/FWEventItem.h"
#include "Fireworks/Calo/interface/scaleMarker.h"

#include "DataFormats/METReco/interface/MET.h"

////////////////////////////////////////////////////////////////////////////////
//
//   RPZ proxy builder with shared MET shape
//
////////////////////////////////////////////////////////////////////////////////

class FWMETProxyBuilder : public FWSimpleProxyBuilderTemplate<reco::MET> {
public:
  FWMETProxyBuilder() {}
  ~FWMETProxyBuilder() override {}

  bool haveSingleProduct() const override { return false; }  // use buildViewType instead of buildView

  bool havePerViewProduct(FWViewType::EType) const override { return true; }  // used energy scaling

  void scaleProduct(TEveElementList* parent, FWViewType::EType, const FWViewContext* vc) override;

  REGISTER_PROXYBUILDER_METHODS();

  FWMETProxyBuilder(const FWMETProxyBuilder&) = delete;                   // stop default
  const FWMETProxyBuilder& operator=(const FWMETProxyBuilder&) = delete;  // stop default

private:
  using FWSimpleProxyBuilderTemplate<reco::MET>::buildViewType;
  void buildViewType(const reco::MET& iData,
                     unsigned int iIndex,
                     TEveElement& oItemHolder,
                     FWViewType::EType type,
                     const FWViewContext*) override;
};

void FWMETProxyBuilder::scaleProduct(TEveElementList* product, FWViewType::EType type, const FWViewContext* vc) {
  int idx = 0;
  for (auto& c : product->RefChildren()) {
    // line set element is added at the end of the buildViewType function, therefore last child
    TEveScalableStraightLineSet* lineSet = dynamic_cast<TEveScalableStraightLineSet*>(c->LastChild());
    if (lineSet) {
      // compund index in the product is an index of model data in the collection
      const void* modelData = item()->modelData(idx);
      const reco::MET* met = (const reco::MET*)(modelData);
      float value = vc->getEnergyScale()->getPlotEt() ? met->et() : met->energy();
      lineSet->SetScale(vc->getEnergyScale()->getScaleFactor3D() * value);
      for (TEveProjectable::ProjList_i j = lineSet->BeginProjecteds(); j != lineSet->EndProjecteds(); ++j) {
        (*j)->UpdateProjection();
      }
    }
    idx++;
  }
}

void FWMETProxyBuilder::buildViewType(const reco::MET& met,
                                      unsigned int iIndex,
                                      TEveElement& oItemHolder,
                                      FWViewType::EType type,
                                      const FWViewContext* vc) {
  using namespace TMath;
  double phi = met.phi();
  double theta = met.theta();
  double size = 1.f;

  FWViewEnergyScale* caloScale = vc->getEnergyScale();

  TEveScalableStraightLineSet* marker = new TEveScalableStraightLineSet("MET marker");
  marker->SetLineWidth(2);

  if (type == FWViewType::kRhoZ) {
    // body
    double r0;
    if (TMath::Abs(met.eta()) < context().caloTransEta()) {
      r0 = context().caloR1() / sin(theta);
    } else {
      r0 = context().caloZ1() / fabs(cos(theta));
    }
    marker->SetScaleCenter(0., Sign(r0 * sin(theta), phi), r0 * cos(theta));
    double r1 = r0 + 1;
    marker->AddLine(0., Sign(r0 * sin(theta), phi), r0 * cos(theta), 0., Sign(r1 * sin(theta), phi), r1 * cos(theta));

    // arrow pointer
    double r2 = r1 - 0.1;
    double dy = 0.05 * size;
    marker->AddLine(0.,
                    Sign(r2 * sin(theta) + dy * cos(theta), phi),
                    r2 * cos(theta) - dy * sin(theta),
                    0.,
                    Sign(r1 * sin(theta), phi),
                    r1 * cos(theta));
    dy = -dy;
    marker->AddLine(0.,
                    Sign(r2 * sin(theta) + dy * cos(theta), phi),
                    r2 * cos(theta) - dy * sin(theta),
                    0.,
                    Sign(r1 * sin(theta), phi),
                    r1 * cos(theta));

    // segment
    fireworks::addRhoZEnergyProjection(
        this, &oItemHolder, context().caloR1() - 1, context().caloZ1() - 1, theta - 0.04, theta + 0.04, phi);
  } else {
    // body
    double r0 = context().caloR1();
    double r1 = r0 + 1;
    marker->SetScaleCenter(r0 * cos(phi), r0 * sin(phi), 0);
    marker->AddLine(r0 * cos(phi), r0 * sin(phi), 0, r1 * cos(phi), r1 * sin(phi), 0);

    // arrow pointer, xy  rotate offset point ..
    double r2 = r1 - 0.1;
    double dy = 0.05 * size;

    marker->AddLine(r2 * cos(phi) - dy * sin(phi), r2 * sin(phi) + dy * cos(phi), 0, r1 * cos(phi), r1 * sin(phi), 0);
    dy = -dy;
    marker->AddLine(r2 * cos(phi) - dy * sin(phi), r2 * sin(phi) + dy * cos(phi), 0, r1 * cos(phi), r1 * sin(phi), 0);

    // segment
    double min_phi = phi - M_PI / 36 / 2;
    double max_phi = phi + M_PI / 36 / 2;
    TEveGeoManagerHolder gmgr(TEveGeoShape::GetGeoMangeur());
    TEveGeoShape* element =
        fireworks::getShape("spread", new TGeoTubeSeg(r0 - 2, r0, 1, min_phi * 180 / M_PI, max_phi * 180 / M_PI), 0);
    element->SetPickable(kTRUE);
    setupAddElement(element, &oItemHolder);
  }

  marker->SetScale(caloScale->getScaleFactor3D() * (caloScale->getPlotEt() ? met.et() : met.energy()));
  setupAddElement(marker, &oItemHolder);

  // printf("add line %s  %f %f .... eta %f theta %f\n", item()->name().c_str(), met.et(), met.energy(), met.eta(), met.theta());

  context().voteMaxEtAndEnergy(met.et(), met.energy());
}

REGISTER_FWPROXYBUILDER(FWMETProxyBuilder, reco::MET, "recoMET", FWViewType::kAllRPZBits);
