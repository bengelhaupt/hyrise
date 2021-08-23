#pragma once

#include <string>

#include "table_index_type.hpp"
#include "types.hpp"

namespace opossum {

struct TableIndexStatistics {
  std::vector<ColumnID> column_ids;
  std::string name;
  TableIndexType type;
};

// For googletest
bool operator==(const TableIndexStatistics& left, const TableIndexStatistics& right);

}  // namespace opossum
