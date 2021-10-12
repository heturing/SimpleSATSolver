#include "variable.h"
#include <algorithm>
#include <vector>

size_t Variable::index = 0;

std::ostream& operator<<(std::ostream &os, const Variable &v){
    os << v.name;
    return os;
}
