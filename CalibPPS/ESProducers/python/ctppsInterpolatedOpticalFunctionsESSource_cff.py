from CalibPPS.ESProducers.ctppsInterpolatedOpticalFunctionsESSource_cfi import *
from Configuration.Eras.Modifier_run3_common_cff import run3_common
run3_common.toModify(ctppsInterpolatedOpticalFunctionsESSource, useNewLHCInfo = True)