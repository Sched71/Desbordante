#include "order.h"

#include "core/util/config/names_and_descriptions.h"
#include "core/util/config/tabular_data/input_table/option.h"

namespace algos::order {

Order::Order() : Algorithm({}) {
    RegisterOptions();
    MakeOptionsAvailable({util::config::TableOpt.GetName()});
}

void Order::RegisterOptions() {
    using namespace util::config::names;
    using namespace util::config::descriptions;
    using util::config::Option;

    RegisterOption(util::config::TableOpt(&input_table_));
}

void Order::LoadDataInternal() {
    typed_relation_ = model::ColumnLayoutTypedRelationData::CreateFrom(*input_table_, false);
}

void Order::ResetState() {}

unsigned long long Order::ExecuteInternal() {}

}  // namespace algos::order
