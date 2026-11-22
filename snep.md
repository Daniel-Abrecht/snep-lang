# Snep

Snep is a statically typed compiled language inspired by C. It is modular, in such a way as it separates interfaces of program components from their implementations.
It aims to have minimal syntax and be usable in embedded contexts. It follows the tradition of C in trusting the programmer, it won't get in the way with strange
constraints one has to follow, nor will it come with garbage collection. Unlike C, there won't be any UB nor any IB, compilers are not to do whatever they please
for insane reasons, if something remains unclear, this document will be extended to clarify what the sane thing to do is.

Snep has some very high level concepts, such as first-class types, which allow for reflection, and other meta programming stuff.
However, it also rethinks abstractions which are too "high level", such as functions and variables.

A snep source code file must be encoded in UTF-8.


# Modules

In snep, the interface of a module, and it's implementation, are strictly split. They get their own dedicated file.
There doesn't have to be a 1:1 mapping though, a module implementation can use and implement any number of module
interfaces.

## Module Interface

## Module Implementation


# Operators & Precedence

| Precedence | Operator | Type | Description | Applies to |
|------------|----------|---------------|-------------|------------|
|     0    | constant / literal / value | identity | Anything representing a value ||
|     1 LR |   `'` | suffix | Add indirection | type |
|          |  `modifier` | suffix | modifies properties and constraints of a type and / or a values | type or value |
|          |  `member path` | suffix | forms a member type | type |
|          |  `()` | suffix | Function call | function |
|          |  `[]` | suffix | Array subscripting | array or pointer to array |
|          |  `[]` | suffix | Dynamic struct member lookup | struct or pointer to struct |
|          |   `:` | infix | range | unsigned integer, unsigned integer |
|          |   `@` | suffix | Metaobject access | value or type |
|          |   `#` | suffix | Constraint access | value |
|          |   `$` | suffix | Copy (results in a pointer to an object) | value |
|          |   `.` | suffix | Dereference | pointer |
|          |   `.` | suffix | Remove indirection | type |
|          |   `.` | infix | Member access | struct value or pointer to struct value or pointer to array of struct values, member field identifier |
|     2    |  type | prefix | Cast | value or type |
| 15, 3 RL |  `as` | infix | Cast | value or type, type |
| 4, 18 RL |   `=` | infix | assignment | pointer, value |
| 4, 13 RL |  `:=` | infix | assignment | pointer, value |
| 20, 5 RL |  `as` | infix | Cast | value or type, type |
|     6 RL |  `**` | infix | power | numeric value, numeric value |
|     7 RL |   `-` | prefix | Unary minus | signed numeric value |
|          |   `!` | prefix | logical not | value |
|          |   `~` | prefix | bitwise not | numeric value |
|  11,8 LR |  `<<` | infix | left bitshift | numeric value, numeric value |
|          |  `>>` | infix | right bitshift | numeric value, numeric value |
|     9 LR |   `*` | infix | multiply | numeric value, numeric value |
|          |   `/` | infix | divide | numeric value, numeric value |
|          |   `%` | infix | remainder | numeric value, numeric value |
|          |  `/%` | infix | division and remainder | numeric value, numeric value |
|    10 LR |   `+` | infix | addition | numeric value, numeric value |
|          |   `-` | infix | subtraction | numeric value, numeric value |
|  11,8 LR |  `<<` | infix | left bitshift | numeric value, numeric value |
|          |  `>>` | infix | right bitshift | numeric value, numeric value |
|    12 LR |   `&` | infix | bitwise and | numeric value, numeric value |
|          |   `|` | infix | bitwise or | numeric value, numeric value |
|          |   `^` | infix | bitwise xor | numeric value, numeric value |
|    13 LR |   `<` | infix | less than | numeric value, numeric value |
|          |   `>` | infix | greater than | numeric value, numeric value |
|          |  `<=` | infix | less or equal | numeric value, numeric value |
|          |  `>=` | infix | greater than equal | numeric value, numeric value |
| 4, 14 RL |  `:=` | infix | assignment | pointer, value |
| 15, 3 RL |  `as` | infix | Cast | value or type, type |
|    16 LR |  `==` | infix | equal | value or type, value or type |
|          |  `!=` | infix | not equal | value, value |
|    17 LR |  `&&` | infix | logic and | boolean value, boolean value |
|    18 LR |  `||` | infix | logic or | boolean value, boolean value |
| 4, 19 RL |   `=` | infix | assignment | pointer, value |
| 20, 5 RL |  `as` | infix | Final cast | value or type, type |

Pointers, arrays and functions are a kind of value. Types, modifiers and member paths are not a kind of value.

The `as` and bitshift operators have different precedences for the left and right hand side. For example, in the case
of `1 | 2 + 3 * 4 << 1 * 2 + 3 | 4`, it becomes `(1 | ((((2 + (3 * 4)) << 1) * 2) + 3)) | 4`. In addition to this, the `as` operator
has a special case, if its righthand expression consumes the entirety of the remaining expression, it's lefthand
expression is all the stuff left of it / it gets the highest precedence. The parser checks for that special case first
when parsing the expression.

Parentheses can be used to alter the precedence as needed.

The constraint access operator `#` only exists to make it easier to describe things relating to constraints in this
specification. A real compiler shouldn't implement it. See also the section [metadata](#metadata) for why no reflection
is intended for constraints.


# Declararions, definitions, named constants and values

Snep has no variables, instead, constants can be declared and defined. A definition can look like this:
```snep
local myconstant = 10 as int;
```
This defines a named constant `myconstant`, with a value of 10, and a type of `int`. The type (here `as int`) is optional for a definition.

A declaration can look like this:
```snep
local myconstant = ? as int;
```

This declares a named constant `myconstant`. A declaration allows referencing (referring to the variable by name)
a named constant in the whole block it is declared in, including blocks within that block. The constant in the example
above has `local` visibility. The following visibility specifiers exist:

| specifier | meaning |
|-----------|---------|
| `local` | This constant is unique to the current block, it can't be referred to from anywhere else. |
| `module` | This constant is unique to the current module, it can't be referred to from other modules, but any module declaration with the same name within the same module refers to the same constant. |
| `global` | This constant is global, any global declaration with the same name refers to the same constant. |
| `extern` | This constant is extern, any extern declaration with the same name refers to the same constant. Additionally, any extern definition exports the symbol, and any declaration may import the symbol from another program or library, during dynamic linking. It is visible externally. |

The visibility specifier of declarations and definitions is mandatory in code blocks, there it serves the additional
purpose of differentiating definitions and declarations from assignment statements, which otherwise look exactly the
same, but have very different semantics. It can be omitted in other kinds of blocks. For the top-level block, the default
visibility is `module`.

If a block declares or defines a named constant, that is already defined in a parent block, it shadows that named constant
within the scope of it's defining block.

A named constant declared with the `global` or `extern` visibility but not defined in the same module has a value with an
availability of *linktime*, their value is unknown during compile time. Values defined in the same module have an
availability of *compiletime*. See [value availability](#value-availability) for details on what availability values can
have and how value avaiability affects expressions.
A named constant declared with the `global`, `extern` or `module` visibility can not be created from a value of
availability *runtime*, doing so must generate a compile time error.  
In a stackless code block, named constants also mustn't have an availability of *runtime*.

In a code block, declaring a named constant of non-local visibility defines it in the current scope only.
It is not allowed to define a named constant of non-local visibility in a code block.

In snep, there is no way to reference a constant nor a value, as in creating a pointer to it, but there does exist a
copy operator instead. Also, the compiler is allowed to effectively create a reference to a constant as an optimization,
so long as it can safely create a global or external object storing it's value, and the object, and thus any pointer or
reference to it, is immutable. This can have some implications when comparing pointers.

More importantly though, the compiler can assume nothing references a constant or value, they do not represent an object
with a lifetime, memory representation, and so on. This allows for converting values in various ways, with no regard to
it's representation in memory, and no worry anything may reference it.  
The specifics for conversion rules can be found in the [Type conversion and inference] section.

Values are the result of an expression. A value is a lot like a constant, once a value is computed, it can not be
modified. A value is never an object. But objects contain values and may be mutable. Additionally, a value can be a
pointer, and a pointer can point to an object.


# Namespaces

A namespace can be used to group things under a common name. They also prevent unrelated things from clashing.
If you have, for example, a bunch of named constants, you can use the same name for them in differently named
namespaces.

There is no separation between namespace names and named constants in a scope, so if there is a namespace `a` defined in
a scope, you can not have a named constant `a` defined there too. The member operator can be used to access named
constants in a namespace, and to navigate nested namespaces.

When a named constant is defined, you can specify namespace components as well. For example:
```
namespace com.example {
  www.a.b = 123;
};
```
Here, `com`, `example`, `www` and `a` must be (nested) namespaces. They do not have to be explicitly defined anywhere.
The full location of the named constant is `com.example.www.a.b` in this case. The dots there are member
operator, and the names are identifiers, do not make assumptions about what an identifier may contain.

# Objects, pointers, copying values and dereferencing pointers

Objects contain values and may be mutable. However a value can never be an object. A value can be a pointer to an
object, though. The only way to change the value of an object, is using the assignment operator. The assignment operator
takes a pointer to an object, and a value to be assigned to the object the pointer points to. The object then contains
the new value.

A small example:
```snep
local i = 0$;
i = i. + 1;
```

Here, the `$` creates a copy of the value `0`, the result is a pointer to a mutable object containing the value 0.
The constant `i` is defined to be that pointer value. `i.` takes the value `i` points to, it dereferences the pointer.
The `+ 1` adds 1 to that, 0 + 1 will be 1, that's the result of the expression. It then is assigned to the object `i` points to.

You can also do things like `0$ = 1`, `0$` is a pointer to a mutable object after all, so a value can be assigned to it.
Not that that's particularly useful to do, though.

Where the object is allocated depends on the current context. Usually, it defaults to being a global object,
existing from when the program starts until the program ends. In a stackful block, it defaults to being a local object
instead. The global and local keywords can be used to explicitly specify where the object is allocated. However,
you can only allocate an object locally in a stackful block, and you can't use a local object in the definition
of a global named constant.

An assignment expression does not return a value.

The availability of the value resulting from dereferencing a pointer value is equivalent to the result of the expression
`value.#.availability` or `value#.target..availability`. The metadata objects also have values subject to availability.


# member and array access operators

A member can be accessed using `value.name`. If the value is a struct value, the result is the value of the struct member.
If the value is a pointer to s struct object, the result is a pointer to the struct member. Since the dereference
postfix operator is also `value.`, they combine very nicely, you can dereference then access a member, or access a member
then dereference it, or dereference multiple pointer levels, simply by adding a `.` in the right place.

Accessing arrays is done using brackets `value[index]`. It works basically the same as way a member access. When an
Array value is is indexed, the result is the value in the array at the specified index. If a pointer to an array is
indexed, the result is a pointer to the object at the specified index.

For those knowing C, please note that there is no array decay in snep, and the array access operator does not operate
on pointers directly. You can't for example, use it on a pointer to an integer, you would need a pointer to an array
of ints.

Here is a small example for removing an entry from a singly linked list. It uses a triple pointer for `it`:
```snep
number_list_type = {
  value = ? as int;
  next = ? as number_list_type rw';
};
number_list = {1,{2,{3}$}$}$$ as number_list_type rw'rw';
number_list_remove = (boolean <= {list = ? as number_list_type rw'rw'; value = ? as int;}) {
  for(local it=list$; local entry = it..; it=entry.next$){
    if(entry.value. == value){
      it. = entry.next.;
      return true;
    }
  }
  return false;
};
```

In addition to how a member access operator can be applied to a pointer to a struct value, it can also be applied to a
pointer to an array or multidimensional array to a struct value. For that purpose, there exists a special type for
"member of struct or array" (a member type), and for member paths itself. You can assign a named constant to a
member path: `x=(.a.b[123])`, a member path can be applied to a non-pointer type as a suffix operator to form a
member type.

Let's look at the following value as an example `x={{1,2},{3,4},{5,6}} as { int a; int b; }[3]'`.  
Now, `x.a` will have a type of `{ int a; int b; }(.a)[3]'`. So `x.a[1]` would have a type of  `{ int a; int b; }(.a)'`.
A member value and a pointer to member value can be implicitly converted to the type of the corresponding member.


# Blocks

A block is a section of code enclosed in braces `{}`.
There are various types of blocks, the kind of block can vary depending on context.
One example of this are [struct literals], they are described in the section [Literals and constants].
All other kinds of blocks are described below.

## Struct types

A block in the following places defaults to being a struct type:
* The right hand side of an `as` operator defaults to being a type. 
* The input and output type of a function type shorthand expression `({out=? as int;} <= {in = ? as int;})`
* A definition with no target type

To explicitly mark a block as a struct block, it can be prefixed with the `struct` or `type` keywords.

## Code blocks

PRELIMINARY: The syntax and inference rules for blocks have not been finalized yet.

To force a block to be treated as a code block, it can be prefixed with `<`. Some keywords let a block that follows
it default to being a codeblock, but would also accept other expressions or values. For example, `x = if(x) {} else {}`.
If a struct literal was meant instead, you can enclose it in braces `x = if(x) ({}) else ({})`. A block in a code
block as a standalone statement is always a code block. If the target type of a block in an expression is a routine
type, it is also a code block.

A code block can be stackless or stackful. A code block in a stackful block is per default stackful, a code block in a
stackless block is per default stackless. See [Routines] for more information about that.

If a block is part of an expression that expects a value, it has to return a value. Here is an example for this:
```
local c = myblock: <{
  break myblock = 123;
};
```
The `break` keyword and the label are optional here, it's mainly useful when returning no value, and when returning from
a labeled block.


## Dataflow blocks

These blocks are always prefixed by `dataflow` keyword. See [dataflow] for details on dataflows. 

# Literals and constants

## Integer literals

The simplest case is just a bunch of decimal digits, such as `123`, in which case it's a base 10 integer number.
If it's prefixed with `0x`, it is base 16, a hexadecimal number. 0b for base 2 / binary.
An arbitrary base up to 62 is possible by specifying the base using the `!` prefix. For example, `2!11` would be 3 in base 2.
The base is always specified in decimal.  
The digits are: `0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz`  
For any base up to 36, the digits a-z are equivalent to the digits A-Z.

TODO: fixed point

## Float literals

TODO

## String literals

String literals are enclosed in these backticks. If not prefixed by `H` or `B`, they contain raw text and escape sequences
which after resolution can encode arbitrary byte sequences. The source code itself must be encoded entirely in valid UTF-8
and unicode, but that doesn't restrict the final value of string literals, consider them binary data. Backtick and
backslash can be escaped by preceeding them with a backslash. `\n` `\t` `\r` are for newline, tab and carriage return
respectively. `\xNN` can be used for encoding an arbitrary byte. String literals can be multiline. If the first line
starts with a newline, it is removed. If the last line consists only of spaces, it, including the preceeding newline,
is removed.
If a string literal is preceeded by an `H`, it contains hex encoded data instead. A hex string literal
should not have spaces between nibbles. If a string literal is preceeded by `B`, it is a base64 or base64url string literal.
The final `=` are optional, but recommended. If multiple base64 strings are put in the same literal, this may be necessary.
For example, B\`SGVsbG8gV29ybGQh\` encodes `Hello World!`. So does B\`SGVsbG8\` B\`IFdvcmxkIQ\` and B\`SGVsbG8=IFdvcmxkIQ==\`
(`Hello` ` World!`). but B\`SGVsbG8IFdvcmxkIQ\` isn't even valid, and if we remove the `Q`, the ` World!` part becomes
complete gibberish.  
If 2 string literals are next to each other, they are merged. Each literal may have it's own independent prefix, the
decoded values of the literals are what's merged.

## Struct literal

It's easyest to specify the syntax formally:
```abnf
object_literal   = "{" [initializer_list] "}"
initializer_list = initializer *(";" initializer) [";"]
initializer = expression / member "=" expression / declaration "=" expression / declaration
```
(The complete grammar can be found in the section grammar).

If there is a `{}` block, it's an object literal. There are many ways to define an object literal.
An object literal is also an expression (or part of one), and any expression may have a target type which may be inferred,
for example by a definition. Such a type inference can go both ways, the object literals type will be adjusted to be
compatible with the inferred type, and the final type of the object literal may affect the type of the expression,
and things like the type of a definition.

If the type is inferred, then the initial members equal those of the inferred type. If an initialisation then specifies
a member, and it matches a field from the inferred type, that will be the member being initialized.  
Otherwise, initializers adds those fields after the fields for the inferred object, in order.
Unnamed initializers initialize fields in order. If there is an inferred type, it'll start with the first field
corresponding to it. After a named member, initialisation of unnamed members continues after the designated member field.  
However, note that this is about the order of members in the object, not the evaluation order of the expressions.
The initializer expressions are all evaluated in order.

## Type system

Every object has a type. Let's first consider the types of constants that can be created directly using literals:

| literal | meta type object | shorthand type notation |
|---------|------|-------------------------|
| 123     | `snep.type.int { signed=1, bits=8, exponent=0 }` | `i8` (for this example constant only) |
| \`Hello World!\` | `snep.type.array { type=u8, length=12 }` | `u8[12]` |
| {1,"b"} | `snep.type.struct { fields={ snep.type.int {signed=1, bits=2}, u8[1]@ }$ }` |  |

Note: a type meta object is an object created from a builtin meta type. For example, `snep.type.struct` is the meta type
of the struct meta type object. For the most part, meta type objects can be used like regular objects, and meta types can be
used like regular types. However, meta type objects have a special field called type, which allows them to be used as
the type they describe.

# Identifiers

An identifier literal has the following syntax. It starts with a codepoint with a major unicode category of `L`, or a
unicode category of `Pc` or `Nl`. The following codepoints can additionally have a major category of `N`.

The name of an identifier can contain any binary data, there are no limitations. Currently, a identifier literal is
the only way to create an identifier, so in practice that limits what name they can currently have, but in the future,
other ways to create identifiers may be added.

There exist various reserved identifiers which are pre-defined and can not be shadowed or overwritten in any way.
All identifiers starting with `_` are reserved. Here are all the keyword that are reserved everywhere and can't be
used for defining an identifier:
* type
* struct
* union
* int
* signed
* unsigned
* float
* extern
* global
* module
* local
* as
* rw
* ro
* wo
* unique
* return
* yield
* routine
* stack
* modifier
* dataflow
* coroutine
* generator
* goto
* break
* continue
* while
* for
* if

# Scopes, and identifier shadowing

A scope referes to the set of namespaced identifiers defined in any given context. Scopes are hirarchical, a scope
A can have a parent scope B. The identifiers in A can then be accessed in scope B as well, but while they are inherited
by scope B in that sense, they are not defined in scope B itself. An identifier defined in a scope shadows any identifer
of the same name and namespace from a parent scope. If something is declared or defined more than once in the same scope,
it must be defined in exactly the same way. For named constants, that means the value and type must be guaranteed to match
exactly. The `global`, `extern` and `module` keywords do not change the scope something is defined in, visibility is a
distinct concept.

Unless stated othewise, assume any block has it's own scope, and namespaced identifier, such as named constants,
are defined in the current scope.  
There exist exceptions to that, for example, a label in a code block is always defined in the scope of the containing
routines code block, rather than the current scope.

# Routines

A routine is an abstraction for a code block that can be executed at a later point. Routines can take arguments.
For routines that don't take arguments, the empty object type can be specified.

Example:
```
a = routine <{
  puts("1");
  local mycontinuationpoint = yield b();
  puts("3");
  yield b.mycontinuationpoint();
};

b = routine <{
  puts("2");
  yield a.mycontinuationpoint();
mycontinuationpoint:
  puts("4");
};

main = a;
```

Unless explicitly specified, a routine is not stackful. This doesn't mean that it doesn't use the current stack at all,
rather, routines that are not stackful can jump between each others continuation points without increasing the stack
size and without losing state. Their entire state is provided to them as an argument on resumption.  
Routines that are not stackful do not return arguments. If a stackless routine returns it returns to the most recent
routine call on the current stack.

Aside from making a whole routine stackful (see the section [Functions]), it is also possible to make a block in a
routine stackful, using the `stack` keyword. Only blocks that are stackful can contain local objects, that are objects
created and valid only within the block / during its execution. A block in a stackful block is implicitly stackful too.
Stackful routines and stackful blocks can return values. If a stackless block is part of an expression, it can return
a value too. Stackless operations are not available in a stackful block.

Example:
```
a = routine <{
  stack <{
    local myvar = string_rw { "Hello World"$ };
    puts(myvar);
    // yield is not allowed in here
  };
  // Here, yield is allowed again
};
```

Functions, coroutines, generators, and async functions, and dataflows are all simply special cases of a routine.
There is some dedicated syntax for common pattern to make them easier to use, syntactic sugar basically. This
also helps establish conventions for various things, which is important because there are often many, in some
way incompatible ways to achieve the same result. For example, you could store a value you wish to return from
a generator in it's own state, in some other routines / continuations state, somewhere a pointer points to, etc.



## Stackless operations

Stackless operations are not available in a stackful block, they can only be used in stackless reoutine blocks.
Most stackless operations leave the routine that contains them, but do not return to the last caller directly,
but to a different routine. If that routine does complete, then it will return to the last caller.

### yield statement

The `yield` keyword jumps to another stackless routine. This does not change the caller routine. The `yield` statement
takes a stackless routine call expression, which defines where it jumps to, and which arguments are passed.

A yield statement can be used as the initializer for a named constant. Its value is a continuation, using which execution
can be resumed right after the yield statement. The availability of the value of a yield expression is linktime.
This is useful for passing the continuation point to the routine yielded to.

A yield statement is also allowed as the value of an assignment expression. When this is done, the assignment happens
immediatly before jumping to the target routine.

## Functions

The function type notation is `(output <= input)`. Input and output are expressions resulting in a type. The type must
either be a struct or pointer to struct type. The parentheses are never optional when using the function type notation.
This ensures that things never get ambiguos when nesting function types.

A named constant can also contain a function type, but a named constant with a function type doesn't itself need to be
enclosed in parentheses.

A function is a stackful routine. The following assertion will always hold:
```
puts1_type = routine {input = ? as string;} stack {output = ? as int;};
puts2_type = ({output = ? as int;} <= {input = ? as string;});
assert(puts1_type == puts2_type);
```

## Coroutines

### Conventions for streaming data

## Generators

### Conventions for yielding a result

## Dataflows

Dataflows define what tasks are executed based on what data is needed or available to a task. In the context of snep,
you can think of the tasks as an instance of a routines state; you may need multiple instances of the same routine to
process different inputs and outputs of other tasks or the dataflow routine itself. A dataflow defines how these
tasks are connected, the outputs of which tasks go to the inputs of which other tasks, and so on.

There are two common but quiet different applications for this.

Task based dataflows usually run tasks as soon as all their prerequisites are met; for example, the task got all the data
it needs, or it didn't need any data in the first place, or another task that needs to complete first has completed,
so now the task can and will run. This kind of dataflow usually uses eager evaluations for it's tasks.

Data driven dataflows are used to process streams of data. For this type of dataflow, it is usually more useful to
lazily evaluate tasks, to only run them when their data is actually needed, since one is usually only interested
in the resulting data, rather than that all the tasks that could run have run, or that they run in a particular order.
Of course, the prerequisites of a task still need to be met for it to run, so if a task needs more data itself too,
the task that produces it will run first. (This is a slight simplification, there is quite some freedom in how
inputs and outputs can be connected).

TODO

## Stack switching / green threads, and event loops



# Type inference and conversion rules

# Type metadata

Any object has a type, any type has a type meta object.  
A type meta object is an object created from a builtin meta type. For example, `snep.type.struct` is the meta type
of the struct meta type object. Meta type objects are also regular objects, and meta types are also regular types,
so they can be used the same way. However, meta type objects have a special builtin field called type, which allows
them to be used as the type they describe.  
The meta object for a type can be obtained using the `@` postfix operator. It can also be applied to an object, to get
it's types' meta object. This also means you can get the type of an object. For example, `1@.type` is the type of the
object containing the integer constant 1.  


# Context dependent aspects of the language

OUTDATED: This section left over from early drafting, some parts of it are no longer matching current syntax decisions.

In snep, a struct and a struct literal both use `{}` blocks.  
For structs, they may be prefixed with the `struct` keyword if it's not clear that it's a type from the context.  
Determining if a block is a code block or a struct literal is a bit more difficult, though. In an expression, if there
is an infered type (from a cast for example), and that type is a function type, then the block is a code block.  
If it is prefixed by `#`, it's also a code block. In places where a statement is expected, a block is also treated as
a code block.

Since the type of a block, and various things within an expression, can depend on the type of a named constant,
the type of a named constant must be determined in that case. Specifically, it must be known if the named constant is a
type, an object, or a function.  
This is deterrmined for each block, from topmost block to inner most block, and from top to bottom.  
In a code block, every definition and assignment is processes from top to bottom, after that the defined identifiers
type is known.


# Metadata

For a given expression, various metadata for different parts of it that may be relevant can exist. In snep, you can
think of the following categories you'll need to distinguish (may be extended in the future):
* Code metadata (information about syntactic elements, such as named constants, routines, operations, control constructs, etc.)
* Type metadata (Is it an integer, pointer, struct, what fields does it have, etc.)
* Constraint metadata (Availability, immutability, read/write-able, min / max dimensionality of arrays, etc.)

During type inference, some unknown type information is derived from constraint information, and some known type
information is used to infer constraints. Constraint metadata are separate from type metadata for two reasons.  
First off, they are sometimes more precise or more comprehensive than type information. Most notably, a newly created
struct value has constraints for every member field, and an array value has constraints for every single entry, which
match the constraints of the value used during its initialization. This includes the availability of the value.
This allows resolving many things at compile time and linktime which otherwise would require runtime overhead or would
not be possible to be expressed otherwise at all.
Secondly, unlike type and code metadata, constraint metadata can not be queried and is strictly internal to the compiler.
If availability, immutability, read / writability, were all recorded at the same detail in the type information,
and not relaxed at all, it'd lead to a combinatorial explosion of type objects that would have to be stored.
Since type information is usually specified manually and explicitly, the amount of type objects that need to be stored
is reduced drastically.

# Compilation & linking

During compilation, a snep module source file is turned into an a object file.
The format of snep object files is left up to the implementation. On most platforms, other languages like C also produce
object files, on such platforms, using the existing object file format may make sense. whatever the format chosen is,
the linker should support linking snep object files and non-snep sources together. A platform should specify an ABI
for snep programs to keep things compatible between different compilers and linkers. 

The produced object file contains various hints for the linker. One of these is a list of library names the program
should be linked against. This must always include `sneplang`, the snep standard library.

Multiple object files can be combined in a static library, a static library is basically an archive containing object files.

The linker is expected to take object files, static and dynamic libraries, and produce a shared library or a program.  
On most platforms and with most languages, it has become customary to use the compiler frontend for linking the program
as well. That will then automatically link against the languages standard library and so on. In snep, doing the same is
not recommended. The linker is expected to honour the library hints in the object files, and link against the standard
library that way. Furthermore, symbols of libraries linked against in this way must be resolved such that the order
does not matter for strong symbols, all required symbols must be resolved, and duplicate / conflicting strong symbols
should result in an error.

## Sections and symbols

In this section, for simplicities sake, we assume an object / executable file format similar to ELF, with sections and
symbols. Most things written here do not have to be strictly followed, but are meant to illustrate how it's all meant
to work in a snep program, except when it's explicitly written that it *must* be that way.

The `.text` section is for the machine code generated from routines (functions, coroutines, etc.).  
The `.data` section is mainly for the initial data of mutable global objects.  
The `.rodata` section is for immutable global objects. That includes things like string literals. Metadata objects are
also located here.  

All routines and global objects, including metadata objects, get their own subsection. If nothing references these
subsection, and they are not marked as required, the linker must remove it.

All global and external named constants have symbols. Declarations consume them, definitions produce them.
Usually, only objects have a representation and are stored in one of the data sections, values do not.
When there is a global named constant containing a pointer value, that value represents the location of
the symbol itself as placed by the linker during the linking step, the representation of the object it
points to must be what is stored in the section, not the value of the pointer. Of course, if the object
the pointer value points to is an object containing a pointer value, then the representation of the
object containing that pointer is not treated differently from any other object here.  
There is a special case in regards to how global and external named constants that are not pointer values are treated.
Since all global and external named constants generate symbols, that includes ones with non-pointer values.
In this case, the named constant must be treated as if it was stored in an object for the purpose of storing the data,
the symbol refers to said object, and the representation and way it's stored in general must be identical to the one
it would have had if the named constant had been a pointer value to said object.  
In other languages, this can't be done because they tend to have variables and/or values that can be referenced
and written to, but this is not the case in snep, here, they are always immutable and unreferencable.
Although, keep in mind that the [copy operator](copy-operator) is allowed to return a pointer to an existing object
if said object has identical representation, can be proven to be immutable, can be proven to have sufficient
lifetime, and the target pointer type is to a readonly or immutable object. For the special case discussed above,
this is usually the case, and the compiler should take advantage of this optimization when it can.

Due to the nature of splitting compilation and linking, there are some limitations on what is known when about
global objects, and global named constants. The value of a symbol, and thus the value of a global named pointer
constant, is only known after linking, perhaps even only after dynamic linking. The availability of such a value
is `linktime`. The value of a global constant defined in a different module is also only known after linking,
but its availability is `runtime`. That is because you can have things like symbols relative to other symbols,
and you can resolve the difference of symbols at linktime to some extent, but you can't usually do anything with
the data at linktime. See [value availability](#value-availability) for details on how value availability affects
expressions.

You can have multiple Symbols for the same object. Expressions like the following are also allowed:
```
global x = { a=1; b=2; }$;
global y = x.b;
```
As per the usual member operator rules, `x.b` results in a pointer to the member `b` in `x`, so `y` will be a
pointer to that member, and there will be a symbol `y` associated with that member in the object, which will
be inside the object with symbol `x`. 

For named constants that are spread arrays or pointers to spread arrays (these are unordered arrays with values
defined in any number of modules), the final value of the spread array will be finalized after static linking.
As such, they can be global, but they mustn't be external. The size of such an array has an availability of `linktime`.

## Symbol names

Global named constants generate symbols. These symbols need a name.  
It must be ensured that symbols do not collide; constants in different namespaces or with different name must always have
distinct symbol names. There is thus a need for a delimiter to separate the symbols names components, and a need to
escape the delimiter if the same byte sequence appears in the symbol name components name.  
There are also usually platform related restriction to what a symbol name can contain. These also need to be escaped.  
These are done in 2 separate escaping steps, first the platform independent escaping, then the platform dependent
escaping. Following is described the exact steps for generating the symbol name.

1) Take the components of the location of the named identifier (see [Namespaces]). Escape the following characters:
   * Invalid UTF-8 sequences using `\xXX`
   * Control characters using `\r\n\t\v\e\b` if possible, else using `\xXX`
   * Codepoints not valid in unicode using `\xXX`
   * Noncharacters using `\xXX`
   * `\\` as `\\\\`
   * `.` as `\.`
   * `:` as `\:`
2) Concatenate the escaped components with `.`
3) Escape the characters that would cause problems for the target platform
4) prefix it with `_snep.`

### Target platform specific escaping
#### ELF + linkers that support GNU linker scripts

Escape `'` as '\`, `"` as `''`, `\n` as `'n` and null bytes as `'0`.

As an example, this ``$`a.b"c\nd`.e = 123;`` becomes: `_snep.a\.b''c\nd.e`. In most cases, the transformations are
fairly minimal, `test.abc.x123` becomes `_snep.test.abc.x123`.
