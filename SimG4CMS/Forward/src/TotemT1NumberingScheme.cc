// -*- C++ -*-
//
// Package:     Forward
// Class  :     TotemT1NumberingScheme
//
// Implementation:
//     <Notes on implementation>
//
// Original Author:
//         Created:  Tue May 16 10:14:34 CEST 2006
//

// system include files

// user include files
#include "SimG4CMS/Forward/interface/TotemT1NumberingScheme.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

//
// constructors and destructor
//
TotemT1NumberingScheme::TotemT1NumberingScheme(int i) {
  edm::LogVerbatim("ForwardSim") << " Creating TotemT1NumberingScheme";
  setCurrentDetectorPosition(i);
}

TotemT1NumberingScheme::~TotemT1NumberingScheme() {}
