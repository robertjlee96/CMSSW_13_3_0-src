#include "RecoParticleFlow/PFProducer/interface/BlockElementLinkerBase.h"
#include "DataFormats/ParticleFlowReco/interface/PFCluster.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementCluster.h"
#include "RecoParticleFlow/PFClusterTools/interface/LinkByRecHit.h"

class PreshowerAndECALLinker : public BlockElementLinkerBase {
public:
  PreshowerAndECALLinker(const edm::ParameterSet& conf)
      : BlockElementLinkerBase(conf),
        useKDTree_(conf.getParameter<bool>("useKDTree")),
        debug_(conf.getUntrackedParameter<bool>("debug", false)) {}

  bool linkPrefilter(const reco::PFBlockElement*, const reco::PFBlockElement*) const override;

  double testLink(const reco::PFBlockElement*, const reco::PFBlockElement*) const override;

private:
  bool useKDTree_, debug_;
};

DEFINE_EDM_PLUGIN(BlockElementLinkerFactory, PreshowerAndECALLinker, "PreshowerAndECALLinker");

bool PreshowerAndECALLinker::linkPrefilter(const reco::PFBlockElement* elem1, const reco::PFBlockElement* elem2) const {
  bool result = false;
  // PS-ECAL KDTree multilinks are stored to PS's elem
  switch (elem1->type()) {
    case reco::PFBlockElement::PS1:
    case reco::PFBlockElement::PS2:
      result = (elem1->isMultilinksValide(elem2->type()) && !elem1->getMultilinks(elem2->type()).empty() &&
                elem2->isMultilinksValide(elem1->type()));
      break;
    case reco::PFBlockElement::ECAL:
      result = (elem2->isMultilinksValide(elem1->type()) && !elem2->getMultilinks(elem1->type()).empty() &&
                elem1->isMultilinksValide(elem2->type()));
      break;
    default:
      break;
  }
  return (useKDTree_ ? result : true);
}

double PreshowerAndECALLinker::testLink(const reco::PFBlockElement* elem1, const reco::PFBlockElement* elem2) const {
  const reco::PFBlockElementCluster *pselem(nullptr), *ecalelem(nullptr);
  double dist(-1.0);
  if (elem1->type() < elem2->type()) {
    pselem = static_cast<const reco::PFBlockElementCluster*>(elem1);
    ecalelem = static_cast<const reco::PFBlockElementCluster*>(elem2);
  } else {
    pselem = static_cast<const reco::PFBlockElementCluster*>(elem2);
    ecalelem = static_cast<const reco::PFBlockElementCluster*>(elem1);
  }
  const reco::PFClusterRef& psref = pselem->clusterRef();
  const reco::PFClusterRef& ecalref = ecalelem->clusterRef();
  if (psref.isNull() || ecalref.isNull()) {
    throw cms::Exception("BadClusterRefs") << "PFBlockElementCluster's refs are null!";
  }
  // Check if the linking has been done using the KDTree algo
  // Glowinski & Gouzevitch
  if (useKDTree_ && pselem->isMultilinksValide(ecalelem->type())) {  // KDTree algo
    const reco::PFMultilinksType& multilinks = pselem->getMultilinks(ecalelem->type());
    const math::XYZPoint& ecalxyzpos = ecalref->position();
    const math::XYZPoint& psxyzpos = psref->position();

    // Check if the link PS/Ecal exist
    reco::PFMultilinksType::const_iterator mlit = multilinks.begin();
    for (; mlit != multilinks.end(); ++mlit)
      if (mlit->clusterRef == ecalref)
        break;

    // If the link exist, we fill dist and linktest.
    if (mlit != multilinks.end()) {
      dist = LinkByRecHit::computeDist(
          ecalxyzpos.X() / 1000., ecalxyzpos.Y() / 1000., psxyzpos.X() / 1000., psxyzpos.Y() / 1000., false);
    }
  } else {  //Old algorithm
    dist = LinkByRecHit::testECALAndPSByRecHit(*ecalref, *psref, debug_);
  }
  return dist;
}
