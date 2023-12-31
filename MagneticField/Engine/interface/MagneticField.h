#ifndef MagneticField_MagneticField_h
#define MagneticField_MagneticField_h

/** \class MagneticField
 *
 *  Base class for the different implementation of magnetic field engines.
 *
 *  \author N. Amapane - CERN
 */

#include <atomic>

#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "FWCore/Utilities/interface/Visibility.h"
#include "FWCore/Utilities/interface/Likely.h"
#include "FWCore/Utilities/interface/thread_safety_macros.h"

class MagneticField {
public:
  MagneticField();
  MagneticField(const MagneticField& orig);
  virtual ~MagneticField();

  /// Derived classes can implement cloning without ownership of the
  /// underlying engines.
  virtual MagneticField* clone() const { return nullptr; }

  /// Field value ad specified global point, in Tesla
  virtual GlobalVector inTesla(const GlobalPoint& gp) const = 0;

  /// Field value ad specified global point, in KGauss
  GlobalVector inKGauss(const GlobalPoint& gp) const { return inTesla(gp) * 10.F; }

  /// Field value ad specified global point, in 1/Gev
  GlobalVector inInverseGeV(const GlobalPoint& gp) const { return inTesla(gp) * 2.99792458e-3F; }

  /// True if the point is within the region where the concrete field
  // engine is defined.
  virtual bool isDefined(const GlobalPoint& /*gp*/) const { return true; }

  /// Optional implementation that derived classes can implement to provide faster query
  /// by skipping the check to isDefined.
  virtual GlobalVector inTeslaUnchecked(const GlobalPoint& gp) const {
    return inTesla(gp);  // default dummy implementation
  }

  /// The nominal field value for this map in kGauss
  int nominalValue() const { return theNominalValue; }

  /// The inverse of field z component for this map in GeV
  float inverseBzAtOriginInGeV() const { return theInverseBzAtOriginInGeV; }

protected:
  // need to be called from the constructor of the deriving classes
  void setNominalValue();

private:
  //nominal field values
  int theNominalValue = 0;
  float theInverseBzAtOriginInGeV = 0;
};

#endif
