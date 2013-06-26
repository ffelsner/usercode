#ifndef HeavyFlavorAnalysis_Onia2MuMu_Onia2MuMuPAT_h
#define HeavyFlavorAnalysis_Onia2MuMu_Onia2MuMuPAT_h


// system include files
#include <memory>

// FW include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "CommonTools/Utils/interface/PtComparator.h"

// DataFormat includes
#include <DataFormats/PatCandidates/interface/CompositeCandidate.h>
#include <DataFormats/PatCandidates/interface/Muon.h>

#include <CommonTools/UtilAlgos/interface/StringCutObjectSelector.h>
#include "RecoVertex/VertexTools/interface/InvariantMassFromVertex.h"
#include "HeavyFlavorAnalysis/Onia2MuMu/interface/VertexReProducer.h"
#include "RecoVertex/KinematicFitPrimitives/interface/ParticleMass.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "RecoVertex/PrimaryVertexProducer/interface/DAClusterizerInZ.h"
#include "RecoVertex/PrimaryVertexProducer/interface/TrackFilterForPVFinding.h"
#include "RecoVertex/PrimaryVertexProducer/interface/HITrackFilterForPVFinding.h"
#include "TrackingTools/GeomPropagators/interface/AnalyticalImpactPointExtrapolator.h"
#include "RecoVertex/VertexPrimitives/interface/ConvertToFromReco.h"

// Kinematic vertex fit
#include <RecoVertex/KinematicFitPrimitives/interface/KinematicParticleFactoryFromTransientTrack.h>
#include "RecoVertex/KinematicFit/interface/KinematicConstrainedVertexFitter.h"
#include "RecoVertex/KinematicFit/interface/KinematicParticleVertexFitter.h"

template<typename T>
struct GreaterByVProb {
  typedef T first_argument_type;
  typedef T second_argument_type;
  bool operator()( const T & t1, const T & t2 ) const {
    return t1.userFloat("vProb") > t2.userFloat("vProb");
  }
};


//
// class decleration
//

class Onia2MuMuPAT : public edm::EDProducer {
 public:
  explicit Onia2MuMuPAT(const edm::ParameterSet&);
  ~Onia2MuMuPAT();

 private:
  virtual void beginJob() ;
  virtual void produce(edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;
  bool isAbHadron(int pdgID);
  bool isAMixedbHadron(int pdgID, int momPdgID);
  bool searchForTheThirdTrack(const edm::Event&, const edm::EventSetup&, pat::CompositeCandidate&, const pat::Muon*, const pat::Muon*,reco::Vertex&,reco::Track&, bool&);
  std::pair<int, float> findJpsiMCInfo(reco::GenParticleRef genJpsi);
  math::XYZTLorentzVector fromPtEtaPhiToPxPyPz( const double & pt, const double & eta, const double & phi, const double & mass) const;
  void fillCandMuons(pat::CompositeCandidate & myCand, const pat::Muon & mu1, const pat::Muon & mu2);
//  void buildMuonLessCollection(const edm::Event& iEvent, const pat::Muon &mu1, const pat::Muon &mu2, reco::TrackCollection &muonLess);
  void buildMuonlessPV(const edm::Event&, const edm::EventSetup&,const pat::Muon *,const pat::Muon *, const reco::Track &, reco::Vertex & );

  std::pair<reco::Vertex, reco::Vertex> buildMuonlessPV(const reco::Muon * rmu1, const reco::Muon * rmu2,
                                                        reco::TrackCollection & muonLess, const reco::Vertex &thePrimaryV,
                                                        const edm::ESHandle<TransientTrackBuilder> &theTTBuilder,
                                                        const reco::BeamSpot & bs);

  // ----------member data ---------------------------
 private:
  edm::InputTag muons_;
  edm::InputTag thebeamspot_;
  edm::InputTag thePVs_;
  StringCutObjectSelector<pat::Muon> higherPuritySelection_;
  StringCutObjectSelector<pat::Muon> lowerPuritySelection_;
  StringCutObjectSelector<reco::Candidate::LorentzVector, true> dimuonSelection_;
  bool addCommonVertex_, addMuonlessPrimaryVertex_;
  bool resolveAmbiguity_;
  bool addMCTruth_;
  bool addThirdTrack_;
  double minTrackPt_;
  double trackMass_;
  double diMuPlusTrackMassMax_, diMuPlusTrackMassMin_;
  double diMuMassMax_, diMuMassMin_;
  GreaterByPt<pat::CompositeCandidate> pTComparator_;
  GreaterByVProb<pat::CompositeCandidate> vPComparator_;
  StringCutObjectSelector<reco::Candidate, true> prefilter_;

  // int findVertexId( const TransientVertex & theOriginalPV, const std::vector<TransientVertex> & priVtxs,
  int findVertexId(const std::pair<reco::Vertex, reco::Vertex> &theOriginalPV,
                   const std::vector<std::pair<reco::Vertex, reco::Vertex> > &priVtxs,
                   const reco::Track & track, const double & maxDeltaR );
  float computeDcaXY(const TrajectoryStateClosestToPoint & tt1, const TrajectoryStateClosestToPoint & tt2);
  float computeDca(const TrajectoryStateClosestToPoint & tt1, const TrajectoryStateClosestToPoint & tt2);
  void setP4AndCharge(RefCountedKinematicTree & vertexFitTree, pat::Muon & mu,
                      const edm::ESHandle<TransientTrackBuilder> &theTTBuilder) const;

  InvariantMassFromVertex massCalculator;
  ParticleMass muon_mass;
  float muon_sigma;
  AdaptiveVertexFitter avtxFitter_;

  std::vector<double> muMasses_;
  reco::TrackCollection muonLess_;

  struct SimpleTrack
  {
    SimpleTrack(const double & inP, const double & inPt, const double & inEta, const double & inPhi,
                const int inNdof, const double & inDoca,
                const double & inDocaSignificance,
                const int inVertexId, const bool inHighPurity,
                const double & inDeltaRMu1, const double & inDeltaRMu2, const double inDeltaRCand) :
      p(inP), pt(inPt), eta(inEta), phi(inPhi),
      ndof(inNdof), doca(inDoca),
      docaSignificance(inDocaSignificance),
      vertexId(inVertexId), highPurity(inHighPurity),
      deltaRMu1(inDeltaRMu1), deltaRMu2(inDeltaRMu2), deltaRCand(inDeltaRCand)
    {}

    bool operator<(const SimpleTrack & compTrack) const {
      return( doca < compTrack.doca );
    }

    double p;
    double pt;
    double eta;
    double phi;
    int ndof;
    double doca;
    double docaSignificance;
    int vertexId;
    bool highPurity;
    double deltaRMu1;
    double deltaRMu2;
    double deltaRCand;
  };
};

//
// constants, enums and typedefs
//


//
// static data member definitions
//

//
// constructors and destructor
//

#endif
