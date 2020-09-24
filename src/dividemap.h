/**
 * Name: Max Pike
 * --------------
 * DivideMap
 * --------------
 * This class overlays a general map by allowing for the user to create a subset within a map
 * while also maintaining the original map.  This is especially useful for a program
 * that is refining a search, but also needs access to the information stored within
 * the original map.  It achieves this by storing the subset as a set<string> instead of two different
 * maps that continually change.
 */

#ifndef _dividemap_
#define _dividemap_

#include "hashmap.h"
#include "set.h"
#include "error.h"

template <typename ValueType>
class DivideMap {

public:

    /**
     * DivideMap::DivideMap()
     */
    DivideMap() {}

    /**
     * DivideMap::~DivideMap()
     */
    ~DivideMap() {}

    /**
     * DivideMap::put
     * --------------
     * Puts the new key value pair in the map, as well as the set "submap"
     */
    void put(const std::string& str, ValueType& answer) {
        map[str] = answer;
        submap += str;
    }

    /**
     * DivideMap::get
     * --------------
     * Returns the value associated with the key.  If the key
     * is not present it will throw an error exception.
     */
    ValueType get(const std::string& str) {
        if(!map.containsKey(str)) error("Map does not contain key");
        return map[str];
    }

    /**
     * DivideMap::containsKey
     * ----------------------
     * Returns a bool indicating whether or not a key is contained in the map.
     */
    bool containsKey(const std::string& str) {
        return map.containsKey(str);
    }

    /**
     * DivideMap::refineSubset
     * ----------------------
     * Removes the key from the set, so as to create a subset map that is more refined.
     */
    void refineSubset(const std::string& str) {
        if(submap.contains(str)) submap -= str;
    }

    /**
     * DivideMap::subMapGet
     * --------------------
     * Returns a key within the subset map.  If it is not contained within
     * the submap, it will throw an error exception.
     */
    ValueType subMapGet(const std::string& str) {
        if(!submap.contains(str)) error("Map does not contain key");
        return map[str];
    }

    /**
     * DivideMap::subMapContainsKey
     * ----------------------------
     * Returns whether or not a key is contained within the subset map.
     */
    bool subMapContainsKey(const std::string& str) {
        return submap.contains(str);
    }

    /**
     * DivideMap::size
     * ---------------
     * Returns the size of the entire map.
     */
    int size() {
        return map.size();
    }

    /**
     * DivideMap::subMapSize
     * ---------------------
     * Returns the size of the subset map.
     */
    int subMapSize() {
        return submap.size();
    }

    /**
     * DivideMap::getMapKeys
     * ---------------------
     * Returns the keys in the entire Map as a set<string>
     */
    Set<std::string> getMapKeys() {
        Set<std::string> returnSet;
        for(std::string str: map)
            returnSet += str;
        return returnSet;
    }

    /**
     * DivideMap::getSubMapKeys
     * ------------------------
     * Returns the keys of the subset map as a set<string>
     */
    Set<std::string> getSubMapKeys() {
        return submap;
    }

    /**
     * DivideMap::operator []
     * ----------------------
     * Implements the operator which allows the value to returned by reference.
     */
    ValueType& operator[](const std::string& str){
        return map[str];
    }

private:
    HashMap<std::string, ValueType> map; // the larger map
    Set<std::string> submap; // the subset map stored as a set of keys
};

#endif
