#ifndef DQMServices_StreamerIO_TriggerSelector_h
#define DQMServices_StreamerIO_TriggerSelector_h

#include <memory>
#include <string>
#include <vector>

namespace edm {
  class EventSelector;
  class HLTGlobalStatus;
  class TriggerResults;
}  // namespace edm

namespace dqmservices {
  /**
 * Event selector allowing for and/not combination of triggers/paths
 *
 */

  class TriggerSelector {
  public:
    typedef std::vector<std::string> Strings;

    /**
   * Initializes TriggerSelector to use edm::EventSelector for selection.
   */
    TriggerSelector(Strings const& pathspecs, Strings const& names);

    /**
   * Takes selection string and list of triggers
   */
    TriggerSelector(std::string const& expression, Strings const& triggernames);

    ~TriggerSelector() = default;

    /**
   * Returns status of always positive bit
   */
    bool wantAll() const { return acceptAll_; }

    /**
   * Evaluates if trigger results pass selection
   */
    bool acceptEvent(edm::TriggerResults const&) const;

    /*
   * Takes array of trigger results and a number of triggers in array and
   * returns
   * if it passes selection
   */
    bool acceptEvent(unsigned char const*, int) const;

    /*
   * Returns if HLTGlobalStatus passes selection
   */
    bool returnStatus(edm::HLTGlobalStatus const& trStatus) const { return masterElement_->returnStatus(trStatus); }

    /*
   * Does XMl compatible formatting of the selection string
   */
    static std::string makeXMLString(std::string const& input);

  private:
    bool acceptAll_;

    /*
   * Starts parsing selection string
   */
    void init(std::string const& path, Strings const& triggernames);

    /*
   * Removes extra spaces from string
   */
    static std::string trim(std::string input);

    /*
   * Class used for storing internal representation of the selection string
   */
    class TreeElement {
      enum TreeOperator { NonInit = 0, AND = 1, OR = 2, NOT = 3, BR = 4 };

    public:
      /*
     * Parser of selection string. Splits string into tokens and initializes new
     * elements to parse them.
     */
      TreeElement(std::string const& inputString, Strings const& tr, TreeElement* parentElement = nullptr);
      ~TreeElement();

      /*
     * Returns selection status of current element calculated recursively from
     * it's child elements
     */
      bool returnStatus(edm::HLTGlobalStatus const& trStatus) const;

      /*
     * Returns operator type of the element
     */
      TreeOperator op() const { return op_; }

      /*
     * Returns parent element
     */
      TreeElement* parent() const { return parent_; }

    private:
      TreeElement* parent_;
      std::vector<TreeElement*> children_;
      TreeOperator op_;
      int trigBit_;
    };

    std::shared_ptr<TreeElement> masterElement_;

    // keep a copy of initialization string
    std::string expression_;

    std::shared_ptr<edm::EventSelector> eventSelector_;
    bool useOld_;

    static const bool debug_ = false;
  };

}  // namespace dqmservices

#endif  // DQMServices_StreamerIO_TriggerSelector_h
