#pragma once

#include <utility>

#include "storage/index/abstract_table_index.hpp"
#include <tsl/robin_map.h>

namespace opossum {

template <typename V>
struct FlatMapIterator
{
  using iterator_category = std::forward_iterator_tag;
  using difference_type   = std::ptrdiff_t;
  using value_type        = V;
  using pointer           = V*;
  using reference         = V&;

  using map_key = AllTypeVariant;
  using map_iterator = typename tsl::robin_map<map_key, std::vector<V>>::iterator;

  FlatMapIterator(map_iterator begin, map_iterator end)
      :_map_itr(begin), _end(end) {}

  reference operator*() const {
    return *_map_itr.second[_v_idx];
  }

  pointer operator->() {
    return _map_itr.second[_v_idx];
  }

  FlatMapIterator& operator++() {
    if(_map_itr == _end) return FlatMapIterator<RowID>(_end, _end); // TODO(pi): maybe use different end return type?
    if(++_v_idx >= _map_itr.second->size()) {
      _map_itr++;
      _v_idx = 0;
    }
    return *this;
  }
  FlatMapIterator operator++(int) {
    FlatMapIterator tmp = *this;
    ++(*this);
    return tmp;
  }
  friend bool operator== (const FlatMapIterator& a, const FlatMapIterator& b) {
    return a._map_itr == b._map_itr && a._v_idx == b._v_idx;
  }
  friend bool operator!= (const FlatMapIterator& a, const FlatMapIterator& b) {
    return a._map_itr != b._map_itr || a._v_idx != b._v_idx;
  }

 private:
  map_iterator _map_itr;
  map_iterator _end;

  size_t _v_idx = 0; //TODO(pi): what happens if vector is empty?!
};

class PartialHashIndexTest;

class PartialHashIndex : public AbstractTableIndex<PartialHashIndex::Iterator> {
  friend PartialHashIndexTest;

 public:
  using Iterator = FlatMapIterator<RowID>;

  /**
 * Predicts the memory consumption in bytes of creating this index.
 * See AbstractIndex::estimate_memory_consumption()
 * The introduction of PMR strings increased this significantly (in one test from 320 to 896). If you are interested
 * in reducing the memory footprint of these indexes, this is probably the first place you should look.
 */
  static size_t estimate_memory_consumption(ChunkOffset row_count, ChunkOffset distinct_count, uint32_t value_bytes);

  PartialHashIndex() = delete;
  PartialHashIndex(const std::vector<std::pair<ChunkID, std::shared_ptr<Chunk>>>&, const ColumnID);


 protected:
  Iterator _cbegin() const override;
  Iterator _cend() const override;
  std::vector<std::shared_ptr<const AbstractSegment>> _get_indexed_segments() const override;
  size_t _memory_consumption() const override;

  std::pair<Iterator, Iterator> _equals(const AllTypeVariant& value) const override;

  // ToDO(pi) impl on cpp
  bool _is_index_for(const ColumnID column_id) const override {
    return column_id == _column_id;
  }
  // returns sorted array
  std::set<ChunkID> _get_indexed_chunk_ids() const override {
    return _indexed_chunk_ids;
  }
 private:
  tsl::robin_map<AllTypeVariant, std::vector<RowID>> _map; // ToDo(pi) check unordered map? sortiert wegen greater than etc.
  // TODO(pi): Decide whether we store column id here or use tablestatistics on the table
  ColumnID _column_id;
  std::vector<RowID> _row_ids;
  std::set<ChunkID> _indexed_chunk_ids; // constant time lookup
  std::vector<std::shared_ptr<const AbstractSegment>> _indexed_segments;

};

}  // namespace opossum
