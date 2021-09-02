#ifndef OALP_HASH_MAP_HPP
#define OALP_HASH_MAP_HPP

#include <vector>
#include <utility>

template<typename Key, typename Value>
class OALPHashMap {
public:
template <typename MapType, typename BucketType>
class Iterator {
    public:
    explicit Iterator(MapType* hashmap) : hashmap_(hashmap) {
        skipEmptyBuckets();
    }
    explicit Iterator(MapType* hashmap, std::size_t pos) 
        : hashmap_(hashmap), index_(pos) 
    {}
    bool operator==(const Iterator& rhs) const {
        return rhs.index_ == index_;
    }
    bool operator!=(const Iterator& rhs) const {
        return !(rhs == *this);
    }
    BucketType* operator->() const {
        return &(hashmap_->buckets_[index_]);
    }
    BucketType& operator*() const {
        return hashmap_->buckets_[index_];
    }
    Iterator& operator++() {
        ++index_;
        skipEmptyBuckets();
        return *this;
    }
    std::size_t getIndex(){return index_;}
    private:
    void skipEmptyBuckets() {
        while (hashmap_->buckets_[index_].first == hashmap_->empty_key_) {
            if (index_ > hashmap_->buckets_.size()) break;
            ++index_;
        }
    }
    MapType* hashmap_;
    std::size_t index_;
};

OALPHashMap(std::size_t initial_buckets) {
    std::size_t factor_of_two = 1; // allows faster bitwise masking as opposed to modulus hashing
    while(factor_of_two < initial_buckets)
        factor_of_two <<= 1; // *=2
    buckets_.resize(factor_of_two, std::make_pair(empty_key_, Value()));
}
OALPHashMap(const OALPHashMap& rhs, std::size_t initial_buckets)
    : OALPHashMap(initial_buckets) {
    for (auto itr = rhs.begin(); itr != rhs.end(); ++itr)
        insert(*itr);
}

using Iter = Iterator<OALPHashMap, std::pair<Key, Value>>;

Iter begin() {
    return Iter(this);
}

Iter end() {
    return Iter(this, buckets_.size());
}

using ConstIterator = Iterator<const OALPHashMap, const std::pair<Key, Value>>;

ConstIterator end() const {
    return ConstIterator(this, buckets_.size());
}

ConstIterator begin() const {
    return ConstIterator(this, buckets_.size());
}

std::size_t size() const {
    return map_size_;
}

std::pair<Iter, bool> insert(const std::pair<Key, Value>& val) {
    return emplace(val.first, val.second);
}

std::pair<Iter, bool> insert(std::pair<Key, Value>&& val) {
    return emplace(val.first, std::move(val.second));
}

template<typename... Args>
std::pair<Iter, bool> emplace(Key key, Args&&... args) { //https://en.wikipedia.org/wiki/Linear_probing#Insertion
    checkIfReserveNeeded();
    std::size_t bucket_index = getHashIndex(key);
    for(;;) {
        if (buckets_[bucket_index].first == empty_key_) {
            buckets_[bucket_index].first = key;
            buckets_[bucket_index].second = Value(std::forward<Args>(args)...);
            map_size_++;
            return std::make_pair(Iter(this, bucket_index), true);
        }
        else if (buckets_[bucket_index].first == key)
            return std::make_pair(Iter(this, bucket_index), false);
        else
            bucket_index = iterateLinearProbe(bucket_index);
    }
}
 
void checkIfReserveNeeded() {
    reserve(map_size_ + 1);
}

void erase(Iter itr) { //https://en.wikipedia.org/wiki/Linear_probing#Deletion
    std::size_t probe_index = iterateLinearProbe(itr.getIndex());
    std::size_t erase_index = itr.getIndex();
    for (;;) {
        if (buckets_[probe_index].first == empty_key_) {
            buckets_[erase_index].first = empty_key_;
            --map_size_;
            return;
        }
        if (hashIsEarlier(erase_index, probe_index, buckets_[probe_index].first)) {
            buckets_[erase_index] = buckets_[probe_index];
            erase_index = probe_index;
        }
        probe_index = iterateLinearProbe(probe_index);
    }
}

Iter find(Key key) { //https://en.wikipedia.org/wiki/Linear_probing#Search
    std::size_t probe_index = getHashIndex(key);
    for (;;) {
        if (buckets_[probe_index].first == key)
            return Iter(this, key);
        if (buckets_[probe_index].first == empty_key_)
            return end();
        probe_index = iterateLinearProbe(probe_index);
    }   
}

void reserve(std::size_t next_size) {
    if (next_size * 2 > buckets_.size()) { //50% load-factor
        OALPHashMap temp(*this, buckets_.size() * 2);
        swap(temp);
    }
}

private:
    struct Hash {
        size_t operator()(uint64_t h) const noexcept {
            h ^= h >> 33;
            h *= 0xff51afd7ed558ccd;
            h ^= h >> 33;
            h *= 0xc4ceb9fe1a85ec53;
            h ^= h >> 33;
            return h;
        }
    };
    Hash hash_func_;
    void swap(OALPHashMap& swap_from) {
        std::swap(buckets_, swap_from.buckets_);
        std::swap(map_size_, swap_from.map_size_);
        std::swap(empty_key_, swap_from.empty_key_);
    }
    bool hashIsEarlier(std::size_t erase_index, std::size_t probe_index, Key probe_key) {
        std::size_t hash_index = getHashIndex(probe_key);
        std::size_t hidx_edist = (buckets_.size() + (erase_index - hash_index)) & (buckets_.size() - 1);
        std::size_t hidx_pdist = (buckets_.size() + (probe_index - hash_index)) & (buckets_.size() - 1);
        return hidx_edist < hidx_pdist;
    }
    std::size_t getHashIndex(Key k) {
        return hash_func_(k) & (buckets_.size() - 1);
    }
    std::size_t iterateLinearProbe(std::size_t bucket_index) {
        return (bucket_index + 1) & (buckets_.size() - 1);
    }
    
    std::vector<std::pair<Key, Value>> buckets_;
    Key empty_key_ = 0;
    std::size_t map_size_;
};

#endif
