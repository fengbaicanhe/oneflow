/*
Copyright 2020 The OneFlow Authors. All rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#include "oneflow/core/framework/framework.h"

namespace oneflow {

REGISTER_USER_OP("tril")
    .Input("in")
    .Output("out")
    .Attr("diagonal", UserOpAttrType::kAtInt64)
    .SetTensorDescInferFn([](user_op::InferContext* ctx) -> Maybe<void> {
      const user_op::TensorDesc* in = ctx->TensorDesc4ArgNameAndIndex("in", 0);
      user_op::TensorDesc* out = ctx->TensorDesc4ArgNameAndIndex("out", 0);
      int64_t diagonal = ctx->Attr<int64_t>("diagonal");
      size_t num_axes = in->shape().NumAxes();
      CHECK_GE_OR_RETURN(in->shape().NumAxes(), 2);
      CHECK_LE_OR_RETURN(diagonal,
                         std::min(in->shape().At(num_axes - 1), in->shape().At(num_axes - 2)));
      CHECK_GE_OR_RETURN(diagonal,
                         -1 * std::min(in->shape().At(num_axes - 1), in->shape().At(num_axes - 2)));
      *out = *in;
      if (in->is_dynamic()) { *out->mut_is_dynamic() = true; }
      return Maybe<void>::Ok();
    })
    .SetBatchAxisInferFn([](user_op::BatchAxisContext* ctx) -> Maybe<void> {
      *ctx->BatchAxis4ArgNameAndIndex("out", 0) = *ctx->BatchAxis4ArgNameAndIndex("in", 0);
      return Maybe<void>::Ok();
    })
    .SetGetSbpFn([](user_op::SbpContext* ctx) -> Maybe<void> {
      const user_op::TensorDesc& in = ctx->LogicalTensorDesc4InputArgNameAndIndex("in", 0);
      FOR_RANGE(int64_t, i, 0, in.shape().NumAxes()) {
        ctx->NewBuilder().Split(ctx->inputs(), i).Split(ctx->outputs(), i).Build();
      }
      ctx->NewBuilder()
          .PartialSum(user_op::OpArg("in", 0))
          .PartialSum(user_op::OpArg("out", 0))
          .Build();
      return Maybe<void>::Ok();
    });

REGISTER_USER_OP("tril_grad")
    .Input("dy")
    .Output("dx")
    .Attr("diagonal", UserOpAttrType::kAtInt64)
    .SetTensorDescInferFn([](user_op::InferContext* ctx) -> Maybe<void> {
      const user_op::TensorDesc* dy = ctx->TensorDesc4ArgNameAndIndex("dy", 0);
      user_op::TensorDesc* dx = ctx->TensorDesc4ArgNameAndIndex("dx", 0);
      int64_t diagonal = ctx->Attr<int64_t>("diagonal");
      size_t num_axes = dy->shape().NumAxes();
      CHECK_GE_OR_RETURN(dy->shape().NumAxes(), 2);
      CHECK_LE_OR_RETURN(diagonal,
                         std::min(dy->shape().At(num_axes - 1), dy->shape().At(num_axes - 2)));
      CHECK_GE_OR_RETURN(diagonal,
                         -1 * std::min(dy->shape().At(num_axes - 1), dy->shape().At(num_axes - 2)));
      *dx = *dy;
      if (dy->is_dynamic()) { *dx->mut_is_dynamic() = true; }
      return Maybe<void>::Ok();
    })
    .SetBatchAxisInferFn([](user_op::BatchAxisContext* ctx) -> Maybe<void> {
      *ctx->BatchAxis4ArgNameAndIndex("dx", 0) = *ctx->BatchAxis4ArgNameAndIndex("dy", 0);
      return Maybe<void>::Ok();
    })
    .SetGetSbpFn([](user_op::SbpContext* ctx) -> Maybe<void> {
      const user_op::TensorDesc& dy = ctx->LogicalTensorDesc4InputArgNameAndIndex("dy", 0);
      FOR_RANGE(int64_t, i, 0, dy.shape().NumAxes()) {
        ctx->NewBuilder().Split(ctx->inputs(), i).Split(ctx->outputs(), i).Build();
      }
      ctx->NewBuilder()
          .PartialSum(user_op::OpArg("dy", 0))
          .PartialSum(user_op::OpArg("dx", 0))
          .Build();
      return Maybe<void>::Ok();
    });

REGISTER_USER_OP_GRAD("tril").SetGenBackwardOpConfFn([](const user_op::UserOpWrapper& op,
                                                        user_op::AddOpFn AddOp) {
  if (op.NeedGenGradTensor4OpInput("in", 0)) {
    user_op::UserOpConfWrapperBuilder builder(op.op_name() + "_grad");
    user_op::UserOpConfWrapper grad_op = builder.Op("tril_grad")
                                             .Input("dy", op.GetGradTensorWithOpOutput("out", 0))
                                             .Output("dx")
                                             .Attr("diagonal", op.attr<int64_t>("diagonal"))
                                             .Build();
    op.BindGradTensorWithOpInput(grad_op.output("dx", 0), "in", 0);
    AddOp(grad_op);
  }
});

}  // namespace oneflow
