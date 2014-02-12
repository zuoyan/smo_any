#ifndef FILE_0B404C23_FD6C_4120_B8CA_9AAC2A15DBF8_H
#define FILE_0B404C23_FD6C_4120_B8CA_9AAC2A15DBF8_H
#include "smo_any/types.hpp"
#include "smo_any/concept/equal.hpp"
#include "smo_any/concept/binary_op.hpp"

namespace smo_any {

SMO_ANY_BINARY_OP(concept_operator_lt, <, const);
SMO_ANY_BINARY_OP(concept_operator_le, <=, const);
SMO_ANY_BINARY_OP(concept_operator_gt, >, const);
SMO_ANY_BINARY_OP(concept_operator_ge, >=, const);

struct concept_compare
    : smo_any::concepts<concept_operator_lt<bool>, concept_operator_le<bool>,
                        concept_operator_gt<bool>, concept_operator_ge<bool> > {
};

}  // namespace smo_any
#endif // FILE_0B404C23_FD6C_4120_B8CA_9AAC2A15DBF8_H
