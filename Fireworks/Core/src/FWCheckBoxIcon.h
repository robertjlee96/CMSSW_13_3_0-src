#ifndef Fireworks_Core_FWCheckBoxIcon_h
#define Fireworks_Core_FWCheckBoxIcon_h
// -*- C++ -*-
//
// Package:     Core
// Class  :     FWCheckBoxIcon
//
/**\class FWCheckBoxIcon FWCheckBoxIcon.h Fireworks/Core/interface/FWCheckBoxIcon.h

 Description: <one line class summary>

 Usage:
    <usage>

*/
//
// Original Author:  Chris Jones
//         Created:  Thu Feb 19 16:25:14 CST 2009
//

// system include files

// user include files
#include "Fireworks/Core/src/FWBoxIconBase.h"

// forward declarations

class FWCheckBoxIcon : public FWBoxIconBase {
public:
  FWCheckBoxIcon(unsigned int iEdgeLength);
  ~FWCheckBoxIcon() override;

  // ---------- const member functions ---------------------
  bool isChecked() const { return m_checked; }

  // ---------- static member functions --------------------
  static const TString& coreIcondir();

  // ---------- member functions ---------------------------
  void setChecked(bool iChecked) { m_checked = iChecked; }

  FWCheckBoxIcon(const FWCheckBoxIcon&) = delete;  // stop default

  const FWCheckBoxIcon& operator=(const FWCheckBoxIcon&) = delete;  // stop default

private:
  void drawInsideBox(Drawable_t iID, GContext_t iContext, int iX, int iY, unsigned int iSize) const override;

  // ---------- member data --------------------------------
  bool m_checked;
};

#endif
