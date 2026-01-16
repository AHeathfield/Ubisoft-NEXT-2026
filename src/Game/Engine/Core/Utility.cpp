#include "Utility.h"
#include <cstdlib>
#include <string>

#if BUILD_PLATFORM_APPLE
#include <SDL3/SDL_log.h>
#endif

namespace utility
{
    bool getRandomBool()
    {
        int num = rand() % 2;
        return (num == 0);
    }

    bool getTrueOneXChance(int x)
    {
        int num = rand() % x;
        return (num == 0);
    }

    bool isIntOdd(int num)
    {
        return ((num + 1) % 2 == 0);
    }

    bool isIntEven(int num)
    {
        return (num % 2 == 0);
    }

    
    void initVectorInt(std::vector<int>& array, int size, int value)
    {
        for (int i = 0; i < size; i++)
        {
            array.push_back(value);
        }
    }

    int getRandomIntFromZeroToX(int x)
    {
        return rand() % x;
    }

    void log(const std::string& message)
    {
#if BUILD_PLATFORM_APPLE
        SDL_Log("%s", message.c_str());
#endif
    }


    // TODO: doesn't work since it's SDL based
    //std::string getProjectPath(const std::string& file) {
    //    // const char* basePath = SDL_GetBasePath();
    //    std::string path = SDL_GetBasePath();

    //    path += file; // append relative asset path
    //    return path;
    //}
    std::string getIntVectorString(const std::vector<int>& v)
    {
        const int size = v.size();
        std::string result = "{";
        for (int i = 0; i < size; i++)
        {
            if (i + 1 < size)
            {
                result += std::to_string(v[i]) + ", ";
            }
            else
            {
                result += std::to_string(v[i]);
            }
        }
        result += "}";
        return result;
    }

    std::string getUInt32VectorString(const std::vector<uint32_t>& v)
    {
        const int size = v.size();
        std::string result = "{";
        for (int i = 0; i < size; i++)
        {
            if (i + 1 < size)
            {
                result += std::to_string(v[i]) + ", ";
            }
            else
            {
                result += std::to_string(v[i]);
            }
        }
        result += "}";
        return result;
    }

    float degToRad(float deg)
    {
        return (deg * 3.141592653589f / 180.f);
    }
    float radToDeg(float rad)
    {
        return (rad * 180.f / 3.141592653589f);
    }

}
