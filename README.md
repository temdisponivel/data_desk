# Data Desk

## Description

Data Desk is a project utility that parses a simple C-like data description format. Input files in this data description format are parsed to create corresponding abstract syntax trees which represent the information extracted from the files. These abstract syntax trees are then sent to project-specific custom code that is written by the user. This custom code is simply a dynamic library with a few exported functions that are used as callbacks for the parser. Below is a list of the callbacks.

* `DataDeskCustomInitCallback(void)` is called when the parser starts.
* `DataDeskCustomFileCallback(DataDeskASTNode *root, char *filename)` is called when the parser finishes parsing a file.
* `DataDeskCustomConstantCallback(DataDeskConstant constant_info, char *filename)` is called for every constant definition that is parsed.
* `DataDeskCustomStructCallback(DataDeskStruct struct_info, char *filename)` is called for every structure that is parsed.
* `DataDeskCustomDeclarationCallback(DataDeskDeclaration declaration, char *filename)` is called for every declaration that is parsed.
* `DataDeskCustomCleanUpCallback(void)` is called before the parser shuts down.

The abstract syntax tree is formed completely by `DataDeskASTNode` structures. This structure can be found in the `data_desk.h` file.

Data Desk also offers a number of utility functions for introspecting on abstract syntax trees it passes to your custom code. A list of these is in the `data_desk.h` file, which can be included into your custom layer.

## Usage

To use Data Desk, you'll need to do a few things:

1. Get Data Desk
2. Make or get some Data Desk format files (.ds)
3. Make a project-specific custom layer

### Step 1: Get Data Desk

1. Run the command `git clone https://github.com/ryanfleury/data_desk`
2. `cd data_desk`
3. `build` on Windows or `./build.sh` on Mac/Linux

NOTE: The `build` command on Windows expects to find `cl` (MSVC). Your environment should know about this. The easiest way to do this is to call `vcvarsall.bat` in your terminal environment, which is packaged with Visual Studio.

### Step 2: Make or get Data Desk format files (.ds)

Grab an [example](https://github.com/ryanfleury/data_desk/blob/master/examples/) or make your own.

### Step 3: Make a project-specific custom layer

1. An easy way to write the code for this is to check out the custom layer template, located [here](https://github.com/ryanfleury/data_desk/blob/master/examples/custom_layer_template/custom_layer_template.c). Fill out the functions in your custom layer code however you want to. There are some helper functions available in `data_desk.h` that might be useful for you here. This can be dropped into your code and used.

2. To build a custom layer, you just need to build a DLL with the function callbacks you've written as the appropriate exported symbols. `data_desk.h` outlines what symbols are used for each callback.

### Step 4: Run Data Desk

To run Data Desk with your custom layer, you can use the following command template:

`data_desk --custom /path/to/custom/layer /file/to/parse/1 /file/to/parse/2 ...`

## Data Desk (.ds) File Documentation

A valid Data Desk file is defined as a set of zero or more `Declaration`s, `Struct`s, `Enum`s, `Flags`s, `Comment`s, or `Const`s. Each of the following sections defines these (and what they are comprised of).

1. [Identifiers](#identifiers)
2. [Numeric Constants](#numeric-constants)
3. [String Constants](#string-constants)
4. [Character Constants](#character-constants)
5. [Binary Operators](#binary-operators)
6. [Expressions](#expressions)
7. [Types](#types)
8. [Declarations](#declarations)
9. [Structs](#structs)
10. [Enums](#enums)
11. [Flags](#flags)
12. [Constant Expressions](#constant-expressions)
13. [Comments](#comments)
14. [Tags](#tags)

### Identifiers

`Identifier`s are defined as a sequence of characters that begin with either an underscore or an alphabetic character, and contain numeric characters, alphabetic characters, or underscores (similar to C).

### Numeric Constants

Numeric constants (`Number`s) are defined as a sequence of characters that begin with a numeric character, and contain only numeric characters, periods, or alphabetic characters.

NOTE: Data Desk does not guarantee the correctness as defined by programming languages of your numeric constants. For example, the following will be interpreted by Data Desk as a numeric constant: `1.2.3.a.b.c`. Because Data Desk does not do any evaluation of numeric constants, it will not enforce validity of numeric constants.

### String Constants

String constants (`String`s) can be *single-line* or *multi-line*.

A *single-line* string constant is defined similarly to those in C. It begins with a double-quote character, and ends with a *non-escaped* double-quote character. Double-quote characters can be escaped with a backslash.

A *multi-line* string constant is defined as beginning with three double-quote characters (`"""`), and ending with three double-quote characters (`"""`).

### Character Constants

Character constants (`Char`s) are defined almost identically to single-line string constants, but with single-quote beginning and ending characters instead of double-quote characters.

### Binary Operators

Data Desk defines a subset of the binary operators found in C. It does not define shorthand assignment operators, like `+=` or `>>=`. The following binary operators are defined (in order of ascending precedence):

 * `+`: Addition
 * `-`: Subtraction
 * `*`: Multiplication
 * `/`: Division
 * `%`: Modulus
 * `<<`: Left Bitshift
 * `>>`: Right Bitshift
 * `&`: Bitwise And
 * `|`: Bitwise Or
 * `&&`: Boolean And
 * `||`: Boolean Or

### Expressions

An expression (`Expr`) in Data Desk is defined as being one of the following:
 * `Identifier`
 * `Number`
 * `String`
 * `Char`
 * `Expr` `Binary Operator` `Expr`

### Types

Types are used in declarations. They are defined as being the following:

* A group of 0 or more `*` characters, representing the number of layers of indirection
* A type name, which can be:
  * Some `Identifier` referring to a type name
  * A `Struct` definition (refer to next section)
* A group of 0 or more array size specifiers, being defined as a `[` character, some `Expression`, and a `]` character

### Declarations

`Declaration`s are defined as follows:

`Identifier` `:` `Type` `;`

### Structs

`Struct`s are groups of zero or more declarations. They are defined as:

`struct` `Identifier`
`{`
Zero or more `Declaration`s
`}`

### Enums

`Enum`s are groups of one or more identifiers. They are defined as:

`enum` `Identifier`
`{`
One or more `Identifier`s, each followed by `,` characters.
`}`

When transpiled to C, these will be defined as a normal C `enum`; that is, the first one will be defined as a constant that evaluates to 0, the next to 1, and so on.

### Flags

`Flags`s are groups of one or more identifiers. They are defined as:

`flags` `Identifier`
`{`
One or more `Identifier`s, each followed by `,` characters.
`}`

When transpiled to C, these will be defined as several C preprocessor macros that evaluate to unique bits inside of an integral value. These are similar to `Enum`s, but their purpose is to define unique bits instead of unique integral values for a set of constants.

### Constant Expressions

Constant expressions (`Const`s) are defined as:

`Identifier` `::` `Expression` `;`

### Comments

Comments are ignored by the parser. They can be *single-line* or *multi-line*.

*Single-line* comments can be defined with two `/` characters. They are terminated by a newline character.

*Multi-line* comments can be defined with a `/*` pattern. They are terminated by a `*/` pattern. They can also be nested. For example, if there exists the pattern `/*/*`, it will require `*/*/` to terminate.

### Tags

`Struct`s, `Declaration`s (including those within `Struct`s), or `Const`s can be preceded with one or more `Tag`s. A `Tag` is defined as beginning with a `@` character, and ending with whitespace. These are used to annotate meta-information about various things. They will be passed to custom-layer code.