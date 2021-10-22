#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>

template<typename T> std::ostream& operator<<(std::ostream &os, const std::vector<T> &v){  
    std::copy(v.begin(), v.end(), std::ostream_iterator<T>(os, " "));
    return os;
}


#endif