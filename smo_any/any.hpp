#ifndef FILE_098CE0E5_C5F2_4F9C_805D_79D10F087671_H
#define FILE_098CE0E5_C5F2_4F9C_805D_79D10F087671_H
#include "smo_any/basic_any.hpp"

namespace smo_any {

template <class... Concept>
using any = basic_any<any_store, Concept...>;

}  // namespace smo_any
#endif // FILE_098CE0E5_C5F2_4F9C_805D_79D10F087671_H
