#include "ComponentColumn.h"
#include <string>
namespace ECS {

std::string ComponentColumn::to_s(int until) const
{
        std::stringstream s;
        s << "Col size: " << element_size << " [";

        for (int j = 0; j < until; j++) {
                s << "{ ";
                for (unsigned i = 0; i < (element_size); i++) {
                        s << (int)((char*)data)[element_size * j + i] << ", ";
                }
                s << "}, ";
        }

        s << "]";
        return s.str();
}

}  // namespace ECS
