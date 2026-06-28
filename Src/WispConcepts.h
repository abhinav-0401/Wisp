//
// Created by Abhinav Jha on 20/06/26.
//

#ifndef WISP_WISPCONCEPTS_H
#define WISP_WISPCONCEPTS_H

#include <cstdint>
#include <string>

namespace Wisp {

template <typename T>
concept AllowedLiteral = std::same_as<T, std::int32_t>
                        || std::same_as<T, float>
                        || std::same_as<T, std::string>
                        || std::same_as<T, bool>;

}   // namespace Wisp

#endif //WISP_WISPCONCEPTS_H
