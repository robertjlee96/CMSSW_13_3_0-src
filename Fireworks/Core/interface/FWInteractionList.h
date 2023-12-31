#ifndef Fireworks_Core_FWInteractionList_h
#define Fireworks_Core_FWInteractionList_h
// -*- C++ -*-
//
// Package:     Core
// Class  :     FWInteractionList
//
/**\class FWInteractionList FWInteractionList.h Fireworks/Core/interface/FWInteractionList.h

 Description: [one line class summary]

 Usage:
    <usage>

*/
//
// Original Author:  Alja Mrak-Tadel
//         Created:  Mon Apr 19 12:48:12 CEST 2010
//

// system include files

// user include files
#include <set>
#include <vector>

// forward declarations
class TEveElement;
class TEveCompound;
class FWEventItem;
class FWModelId;

class FWInteractionList {
public:
  FWInteractionList(const FWEventItem* item);
  virtual ~FWInteractionList();

  // ---------- const member functions ---------------------

  const FWEventItem* item() const { return m_item; }
  bool empty() const { return m_compounds.empty(); }

  // ---------- static member functions --------------------

  // ---------- member functions ---------------------------
  void added(TEveElement*, unsigned int);
  //   void removed(TEveElement*, int);

  void modelChanges(const std::set<FWModelId>&);
  void itemChanged();

  FWInteractionList(const FWInteractionList&) = delete;  // stop default

  const FWInteractionList& operator=(const FWInteractionList&) = delete;  // stop default

private:
  // ---------- member data --------------------------------

  std::vector<TEveCompound*> m_compounds;
  const FWEventItem* m_item;
};

#endif
