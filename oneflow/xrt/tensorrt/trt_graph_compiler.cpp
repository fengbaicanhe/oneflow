#include "oneflow/xrt/node_util.h"
#include "oneflow/xrt/tensorrt/trt_graph_compiler.h"

namespace oneflow {
namespace xrt {
namespace tensorrt {

void TrtGraphCompiler::SetupKernelContextParam(
    const XrtNode *node, OpKernelContext::Param *context_param) {
  util::Map<Argument, TrtValue> input_ops;
  util::Map<std::string /* produce/consume key */, Argument> input_output_args;
  for (const XrtEdge *edge : node->in_edges()) {
    if (!edge->IsControlEdge()) {
      const Argument &arg = edge->argument();
      CHECK_GT(operands_.count(arg), 0);
      const Operand &operand = operands_.at(arg);
      input_ops.emplace(arg, operand);
      const std::string &k = arg.meta_data().consume_key;
      input_output_args.emplace(k, arg);
    }
  }
  for (const XrtEdge *edge : node->out_edges()) {
    if (!edge->IsControlEdge()) {
      const Argument &arg = edge->argument();
      const std::string &k = arg.meta_data().produce_key;
      input_output_args.emplace(k, arg);
    }
  }

  size_t num_outputs = input_output_args.size() - input_ops.size();
  CHECK_GE(num_outputs, 0) << "Outputs number should >= 0.";
  context_param->builder = builder_.get();
  context_param->message = OpMessage(node);
  context_param->arguments = std::move(input_output_args);
  context_param->inputs = std::move(input_ops);
  context_param->num_outputs = num_outputs;
}

std::shared_ptr<Executable> TrtGraphCompiler::Compile(
      const XrtGraph *graph, const std::vector<Parameter> &entry_params,
      const std::vector<Parameter> &return_params,
      const std::vector<InputOutputAlias> &aliases) {
  algorithm::TopologyVisit(*graph, [&](const XrtNode *node) {
    OpKernelContext::Param param;
    SetupKernelContextParam(node, &param);
    OpKernelContext context(param);
    // Do compile
    auto op_kernel = BuildOpKernel(node->type());
    op_kernel->Compile(&context);

    // Always insert the new output into `operands_`.
    const auto &outputs = op_context.outputs();
    for (auto it = outputs.begin(); it != outputs.end(); ++it) {
      operands_[it->first] = it->second;
    }
  }

  return std::make_shared<TrtExecutable>(builder_->buildCudaEngine());
}

}  // namespace tensorrt
}  // namespace xrt
}  // namespace oneflow
