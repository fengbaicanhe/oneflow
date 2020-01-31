#include "oneflow/core/common/util.h"
#include "oneflow/core/common/flat_msg.h"

namespace oneflow {

namespace {

template<int field_counter, typename WalkCtxType, typename FieldType>
struct DumpFieldName {
  static void Call(WalkCtxType* ctx, FieldType* field, const char* field_name) {
    ctx->push_back(field_name);
  }
};

template<typename T>
std::vector<std::string> GetFieldNames(T* flat_msg) {
  std::vector<std::string> field_names;
  flat_msg->template __WalkField__<DumpFieldName>(&field_names);
  return field_names;
}

template<typename T>
void CheckSoleFieldName(T* flat_msg, const std::string& expected) {
  const auto& field_names = GetFieldNames(flat_msg);
  ASSERT_EQ(field_names.size(), 1);
  ASSERT_EQ(field_names.at(0), expected);
}
// clang-format off
BEGIN_FLAT_MSG(TestOptional)
  FLAT_MSG_DEFINE_OPTIONAL(int32_t, bar);
END_FLAT_MSG(TestOptional)
// clang-format on

TEST(FlatMsg, optional) {
  FLAT_MSG(TestOptional) foo_box;
  auto& foo = *foo_box.Mutable();
  ASSERT_TRUE(!foo.has_bar());
  ASSERT_EQ(foo.bar(), 0);
  ASSERT_TRUE(GetFieldNames(&foo).empty());
  *foo.mutable_bar() = 9527;
  ASSERT_TRUE(foo.has_bar());
  ASSERT_EQ(foo.bar(), 9527);
  auto field_names = GetFieldNames(&foo);
  ASSERT_EQ(field_names.size(), 1);
  ASSERT_EQ(field_names.at(0), "bar_");
}

// clang-format off
BEGIN_FLAT_MSG(FooOneof)
  FLAT_MSG_DEFINE_ONEOF(type,
      FLAT_MSG_ONEOF_FIELD(int32_t, case_0)
      FLAT_MSG_ONEOF_FIELD(int64_t, case_1)
      FLAT_MSG_ONEOF_FIELD(TestOptional, bar));
END_FLAT_MSG(FooOneof)
// clang-format on

TEST(FlatMsg, oneof) {
  FLAT_MSG(FooOneof) foo_box;
  auto& foo = *foo_box.Mutable();
  ASSERT_TRUE(GetFieldNames(&foo).empty());
  ASSERT_TRUE(!foo.has_bar());
  ASSERT_EQ(foo.bar().bar(), 0);
  foo.mutable_case_0();
  CheckSoleFieldName(&foo, "case_0_");
  ASSERT_TRUE(foo.has_case_0());
  FLAT_MSG_ONEOF_ENUM_TYPE(FooOneof, type) x = foo.type_case();
  ASSERT_TRUE(x == FLAT_MSG_ONEOF_ENUM_VALUE(FooOneof, case_0));
  *foo.mutable_case_1() = 9527;
  CheckSoleFieldName(&foo, "case_1_");
  ASSERT_TRUE(foo.has_case_1());
  ASSERT_EQ(foo.case_1(), 9527);
}

// clang-format off
BEGIN_FLAT_MSG(FooRepeated)
  FLAT_MSG_DEFINE_REPEATED(char, char_field, 1);
  FLAT_MSG_DEFINE_REPEATED(TestOptional, bar, 10);
END_FLAT_MSG(FooRepeated)
// clang-format on

TEST(FlatMsg, repeated) {
  FLAT_MSG(FooRepeated) foo_box;
  auto& foo = *foo_box.Mutable();
  ASSERT_EQ(foo.bar_size(), 0);
  ASSERT_EQ(foo.bar().size(), 0);
  auto* bar = foo.mutable_bar()->Add();
  ASSERT_TRUE(!bar->has_bar());
  ASSERT_EQ(foo.bar_size(), 1);
  ASSERT_EQ(foo.bar().size(), 1);
  bar->set_bar(9527);
  ASSERT_TRUE(bar->has_bar());
  ASSERT_EQ(bar->bar(), 9527);
  bar = foo.mutable_bar()->Add();
  ASSERT_TRUE(!bar->has_bar());
  ASSERT_EQ(foo.bar_size(), 2);
  ASSERT_EQ(foo.bar().size(), 2);
  bar->set_bar(9528);
  for (const auto& x : foo.bar()) { ASSERT_TRUE(x.has_bar()); }
  foo.clear_bar();
  ASSERT_EQ(foo.bar_size(), 0);
}

}  // namespace

}  // namespace oneflow