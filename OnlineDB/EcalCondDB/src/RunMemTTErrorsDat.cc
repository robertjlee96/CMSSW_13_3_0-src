#include <stdexcept>
#include <string>
#include "OnlineDB/Oracle/interface/Oracle.h"

#include "OnlineDB/EcalCondDB/interface/RunMemTTErrorsDat.h"
#include "OnlineDB/EcalCondDB/interface/RunIOV.h"

using namespace std;
using namespace oracle::occi;

RunMemTTErrorsDat::RunMemTTErrorsDat() {
  m_env = nullptr;
  m_conn = nullptr;
  m_writeStmt = nullptr;
  m_readStmt = nullptr;

  m_errorBits = 0;
}

RunMemTTErrorsDat::~RunMemTTErrorsDat() {}

void RunMemTTErrorsDat::prepareWrite() noexcept(false) {
  this->checkConnection();

  try {
    m_writeStmt = m_conn->createStatement();
    /* Using TO_NUMBER because OCCI does not support 64-bit integers well */
    m_writeStmt->setSQL(
        "INSERT INTO run_mem_tt_errors_dat (iov_id, logic_id, "
        "error_bits) "
        "VALUES (:iov_id, :logic_id, "
        "to_number(:error_bits))");
  } catch (SQLException& e) {
    throw(std::runtime_error("RunMemTTErrorsDat::prepareWrite():  " + e.getMessage()));
  }
}

void RunMemTTErrorsDat::writeDB(const EcalLogicID* ecid, const RunMemTTErrorsDat* item, RunIOV* iov) noexcept(false) {
  this->checkConnection();
  this->checkPrepare();

  int iovID = iov->fetchID();
  if (!iovID) {
    throw(std::runtime_error("RunMemTTErrorsDat::writeDB:  IOV not in DB"));
  }

  int logicID = ecid->getLogicID();
  if (!logicID) {
    throw(std::runtime_error("RunMemTTErrorsDat::writeDB:  Bad EcalLogicID"));
  }

  try {
    m_writeStmt->setInt(1, iovID);
    m_writeStmt->setInt(2, logicID);
    m_writeStmt->setString(3, std::to_string(item->getErrorBits()));
    m_writeStmt->executeUpdate();
  } catch (SQLException& e) {
    throw(std::runtime_error("RunMemTTErrorsDat::writeDB():  " + e.getMessage()));
  }
}

void RunMemTTErrorsDat::fetchData(map<EcalLogicID, RunMemTTErrorsDat>* fillMap, RunIOV* iov) noexcept(false) {
  this->checkConnection();
  fillMap->clear();

  iov->setConnection(m_env, m_conn);
  int iovID = iov->fetchID();
  if (!iovID) {
    //  throw(std::runtime_error("RunMemTTErrorsDat::writeDB:  IOV not in DB"));
    return;
  }

  try {
    /* Using TO_CHAR because OCCI does not support 64-bit integers well */
    m_readStmt->setSQL(
        "SELECT cv.name, cv.logic_id, cv.id1, cv.id2, cv.id3, cv.maps_to, "
        "to_char(d.error_bits) "
        "FROM channelview cv JOIN run_mem_tt_errors_dat d "
        "ON cv.logic_id = d.logic_id AND cv.name = cv.maps_to "
        "WHERE d.iov_id = :iov_id");
    m_readStmt->setInt(1, iovID);
    ResultSet* rset = m_readStmt->executeQuery();

    std::pair<EcalLogicID, RunMemTTErrorsDat> p;
    RunMemTTErrorsDat dat;
    while (rset->next()) {
      p.first = EcalLogicID(rset->getString(1),   // name
                            rset->getInt(2),      // logic_id
                            rset->getInt(3),      // id1
                            rset->getInt(4),      // id2
                            rset->getInt(5),      // id3
                            rset->getString(6));  // maps_to

      dat.setErrorBits(std::stoul(rset->getString(7)));

      p.second = dat;
      fillMap->insert(p);
    }

  } catch (SQLException& e) {
    throw(std::runtime_error("RunMemTTErrorsDat::fetchData():  " + e.getMessage()));
  }
}
