#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <boost/unordered/unordered_flat_set.hpp>
#include <boost/unordered/unordered_flat_map.hpp>

#include <boost/unordered/unordered_flat_map.hpp>

#include "config/thread_number/option.h"
#include "config/thread_number/type.h"
#include "model/table/tuple_index.h"

namespace algos::order {

class SortedPartition {
public:
    using EquivalenceClass = boost::unordered_flat_set<model::TupleIndex>;
    using EquivalenceClasses = std::vector<EquivalenceClass>;
    using PartitionIndex = unsigned long;
    using HashProduct = boost::unordered_flat_map<PartitionIndex, SortedPartition::EquivalenceClasses>;

private:
    EquivalenceClasses sorted_partition_;
    boost::unordered_flat_map<model::TupleIndex, PartitionIndex> hash_partition_;
    unsigned long num_rows_ = 0;

    void BuildHashTable();

public:
    SortedPartition() = default;
    explicit SortedPartition(unsigned long num_rows) noexcept : num_rows_(num_rows){};
    SortedPartition(EquivalenceClasses&& eq_classes, unsigned long num_rows)
        : sorted_partition_(std::move(eq_classes)), num_rows_(num_rows){};
    void Intersect(SortedPartition const& other);
    void IntersectParallel(SortedPartition const& other, config::ThreadNumType thread_num);

    SortedPartition::HashProduct BuildHashProduct(
            SortedPartition::EquivalenceClasses::const_iterator begin,
            SortedPartition::EquivalenceClasses::const_iterator end);

    EquivalenceClasses const& GetEqClasses() const {
        return sorted_partition_;
    }

    std::size_t Size() const {
        return sorted_partition_.size();
    }
};

}  // namespace algos::order
