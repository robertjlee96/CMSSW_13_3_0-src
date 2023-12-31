#ifndef Fireworks_TableWidget_FWTextTableCellRenderer_h
#define Fireworks_TableWidget_FWTextTableCellRenderer_h
// -*- C++ -*-
//
// Package:     TableWidget
// Class  :     FWTextTableCellRenderer
//
/**\class FWTextTableCellRenderer FWTextTableCellRenderer.h Fireworks/TableWidget/interface/FWTextTableCellRenderer.h

 Description: A Cell Renderer who draws text and can show selection of a cell

 Usage:
    Use when the cells of a table are simple text and you want to be able to also show that a cell has been selected.

*/
//
// Original Author:  Chris Jones
//         Created:  Mon Feb  2 16:43:50 EST 2009
//

// system include files
#include <string>
#include "GuiTypes.h"
#include "TGResourcePool.h"
#include "TGGC.h"

// user include files
#include "Fireworks/TableWidget/interface/FWTableCellRendererBase.h"

// forward declarations

class FWTextTableCellRenderer : public FWTableCellRendererBase {
public:
  static const TGGC& getDefaultGC();
  static const TGGC& getDefaultHighlightGC();

  enum Justify { kJustifyLeft, kJustifyRight, kJustifyCenter };

  FWTextTableCellRenderer(const TGGC* iContext = &(getDefaultGC()),
                          const TGGC* iHighlightContext = &(getDefaultHighlightGC()),
                          Justify iJustify = kJustifyLeft);
  ~FWTextTableCellRenderer() override;

  // ---------- const member functions ---------------------
  const TGGC* graphicsContext() const { return m_context; }
  const TGGC* highlightContext() const { return m_highlightContext; }
  UInt_t width() const override;
  UInt_t height() const override;

  const TGFont* font() const;
  // ---------- static member functions --------------------

  // ---------- member functions ---------------------------
  void setData(const std::string&, bool isSelected);
  void setData(const char*, bool isSelected);
  const std::string& data() { return m_data; }
  void setGraphicsContext(const TGGC* iContext) { m_context = iContext; }
  void setHighlightContext(const TGGC* context) { m_highlightContext = context; }
  void setJustify(Justify);
  bool selected() { return m_isSelected; }

  void draw(Drawable_t iID, int iX, int iY, unsigned int iWidth, unsigned int iHeight) override;

  FWTextTableCellRenderer(const FWTextTableCellRenderer&) = delete;  // stop default

  const FWTextTableCellRenderer& operator=(const FWTextTableCellRenderer&) = delete;  // stop default

private:
  // ---------- member data --------------------------------
  const TGGC* m_context;
  const TGGC* m_highlightContext;
  TGFont* m_font;
  std::string m_data;
  bool m_isSelected;
  Justify m_justify;
};

#endif
