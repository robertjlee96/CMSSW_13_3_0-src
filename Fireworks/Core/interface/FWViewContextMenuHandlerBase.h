#ifndef Fireworks_Core_FWViewContextMenuHandlerBase_h
#define Fireworks_Core_FWViewContextMenuHandlerBase_h
// -*- C++ -*-
//
// Package:     Core
// Class  :     FWViewContextMenuHandlerBase
//
/**\class FWViewContextMenuHandlerBase FWViewContextMenuHandlerBase.h Fireworks/Core/interface/FWViewContextMenuHandlerBase.h

 Description: Base class for handling context menu's from views

 Usage:
    <usage>

*/
//
// Original Author:  Chris Jones
//         Created:  Mon Nov  2 13:46:26 CST 2009
//

// system include files

// user include files

// forward declarations
class FWModelId;
class FWModelContextMenuHandler;

class FWViewContextMenuHandlerBase {
public:
  FWViewContextMenuHandlerBase();
  virtual ~FWViewContextMenuHandlerBase();

  class MenuEntryAdder {
    friend class FWViewContextMenuHandlerBase;
    MenuEntryAdder(FWModelContextMenuHandler&);
    FWModelContextMenuHandler* m_handler;
    // int m_lastIndex;
  public:
    /**Add an entry by name of iEntryName to the context menu.  Returns the entry index
       which simply increments after each 'addEntry' call and starts at 0 */
    int addEntry(const char* iEntryName, int idx, bool enable = true);
  };

  // ---------- const member functions ---------------------

  // ---------- static member functions --------------------

  // ---------- member functions ---------------------------
  void addTo(FWModelContextMenuHandler&, const FWModelId& id);

  /**Called when a menu item was selected
    iEntryIndex: the index of the selected menu item.  Same as returned from 'addEntry'
    iX, iY: Screen coordinates of where mouse was clicked
    */
  virtual void select(int iEntryIndex, const FWModelId& id, int iX, int iY) = 0;

  FWViewContextMenuHandlerBase(const FWViewContextMenuHandlerBase&) = delete;  // stop default

  const FWViewContextMenuHandlerBase& operator=(const FWViewContextMenuHandlerBase&) = delete;  // stop default

private:
  ///Called when have to add entries to the context menu
  virtual void init(MenuEntryAdder&, const FWModelId& id) = 0;
};

#endif
