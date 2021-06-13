#include "abstract_range_index.hpp"

#include <memory>
#include <vector>

#include "storage/index/adaptive_radix_tree/adaptive_radix_tree_index.hpp"
#include "storage/index/b_tree/b_tree_index.hpp"
#include "storage/index/group_key/composite_group_key_index.hpp"
#include "storage/index/group_key/group_key_index.hpp"
#include "storage/index/partial_hash_index/partial_hash_index.hpp"

namespace opossum {

size_t AbstractIndex::estimate_memory_consumption(SegmentIndexType type, ChunkOffset row_count,
                                                  ChunkOffset distinct_count, uint32_t value_bytes) {
  switch (type) {
    case SegmentIndexType::GroupKey:
      return GroupKeyIndex::estimate_memory_consumption(row_count, distinct_count, value_bytes);
    case SegmentIndexType::CompositeGroupKey:
      return CompositeGroupKeyIndex::estimate_memory_consumption(row_count, distinct_count, value_bytes);
    case SegmentIndexType::AdaptiveRadixTree:
      return AdaptiveRadixTreeIndex::estimate_memory_consumption(row_count, distinct_count, value_bytes);
    case SegmentIndexType::BTree:
      return BTreeIndex::estimate_memory_consumption(row_count, distinct_count, value_bytes);
    case SegmentIndexType::PartialHash:
      return PartialHashIndex::estimate_memory_consumption(row_count, distinct_count, value_bytes);
    case SegmentIndexType::Invalid:
      Fail("SegmentIndexType is invalid.");
  }
  Fail("GCC thinks this is reachable.");
}

AbstractIndex::AbstractIndex(const SegmentIndexType type) : _type{type} {}

bool AbstractIndex::is_index_for(const std::vector<std::shared_ptr<const AbstractSegment>>& segments) const {
  auto indexed_segments = _get_indexed_segments();
  if (segments.size() > indexed_segments.size()) return false;
  if (segments.empty()) return false;

  for (size_t i = 0; i < segments.size(); ++i) {
    if (segments[i] != indexed_segments[i]) return false;
  }
  return true;
}

AbstractIndex::Iterator AbstractIndex::cbegin() const { return _cbegin(); }

AbstractIndex::Iterator AbstractIndex::cend() const { return _cend(); }

AbstractIndex::Iterator AbstractIndex::null_cbegin() const { return _null_positions.cbegin(); }

AbstractIndex::Iterator AbstractIndex::null_cend() const { return _null_positions.cend(); }

SegmentIndexType AbstractIndex::type() const { return _type; }

size_t AbstractIndex::memory_consumption() const {
  size_t bytes{0u};
  bytes += _memory_consumption();
  bytes += sizeof(std::vector<ChunkOffset>);  // _null_positions
  bytes += sizeof(ChunkOffset) * _null_positions.capacity();
  bytes += sizeof(_type);
  return bytes;
}

}  // namespace opossum
