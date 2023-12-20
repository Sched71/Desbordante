#pragma once

#include "algorithms/order/order.h"
#include "get_algorithm.h"
#include "py_algorithm.h"

namespace python_bindings {

class PyOrder : public PyAlgorithm<algos::order::Order, PyAlgorithmBase> {
public:
    using ODPairs =
            std::vector<std::pair<algos::order::AttributeList, algos::order::AttributeList>>;

private:
    using PyAlgorithmBase::algorithm_;

public:
    [[nodiscard]] ODPairs GetValidODs() const {
        algos::order::OrderDependencies ods =
                GetAlgorithm<algos::order::Order>(algorithm_).GetValidODs();
        ODPairs res;
        for (auto const& [lhs, rhs_list] : ods) {
            for (algos::order::AttributeList const& rhs : rhs_list) {
                res.emplace_back(lhs, rhs);
            }
        }
        return res;
    }
};

}  // namespace python_bindings
