#pragma once

namespace engine
{
/// <summary>
/// ログを出力するクラスです。
/// </summary>
class Logger
{
public:
    /// <summary>
    /// typeidの名前から、最後の':'または空白より後ろの部分を型名として取り出します。
    /// </summary>
    /// <param name="typeid_name">typeid(T).name()の値</param>
    static std::string GetTypeName(const char *typeid_name)
    {
        const auto str = std::string(typeid_name);
        auto n = str.find_last_of(':');
        if (n == std::string::npos)
        {
            n = str.find_last_of(' ');
            if (n == std::string::npos)
            {
                Error("Could not find type name for %s", typeid_name);
                n = 5;
            }
        }

        return str.substr(n + 1);
    }

    /// <summary>
    /// printf形式で文字列をフォーマットします。
    /// </summary>
    /// <param name="fmt">printf形式の書式</param>
    /// <param name="args">書式に渡す引数</param>
    /// <returns>フォーマットされた文字列。失敗した場合 nullptr</returns>
    template <typename... Args>
    static std::unique_ptr<char[]> FormatString(const char *fmt, Args... args)
    {
        const auto size_s = std::snprintf(nullptr, 0, fmt, args...) + 1;
        if (size_s <= 0)
        {
            return nullptr;
        }
        const auto size = static_cast<size_t>(size_s);
        std::unique_ptr<char[]> buffer(new char[size]);
        std::snprintf(buffer.get(), size, fmt, args...);
        return buffer;
    }

    /* Info Alias */
    /// <summary>
    /// 指定されたlevelを付けて標準出力にログを出力します。
    /// </summary>
    /// <param name="level">先頭に付ける文字列</param>
    /// <param name="msg">printf形式の書式</param>
    /// <param name="args">書式に渡す引数</param>
    template <typename... Args>
    static void Log(const char *level, const char *msg, Args... args)
    {
        std::cout << level << ": " << FormatString(msg, args...) << std::endl;
    }

    /// <summary>
    /// "INFO"のログを標準出力に出力します。
    /// </summary>
    /// <param name="msg">printf形式の書式</param>
    /// <param name="args">書式に渡す引数</param>
    template <typename... Args>
    static void Log(const char *msg, Args... args)
    {
        Log("INFO", msg, args...);
    }

    /// <summary>
    /// T のクラス名を付けて"INFO"のログを標準出力に出力します。
    /// </summary>
    /// <param name="msg">出力する文字列</param>
    template <typename T>
    static void Log(const char *msg)
    {
        Log("INFO", "[%s] %s", GetTypeName(typeid(T).name()).c_str(), msg);
    }

    /// <summary>
    /// T のクラス名を付けて"INFO"のログを標準出力に出力します。
    /// </summary>
    /// <param name="msg">printf形式の書式</param>
    /// <param name="args">書式に渡す引数</param>
    template <typename T, typename... Args>
    static void Log(const char *msg, Args... args)
    {
        Log<T>(FormatString(msg, args...).get());
    }

    /* Error Alias */
    /// <summary>
    /// 指定されたlevelを付けて標準エラー出力にログを出力します。
    /// </summary>
    /// <param name="level">先頭に付ける文字列</param>
    /// <param name="msg">printf形式の書式</param>
    /// <param name="args">書式に渡す引数</param>
    template <typename... Args>
    static void Error(const char *level, const char *msg, Args... args)
    {
        std::cerr << level << ": " << FormatString(msg, args...) << std::endl;
    }

    /// <summary>
    /// "ERROR"のログを標準エラー出力に出力します。
    /// </summary>
    /// <param name="msg">printf形式の書式</param>
    /// <param name="args">書式に渡す引数</param>
    template <typename... Args>
    static void Error(const char *msg, Args... args)
    {
        Error("ERROR", msg, args...);
    }

    /// <summary>
    /// T のクラス名を付けて"ERROR"のログを標準エラー出力に出力します。
    /// </summary>
    /// <param name="msg">出力する文字列</param>
    template <typename T>
    static void Error(const char *msg)
    {
        Error("ERROR", "[%s] %s", GetTypeName(typeid(T).name()).c_str(), msg);
    }

    /// <summary>
    /// T のクラス名を付けて"ERROR"のログを標準エラー出力に出力します。
    /// </summary>
    /// <param name="msg">printf形式の書式</param>
    /// <param name="args">書式に渡す引数</param>
    template <typename T, typename... Args>
    static void Error(const char *msg, Args... args)
    {
        Error<T>(FormatString(msg, args...).get());
    }

    /* Warn Alias */
    /// <summary>
    /// 指定されたlevelを付けて標準エラー出力にログを出力します。
    /// </summary>
    /// <param name="level">先頭に付ける文字列</param>
    /// <param name="msg">printf形式の書式</param>
    /// <param name="args">書式に渡す引数</param>
    template <typename... Args>
    static void Warn(const char *level, const char *msg, Args... args)
    {
        std::cerr << level << ": " << FormatString(msg, args...) << std::endl;
    }

    /// <summary>
    /// "WARN"のログを標準エラー出力に出力します。
    /// </summary>
    /// <param name="msg">printf形式の書式</param>
    /// <param name="args">書式に渡す引数</param>
    template <typename... Args>
    static void Warn(const char *msg, Args... args)
    {
        Error("WARN", msg, args...);
    }

    /// <summary>
    /// T のクラス名を付けて"WARN"のログを標準エラー出力に出力します。
    /// </summary>
    /// <param name="msg">出力する文字列</param>
    template <typename T>
    static void Warn(const char *msg)
    {
        Error("WARN", "[%s] %s", GetTypeName(typeid(T).name()).c_str(), msg);
    }

    /// <summary>
    /// T のクラス名を付けて"WARN"のログを標準エラー出力に出力します。
    /// </summary>
    /// <param name="msg">printf形式の書式</param>
    /// <param name="args">書式に渡す引数</param>
    template <typename T, typename... Args>
    static void Warn(const char *msg, Args... args)
    {
        Warn<T>(FormatString(msg, args...).get());
    }
};
}