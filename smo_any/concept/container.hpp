#ifndef FILE_23C9C6A9_4F2E_4D76_89D5_816DF6E38C54_H
#define FILE_23C9C6A9_4F2E_4D76_89D5_816DF6E38C54_H
#include "smo_any/concept/iterator.hpp"

namespace smo_any {
SMO_ANY_MEMBER(concept_push_back, push_back, false);
SMO_ANY_MEMBER(concept_insert, insert, false);

SMO_ANY_MEMBER(concept_const_begin, begin, true);
SMO_ANY_MEMBER(concept_const_end, end, true);
SMO_ANY_MEMBER(concept_begin, begin, false);
SMO_ANY_MEMBER(concept_end, end, false);
SMO_ANY_METHOD(concept_const_free_begin, begin, true);
SMO_ANY_METHOD(concept_const_free_end, end, true);
SMO_ANY_METHOD(concept_free_begin, begin, false);
SMO_ANY_METHOD(concept_free_end, end, false);

template <class V, class R = const V&>
struct concept_range
    : concepts<concept_const_begin<any_forward_iterator<V, R>(void)>,
               concept_const_end<any_forward_iterator<V, R>(void)>> {};

template <class V, class R = const V&>
using any_range = any<concept_range<V, R>>;

}  // namespace smo_any
#endif // FILE_23C9C6A9_4F2E_4D76_89D5_816DF6E38C54_H
