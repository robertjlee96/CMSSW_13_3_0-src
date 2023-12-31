#include "CalibFormats/CaloObjects/interface/CaloSamples.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include <cmath>
#include <iostream>

CaloSamples::CaloSamples() : id_(), size_(0), presamples_(0), preciseSize_(0), precisePresamples_(0) { setBlank(); }

CaloSamples::CaloSamples(const DetId &id, int size)
    : id_(id),
      size_(size),
      presamples_(0),
      data_(size_, 0.0),
      deltaTprecise_(0.0f),
      preciseSize_(0),
      precisePresamples_(0) {
  setBlank();
}

CaloSamples::CaloSamples(const DetId &id, int size, int presize)
    : id_(id),
      size_(size),
      presamples_(0),
      data_(size_, 0.0),
      deltaTprecise_(0.0f),
      preciseSize_(presize),
      precisePresamples_(0) {
  setBlank();
}

// add option to set these later.
void CaloSamples::resetPrecise() { preciseData_.resize(preciseSize_, 0); }

void CaloSamples::setPresamples(int pre) { presamples_ = pre; }

CaloSamples &CaloSamples::scale(double value) {
  for (int i = 0; i < size_; i++)
    data_[i] *= value;
  for (std::vector<float>::iterator j = preciseData_.begin(); j != preciseData_.end(); ++j)
    (*j) *= value;
  return (*this);
}

CaloSamples &CaloSamples::operator+=(double value) {
  for (int i = 0; i < size_; i++)
    data_[i] += value;
  for (std::vector<float>::iterator j = preciseData_.begin(); j != preciseData_.end(); ++j)
    (*j) += value * deltaTprecise_ / 25.0;  // note that the scale is conserved!
  return (*this);
}

CaloSamples &CaloSamples::operator+=(const CaloSamples &other) {
  bool addHighFidelityPreMix = false;
  if (size_ != other.size_ || presamples_ != other.presamples_ || preciseSize_ != other.preciseSize_) {
    if (presamples_ == other.presamples_ && preciseSize_ == other.size_) {
      addHighFidelityPreMix = true;
    } else {
      edm::LogError("CaloHitResponse") << "Mismatched calo signals ";
    }
  }
  if (addHighFidelityPreMix) {
    int sampleBin(0);
    for (int i = 0; i < preciseSize_; ++i) {
      sampleBin = floor(i * deltaTprecise_ / 25);
      data_[sampleBin] += other.data_[i];
      preciseData_[i] += other.data_[i];
    }
  } else {
    int i;
    for (i = 0; i < size_; ++i) {
      data_[i] += other.data_[i];
    }
    if (preciseData_.empty() && !other.preciseData_.empty())
      resetPrecise();
    if (!other.preciseData_.empty()) {
      for (i = 0; i < preciseSize_; ++i) {
        preciseData_[i] += other.preciseData_[i];
      }
    }
  }
  return *this;
}

CaloSamples &CaloSamples::offsetTime(double offset) {
  std::vector<double> data(size_, 0.0);
  for (int i(0); i != size_; ++i) {
    double t = i * 25. - offset;
    int firstbin = floor(t / 25.);
    double f = t / 25. - firstbin;
    int nextbin = firstbin + 1;
    double v1 = (firstbin < 0 || firstbin >= size_) ? 0. : data_[firstbin];
    double v2 = (nextbin < 0 || nextbin >= size_) ? 0. : data_[nextbin];
    data[i] = (v1 * (1. - f) + v2 * f);
  }
  for (int i(0); i != size_; ++i) {
    data_[i] = data[i];
  }
  return (*this);
}

bool CaloSamples::isBlank() const  // are the samples blank (zero?)
{
  for (int i(0); i != size_; ++i) {
    if (1.e-6 < fabs(data_[i]))
      return false;
  }
  return true;
}

void CaloSamples::setBlank()  // keep id, presamples, size but zero out data
{
  std::fill(data_.begin(), data_.end(), (double)0.0);
  std::fill(preciseData_.begin(), preciseData_.end(), (double)0.0);
}

std::ostream &operator<<(std::ostream &s, const CaloSamples &samples) {
  s << "DetId " << samples.id();
  // print out every so many precise samples
  float preciseStep = samples.preciseSize() / samples.size();
  s << ", " << samples.size() << " samples";
  if (preciseStep > 0)
    s << ", " << samples.preciseSize() << " preciseSamples"
      << ", " << preciseStep << " preciseStep";
  s << '\n';
  for (int i = 0; i < samples.size(); i++) {
    s << i << ":" << samples[i] << " precise:";
    int precise_start(i * preciseStep), precise_end(precise_start + preciseStep);
    for (int j(precise_start); ((j < precise_end) && (j < samples.preciseSize())); ++j)
      s << " " << samples.preciseAt(j);
    s << std::endl;
  }
  return s;
}
