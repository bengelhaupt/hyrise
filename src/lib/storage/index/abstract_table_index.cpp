#include "abstract_table_index.hpp"

namespace opossum {

<typename Iterator>
AbstractTableIndex::AbstractTableIndex(const IndexType type) : AbstractIndex<RowID, Iterator>(type) {}

std::pair<AbstractTableIndex::Iterator, AbstractTableIndex::Iterator> AbstractTableIndex::equals(const AllTypeVariant& value) const {
  return _equals(value);
}

bool AbstractTableIndex::is_index_for(const ColumnID column_id) const {
  return _is_index_for(column_id);
}

std::set<ChunkID> AbstractTableIndex::get_indexed_chunk_ids() const {
  return _get_indexed_chunk_ids();
}

}  // namespace opossum
