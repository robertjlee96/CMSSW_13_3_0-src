#ifndef SimG4CMS_HcalTestNumberingScheme_h
#define SimG4CMS_HcalTestNumberingScheme_h
///////////////////////////////////////////////////////////////////////////////
// File: HcalTestNumberingScheme.h
// Description: Numbering scheme for hadron calorimeter (detailed for TB)
///////////////////////////////////////////////////////////////////////////////

#include "SimG4CMS/Calo/interface/HcalNumberingScheme.h"
#include "DataFormats/HcalDetId/interface/HcalTestNumbering.h"

class HcalTestNumberingScheme : public HcalNumberingScheme {
public:
  HcalTestNumberingScheme(bool forTB);
  HcalTestNumberingScheme() = delete;
  ~HcalTestNumberingScheme() override;
  uint32_t getUnitID(const HcalNumberingFromDDD::HcalID& id) override;
  static uint32_t packHcalIndex(int det, int z, int depth, int eta, int phi, int lay);
  static void unpackHcalIndex(const uint32_t& idx, int& det, int& z, int& depth, int& eta, int& phi, int& lay);

private:
  bool forTBH2;
};

#endif
