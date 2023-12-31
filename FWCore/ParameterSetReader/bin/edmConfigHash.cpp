
// This will return the ParameterSetID of the parameter set
// defined in the python file or configuration string.
// Warning, this may not be the same as the ParameterSetID
// of a cmsRun process, because validation code may insert
// additional parameters into the configuration.

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSetReader/interface/ParameterSetReader.h"
#include "FWCore/Utilities/interface/Exception.h"

#include <memory>

#include <iostream>
#include <string>

int main(int argc, char** argv) try {
  // config can either be a name or a string
  std::string config;
  std::vector<std::string> pythonArgs;

  if (argc == 1) {
    // Read input from cin into configstring..
    std::string line;
    while (std::getline(std::cin, line)) {
      config += line;
      config += '\n';
    }
  } else {
    config = std::string(argv[1]);
    pythonArgs.reserve(argc - 1);
    for (int iarg = 1; iarg < argc; ++iarg) {
      pythonArgs.push_back(argv[iarg]);
    }
  }

  std::shared_ptr<edm::ParameterSet> parameterSet = edm::readConfig(config, pythonArgs);
  parameterSet->registerIt();
  std::cout << parameterSet->id() << std::endl;
  return 0;
} catch (cms::Exception const& e) {
  std::cout << e.explainSelf() << std::endl;
  return 1;
} catch (std::exception const& e) {
  std::cout << e.what() << std::endl;
  return 1;
}
