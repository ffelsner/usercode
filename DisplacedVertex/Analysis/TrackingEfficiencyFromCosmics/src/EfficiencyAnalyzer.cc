// -*- C++ -*-
//
// Package:    EfficiencyAnalyzer
// Class:      EfficiencyAnalyzer
//
/**\class EfficiencyAnalyzer EfficiencyAnalyzer.cc Analysis/EfficiencyAnalyzer/src/EfficiencyAnalyzer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Marco De Mattia,42 R-23,
//         Created:  Mon Jul 4 18:38:0 CEST 2011
// $Id: EfficiencyAnalyzer.cc,v 1.1 2011/07/04 17:01:11 demattia Exp $
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "Analysis/TrackingEfficiencyFromCosmics/interface/EfficiencyTree.h"

#include <boost/foreach.hpp>

//
// class declaration
//

class EfficiencyAnalyzer : public edm::EDAnalyzer {
public:
  explicit EfficiencyAnalyzer(const edm::ParameterSet&);
  ~EfficiencyAnalyzer();

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  virtual void beginJob();
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob();

  virtual void beginRun(edm::Run const&, edm::EventSetup const&);
  virtual void endRun(edm::Run const&, edm::EventSetup const&);
  virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
  virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);

  void fillHistogram(const TString & name, const TString & title, const boost::shared_ptr<Efficiency> & eff );

  // ----------member data ---------------------------
  std::auto_ptr<Efficiency> efficiency_;
  std::string inputFileName_;
  edm::Service<TFileService> fileService_;
};

void EfficiencyAnalyzer::fillHistogram(const TString & name, const TString & title, const boost::shared_ptr<Efficiency> & eff )
{
  TH1F * hEff = fileService_->make<TH1F>(name, title, eff->bins(0), eff->min(0), eff->min(0));
  for( unsigned int i=0; i<eff->getLinearSize(); ++i ) {
    hEff->SetBinContent(i+1, eff->getEff(i));
  }
}

EfficiencyAnalyzer::EfficiencyAnalyzer(const edm::ParameterSet& iConfig) :
  inputFileName_(iConfig.getParameter<std::string>("InputFileName"))
{
  efficiency_.reset(new Efficiency);
  EfficiencyTree tree;
  tree.readTree(inputFileName_, &*efficiency_);

  boost::shared_array<unsigned int> vKeep(new unsigned int[3]);
  vKeep[0] = 1;
  vKeep[1] = 0;
  vKeep[2] = 0;
  boost::shared_ptr<Efficiency> effVsDxy(efficiency_->projectAndRebin(vKeep));
  fillHistogram("EffVsDxy", "Efficiency vs absolute transverse impact parameter", effVsDxy );

  vKeep[0] = 0;
  vKeep[1] = 1;
  vKeep[2] = 0;
  boost::shared_ptr<Efficiency> effVsDz(efficiency_->projectAndRebin(vKeep));
  fillHistogram("EffVsDz", "Efficiency vs absolute longitudinal impact parameter", effVsDz);

  vKeep[0] = 0;
  vKeep[1] = 0;
  vKeep[2] = 1;
  boost::shared_ptr<Efficiency> effVsPt(efficiency_->projectAndRebin(vKeep));
  fillHistogram("EffVsPt", "Efficiency vs Pt", effVsPt);


  // unsigned int S = efficiency_->getLinearSize();
  // for( unsigned int i=0; i<S; ++i ) {
  //   std::cout << "reco eff["<<i<<"] = " << efficiency_->getEff(i) << " +/- " << efficiency_->getEffError(i) << std::endl;
  // }
}

EfficiencyAnalyzer::~EfficiencyAnalyzer() {}

void EfficiencyAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
}

// ------------ method called once each job just before starting event loop  ------------
void EfficiencyAnalyzer::beginJob()
{
  edm::Service<TFileService> fileService;
}

// ------------ method called once each job just after ending the event loop  ------------
void EfficiencyAnalyzer::endJob()
{
}

// ------------ method called when starting to processes a run  ------------
void EfficiencyAnalyzer::beginRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a run  ------------
void EfficiencyAnalyzer::endRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when starting to processes a luminosity block  ------------
void EfficiencyAnalyzer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void EfficiencyAnalyzer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void EfficiencyAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(EfficiencyAnalyzer);

