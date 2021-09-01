#ifndef OALP_HASH_MAP_HPP
#define OALP_HASH_MAP_HPP

#include <vector>
#include <utility>

template<typename Key, typename Value>
class OALPHashMap {
public:
class Iterator {
    public:
    explicit Iterator(OALPHashMap* hashmap) {
        skipEmptyBuckets();
    }
    explicit Iterator(OALPHashMap* hashmap, std::size_t pos) 
        : index_(pos) 
    {}
    void makeIndexEnd() {
        index_ = buckets_.size();
    }
    bool operator==(const Iterator& rhs) const {
        return rhs.index_ == index_;
    }
    bool operator!=(const Iterator& rhs) const {
        return !(rhs == *this);
    }
    std::pair<Key, Value>* operator->() const {
        return &buckets_[index_];
    }
    std::pair<Key, Value>& operator*() const {
        return buckets_[index_];
    }
    Iterator& operator++() {
        ++index_;
        skipEmptyBuckets();
        return *this;
    }
    std::size_t getIndex(){return index_;}
    private:
    void skipEmptyBuckets() {
        while (buckets_[index_].first == empty_key_) {
            if (index_ > buckets_.size()) break;
            ++index_;
        }
    }
    std::size_t index_;
};

OALPHashMap(std::size_t initial_buckets) {
    std::size_t factor_of_two = 1; // allows faster bitwise masking as opposed to modulus hashing
    while(factor_of_two < initial_buckets)
        factor_of_two <<= 1; // *=2
    buckets_.resize(factor_of_two, std::make_pair<Key, Value>(empty_key_, Value()));
}
OALPHashMap(const OALPHashMap& rhs, std::size_t initial_buckets)
    : OALPHashMap(initial_buckets) {
    for (auto itr = rhs.begin(); itr != rhs.end(); ++itr)
        insert(*itr);
}

Iterator begin() {
    return Iterator(this);
}

Iterator end() {
    return Iterator(this, size());
}

std::size_t size() const {
    return map_size_;
}

std::pair<Iterator, bool> insert(value& val) {
    return emplace(value.first, value.second);
}

template<typename... Args>
std::pair<Iterator, bool> emplace(Key key, Args&&... args) { //https://en.wikipedia.org/wiki/Linear_probing#Insertion
    if ((map_size_ + 1) * 2 > buckets_.size()) { //50% load factor threshold
        
    }
    std::size_t bucket_index = getHashIndex(key);
    for(;;) {
        if (buckets_[bucket_index].first == empty_key_) {
            buckets_[bucket_index].first = key;
            buckets_[bucket_index].second = Value(std::forward<Args>(args)...);
            map_size_++;
            return std::make_pair(Iterator(this, bucket_index), true);
        }
        else if (buckets_[bucket_index].first == key)
            return std::make_pair(Iterator(this, bucket_index), false);
        else
            bucket_index = iterateLinearProbe(bucket_index);
    }
}

void reserve(std::size_t next_size) {
    if (next_size * 2 > buckets_.size()) {//50% load-factor
        OALPHashMap temp(*this, buckets_.size() * 2);
        swap(temp);
    }
}
 
void erase(Iterator itr) { //https://en.wikipedia.org/wiki/Linear_probing#Deletion
    std::size_t probe_index = iterateLinearProbe(itr.idx);
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

Iterator find(Key key) { //https://en.wikipedia.org/wiki/Linear_probing#Search
    std::size_t probe_index = getHashIndex(key);
    for (;;) {
        if (buckets_[probe_index].first == key)
            return Iterator(this, key);
        if (buckets_[probe_index].first == empty_key_)
            return end();
        probe_index = iterateLinearProbe(probe_index);
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
