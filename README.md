# Coda-P
Coda-C Property List Library

## ⭐️ The Coda-C Property List Library ⭐️

This library provides a classed system of objects that model the Apple Propery List or 'plist'.
A plist is any one of the supported objects where containers reference
supported objects, but is most often a Dictionary.
Plists are easy to create and modify programmatically, and even easier to save and load from disk.

## 🔵 Features:

* Supports XML serialization in 'plist' or 'codalist' formats
* Supports JSON, and allows JSON5 input
* Supports Binary Property List serialization
* Native C object types.
* Retain counted objects simplify changes to plists
* A pure C source and API for increased portability

## 🟤 Requirements

* gcc/clang
* Coda-C Library

## 🟢 Building

You need a modern C compiler.  Testing has been done for 64-bit Apple and Linux systems with gcc/clang.
A Makefile is provided to install the dynamic library, headers, and 'coda-list' in /usr/local or /usr for Linux.
Note:  For Linux, "sudo ldconfig" should be run after "sudo make install".
After installing but before cleaning, "make test" will run two small test batteries to verify the products.

## 🟣 Links

* Documentation:  [Docs](https://github.com/actguru/coda-c_plist-docs)
* Homepage:  [Coda-C](https://coda-c.com/)

## 🟠 Release notes XML:

1. "\<key/\>" is not allowed for blank dictionary keys.
2. XML attributes like aaa in ( \<true aaa="x" /\> ) are not allowed.
3. Set is serialized to Array for Apple XML.
4. \<null\> is encoded as { CF$Null : true }.

## 🟠 Release notes JSON:

1. Single leaf plists must "quote" the string.
2. \<data\> is encoded as { CF$Data : "BASE64 data..." }.
3. \<uid\> is encoded as { CF$UID : ival }.
4. \<date\> is encoded as { CF$Date : "YYYY-MM-DDThh:mm:ssZ" }.
5. Special encodings may be disabled.
6. \\u0000 is mapped to 0x10FFFF.

# coda-list

### ⭐️ The "coda-list" Property List Utility ⭐️

This program slices and dices plists of various formats.
Each argument to this program is an instruction for inspecting, loading, saving, or altering plists, etc.

### 🔵 Features:

* Create specific new compoents or add JSON branches
* Combine and prune plists
* BASE-64 data extraction and file insertions
* Easily convert formats 
* A pure C source for increased portability

### 🟢 Building

This program is built as a command line tool while making the PList library.

### 🟠 Release notes:

1. Single leaf \<data\> plist save as raw data.
2. Use 'man coda-list' for the online manual.
