//
// Created by alex on 13.04.2021.
//

#include "LatticeObservations.h"

NodeCategory LatticeObservations::updateDependencyCategory(Vertical const& vertical) {
    //бежим по множеству подмножеств и смотрим. если получили все независимости, то это мин зависимость
    if (vertical.getArity() > 1) {
        boost::dynamic_bitset<> columnIndices = vertical.getColumnIndices(); //копируем индексы
        bool hasUncheckedSubset = false;

        for (size_t index = columnIndices.find_first();
             index < columnIndices.size();
             index = columnIndices.find_next(index))
        {
            columnIndices[index] = false; //убираем одну из колонок
            auto const subsetVerticalIter = this->find(Vertical(vertical.getSchema(), columnIndices));

            //если какое-то подмножество не посещено либо оно не является антизависимостью, то не подходит

            if (subsetVerticalIter == this->end()) {
                //если нашли нерассмотренное подмножество
                hasUncheckedSubset = true;
            } else {
                NodeCategory const &subsetVerticalCategory = subsetVerticalIter->second;
                if (subsetVerticalCategory == NodeCategory::minimalDependency ||
                    subsetVerticalCategory == NodeCategory::dependency ||
                    subsetVerticalCategory == NodeCategory::candidateMinimalDependency
                ) {
                    return NodeCategory::dependency;
                }
            }
            //если все норм
            columnIndices[index] = true; //возвращаем как было
        }
        return hasUncheckedSubset ? NodeCategory::candidateMinimalDependency : NodeCategory::minimalDependency;
    } else {
        return NodeCategory::minimalDependency;
    }
}

/*NodeCategory LatticeObservations::updateNonDependencyCategory(Vertical const& vertical, int rhsIndex) {
    boost::dynamic_bitset<> columnIndices = vertical.getColumnIndices();
    //columnIndices[rhsIndex] = true;
    bool hasUncheckedSuperset = false;

    for (size_t index = 0; index < columnIndices.size(); index++) {
        if (!columnIndices[index] && index != rhsIndex) {
            columnIndices[index] = true; //убираем одну из колонок
            auto const supersetVerticalIter = this->find(Vertical(vertical.getSchema(), columnIndices)); //TODO !!!лучше переделать без второго flip а просто бежать циклом по нулям

            if (supersetVerticalIter == this->end()) {
                //если нашли нерассмотренное надмножество
                hasUncheckedSuperset = true;
            } else {
                NodeCategory const &supersetVerticalCategory = supersetVerticalIter->second;
                if (supersetVerticalCategory == NodeCategory::maximalNonDependency ||
                    supersetVerticalCategory == NodeCategory::nonDependency ||
                    supersetVerticalCategory == NodeCategory::candidateMaximalNonDependency
                ) {
                    return NodeCategory::nonDependency;
                }
            }
            columnIndices[index] = false; //возвращаем как было
        }
    }
    return hasUncheckedSuperset ? NodeCategory::candidateMaximalNonDependency : NodeCategory::maximalNonDependency;
}*/

NodeCategory LatticeObservations::updateNonDependencyCategory(Vertical const& vertical, int rhsIndex) {
    boost::dynamic_bitset<> columnIndices = vertical.getColumnIndices();
    columnIndices[rhsIndex] = true;
    columnIndices.flip();

    bool hasUncheckedSuperset = false;

    for (size_t index = columnIndices.find_first();
         index < columnIndices.size();
         index = columnIndices.find_next(index))
    {
        auto const supersetVerticalIter = this->find(vertical.Union(*vertical.getSchema()->getColumn(index))); //TODO !!!лучше переделать без второго flip а просто бежать циклом по нулям

        if (supersetVerticalIter == this->end()) {
            //если нашли нерассмотренное надмножество
            hasUncheckedSuperset = true;
        } else {
            NodeCategory const &supersetVerticalCategory = supersetVerticalIter->second;
            if (supersetVerticalCategory == NodeCategory::maximalNonDependency ||
                supersetVerticalCategory == NodeCategory::nonDependency ||
                supersetVerticalCategory == NodeCategory::candidateMaximalNonDependency
            ) {
                return NodeCategory::nonDependency;
            }
        }
    }
    return hasUncheckedSuperset ? NodeCategory::candidateMaximalNonDependency : NodeCategory::maximalNonDependency;
}

bool LatticeObservations::isCandidate(Vertical const& node) {
    auto nodeIter = this->find(node);
    if (nodeIter == this->end()) {
        return false;
    } else {
        return nodeIter->second == NodeCategory::candidateMaximalNonDependency ||
               nodeIter->second == NodeCategory::candidateMinimalDependency;
    }
}


std::unordered_set<Vertical>
LatticeObservations::getUncheckedSubsets(Vertical const& node, size_t rhsIndex, ColumnOrder const& columnOrder) {
    boost::dynamic_bitset<> indices = node.getColumnIndices();
    std::unordered_set<Vertical> uncheckedSubsets;

    /*for (size_t index = indices.find_first(); index < indices.size(); index = indices.find_next(index)) {
        if (index != rhsIndex) {    //в метаноме этого условия нет
            indices[index] = false;
            Vertical subsetNode = Vertical(node.getSchema(), indices);

            if (this->find(subsetNode) == this->end()) {
                uncheckedSubsets.insert(std::move(subsetNode));
            }
            indices[index] = true;
        }
    }*/

    for (int columnIndex : columnOrder.getOrderHighDistinctCount(node)) {
        indices[columnIndex] = false;
        Vertical subsetNode = Vertical(node.getSchema(), indices);
        if (this->find(subsetNode) == this->end()) {
            uncheckedSubsets.insert(std::move(subsetNode));
        }
        indices[columnIndex] = true;
    }

    return uncheckedSubsets;
}

std::unordered_set<Vertical>
LatticeObservations::getUncheckedSupersets(Vertical const& node, size_t rhsIndex, ColumnOrder const& columnOrder) {
    boost::dynamic_bitset<> flippedIndices = node.getColumnIndices().flip();
    std::unordered_set<Vertical> uncheckedSupersets;

    /*for (size_t index = 0; index < indices.size(); index++) {
        if (index != rhsIndex && !indices[index]) {
            indices[index] = true;
            Vertical supersetNode = Vertical(node.getSchema(), indices);

            if (this->find(supersetNode) == this->end()) {
                uncheckedSupersets.insert(std::move(supersetNode));
            }
            indices[index] = false;
        }
    }*/
    flippedIndices[rhsIndex] = false;

    for (int columnIndex : columnOrder.getOrderHighDistinctCount(Vertical(node.getSchema(), flippedIndices))) {
        boost::dynamic_bitset<> indices = node.getColumnIndices();

        indices[columnIndex] = true;
        Vertical subsetNode = Vertical(node.getSchema(), indices);
        if (this->find(subsetNode) == this->end()) {
            uncheckedSupersets.insert(std::move(subsetNode));
        }
    }

    return uncheckedSupersets;
}


