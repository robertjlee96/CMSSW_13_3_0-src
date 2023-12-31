#ifndef Fireworks_TableWidget_FWFramedTextTableCellRenderer_h
#define Fireworks_TableWidget_FWFramedTextTableCellRenderer_h
// -*- C++ -*-
//
// Package:     TableWidget
// Class  :     FWFramedTextTableCellRenderer
//
/**\class FWFramedTextTableCellRenderer FWFramedTextTableCellRenderer.h Fireworks/TableWidget/interface/FWFramedTextTableCellRenderer.h

 Description: A Cell Renderer who draws text with an outline and fills in the background

 Usage:
    The background color of the text graphics context will be used to draw an outline around the regular text
 and the color of the regular text is set by the foreground color of the text graphics context.

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

class FWFramedTextTableCellRenderer : public FWTableCellRendererBase {
public:
  static const TGGC& getDefaultGC();
  static const TGGC& getFillGC();

  enum Justify { kJustifyLeft, kJustifyRight, kJustifyCenter };

  FWFramedTextTableCellRenderer(const TGGC* iTextContext = &(getDefaultGC()),
                                const TGGC* iFillContext = &(getFillGC()),
                                Justify iJustify = kJustifyLeft);
  ~FWFramedTextTableCellRenderer() override;

  // ---------- const member functions ---------------------
  const TGGC* graphicsContext() const { return m_context; }
  UInt_t width() const override;
  UInt_t height() const override;

  const TGFont* font() const;
  // ---------- static member functions --------------------

  // ---------- member functions ---------------------------
  void setData(const std::string&);
  void setGraphicsContext(const TGGC* iContext) { m_context = iContext; }
  void setJustify(Justify);

  void draw(Drawable_t iID, int iX, int iY, unsigned int iWidth, unsigned int iHeight) override;

  FWFramedTextTableCellRenderer(const FWFramedTextTableCellRenderer&) = delete;  // stop default

  const FWFramedTextTableCellRenderer& operator=(const FWFramedTextTableCellRenderer&) = delete;  // stop default

private:
  // ---------- member data --------------------------------
  const TGGC* m_context;
  const TGGC* m_frameContext;
  TGFont* m_font;
  std::string m_data;
  Justify m_justify;
};

#endif
