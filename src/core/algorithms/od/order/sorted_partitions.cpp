#include "sorted_partitions.h"

#include <future>
#include <unordered_map>
#include <iostream>
#include <vector>

#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/thread.hpp>

#include "model/table/tuple_index.h"

namespace algos::order {

void SortedPartition::BuildHashTable() {
    hash_partition_.reserve(num_rows_);
    for (PartitionIndex i = 0; i < sorted_partition_.size(); ++i) {
        if (sorted_partition_[i].size() == 1) {
            continue;
        }
        for (model::TupleIndex tuple_index : sorted_partition_[i]) {
            hash_partition_.emplace(tuple_index, i);
        }
    }
}

SortedPartition::HashProduct SortedPartition::BuildHashProduct(
        SortedPartition::EquivalenceClasses::const_iterator begin,
        SortedPartition::EquivalenceClasses::const_iterator end) {
    HashProduct hash_product;
    hash_product.reserve(hash_partition_.size());
    for (auto it = begin; it != end; ++it) {
        /*if (end - begin <= 1) {
            break;
        }*/
        std::unordered_set<PartitionIndex> visited_positions;
        for (model::TupleIndex tuple_index : *it) {
            auto it = hash_partition_.find(tuple_index);
            if (it == hash_partition_.end()) {
                continue;
            }
            PartitionIndex position = it->second;
            visited_positions.insert(position);
            auto pos_it = hash_product.find(position);
            if (pos_it == hash_product.end()) {
                pos_it = hash_product.emplace(position, EquivalenceClasses{{}}).first;
            }
            pos_it->second.back().insert(tuple_index);
        }
        for (PartitionIndex position : visited_positions) {
            hash_product[position].push_back({});
        }
    }
    return hash_product;
}

void SortedPartition::Intersect(SortedPartition const& other) {
    BuildHashTable();
    HashProduct hash_product = BuildHashProduct(other.sorted_partition_.begin(), other.sorted_partition_.end());
    SortedPartition res(num_rows_);
    res.sorted_partition_.reserve(num_rows_);
    for (std::size_t i = 0; i < sorted_partition_.size(); ++i) {
        if (EquivalenceClass const& eq_class = sorted_partition_[i]; eq_class.size() == 1) {
            res.sorted_partition_.push_back(eq_class);
        } else {
            for (EquivalenceClass& eq_class : hash_product[i]) {
                if (!eq_class.empty()) {
                    res.sorted_partition_.push_back(std::move(eq_class));
                }
            }
        }
    }
    res.sorted_partition_.shrink_to_fit();
    *this = std::move(res);
}

void PrintSortedPartitions(SortedPartition const& p) {
    for (SortedPartition::EquivalenceClass eq_class : p.GetEqClasses()) {
        std::cout << "{"; 
        for (auto it = eq_class.begin(); it != eq_class.end(); ++ it) {
            std::cout << *it;
            if (std::next(it) != eq_class.end()) {
                std::cout << ", ";
            }
        }
        std::cout << "} ";
    }
    std::cout << '\n';
}

void SortedPartition::IntersectParallel(SortedPartition const& other,
                                        config::ThreadNumType thread_num) {
    BuildHashTable();

    boost::asio::thread_pool pool(thread_num);
    std::vector<std::future<HashProduct>> futures;
    unsigned long chunk_size = num_rows_ / thread_num + 1;
    unsigned long current_chunk_size = 0;
    auto chunk_begin = other.sorted_partition_.begin();
    for (auto eq_class_it = other.sorted_partition_.begin();
         eq_class_it != other.sorted_partition_.end(); ++eq_class_it) {
        current_chunk_size += eq_class_it->size();
        if (current_chunk_size > chunk_size){
            //std::cout << eq_class_it - other.sorted_partition_.begin() << '\n';
            current_chunk_size = 0;
            std::packaged_task<HashProduct()> task([this, chunk_begin, eq_class_it]() {
                return BuildHashProduct(chunk_begin, eq_class_it);
            });
            futures.push_back(task.get_future());
            boost::asio::post(pool, std::move(task));
            chunk_begin = eq_class_it;
        }
    }
    auto end = other.sorted_partition_.end();
    std::packaged_task<HashProduct()> task([this, chunk_begin, end]() {
        return BuildHashProduct(chunk_begin, end);
    });
    futures.push_back(task.get_future());
    boost::asio::post(pool, std::move(task));

    pool.join();

    std::vector<HashProduct> results;
    for (auto& future : futures) {
        results.push_back(future.get());
    }

    SortedPartition res(num_rows_);
    res.sorted_partition_.reserve(num_rows_);
    for (std::size_t i = 0; i < sorted_partition_.size(); ++i) {
        if (EquivalenceClass const& eq_class = sorted_partition_[i]; eq_class.size() == 1) {
            res.sorted_partition_.push_back(eq_class);
        } else {
            for (HashProduct& hash_product : results){
                auto eq_classes_it = hash_product.find(i);
                if (eq_classes_it == hash_product.end()) {
                    continue;
                }
                for (EquivalenceClass& eq_class : eq_classes_it->second) {
                    if (!eq_class.empty()) {
                        res.sorted_partition_.push_back(std::move(eq_class));
                    }
                }
            }
        }
    }
    //std::cout << '\n';
    //PrintSortedPartitions(*this);
    //PrintSortedPartitions(other);
    //PrintSortedPartitions(res);
    res.sorted_partition_.shrink_to_fit();
    *this = std::move(res);
}

}  // namespace algos::order
