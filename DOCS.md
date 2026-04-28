# Ardore Documentation

## What is Ardore?

Ardore is a **user-driven transpiler**. It translates code written in your custom syntax into another language (C++ by default). All language rules are stored in editable `.txt` files. No parser code. No BNF. No dependencies.

## How it works

1. You describe your language in `syntax/*.txt` files
2. You write code in your language (`.ardor` file)
3. Ardore tokenizes, classifies, translates, and generates output code

## Project Structure

```
/
├── ardor.cpp — main entry point
├── cutter.h / cutter.cpp — lexer / tokenizer
├── emmiter.h / emmiter.cpp — classifier, translator, patterner, generator
├── types.hpp — data structures
├── syntax/ — language definition files
│ ├── special_chars.txt
│ ├── active_chars.txt
│ ├── classifications.txt
│ ├── patterns.txt
│ ├── extension.txt
│ ├── modules.txt
│ └── modules/
├── data/ — output folder
└── example.ardor — example source file
```


## Syntax Files

### special_chars.txt
Characters that act as separators (operators, brackets, etc.)

**Format:**

```
special
=
(
)
+
-
active
"
(
)
```


**Sections:**
- `special` — characters that are always separate tokens
- `active` — characters that toggle "string mode" (quotes, brackets that treat spaces as part of the word)

### active_chars.txt
Characters that toggle string mode. When an active character is found, spaces inside are kept as part of the word until the matching closing character is found.

### classifications.txt
Your dictionary: word → type → translation.

**Format:**

```
word TYPE translation
```


**Example:**

```
let KW_LET auto
print KW_PRINT std::cout
= OP_ASSIGN =
( OP_LRACE (
) OP_RRACE )
```


**How it works:**
- `word` — the word in your language
- `TYPE` — internal type for pattern matching (use UPPERCASE)
- `translate` — what it becomes in the target language

### patterns.txt
Code generation templates: pattern → output code.

**Format:**

```
PATTERN : generated code
```


**Special symbols:**
- `%0`, `%1`, `%2` — insert token by position (counting from 0)
- `WORD` — any unclassified word (variable names, values)

**Example:**

```
KW_LET WORD OP_ASSIGN WORD : auto %1 = %3;
KW_PRINT WORD : std::cout << %1;
KW_PRINT STRING : std::cout << %1;
```


**How patterns work:**
1. Ardore collects token types for a line: `KW_LET WORD OP_ASSIGN WORD`
2. It searches for a matching pattern
3. When found, it replaces `%N` with the corresponding token's word

### extension.txt
Output file extension.

**Example:**

```
cpp
```


Change this to `py`, `lua`, `js`, etc., depending on your target language.

### modules.txt
List of module files in `syntax/modules/`. Each module adds additional syntax rules.

**Example:**

```
loops.txt
conditions.txt
functions.txt
```


### modules/ folder
Each module file defines trigger words and their translations.

**Format:**

```
trigger_word : translation
```


**Example (`loops.txt`):**

```
while : trigger
for : trigger
```


A `trigger` translation means: "when this word is found, apply this module's rules to the current line."

## Writing Your Language

### Step 1: Define separators
Edit `special_chars.txt` — list all operators, brackets, and special symbols.

### Step 2: Define active characters
Edit `active_chars.txt` — list characters that toggle string/comment mode.

### Step 3: Define keywords
Edit `classifications.txt` — list all words with their types and translations.

### Step 4: Define patterns
Edit `patterns.txt` — create templates for each possible line structure.

### Step 5: Write code
Create an `.ardor` file with your source code.

### Step 6: Run Ardore

```
./ardor.exe
```

Enter your filename when prompted.

### Step 7: See output
Generated code appears in `data/` folder.

## Example: Creating a Custom Syntax

### classifications.txt

```
let KW_VARIABLE auto
print KW_PRINT std::cout
~ OP_TILDE <<
= OP_ASSIGN =
" OP_QUOT "
```

### patterns.txt

```
KW_VARIABLE WORD OP_ASSIGN WORD : auto %1 = %3;
KW_PRINT OP_TILDE OP_QUOT WORD OP_QUOT : std::cout << %3;
```

### example.ardor

```
let x = 10
print ~ "Hello World!"
```

### Output (C++)
```cpp
auto x = 10;
std::cout << "Hello World!";
```

## Target Languages
By default, Ardore outputs C++. To target another language:

1. Edit `classifications.txt` — change translations to the target language keywords

2. Edit `patterns.txt` — change output patterns to the target language syntax

3. Edit `extension.txt` — change to the target file extension

## Example: C++ → Python

| Classification | C++ translation | Python translation |
|----------------|-----------------|--------------------|
| KW_PRINT | std::cout | print |
| KW_LET | auto | none |

## Modules

Modules add reusable syntax rules. They are especially useful for constructs like loops, conditions, and function calls

**Creating a module:**

1. Create a file in `syntax/modules/`, e.g., `loops.txt`
2. Define trigger words:
```
while : trigger
```
3. Define translations for related words:
```
do : do
loop : while
```

## Using a module

1. Add the module filename to `modules.txt`
2. When Ardore finds a trigger word in the code, it loads the module's rules
3. All words in the module are replaced according to their translations

## Error Messages

| Error | Meaning | Solution |
|-------|---------|----------|
| `SYSTEM ERROR: File not found` | Missing syntax file | Check `syntax/` folder |
| `SYSTEM ERROR: File empty` | Syntax file has no content | Add rules to the file |
| `ERROR: Error in a line N` | No pattern matches this line | Add a matching pattern to `patterns.txt` |
| `ERROR: File don't exists` | Source file not found | Check filename spelling |
| `ERROR: File don't open` | File permissions issue | Check read/write permissions |

## Build from Source

Requires **g++** with C++20 support ([MinGW-w64](https://www.mingw-w64.org/))

```
g++ -std=c++20 ardor.cpp cutter.cpp emmiter.cpp -o ardore.exe
```

## Command Line Usage

```
.\ardore.exe
Enter the file name with extension: example.ardor
File "example.ardor" found
```


The program shows workspace path, asks for input filename, and generates output in `data/`.

## Limitations (v0.1.0)

- Single-line patterns only (multi-line via `\n` planned for v0.2.0)
- No recursive pattern compression
- Manual target language configuration
- All tokens in a line must match the pattern exactly (no variadic arguments yet)
- No type inference — all unclassified words become `WORD`

## FAQ

**Q: How many target languages does Ardore support?**
A: Unlimited. You write the translation rules. C++, Python, Lua, JavaScript, or any other text-based language.

**Q: Is it possible to nest structures within each other (if they are inside a loop)?**
A: In version 0.1.0, this is a limited feature. Full nesting using `\n` patterns will be available later.

**Q: Does Ardore check for syntax errors in my language?**
A: No. Ardore is a translator, not a validator. If the generated code has errors, check your `classifications.txt` and `patterns.txt`.

**Q: Can I use Ardore for commercial projects?**
A: Yes! Ardore is MIT licensed. Use it for anything.

## Support

- Discord: https://discord.gg/VnwPgFKvgD
- DonationAlerts: https://dalink.to/dortydog
- GitHub Issues: https://github.com/DorotyDog/ardore/issues

## License

MIT — see [LICENSE](LICENSE) file.
