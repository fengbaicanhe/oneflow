#ifndef ONEFLOW_CORE_COMMON_PREPROCESSOR_H_
#define ONEFLOW_CORE_COMMON_PREPROCESSOR_H_

#include "oneflow/core/common/preprocessor_internal.h"

#define OF_PP_CAT OF_PP_INTERNAL_CAT

#define OF_PP_JOIN OF_PP_INTERNAL_JOIN

#define OF_PP_STRINGIZE OF_PP_INTERNAL_STRINGIZE

#define OF_PP_MAKE_TUPLE_SEQ OF_PP_INTERNAL_MAKE_TUPLE_SEQ

#define OF_PP_FOR_EACH_TUPLE OF_PP_INTERNAL_FOR_EACH_TUPLE

#define OF_PP_SEQ_PRODUCT_FOR_EACH_TUPLE \
  OF_PP_INTERNAL_SEQ_PRODUCT_FOR_EACH_TUPLE

#define OF_PP_PAIR_FIRST OF_PP_INTERNAL_PAIR_FIRST
#define OF_PP_PAIR_SECOND OF_PP_INTERNAL_PAIR_SECOND

#endif  // ONEFLOW_CORE_COMMON_PREPROCESSOR_H_
