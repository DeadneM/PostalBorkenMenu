#pragma once
#include <cstdio>
#include <cstddef>
inline void FormatLevelTimer(unsigned int milliseconds,char* output,size_t capacity){
    std::snprintf(output,capacity,"%02u:%02u.%02u",milliseconds/60000,(milliseconds/1000)%60,(milliseconds/10)%100);
}
