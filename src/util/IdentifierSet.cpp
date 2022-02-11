#include "IdentifierSet.h"

#include <boost/dynamic_bitset.hpp>

namespace util {

IdentifierSet::IdentifierSet(ColumnLayoutRelationData const* const relation,
                             int index) : relation_(relation), tuple_index_(index) {
    data_.reserve(relation_->GetNumColumns());
    for (ColumnData const& col : relation_->GetColumnData()) {
        data_.push_back({.attribute = col.GetColumn(),
                         .cluster_index = col.GetProbingTableValue(tuple_index_)});
    }
}

std::string IdentifierSet::ToString() const {
    if (data_.empty()) {
        return "[]";
    }

    std::string str = "[";
    for (auto p = data_.begin(); p != data_.end() - 1; ++p) {
        str += "(" + p->attribute->GetName() + ", " + std::to_string(p->cluster_index) + "), ";
    }
    str += "(" + data_.back().attribute->GetName() + ", " +
           std::to_string(data_.back().cluster_index) + ")]";
    return str;
}

Vertical IdentifierSet::Intersect(const IdentifierSet &other) const {
    boost::dynamic_bitset<> intersection(relation_->GetNumColumns());
    auto p = data_.begin();
    auto q = other.data_.begin();

    while (p != data_.end() && q != other.data_.end()) {
        if (p->attribute->GetIndex() < q->attribute->GetIndex()) {
            ++p;
        } else {
            if (q->attribute->GetIndex() == p->attribute->GetIndex() &&
                p->cluster_index != 0 &&
                p->cluster_index == q->cluster_index) {
                intersection.set(p->attribute->GetIndex());
                ++p;
            }
            ++q;
        }
    }

    return relation_->GetSchema()->GetVertical(intersection);
}

} // namespace util

