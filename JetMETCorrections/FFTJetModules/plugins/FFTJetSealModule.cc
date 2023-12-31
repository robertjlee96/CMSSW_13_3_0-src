#include "CondCore/ESSources/interface/registration_macros.h"

#include "JetMETCorrections/FFTJetObjects/interface/FFTJetCorrectorSequenceTypes.h"
#include "JetMETCorrections/FFTJetObjects/interface/FFTJetCorrectorSequenceRcdTypes.h"
#include "JetMETCorrections/FFTJetObjects/interface/FFTJetLookupTableRcdTypes.h"
#include "JetMETCorrections/FFTJetObjects/interface/FFTJetLookupTableSequence.h"
#include "JetMETCorrections/FFTJetObjects/interface/FFTJet.h"

REGISTER_PLUGIN(FFTBasicJetCorrectorSequenceRcd, FFTBasicJetCorrectorSequence);
REGISTER_PLUGIN(FFTGenJetCorrectorSequenceRcd, FFTGenJetCorrectorSequence);
REGISTER_PLUGIN(FFTCaloJetCorrectorSequenceRcd, FFTCaloJetCorrectorSequence);
REGISTER_PLUGIN(FFTPFJetCorrectorSequenceRcd, FFTPFJetCorrectorSequence);
REGISTER_PLUGIN(FFTTrackJetCorrectorSequenceRcd, FFTTrackJetCorrectorSequence);
REGISTER_PLUGIN(FFTJPTJetCorrectorSequenceRcd, FFTJPTJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTPFCHS0CorrectorSequenceRcd, FFTPFJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTPFCHS1CorrectorSequenceRcd, FFTPFJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTPFCHS2CorrectorSequenceRcd, FFTPFJetCorrectorSequence);

REGISTER_PLUGIN_NO_SERIAL(FFTBasicJetSysCorrectorSequenceRcd, FFTBasicJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTGenJetSysCorrectorSequenceRcd, FFTGenJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTCaloJetSysCorrectorSequenceRcd, FFTCaloJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTPFJetSysCorrectorSequenceRcd, FFTPFJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTTrackJetSysCorrectorSequenceRcd, FFTTrackJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTJPTJetSysCorrectorSequenceRcd, FFTJPTJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTPFCHS0SysCorrectorSequenceRcd, FFTPFJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTPFCHS1SysCorrectorSequenceRcd, FFTPFJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTPFCHS2SysCorrectorSequenceRcd, FFTPFJetCorrectorSequence);

REGISTER_PLUGIN_NO_SERIAL(FFTGen0CorrectorSequenceRcd, FFTGenJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTGen1CorrectorSequenceRcd, FFTGenJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTGen2CorrectorSequenceRcd, FFTGenJetCorrectorSequence);

REGISTER_PLUGIN_NO_SERIAL(FFTPF0CorrectorSequenceRcd, FFTPFJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTPF1CorrectorSequenceRcd, FFTPFJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTPF2CorrectorSequenceRcd, FFTPFJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTPF3CorrectorSequenceRcd, FFTPFJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTPF4CorrectorSequenceRcd, FFTPFJetCorrectorSequence);

REGISTER_PLUGIN_NO_SERIAL(FFTCalo0CorrectorSequenceRcd, FFTCaloJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTCalo1CorrectorSequenceRcd, FFTCaloJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTCalo2CorrectorSequenceRcd, FFTCaloJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTCalo3CorrectorSequenceRcd, FFTCaloJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTCalo4CorrectorSequenceRcd, FFTCaloJetCorrectorSequence);

REGISTER_PLUGIN_NO_SERIAL(FFTGen0SysCorrectorSequenceRcd, FFTGenJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTGen1SysCorrectorSequenceRcd, FFTGenJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTGen2SysCorrectorSequenceRcd, FFTGenJetCorrectorSequence);

REGISTER_PLUGIN_NO_SERIAL(FFTPF0SysCorrectorSequenceRcd, FFTPFJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTPF1SysCorrectorSequenceRcd, FFTPFJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTPF2SysCorrectorSequenceRcd, FFTPFJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTPF3SysCorrectorSequenceRcd, FFTPFJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTPF4SysCorrectorSequenceRcd, FFTPFJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTPF5SysCorrectorSequenceRcd, FFTPFJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTPF6SysCorrectorSequenceRcd, FFTPFJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTPF7SysCorrectorSequenceRcd, FFTPFJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTPF8SysCorrectorSequenceRcd, FFTPFJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTPF9SysCorrectorSequenceRcd, FFTPFJetCorrectorSequence);

REGISTER_PLUGIN_NO_SERIAL(FFTCalo0SysCorrectorSequenceRcd, FFTCaloJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTCalo1SysCorrectorSequenceRcd, FFTCaloJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTCalo2SysCorrectorSequenceRcd, FFTCaloJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTCalo3SysCorrectorSequenceRcd, FFTCaloJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTCalo4SysCorrectorSequenceRcd, FFTCaloJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTCalo5SysCorrectorSequenceRcd, FFTCaloJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTCalo6SysCorrectorSequenceRcd, FFTCaloJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTCalo7SysCorrectorSequenceRcd, FFTCaloJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTCalo8SysCorrectorSequenceRcd, FFTCaloJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTCalo9SysCorrectorSequenceRcd, FFTCaloJetCorrectorSequence);

REGISTER_PLUGIN_NO_SERIAL(FFTCHS0SysCorrectorSequenceRcd, FFTPFJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTCHS1SysCorrectorSequenceRcd, FFTPFJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTCHS2SysCorrectorSequenceRcd, FFTPFJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTCHS3SysCorrectorSequenceRcd, FFTPFJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTCHS4SysCorrectorSequenceRcd, FFTPFJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTCHS5SysCorrectorSequenceRcd, FFTPFJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTCHS6SysCorrectorSequenceRcd, FFTPFJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTCHS7SysCorrectorSequenceRcd, FFTPFJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTCHS8SysCorrectorSequenceRcd, FFTPFJetCorrectorSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTCHS9SysCorrectorSequenceRcd, FFTPFJetCorrectorSequence);

REGISTER_PLUGIN(FFTEtaFlatteningFactorsTableRcd, FFTJetLookupTableSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTPileupRhoCalibrationTableRcd, FFTJetLookupTableSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTPileupRhoEtaDependenceTableRcd, FFTJetLookupTableSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTLUT0TableRcd, FFTJetLookupTableSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTLUT1TableRcd, FFTJetLookupTableSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTLUT2TableRcd, FFTJetLookupTableSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTLUT3TableRcd, FFTJetLookupTableSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTLUT4TableRcd, FFTJetLookupTableSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTLUT5TableRcd, FFTJetLookupTableSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTLUT6TableRcd, FFTJetLookupTableSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTLUT7TableRcd, FFTJetLookupTableSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTLUT8TableRcd, FFTJetLookupTableSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTLUT9TableRcd, FFTJetLookupTableSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTLUT10TableRcd, FFTJetLookupTableSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTLUT11TableRcd, FFTJetLookupTableSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTLUT12TableRcd, FFTJetLookupTableSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTLUT13TableRcd, FFTJetLookupTableSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTLUT14TableRcd, FFTJetLookupTableSequence);
REGISTER_PLUGIN_NO_SERIAL(FFTLUT15TableRcd, FFTJetLookupTableSequence);
