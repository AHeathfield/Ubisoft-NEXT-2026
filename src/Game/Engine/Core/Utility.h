#pragma once

#include <functional>
#include <algorithm>
#include <string>
#include <vector>
#include <iterator>
#include <cmath>
#include <cfloat>
#include <unordered_set>
#include <unordered_map>

// Got from cookbook textbook I prefer just having a function rather than a macro but this is fine to
// This is new and some methods may not use this
// used for comparing floats
#define CMP_FLOAT(x, y) \
    (fabsf((x)-(y)) <= FLT_EPSILON * \
    fmaxf(1.0f, \
    fmaxf(fabsf(x), fabsf(y))) \
    )


// A namespace of functions that are not bound to any class but are useful that I have made across other projects
namespace utility
{
    bool getRandomBool();
    bool getTrueOneXChance(int x);
    bool isIntOdd(int num);
    bool isIntEven(int num);

    float degToRad(float deg);
    float radToDeg(float rad);

    float moveToward(const float from, const float to, const float delta);

    // Removes an element from a vector if it exists and shifts other indexes appropriately
    template <typename T>
    void vectorRemoveElement(std::vector<T>& v, const T& element)
    {
        auto it = std::find(v.begin(), v.end(), element);
        if (it != v.end())
        {
            v.erase(it);
        }
    }

    // Move an element in vector efficiently O(n): https://stackoverflow.com/questions/45447361/how-to-move-certain-elements-of-stdvector-to-a-new-index-within-the-vector
    template <typename T>
    bool vectorMoveElement(std::vector<T>& v, int oldIndex, int newIndex)
    {
        if (newIndex >= v.size() || newIndex < 0)
        {
            return false;
        }

        if (oldIndex > newIndex)
            std::rotate(v.rend() - oldIndex - 1, v.rend() - oldIndex, v.rend() - newIndex);
        else
            std::rotate(v.begin() + oldIndex, v.begin() + oldIndex + 1, v.begin() + newIndex + 1);

        return true;
    }


    template <typename T, typename Value>
    int vectorGetIndexOfValue(const std::vector<T>& v, const Value& value)
    {
        auto it = std::find(v.begin(), v.end(), value);
        if (it == v.end())
        {
            return -1;
        }
        return static_cast<int>(it - v.begin());
    }


    void initVectorInt(std::vector<int>& array, int size, int value);


    template <typename Key, typename Value>
    bool hasKey(const std::unordered_map<Key, Value>& map, const Key& key)
    {
        return map.find(key) != map.end();
    }

    template <typename T>
    bool containsValue(const std::unordered_set<T>& set, const T& value)
    {
        auto it = set.find(value);
        return it != set.end();
    }

    template <typename Key, typename Value>
    bool containsValue(const std::unordered_map<Key, Value>& map, const Value& value)
    {
        auto it = map.find(value);
        return it != map.end();
    }

    template <typename Value>
    bool containsValue(const std::vector<Value>& vector, const Value& value)
    {
        return vectorGetIndexOfValue(vector, value) != -1;
    }

    
    std::string getIntVectorString(const std::vector<int>& v);
    std::string getUInt32VectorString(const std::vector<uint32_t>& v);

    int getRandomIntFromZeroToX(int x);

    void log(const std::string& message);

    // TODO: Not sure if this will be useful
    //std::string getProjectPath(const std::string& file);
}
