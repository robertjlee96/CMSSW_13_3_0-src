// -*- C++ -*-
//
// Package:     Services
// Class  :     LoadAllDictionaries
//
/**\class LoadAllDictionaries
 
 Description: Loads all Capability dictionaries
 
 Usage:
 <usage>
 
 */
//
// Original Author:  Chris Jones
//         Created:  Thu Sep 15 09:47:48 EDT 2005
//

// system include files

// user include files
#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PluginManager/interface/PluginManager.h"
#include "FWCore/PluginManager/interface/PluginCapabilities.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

namespace edm {
  namespace service {
    class LoadAllDictionaries {
    public:
      LoadAllDictionaries(const edm::ParameterSet&);
      LoadAllDictionaries(const LoadAllDictionaries&) = delete;                   // stop default
      const LoadAllDictionaries& operator=(const LoadAllDictionaries&) = delete;  // stop default

      //virtual ~LoadAllDictionaries();

      // ---------- const member functions ---------------------

      // ---------- static member functions --------------------
      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

      // ---------- member functions ---------------------------

    private:
      // ---------- member data --------------------------------
    };
  }  // namespace service
}  // namespace edm

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
edm::service::LoadAllDictionaries::LoadAllDictionaries(const edm::ParameterSet& iConfig) {
  bool doLoad(iConfig.getUntrackedParameter<bool>("doLoad"));
  if (doLoad) {
    edmplugin::PluginManager* db = edmplugin::PluginManager::get();

    typedef edmplugin::PluginManager::CategoryToInfos CatToInfos;

    CatToInfos::const_iterator itFound = db->categoryToInfos().find("Capability");

    if (itFound == db->categoryToInfos().end()) {
      return;
    }
    std::string lastClass;
    const std::string mystring("edm::Wrapper");

    for (edmplugin::PluginManager::Infos::const_iterator itInfo = itFound->second.begin(),
                                                         itInfoEnd = itFound->second.end();
         itInfo != itInfoEnd;
         ++itInfo) {
      if (lastClass == itInfo->name_) {
        continue;
      }

      lastClass = itInfo->name_;
      if (lastClass.find(mystring) != std::string::npos) {
        edmplugin::PluginCapabilities::get()->load(lastClass);
      }
      //NOTE: since we have the library already, we could be more efficient if we just load it ourselves
    }
  }
}

void edm::service::LoadAllDictionaries::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.addUntracked<bool>("doLoad", true)->setComment("Only if 'true' do we load all dictionaries");
  descriptions.add("LoadAllDictionaries", desc);
  descriptions.setComment(
      "This service allows you to force all known dictionaries to be loaded at the beginning of the job");
}

using edm::service::LoadAllDictionaries;
DEFINE_FWK_SERVICE_MAKER(LoadAllDictionaries, edm::serviceregistry::ParameterSetMaker<LoadAllDictionaries>);
