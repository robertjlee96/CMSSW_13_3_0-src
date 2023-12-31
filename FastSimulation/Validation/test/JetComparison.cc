#include "FastSimulation/Validation/test/JetComparison.h"

//#define EDM_ML_DEBUG

JetComparison::JetComparison(edm::ParameterSet const& conf)
    : jetToken_(consumes<reco::CaloJetCollection>(edm::InputTag("iterativeCone5CaloJets"))),
      genjetToken_(consumes<reco::GenJetCollection>(edm::InputTag("ak4GenJets"))),
      fMinEnergy(conf.getParameter<double>("MinEnergy")) {
  usesResource(TFileService::kSharedResource);

  nEvent = 0;

  edm::Service<TFileService> tf;
  meNumberJet = tf->make<TH1F>("number_of_jets", "number_of_jets", 500, 0., 500.);
  meEtJet = tf->make<TH1F>("jet_et", "jet_et", 1000, 0., 1000.);
  meEtGen = tf->make<TH1F>("gen_et", "gen_et", 1000, 0., 1000.);
  meEtJetMatched = tf->make<TH1F>("jet_et_two_matched", "jet_et_two_matched", 1000, 0., 1000.);
  meEtGenMatched = tf->make<TH1F>("gen_et_two_matched", "gen_et_two_matched", 1000, 0., 1000.);
  meEtaJet = tf->make<TH1F>("jet_eta", "jet_eta", 500, 0., 5.);
  meEtaGen = tf->make<TH1F>("gen_eta", "gen_eta", 500, 0., 5.);
  meRatio = tf->make<TH2F>("ratio_jet_gen_vs_eta", "ratio_jet_gen_vs_eta", 50, 0., 5., 20000, 0., 5.);
  meHadronicFrac_vs_eta =
      tf->make<TH2F>("hadronic_fraction_vs_eta", "hadronic_fraction_vs_eta", 50, 0., 5., 1000, 0., 1.);
  meNTowers60_vs_eta = tf->make<TH2F>("number_of_towers_with_60_percents_energy_vs_eta",
                                      "number_of_towers_with_60_percents_energy_vs_eta",
                                      50,
                                      0.,
                                      5.,
                                      100,
                                      -0.5,
                                      99.5);
  meNTowers90_vs_eta = tf->make<TH2F>("number_of_towers_with_90_percents_energy_vs_eta",
                                      "number_of_towers_with_90_percents_energy_vs_eta",
                                      50,
                                      0.,
                                      5.,
                                      100,
                                      -0.5,
                                      99.5);
  meDistR = tf->make<TH1F>("distance_R_between_matched_jets", "distance_R_between_matched_jets", 300, 0., 0.3);
  meDistR_vs_eta = tf->make<TH2F>(
      "distance_R_between_matched_jets_vs_eta", "stance_R_between_matched_jets_vs_eta", 50, 0., 5., 300, 0., 0.3);
  meNTowers_vs_eta =
      tf->make<TH2F>("number_of_calotowers_vs_eta", "number_of_calotowers_vs_eta", 50, 0., 5., 51, -0.5, 50.5);
}

void JetComparison::endJob() {
  meNumberJet->Scale(1. / nEvent);
  Float_t x[50];
  Float_t y[50];
  Float_t y_er[50];
  Float_t x_er[50];
  Float_t mean;
  Float_t sigm;

  for (Int_t i = 1; i < 51; i++) {
    mean = (meRatio->ProjectionY("", i, i, "")->GetMean());
    sigm = (meRatio->ProjectionY("", i, i, "")->GetRMS());
    edm::LogVerbatim("JetComparison") << " Mean " << mean;
    x[i - 1] = (i - 0.5) * 0.1;
    y[i - 1] = mean;
    y_er[i - 1] = sigm;
    x_er[i - 1] = 0.;
  }

  edm::Service<TFileService> tf;
  gr = tf->make<TGraphErrors>(50, x, y, x_er, y_er);
  gr->SetMarkerStyle(21);
  gr->SetMarkerColor(44);
}

void JetComparison::beginRun(edm::Run const& run, edm::EventSetup const& iSetup) {}

void JetComparison::analyze(edm::Event const& event, edm::EventSetup const& c) {
  nEvent++;

  const edm::Handle<reco::CaloJetCollection>& jets = event.getHandle(jetToken_);
  meNumberJet->Fill(jets->size());

  for (unsigned int ijet = 0; ijet < jets->size(); ijet++) {
#ifdef EDM_ML_DEBUG
    edm::LogVerbatim("JetComparison") << " JETS energy = " << (*jets)[ijet].et();
#endif
    double etJet = (*jets)[ijet].et();
    double etaJet = (*jets)[ijet].eta();

    meEtJet->Fill(etJet);
    meEtaJet->Fill(etaJet);
  }

  const edm::Handle<reco::GenJetCollection>& jetsgen = event.getHandle(genjetToken_);
  for (unsigned int igen = 0; igen < jetsgen->size(); igen++) {
#ifdef EDM_ML_DEBUG
    edm::LogVerbatim("JetComparison") << " GENS energy = " << (*jetsgen)[igen].et();
#endif
    double etGen = (*jetsgen)[igen].et();
    double etaGen = (*jetsgen)[igen].eta();
    meEtGen->Fill(etGen);
    meEtaGen->Fill(etaGen);
  }

  for (unsigned int ijet = 0; ijet < jets->size(); ijet++) {
    double etaJet = (*jets)[ijet].eta();
    double phiJet = (*jets)[ijet].phi();
    double etJet = (*jets)[ijet].et();
    double fracHadr = (*jets)[ijet].energyFractionHadronic();
    int n60 = (*jets)[ijet].n60();
    int n90 = (*jets)[ijet].n90();
    int nCaloTower = (*jets)[ijet].nConstituents();
    double etaGen;
    double phiGen;
    double etGen;

    unsigned int isize = jetsgen->size();
    if (isize > 2)
      isize = 2;
    for (unsigned int igen = 0; igen < isize; igen++) {
      etaGen = (*jetsgen)[igen].eta();
      phiGen = (*jetsgen)[igen].phi();
      etGen = (*jetsgen)[igen].et();

      double rDist = std::sqrt(deltaR2(etaJet, phiJet, etaGen, phiGen));

      if (rDist < 0.3 && etGen > fMinEnergy) {
        meNTowers_vs_eta->Fill(fabs(etaGen), nCaloTower);
        meDistR->Fill(rDist);
        meDistR_vs_eta->Fill(fabs(etaGen), rDist);
        edm::LogVerbatim("JetComparison")
            << "  JETS energy = " << (*jets)[ijet].et() << " ETA = " << etaJet << " PHI = " << phiJet;
        edm::LogVerbatim("JetComparison")
            << "  GENS energy = " << (*jetsgen)[igen].et() << " ETA = " << etaGen << " PHI = " << phiGen;
        edm::LogVerbatim("JetComparison") << "Jets matched ";
        meRatio->Fill(fabs(etaGen), etJet / etGen);
        meEtJetMatched->Fill(etJet);
        meEtGenMatched->Fill(etGen);
        meNTowers60_vs_eta->Fill(fabs(etaJet), n60);
        meNTowers90_vs_eta->Fill(fabs(etaJet), n90);
        meHadronicFrac_vs_eta->Fill(fabs(etaJet), fracHadr);
      }
    }
  }
}

double JetComparison::deltaR2(double eta0, double phi0, double eta, double phi) {
  double dphi = phi - phi0;
  if (dphi > M_PI)
    dphi -= 2 * M_PI;
  else if (dphi <= -M_PI)
    dphi += 2 * M_PI;
  return dphi * dphi + (eta - eta0) * (eta - eta0);
}

#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(JetComparison);
