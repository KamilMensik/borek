#include <string>
#include <sstream>

#include "ComponentColumn.h"

namespace ECS {

std::string ComponentColumn::to_s(int until) const
{
        std::stringstream s;
        s << "Col size: " << element_size << " [";

        for (int j = 0; j < until; j++) {
                s << "{ " << to_s_internal(&((char*)data)[element_size * j]) << "}, ";
        }

        s << "]";
        return s.str();
}

}  // namespace ECS
