#pragma once

#include "abstract_table_index_iterator.hpp"

namespace opossum {

struct AbstractTableIndexIterator : std::iterator<std::forward_iterator_tag, RowID> {
  using iterator_category = std::forward_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = std::vector<RowID>::value_type;
  using pointer = std::vector<RowID>::const_pointer;
  using reference = std::vector<RowID>::const_reference;

  using map_iterator_t = tsl::robin_map<AllTypeVariant, std::vector<RowID>>::iterator;

  AbstractTableIndexIterator(map_iterator_t begin) : _map_itr(begin), _v_idx(0) {}

  reference operator*() const { return _map_itr->second[_v_idx]; }
  AbstractTableIndexIterator& operator++() {
    if (++_v_idx >= _map_itr->second.size()) {
      _map_itr++;
      _v_idx = 0;
    }
    return *this;
  }
  AbstractTableIndexIterator operator++(int) {
    AbstractTableIndexIterator tmp = *this;
    ++(*this);
    return tmp;
  }
  bool operator==(const AbstractTableIndexIterator& other) const {
    return _map_itr == other._map_itr && _v_idx == other._v_idx;
  }
  bool operator!=(const AbstractTableIndexIterator& other) const {
    return _map_itr != other._map_itr || _v_idx != other._v_idx;
  }

 private:
  map_iterator_t _map_itr;
  size_t _v_idx;
};

} // namespace opossum