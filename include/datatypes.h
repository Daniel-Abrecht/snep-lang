#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef struct Identifier Identifier;
typedef struct DataType DataType;
typedef struct Field Field;
typedef struct Structure Structure;
typedef struct Struct Struct;
typedef struct Union Union;
typedef struct Integer Integer;
typedef struct Float Float;
typedef struct EnumMember EnumMember;
typedef struct Enum Enum;
typedef struct Pointer Pointer;
typedef struct Array Array;
typedef struct Function Function;
typedef struct Variable Variable;
typedef struct Expression Expression;

typedef size_t bit_size_t;

enum DataTypeEnum {
  STRUCT = 1,
  UNION,
  FLOAT,
  INTEGER,
  ENUM,
  POINTER,
  ARRAY,
  FUNCTION
};

struct Identifier {
  size_t size;
  const void* value;
};

struct DataType {
  enum DataTypeEnum type;
  size_t size;
  Identifier name;
  uint8_t lbalignment;
  bool is_type_constexpr; // If true, the size of the data type is known at compile time. Else, it is only known at runtime
  bool is_data_constexpr; // The data the type is known at compile time
  bool is_immutable; // Nothing can change the data
  bool is_readonly;  // The data can change, but not through this type
  bool is_restrict;
  bool is_volatile;
  bool is_atomic;
  bool is_native; // The CPU has opcodes for dealing with such types
};

struct Structure {
  DataType type;
  size_t field_count;
  const Field* field_list;
};

struct Field {
  Identifier name;
  bit_size_t offset;
  const DataType*const* type;
};

struct Struct {
  Structure structure;
};

struct Union {
  Structure structure;
};

enum OverflowMode {
  OVM_UNDEFINED_BEHAVIOUR,
  OVM_WRAP_AROUND,
  OVM_SATURATE,
  OVM_EXCEPTION
};

struct Integer {
  DataType type;
  bool is_unsigned;
  enum OverflowMode overflow : 8;
  size_t bit_size;
};

struct Float {
  DataType type;
  bool is_unsigned;
  bool has_subnormals;
  bool has_NaNs;
  bool has_infinity;
  size_t exponent_bit_size;
  size_t fraction_bit_size;
};

struct EnumMember {
  const Enum* enumeration;
  Identifier name;
  void* value;
};

struct Enum {
  DataType type;
  const DataType* member_type;
  size_t member_count;
  struct EnumMember member_list;  
};

struct Pointer {
  DataType type;
  DataType* parent;
};

struct Array {
  DataType type;
  const DataType* member_type;
  const Expression* start;
  const Expression* end;
};

struct Variable {
  const DataType* type;
  Identifier name;
};

struct Function {
  DataType type;
  size_t parameter_count;
  const Variable* parameter_list;
};
