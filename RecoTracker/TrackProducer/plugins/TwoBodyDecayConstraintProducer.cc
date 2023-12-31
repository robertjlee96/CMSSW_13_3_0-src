/**
 *  \class TwoBodyDecayConstraintProducer TwoBodyDecayConstraintProducer.cc RecoTracker/ConstraintProducerTest/src/TwoBodyDecayConstraintProducer.cc
 *  
 *  Description: Produces track parameter constraints for refitting tracks, according to information TwoBodyDecay kinematic fit.
 *
 *  Original Author:  Edmund Widl
 * 
 */

#include <memory>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "Geometry/Records/interface/GlobalTrackingGeometryRecord.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include "TrackingTools/PatternTools/interface/TrackConstraintAssociation.h"

#include "Alignment/TwoBodyDecay/interface/TwoBodyDecay.h"
#include "Alignment/TwoBodyDecay/interface/TwoBodyDecayFitter.h"
#include "Alignment/TwoBodyDecay/interface/TwoBodyDecayVirtualMeasurement.h"
#include "Alignment/ReferenceTrajectories/interface/TwoBodyDecayTrajectoryState.h"

// // debug
// #include <map>
// #include "TH1F.h"
// #include "TFile.h"
// #include "TLorentzVector.h"
// #include "Alignment/TwoBodyDecay/interface/TwoBodyDecayModel.h"

class TwoBodyDecayConstraintProducer : public edm::global::EDProducer<> {
public:
  explicit TwoBodyDecayConstraintProducer(const edm::ParameterSet&);
  ~TwoBodyDecayConstraintProducer() override = default;

private:
  void produce(edm::StreamID streamid, edm::Event&, const edm::EventSetup&) const override;

  std::pair<bool, TrajectoryStateOnSurface> innermostState(const reco::TransientTrack& ttrack) const;
  bool match(const TrajectoryStateOnSurface& newTsos, const TrajectoryStateOnSurface& oldTsos) const;

  const edm::InputTag srcTag_;
  const edm::InputTag bsSrcTag_;

  const TwoBodyDecayFitter tbdFitter_;

  const double primaryMass_;
  const double primaryWidth_;
  const double secondaryMass_;

  const double sigmaPositionCutValue_;
  const double chi2CutValue_;
  const double errorRescaleValue_;

  edm::EDGetTokenT<reco::TrackCollection> trackCollToken_;
  edm::EDGetTokenT<reco::BeamSpot> bsToken_;

  const edm::ESGetToken<MagneticField, IdealMagneticFieldRecord> magFieldToken_;
  const edm::ESGetToken<GlobalTrackingGeometry, GlobalTrackingGeometryRecord> trackingGeometryToken_;
  //   // debug
  //   std::map<std::string, TH1F*> histos_;
};

TwoBodyDecayConstraintProducer::TwoBodyDecayConstraintProducer(const edm::ParameterSet& iConfig)
    : srcTag_(iConfig.getParameter<edm::InputTag>("src")),
      bsSrcTag_(iConfig.getParameter<edm::InputTag>("beamSpot")),
      tbdFitter_(iConfig),
      primaryMass_(iConfig.getParameter<double>("primaryMass")),
      primaryWidth_(iConfig.getParameter<double>("primaryWidth")),
      secondaryMass_(iConfig.getParameter<double>("secondaryMass")),
      sigmaPositionCutValue_(iConfig.getParameter<double>("sigmaPositionCut")),
      chi2CutValue_(iConfig.getParameter<double>("chi2Cut")),
      errorRescaleValue_(iConfig.getParameter<double>("rescaleError")),
      magFieldToken_(esConsumes()),
      trackingGeometryToken_(esConsumes()) {
  trackCollToken_ = consumes<reco::TrackCollection>(edm::InputTag(srcTag_));
  bsToken_ = consumes<reco::BeamSpot>(edm::InputTag(bsSrcTag_));

  produces<std::vector<TrackParamConstraint> >();
  produces<TrackParamConstraintAssociationCollection>();

  //   //debug
  //   histos_["deltaEta1"] = new TH1F( "deltaEta1", "deltaEta1", 200, -1., 1. );
  //   histos_["deltaP1"] = new TH1F( "deltaP1", "deltaP1", 200, -50., 50. );

  //   histos_["deltaEta2"] = new TH1F( "deltaEta2", "deltaEta2", 200, -1., 1. );
  //   histos_["deltaP2"] = new TH1F( "deltaP2", "deltaP2", 200, -50., 50. );
}

void TwoBodyDecayConstraintProducer::produce(edm::StreamID streamid,
                                             edm::Event& iEvent,
                                             const edm::EventSetup& iSetup) const {
  using namespace edm;

  Handle<reco::TrackCollection> trackColl;
  iEvent.getByToken(trackCollToken_, trackColl);

  Handle<reco::BeamSpot> beamSpot;
  iEvent.getByToken(bsToken_, beamSpot);

  auto const* magField = &iSetup.getData(magFieldToken_);

  auto trackingGeometry = iSetup.getHandle(trackingGeometryToken_);

  edm::RefProd<std::vector<TrackParamConstraint> > rPairs =
      iEvent.getRefBeforePut<std::vector<TrackParamConstraint> >();
  std::unique_ptr<std::vector<TrackParamConstraint> > pairs(new std::vector<TrackParamConstraint>);
  std::unique_ptr<TrackParamConstraintAssociationCollection> output(
      new TrackParamConstraintAssociationCollection(trackColl, rPairs));

  if (trackColl->size() == 2) {
    /// Construct virtual measurement (for TBD)
    TwoBodyDecayVirtualMeasurement vm(primaryMass_, primaryWidth_, secondaryMass_, *beamSpot.product());

    /// Get transient tracks from track collection
    std::vector<reco::TransientTrack> ttracks(2);
    ttracks[0] = reco::TransientTrack(reco::TrackRef(trackColl, 0), magField);
    ttracks[0].setTrackingGeometry(trackingGeometry);
    ttracks[1] = reco::TransientTrack(reco::TrackRef(trackColl, 1), magField);
    ttracks[1].setTrackingGeometry(trackingGeometry);

    /// Fit the TBD
    TwoBodyDecay tbd = tbdFitter_.estimate(ttracks, vm);

    if (!tbd.isValid() or (tbd.chi2() > chi2CutValue_))
      return;

    /// Get the innermost trajectory states
    std::pair<bool, TrajectoryStateOnSurface> oldInnermostState1 = innermostState(ttracks[0]);
    std::pair<bool, TrajectoryStateOnSurface> oldInnermostState2 = innermostState(ttracks[1]);
    if (!oldInnermostState1.second.isValid() || !oldInnermostState2.second.isValid())
      return;

    /// Construct the TBD trajectory states
    TwoBodyDecayTrajectoryState::TsosContainer trackTsos(oldInnermostState1.second, oldInnermostState2.second);
    TwoBodyDecayTrajectoryState tbdTrajState(trackTsos, tbd, secondaryMass_, magField, true);
    if (!tbdTrajState.isValid())
      return;

    /// Match the old and the new estimates for the trajectory state
    bool match1 = match(tbdTrajState.trajectoryStates(true).first, oldInnermostState1.second);
    bool match2 = match(tbdTrajState.trajectoryStates(true).second, oldInnermostState2.second);
    if (!match1 || !match2)
      return;

    // re-scale error of constraintTsos
    tbdTrajState.rescaleError(errorRescaleValue_);

    // produce constraint for first track
    pairs->push_back(tbdTrajState.trajectoryStates(true).first);
    output->insert(reco::TrackRef(trackColl, 0), edm::Ref<std::vector<TrackParamConstraint> >(rPairs, 0));

    // produce constraint for second track
    pairs->push_back(tbdTrajState.trajectoryStates(true).second);
    output->insert(reco::TrackRef(trackColl, 1), edm::Ref<std::vector<TrackParamConstraint> >(rPairs, 1));

    //     // debug
    //     if ( tbd.isValid() ) {
    //       TwoBodyDecayModel model;
    //       const std::pair< AlgebraicVector, AlgebraicVector > fitMomenta = model.cartesianSecondaryMomenta( tbd );

    //       TLorentzVector recoMomentum1( ttracks[0].track().px(), ttracks[0].track().py(), ttracks[0].track().pz(),
    // 				    sqrt((ttracks[0].track().p()*ttracks[0].track().p())+0.105658*0.105658) );
    //       TLorentzVector fitMomentum1( fitMomenta.first[0], fitMomenta.first[1], fitMomenta.first[2],
    // 				   sqrt( fitMomenta.first.normsq()+0.105658*0.105658) );
    //       histos_["deltaP1"]->Fill( recoMomentum1.P() - fitMomentum1.P() );
    //       histos_["deltaEta1"]->Fill( recoMomentum1.Eta() - fitMomentum1.Eta() );

    //       TLorentzVector recoMomentum2( ttracks[1].track().px(), ttracks[1].track().py(), ttracks[1].track().pz(),
    // 				    sqrt((ttracks[1].track().p()*ttracks[1].track().p())+0.105658*0.105658) );
    //       TLorentzVector fitMomentum2( fitMomenta.second[0], fitMomenta.second[1], fitMomenta.second[2],
    // 				   sqrt( fitMomenta.second.normsq()+0.105658*0.105658) );
    //       histos_["deltaP2"]->Fill( recoMomentum2.P() - fitMomentum2.P() );
    //       histos_["deltaEta2"]->Fill( recoMomentum2.Eta() - fitMomentum2.Eta() );
    //     }
  }

  iEvent.put(std::move(pairs));
  iEvent.put(std::move(output));
}

std::pair<bool, TrajectoryStateOnSurface> TwoBodyDecayConstraintProducer::innermostState(
    const reco::TransientTrack& ttrack) const {
  double outerR = ttrack.outermostMeasurementState().globalPosition().perp();
  double innerR = ttrack.innermostMeasurementState().globalPosition().perp();
  bool insideOut = (outerR > innerR);
  return std::make_pair(insideOut, insideOut ? ttrack.innermostMeasurementState() : ttrack.outermostMeasurementState());
}

bool TwoBodyDecayConstraintProducer::match(const TrajectoryStateOnSurface& newTsos,
                                           const TrajectoryStateOnSurface& oldTsos) const {
  LocalPoint lp1 = newTsos.localPosition();
  LocalPoint lp2 = oldTsos.localPosition();

  double deltaX = lp1.x() - lp2.x();
  double deltaY = lp1.y() - lp2.y();

  return ((fabs(deltaX) < sigmaPositionCutValue_) && (fabs(deltaY) < sigmaPositionCutValue_));
}

//define this as a plug-in
DEFINE_FWK_MODULE(TwoBodyDecayConstraintProducer);
