STD::Format

// ============================================================================
// COMPLETE UNDER-THE-HOOD IMPLEMENTATION OF std::format
// This shows how std::format / fmt library works internally
// ============================================================================

#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <cstring>
#include <algorithm>
#include <variant>

// ============================================================================
// PHASE 1: COMPILE-TIME FORMAT STRING PARSING
// ============================================================================

// Represents a single parsed piece of the format string
struct FormatPart {
    enum class Type {
        Literal,    // Plain text like "Temperature: "
        Argument    // Placeholder like {}
    };
    
    Type type;
    std::string_view text;      // For literals: the actual text
    int arg_index;              // For arguments: which argument (0, 1, 2...)
    
    // Format specifiers (e.g., in {:05d}, width=5, fill='0')
    struct FormatSpec {
        int width = 0;
        char fill = ' ';
        char type_spec = '\0';  // 'd' for decimal, 'x' for hex, etc.
        bool show_sign = false;
    } spec;
};

// The result of parsing a format string at compile-time
struct ParsedFormat {
    std::vector<FormatPart> parts;
    int arg_count;
};

// Parse format string into parts (this would be constexpr in real implementation)
ParsedFormat parse_format_string(std::string_view fmt) {
    ParsedFormat result;
    result.arg_count = 0;
    
    for (size_t i = 0; i < fmt.size(); ++i) {
        if (fmt[i] == '{') {
            // Check for escaped brace: {{
            if (i + 1 < fmt.size() && fmt[i + 1] == '{') {
                result.parts.push_back({
                    FormatPart::Type::Literal,
                    fmt.substr(i, 1),  // Just the '{'
                    -1,
                    {}
                });
                ++i;  // Skip second '{'
                continue;
            }
            
            // Find matching '}'
            size_t end = fmt.find('}', i);
            if (end == std::string_view::npos) {
                throw std::runtime_error("Unmatched '{' in format string");
            }
            
            // Parse format specifier between { and }
            std::string_view spec_str = fmt.substr(i + 1, end - i - 1);
            FormatPart::FormatSpec spec;
            
            // Simple parsing (real implementation is more complex)
            if (!spec_str.empty() && spec_str[0] == ':') {
                // Parse things like {:05d}
                size_t spec_pos = 1;
                
                // Parse fill character
                if (spec_pos < spec_str.size() && spec_str[spec_pos] == '0') {
                    spec.fill = '0';
                    ++spec_pos;
                }
                
                // Parse width
                while (spec_pos < spec_str.size() && 
                       spec_str[spec_pos] >= '0' && spec_str[spec_pos] <= '9') {
                    spec.width = spec.width * 10 + (spec_str[spec_pos] - '0');
                    ++spec_pos;
                }
                
                // Parse type specifier (d, x, f, etc.)
                if (spec_pos < spec_str.size()) {
                    spec.type_spec = spec_str[spec_pos];
                }
            }
            
            // Add argument placeholder
            result.parts.push_back({
                FormatPart::Type::Argument,
                "",
                result.arg_count++,
                spec
            });
            
            i = end;  // Skip to closing '}'
        }
        else if (fmt[i] == '}') {
            // Check for escaped brace: }}
            if (i + 1 < fmt.size() && fmt[i + 1] == '}') {
                result.parts.push_back({
                    FormatPart::Type::Literal,
                    fmt.substr(i, 1),  // Just the '}'
                    -1,
                    {}
                });
                ++i;  // Skip second '}'
                continue;
            }
            throw std::runtime_error("Unmatched '}' in format string");
        }
        else {
            // Regular character - accumulate into literal
            size_t start = i;
            while (i < fmt.size() && fmt[i] != '{' && fmt[i] != '}') {
                ++i;
            }
            
            result.parts.push_back({
                FormatPart::Type::Literal,
                fmt.substr(start, i - start),
                -1,
                {}
            });
            --i;  // Will be incremented by loop
        }
    }
    
    return result;
}

// ============================================================================
// PHASE 2: TYPE-ERASED ARGUMENT STORAGE
// ============================================================================

// Forward declare the context
class FormatContext;

// Type-erased storage for a single argument
// This is the magic that allows storing different types in one container
class FormatArg {
public:
    enum class Type {
        None,
        Int,
        UInt,
        Long,
        ULong,
        LongLong,
        ULongLong,
        Double,
        LongDouble,
        Bool,
        Char,
        CString,
        String,
        Pointer,
        Custom  // For user-defined types with formatters
    };
    
private:
    Type type_;
    
    // Union to store the actual value efficiently
    // Only one of these is active at a time based on type_
    union Value {
        int int_value;
        unsigned uint_value;
        long long longlong_value;
        unsigned long long ulonglong_value;
        double double_value;
        long double longdouble_value;
        bool bool_value;
        char char_value;
        const char* cstring_value;
        std::string_view string_value;
        const void* pointer_value;
        
        // For custom types: store pointer + size
        struct {
            const void* data;
            size_t size;
        } custom_value;
        
        Value() : int_value(0) {}
    } value_;
    
    // Function pointer to format this specific type
    // Different for each type (int has format_int, double has format_double, etc.)
    void (*format_func_)(const FormatArg&, const FormatPart::FormatSpec&, FormatContext&);
    
public:
    FormatArg() : type_(Type::None), format_func_(nullptr) {}
    
    // Constructor for int
    static FormatArg make_int(int value) {
        FormatArg arg;
        arg.type_ = Type::Int;
        arg.value_.int_value = value;
        arg.format_func_ = &format_int_impl;
        return arg;
    }
    
    // Constructor for double
    static FormatArg make_double(double value) {
        FormatArg arg;
        arg.type_ = Type::Double;
        arg.value_.double_value = value;
        arg.format_func_ = &format_double_impl;
        return arg;
    }
    
    // Constructor for C-string
    static FormatArg make_cstring(const char* value) {
        FormatArg arg;
        arg.type_ = Type::CString;
        arg.value_.cstring_value = value;
        arg.format_func_ = &format_cstring_impl;
        return arg;
    }
    
    // Constructor for string_view
    static FormatArg make_string(std::string_view value) {
        FormatArg arg;
        arg.type_ = Type::String;
        arg.value_.string_value = value;
        arg.format_func_ = &format_string_impl;
        return arg;
    }
    
    // Call the formatting function for this type
    void format(const FormatPart::FormatSpec& spec, FormatContext& ctx) const {
        if (format_func_) {
            format_func_(*this, spec, ctx);
        }
    }
    
    Type get_type() const { return type_; }
    
    // Accessors for getting the value back
    int get_int() const { return value_.int_value; }
    double get_double() const { return value_.double_value; }
    const char* get_cstring() const { return value_.cstring_value; }
    std::string_view get_string() const { return value_.string_value; }
    
private:
    // These are defined after FormatContext
    static void format_int_impl(const FormatArg& arg, 
                               const FormatPart::FormatSpec& spec, 
                               FormatContext& ctx);
    static void format_double_impl(const FormatArg& arg, 
                                   const FormatPart::FormatSpec& spec, 
                                   FormatContext& ctx);
    static void format_cstring_impl(const FormatArg& arg, 
                                    const FormatPart::FormatSpec& spec, 
                                    FormatContext& ctx);
    static void format_string_impl(const FormatArg& arg, 
                                   const FormatPart::FormatSpec& spec, 
                                   FormatContext& ctx);
};

// Container for all arguments - type-erased array
class FormatArgs {
    std::vector<FormatArg> args_;
    
public:
    FormatArgs() = default;
    
    void add(FormatArg arg) {
        args_.push_back(std::move(arg));
    }
    
    const FormatArg& operator[](size_t index) const {
        return args_[index];
    }
    
    size_t size() const { return args_.size(); }
};

// ============================================================================
// THE KEY FUNCTION: make_format_args
// This converts a parameter pack of any types into type-erased FormatArgs
// ============================================================================

// Base case: no arguments
inline FormatArgs make_format_args() {
    return FormatArgs{};
}

// Helper: create FormatArg from a single value
// This uses template specialization to handle different types
template<typename T>
FormatArg make_format_arg(const T& value) {
    // This is where type deduction happens!
    // The compiler knows the exact type T and calls the right constructor
    
    if constexpr (std::is_same_v<T, int>) {
        return FormatArg::make_int(value);
    }
    else if constexpr (std::is_same_v<T, double>) {
        return FormatArg::make_double(value);
    }
    else if constexpr (std::is_same_v<T, float>) {
        return FormatArg::make_double(static_cast<double>(value));
    }
    else if constexpr (std::is_same_v<T, const char*>) {
        return FormatArg::make_cstring(value);
    }
    else if constexpr (std::is_same_v<T, std::string>) {
        return FormatArg::make_string(std::string_view(value));
    }
    else if constexpr (std::is_same_v<T, std::string_view>) {
        return FormatArg::make_string(value);
    }
    // Add more types as needed...
    else {
        static_assert(!std::is_same_v<T, T>, "Unsupported type for formatting");
    }
}

// Variadic template to handle any number of arguments
template<typename... Args>
FormatArgs make_format_args(const Args&... args) {
    FormatArgs result;
    
    // Fold expression (C++17): calls make_format_arg for each argument
    // This expands to: result.add(make_format_arg(arg1)), result.add(make_format_arg(arg2)), ...
    (result.add(make_format_arg(args)), ...);
    
    return result;
}

// ============================================================================
// PHASE 3: FORMATTING CONTEXT & OUTPUT BUFFER
// ============================================================================

// Manages the output buffer where formatted text is accumulated
class FormatContext {
    std::string buffer_;
    
public:
    FormatContext() {
        buffer_.reserve(256);  // Pre-allocate reasonable size
    }
    
    // Append text to the buffer
    void append(std::string_view text) {
        buffer_.append(text);
    }
    
    // Append a single character
    void append(char c) {
        buffer_.push_back(c);
    }
    
    // Append multiple copies of a character (for padding)
    void append(size_t count, char c) {
        buffer_.append(count, c);
    }
    
    // Get the final result
    std::string&& result() {
        return std::move(buffer_);
    }
    
    const std::string& result_ref() const {
        return buffer_;
    }
};

// ============================================================================
// PHASE 4: TYPE-SPECIFIC FORMATTERS
// ============================================================================

// Convert integer to string (simplified - real version is optimized)
inline std::string int_to_string(long long value) {
    if (value == 0) return "0";
    
    bool negative = value < 0;
    if (negative) value = -value;
    
    char buffer[32];
    int pos = 31;
    buffer[pos] = '\0';
    
    while (value > 0) {
        buffer[--pos] = '0' + (value % 10);
        value /= 10;
    }
    
    if (negative) {
        buffer[--pos] = '-';
    }
    
    return std::string(buffer + pos);
}

// Convert double to string (simplified - real version handles precision, scientific notation, etc.)
inline std::string double_to_string(double value) {
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%g", value);
    return std::string(buffer);
}

// Apply width and padding to a string
inline std::string apply_width(const std::string& str, 
                               const FormatPart::FormatSpec& spec) {
    if (spec.width <= 0 || static_cast<int>(str.size()) >= spec.width) {
        return str;
    }
    
    int padding = spec.width - static_cast<int>(str.size());
    std::string result;
    result.reserve(spec.width);
    
    // Right-aligned by default (padding on left)
    result.append(padding, spec.fill);
    result.append(str);
    
    return result;
}

// Now we can define the format implementations
void FormatArg::format_int_impl(const FormatArg& arg, 
                               const FormatPart::FormatSpec& spec, 
                               FormatContext& ctx) {
    int value = arg.get_int();
    std::string str = int_to_string(value);
    
    // Apply width/padding if specified
    if (spec.width > 0) {
        str = apply_width(str, spec);
    }
    
    ctx.append(str);
}

void FormatArg::format_double_impl(const FormatArg& arg, 
                                   const FormatPart::FormatSpec& spec, 
                                   FormatContext& ctx) {
    double value = arg.get_double();
    std::string str = double_to_string(value);
    
    if (spec.width > 0) {
        str = apply_width(str, spec);
    }
    
    ctx.append(str);
}

void FormatArg::format_cstring_impl(const FormatArg& arg, 
                                    const FormatPart::FormatSpec& spec, 
                                    FormatContext& ctx) {
    const char* value = arg.get_cstring();
    ctx.append(value ? value : "(null)");
}

void FormatArg::format_string_impl(const FormatArg& arg, 
                                   const FormatPart::FormatSpec& spec, 
                                   FormatContext& ctx) {
    std::string_view value = arg.get_string();
    ctx.append(value);
}

// ============================================================================
// PHASE 5: THE MAIN FORMAT FUNCTION
// ============================================================================

// This is the actual format() function that ties everything together
template<typename... Args>
std::string format(std::string_view fmt_str, const Args&... args) {
    // STEP 1: Parse the format string
    // In real std::format with C++20, this happens at compile-time!
    ParsedFormat parsed = parse_format_string(fmt_str);
    
    // STEP 2: Convert arguments to type-erased storage
    // make_format_args uses template magic to preserve type information
    // while storing everything in a uniform container
    FormatArgs format_args = make_format_args(args...);
    
    // STEP 3: Validate argument count (in real implementation, this is compile-time)
    if (parsed.arg_count != static_cast<int>(format_args.size())) {
        throw std::runtime_error("Argument count mismatch");
    }
    
    // STEP 4: Create the formatting context (output buffer)
    FormatContext ctx;
    
    // STEP 5: Process each part of the parsed format string
    for (const auto& part : parsed.parts) {
        if (part.type == FormatPart::Type::Literal) {
            // Just copy literal text to output
            ctx.append(part.text);
        }
        else if (part.type == FormatPart::Type::Argument) {
            // Get the corresponding argument
            const FormatArg& arg = format_args[part.arg_index];
            
            // Call the type-specific formatter
            // The FormatArg knows which formatting function to call
            // based on its type (stored in format_func_ pointer)
            arg.format(part.spec, ctx);
        }
    }
    
    // STEP 6: Return the formatted string
    return ctx.result();
}

// ============================================================================
// USAGE EXAMPLES & DEMONSTRATION
// ============================================================================

#include <iostream>

int main() {
    // Example 1: Basic formatting
    std::string s1 = format("Hello, {}!", "World");
    std::cout << "Example 1: " << s1 << "\n";
    // Output: "Hello, World!"
    
    // Example 2: Multiple arguments
    std::string s2 = format("x={}, y={}", 42, 3.14);
    std::cout << "Example 2: " << s2 << "\n";
    // Output: "x=42, y=3.14"
    
    // Example 3: With format specifiers
    std::string s3 = format("Number: {:05}", 42);
    std::cout << "Example 3: " << s3 << "\n";
    // Output: "Number: 00042"
    
    // Example 4: Complex format
    std::string s4 = format("Temperature: {}°C, Humidity: {}%", 25, 60);
    std::cout << "Example 4: " << s4 << "\n";
    // Output: "Temperature: 25°C, Humidity: 60%"
    
    // ========================================================================
    // Let's trace what happens for: format("x={}, y={}", 42, 3.14)
    // ========================================================================
    
    std::cout << "\n=== Detailed Trace ===\n";
    
    // 1. Parse format string
    ParsedFormat parsed = parse_format_string("x={}, y={}");
    std::cout << "Parsed into " << parsed.parts.size() << " parts:\n";
    for (size_t i = 0; i < parsed.parts.size(); ++i) {
        if (parsed.parts[i].type == FormatPart::Type::Literal) {
            std::cout << "  Part " << i << ": Literal \"" 
                     << parsed.parts[i].text << "\"\n";
        } else {
            std::cout << "  Part " << i << ": Argument #" 
                     << parsed.parts[i].arg_index << "\n";
        }
    }
    
    // 2. Create format args
    FormatArgs format_args = make_format_args(42, 3.14);
    std::cout << "\nCreated " << format_args.size() << " format arguments:\n";
    std::cout << "  Arg 0: type=" << (int)format_args[0].get_type() 
             << " (Int), value=" << format_args[0].get_int() << "\n";
    std::cout << "  Arg 1: type=" << (int)format_args[1].get_type() 
             << " (Double), value=" << format_args[1].get_double() << "\n";
    
    // 3. Format with context
    FormatContext ctx;
    std::cout << "\nFormatting process:\n";
    
    for (const auto& part : parsed.parts) {
        if (part.type == FormatPart::Type::Literal) {
            ctx.append(part.text);
            std::cout << "  Appended literal: \"" << part.text << "\"\n";
            std::cout << "  Buffer now: \"" << ctx.result_ref() << "\"\n";
        } else {
            format_args[part.arg_index].format(part.spec, ctx);
            std::cout << "  Formatted argument #" << part.arg_index << "\n";
            std::cout << "  Buffer now: \"" << ctx.result_ref() << "\"\n";
        }
    }
    
    std::cout << "\nFinal result: \"" << ctx.result_ref() << "\"\n";
    
    return 0;
}

// ============================================================================
// KEY TAKEAWAYS:
// ============================================================================
//
// 1. COMPILE-TIME PARSING: Format string is parsed once at compile-time
//    (in real std::format), finding all {} placeholders and literal text.
//
// 2. TYPE ERASURE WITH TYPE SAFETY: Arguments are stored in FormatArg
//    (type-erased) but the correct formatting function is remembered via
//    function pointer. This is type-safe because make_format_arg uses
//    template deduction to determine the type at compile-time.
//
// 3. SINGLE PASS, SINGLE ALLOCATION: The output buffer is pre-allocated
//    and all formatting happens in a single pass through the format parts.
//    No virtual calls, no repeated allocations like with streams.
//
// 4. EXTENSIBILITY: To support custom types, you just add a new
//    make_format_arg overload and a new FormatArg constructor with its
//    formatting function. The framework handles the rest.
//
// 5. PERFORMANCE: This design eliminates:
//    - Virtual function calls (stream's operator<< uses virtual dispatch)
//    - Multiple allocations (streams reallocate as they grow)
//    - Runtime format parsing (printf parses format at runtime)
//    - Type unsafety (printf's varargs aren't type-checked)
//
// This is why std::format is 5-10x faster than iostream and safer than printf!
// ============================================================================