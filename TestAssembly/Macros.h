/// synopsis
///     void LOG_<type>(<message>[ << <message>[ << ...]]);                     ///< only each first of given amount of messages gets logged in release build, nothing will be omitted in debug build, the amount it based on <type>
///     void TRACE_OBJECT(<obj>);                                               ///< \attention gets removed for release builds, directly accepts google::protobuf::Message if included before this
///     void TRACE_CONTROLFLOW();                                               ///< \attention gets removed for release builds
///     no-return ABORT(<message>[ << <message>[ << ...]]);                     ///< always logs \param message and terminates application
///     no-return ASSERT(<not null obj>[, <message>[ << <message>[ << ...]]]);  ///< on error \param message will be logged and the application terminated
///     void LOG_APPENDIX_SCOPED(const std::string&/const char*);               ///< \param must be valid until the end of the current stack frame, adds \param ad the end of all LOG_<type> except for LOG_INFO messages, the appendix applies thread_local and is cleared on leaving the current stack frame
///     void LOG_APPENDIX_UPDATE(const std::string&/const char*);               ///< \param must be valid until the end of the stack frame of enclosing \see LOG_APPENDIX_SET, updates the above
///     void LOG_APPENDIX_CLEAR();
///     void LOG_APPENDIX_CUSTOM_<ANT|BIRD|CAT|DOG|EAGLE|...>(const std::string&/const char*);  ///< sets additional data which will only be visible in core dumps and not logged in any way \see LOG_APPENDIX_SET
///
/// \attention all those macros are safe to be called with function call syntax i.e.
///     if (foo)
///         MACRO();
///     is always guaranteed to be safe thus no {bracing} needed
///
/// log <type> (in increasing severity)
///     DEBUG                   ///< \attention gets removed for release builds
///     INFO                    ///< messages about the program state mainly for system supervision
///     DATA_ERROR              ///< data error e.g. data is wrong formatted and can not be processed
///     WARNING
///     GENERAL_ERROR
///     CONFIG_ERROR            ///< configuration error or configuration not plausible
///     IMPL_BUG                ///< prefer \see static_assert, but this is still preferable over \see ASSERT()
///     UNKNOWN_ERROR
///     SHOULD_NEVER_REACH_HERE
///     UNHANDLED_THROW_TYPE
///     FLUSH                   ///< does just flush any written data without actually logging anything, there is nearly never a case to forcefully call it
///
/// remark
///     for release builds each LOG_<type> except for LOG_INFO is muted for 5 minutes after it has been written out
///         and then with the next occurrence printing additionally the number of omitted ones
///     everything in here is only specific to the current thread,
///         e.g. an appendix (with/~out custom) is set for the current thread and overwritten if set again
///         therefore two different threads might legally call LOG_APPENDIX_CUSTOM_ANT(currentTaskToWorkOn)
///     for coverage builds logging/tracing is disabled and replaced with NOP (void(0)) to reduce unnecessary branching
///         therefore coverage is disabled for Macros.h/cpp as there is no need to test these macros/functions
///
/// examples
///     LOG_CONFIG_ERROR("aerospike");
///
///     auto curWeather = ...;
///     LOG_GENERAL_ERROR("the current weather" << curWeather << " is not" << Weather::sunshine);
///
///     auto primaryKey = ...;
///     auto guid = ...;
///     auto jsonObject = ...;
///     LOG_DATA_ERROR("processing JSON key=" << primaryKey << " guid=" << guid << " json=" << jsonObject);
///
///     \attention completely avoid try/catch
///     try { ... }
///     catch (const char* msg) { LOG_INFO(msg); }
///     catch (std::exception ex) { LOG_WARNING(ex.what()); }
///     catch (...) { LOG_UNHANDLED_THROW_TYPE(); }
///
///     TRACE_OBJECT(someOtherObject);
///
///     int i = 42;
///     LOG_DATA_ERROR("i in base10 " << i << "; i in oct " << std::oct << i << "; i in hex 0x" << std::hex << i << "; i normal again " << std::dec << i);
///
///     auto* ptr = ...;
///     ASSERT(ptr);
///     ptr->callSomething();
///
///     auto* ptr = ...;
///     ASSERT(ptr, "ptr is missing, we are doomed");
///     ptr->callSomething();
///
///     enum Foo { Bar, Baz, ... } e;
///     ...
///     switch (e)
///     {
///         case Bar: ...; return;
///         case Baz: ...; return;
///         ...
///     }
///     SHOULD_NEVER_REACH_HERE(); // or [-Wswitch] was raised above, further prefer this over a default label in switch itself to enable that warning
///
///     \see src/tests/logging.Macros.cpp for further examples
///
/// include remarks
///     include this header always as last and always into *.cpp (and not other headers)
///     if <thread> is already included TRACE_CONTROLFLOW will print the current thread id
///     if <google/protobuf/message.h> is already included TRACE_OBJECT will accept those and print it formatted
///     if <google/protobuf/repeated_field> is already included container streaming will work with it as well (which is used by TRACE_OBJECT)
///
/// container streaming
///     which is mainly for logging purpose and thus streaming out,
///     the in streaming versions might not work depending on the template parameters

#ifndef ANG_TOOLS_LOGGING_MACROS_H
#define ANG_TOOLS_LOGGING_MACROS_H
/// LCOV_EXCL_START

#include "std.extensions.h"
#include "common.h"
#include <atomic>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>
#if ANG_DEBUG
#   include <cstdio>
#endif

#define LOG_APPENDIX_SCOPED(X)          ANG::logging::Appendix ___currentAppendix(X)
#define LOG_APPENDIX_UPDATE(X)          ANG::logging::Appendix::current(X)
#define LOG_APPENDIX_CLEAR()            ANG::logging::Appendix::current(std::string_view())
#define LOG_APPENDIX_CUSTOM_ANT(X)      ANG::logging::Appendix::custom<1>(X)
#define LOG_APPENDIX_CUSTOM_BIRD(X)     ANG::logging::Appendix::custom<2>(X)
#define LOG_APPENDIX_CUSTOM_CAT(X)      ANG::logging::Appendix::custom<3>(X)
#define LOG_APPENDIX_CUSTOM_DOG(X)      ANG::logging::Appendix::custom<4>(X)
#define LOG_APPENDIX_CUSTOM_EAGLE(X)    ANG::logging::Appendix::custom<5>(X)
///< define plenty to use, as soon as they are really used
///< it will trigger a compilation error which then can be resolved by increasing the internal buffer
///< but in the mean time the buffer is as small as possible

#if ANG_DEBUG
#   include "debug/terminalColors.h"
#   define ___LOG_COLOR_RED      TERMINAL_COLOR_RED
#   define ___LOG_COLOR_GREEN    TERMINAL_COLOR_GREEN
#   define ___LOG_COLOR_YELLOW   TERMINAL_COLOR_YELLOW
#   define ___LOG_COLOR_BLUE     TERMINAL_COLOR_BLUE
#   define ___LOG_COLOR_PURPLE   TERMINAL_COLOR_PURPLE
#   define ___LOG_COLOR_CYAN     TERMINAL_COLOR_CYAN
#   define ___LOG_COLOR_END      TERMINAL_COLOR_NONE
#else
#   define ___LOG_COLOR_RED
#   define ___LOG_COLOR_GREEN
#   define ___LOG_COLOR_YELLOW
#   define ___LOG_COLOR_BLUE
#   define ___LOG_COLOR_PURPLE ""
#   define ___LOG_COLOR_CYAN
#   define ___LOG_COLOR_END ""
#endif

#if ANG_DEBUG || ANG_COVERAGE
#   define ___LOG_SEV_DBG ___LOG_COLOR_CYAN   "[dbg]\t"   ___LOG_COLOR_END
#   define ___LOG_SEV_STA ___LOG_COLOR_PURPLE "[sta]\t"   ___LOG_COLOR_END
#   define ___LOG_SEV_INF ___LOG_COLOR_GREEN  "[inf]\t"   ___LOG_COLOR_END
#   define ___LOG_SEV_DAT ___LOG_COLOR_YELLOW "[dat]\t"   ___LOG_COLOR_END
#   define ___LOG_SEV_WRN ___LOG_COLOR_YELLOW "[wrn]\t"   ___LOG_COLOR_END
#   define ___LOG_SEV_ERR ___LOG_COLOR_RED    "[err]\t"   ___LOG_COLOR_END
#   define ___LOG_SEV_CFG ___LOG_COLOR_RED    "[cfg]\t"   ___LOG_COLOR_END
#   define ___LOG_SEV_CRI ___LOG_COLOR_RED    "[cri]\t"   ___LOG_COLOR_END
#else
#   define ___LOG_SEV_DBG "7\t" ///< graylog DEBUG
#   define ___LOG_SEV_STA "6\t" ///< graylog INFO
#   define ___LOG_SEV_INF "5\t" ///< graylog NOTICE
#   define ___LOG_SEV_DAT "4\t" ///< graylog WARNING
#   define ___LOG_SEV_WRN "3\t" ///< graylog ERROR
#   define ___LOG_SEV_ERR "2\t" ///< graylog CRITICAL
#   define ___LOG_SEV_CFG "1\t" ///< graylog ALERT
#   define ___LOG_SEV_CRI "0\t" ///< graylog EMERGENCY
#endif

#if ANG_DEBUG
#   define LOG_CURRENT_LOCATION_NOTID   __FILE__ << ':' << __LINE__ << '\t' << __FUNCTION__ << '\t'
#   define LOG_CURRENT_LOCATION         LOG_CURRENT_LOCATION_NOTID << ANG::logging::threadIdColored() << '\t'
#else
#   define LOG_CURRENT_LOCATION_NOTID   __FILE__ << ':' << __LINE__ << '\t' << __FUNCTION__ << '\t'
#   define LOG_CURRENT_LOCATION         LOG_CURRENT_LOCATION_NOTID << ANG::logging::threadId() << '\t'
#endif

#define PUT_ERRNO_MESSAGE_1(ERRNO)                      "errno " << ERRNO << " '" << std::strerror(ERRNO) << '\''
#define PUT_ERRNO_MESSAGE_0()                           PUT_ERRNO_MESSAGE_1(ANG::logging::errno_copy())
#define PUT_ERRNO_MESSAGE_FUNC_CHOOSER(_1, X, ...)      X
#define PUT_ERRNO_MESSAGE_FUNC_RECOMPOSER(X)            PUT_ERRNO_MESSAGE_FUNC_CHOOSER X
#define PUT_ERRNO_MESSAGE_CHOOSE_FROM_ARG_COUNT(...)    PUT_ERRNO_MESSAGE_FUNC_RECOMPOSER((__VA_ARGS__, PUT_ERRNO_MESSAGE_1, ))
#define PUT_ERRNO_MESSAGE_NO_ARG_EXPANDER()             , PUT_ERRNO_MESSAGE_0
#define PUT_ERRNO_MESSAGE_MACRO_CHOOSER(...)            PUT_ERRNO_MESSAGE_CHOOSE_FROM_ARG_COUNT(PUT_ERRNO_MESSAGE_NO_ARG_EXPANDER __VA_ARGS__ ())
/// only call this version \param is optional integer variable containing an error number
#define PUT_ERRNO_MESSAGE(...)                          PUT_ERRNO_MESSAGE_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

#if ANG_DEBUG
#   include <cstring>
#   define TRACE_CONTROLFLOW()          ((void)(ANG::logging::Sink() << ___LOG_SEV_DBG << LOG_CURRENT_LOCATION << ___LOG_COLOR_CYAN "tracing control flow" ___LOG_COLOR_END "\tticks=" << ANG::logging::formatTicksSinceBoot() << '\n'))
#   define TRACE_OBJECT(obj)            ((void)(ANG::logging::Sink() << ___LOG_SEV_DBG << LOG_CURRENT_LOCATION << ___LOG_COLOR_CYAN "tracing " << ANG::logging::extractObjectType(#obj) << ___LOG_COLOR_END "\t" << ANG::logging::format(obj) << '\n'))
#   define TRACE_BINARY(stdstr)         ((void)(ANG::logging::Sink() << ___LOG_SEV_DBG << LOG_CURRENT_LOCATION << ___LOG_COLOR_CYAN "tracing " #stdstr ___LOG_COLOR_END "\t" << ANG::logging::formatBinary(stdstr) << '\n'))
#   define TRACE_TYPE(T)                ((void)(ANG::logging::Sink() << ___LOG_SEV_DBG << LOG_CURRENT_LOCATION << ___LOG_COLOR_CYAN "typename " #T ___LOG_COLOR_END "\t" << ANG::logging::demangleType<T>() << '\n'))
#   define TRACE_DEMANGLE(id)           ((void)(ANG::logging::Sink() << ___LOG_SEV_DBG << LOG_CURRENT_LOCATION << ___LOG_COLOR_CYAN "typename " ___LOG_COLOR_END "\t" << ANG::logging::demangleType(id) << '\n'))
#   define TRACE_ERRNO()                ((void)(ANG::logging::Sink() << ___LOG_SEV_DBG << LOG_CURRENT_LOCATION << ___LOG_COLOR_CYAN "errno " ___LOG_COLOR_END "\t" << ANG::logging::errno_copy() << " '" << std::strerror(ANG::logging::errno_copy()) << '\'' << '\n'))
#   define LOG_DEBUG(msg)               ((void)(ANG::logging::Sink() << ___LOG_SEV_DBG << LOG_CURRENT_LOCATION << msg << '\n'))
#else
#   define TRACE_CONTROLFLOW()          ((void)(0))
#   define TRACE_OBJECT(obj)            ((void)(0))
#   define TRACE_BINARY(stdstr)         ((void)(0))
#   define TRACE_TYPE(T)                ((void)(0))
#   define TRACE_DEMANGLE(id)           ((void)(0))
#   define TRACE_ERRNO()                ((void)(0))
#   define LOG_DEBUG(msg)               ((void)(0))
#endif

#if ANG_COVERAGE
#   define LOG_STATISTIC(msg)           ((void)(0))
#   define LOG_INFO(msg)                ((void)(0))
#   define LOG_DATA_ERROR(msg)          ((void)(0))
#   define LOG_WARNING(msg)             ((void)(0))
#   define LOG_GENERAL_ERROR(msg)       ((void)(0))
#   define LOG_CONFIG_ERROR(msg)        ((void)(0))
#   define LOG_IMPL_BUG(msg)            ((void)(0))
#   define ABORT(msg)                   std::abort()
#else
#   define LOG_STATISTIC(msg)                                                   ((void)(ANG::logging::Sink() << ___LOG_SEV_STA << LOG_CURRENT_LOCATION << msg <<                                 '\n'))
#   define LOG_INFO(msg)                                                        ((void)(ANG::logging::Sink() << ___LOG_SEV_INF << LOG_CURRENT_LOCATION << msg << ANG::logging::writeAppendix  << '\n'))
#   define LOG_DATA_ERROR(msg)          ___log_omit<__LINE__>() ? ((void)(0)) : ((void)(ANG::logging::Sink() << ___LOG_SEV_DAT << LOG_CURRENT_LOCATION << msg << ___log_omit<__LINE__>()      << '\n'))
#   define LOG_WARNING(msg)             ___log_omit<__LINE__>() ? ((void)(0)) : ((void)(ANG::logging::Sink() << ___LOG_SEV_WRN << LOG_CURRENT_LOCATION << msg << ___log_omit<__LINE__>()      << '\n'))
#   define LOG_GENERAL_ERROR(msg)       ___log_omit<__LINE__>() ? ((void)(0)) : ((void)(ANG::logging::Sink() << ___LOG_SEV_ERR << LOG_CURRENT_LOCATION << msg << ___log_omit<__LINE__>()      << '\n'))
#   define LOG_CONFIG_ERROR(msg)        ___log_omit<__LINE__>() ? ((void)(0)) : ((void)(ANG::logging::Sink() << ___LOG_SEV_CFG << LOG_CURRENT_LOCATION << msg << ___log_omit<__LINE__>()      << '\n'))
///@{ not possible via operator, \see https://en.cppreference.com/w/cpp/language/operator_other Built-in comma operator
#   if ANG_DEBUG
#       define LOG_IMPL_BUG(msg)    do { if (!___log_omit<__LINE__>())  ANG::logging::Sink() << ___LOG_SEV_CRI << LOG_CURRENT_LOCATION << msg << ___log_omit<__LINE__>() << '\n'; if (ANG::logging::isDebuggerBreakEnabled()) asm("int $3");} while (false)
#   else
#       define LOG_IMPL_BUG(msg)    do { if (!___log_omit<__LINE__>())  ANG::logging::Sink() << ___LOG_SEV_CRI << LOG_CURRENT_LOCATION << msg << ___log_omit<__LINE__>() << '\n';                                                           } while (false)
#   endif
#   define ABORT(msg)               do {                                ANG::logging::Sink() << ___LOG_SEV_CRI << LOG_CURRENT_LOCATION << "terminating application due to: " << msg << ANG::logging::writeAppendix << '\n'; std::abort();   } while (false)
///@}
#endif

#define LOG_UNKNOWN_ERROR()             LOG_GENERAL_ERROR("unknown error")
#define LOG_SHOULD_NEVER_REACH_HERE()   LOG_IMPL_BUG("should never reach here")
#define LOG_UNHANDLED_THROW_TYPE()      LOG_IMPL_BUG("any possibly thrown type should be handled, but currently caught: " << ANG::logging::currentExceptionTypeName())
#define LOG_PROTO_SENTINEL_CASE_FOR(TYPE) /***********************************************/ \
    case std::numeric_limits<std::underlying_type_t<TYPE>>::min():                          \
    case std::numeric_limits<std::underlying_type_t<TYPE>>::max():                          \
        LOG_IMPL_BUG("should never reach here, internal proto sentinel used on switch");    \
        break /*****************************************************************************/

#define LOG_FLUSH()                     ANG::logging::Sink().flush()

///@{ overloads of ASSERT macro to call it with one or both arguments TODO: replace with C++20 contracts
#define ASSERT_2(notNullCheck, msg)     do { if (!(notNullCheck)) /*TODO [[unlikely]]*/ ABORT(msg); } while(false)
#define ASSERT_1(X)                     ASSERT_2(X, "'" ___LOG_COLOR_YELLOW #X ___LOG_COLOR_END "' is nullptr/false")
#define ASSERT_X(_1,_2,X,...)           X
#define ASSERT(...)                     ASSERT_X(__VA_ARGS__, ASSERT_2, ASSERT_1)(__VA_ARGS__) ///< only call this version
///@}

#if ANG_DEBUG
    /// hard coded break point
#   define DEBUGGER_BREAK() /*************************/ \
        do                                              \
        {                                               \
            if (ANG::logging::isDebuggerBreakEnabled()) \
                asm("int $3");                          \
        } while(false) /********************************/
    /// application pauses itself then gdb may be attached
    /// and 'handle SIGSTOP nostop noprint nopass' issued
    /// in order to disable further interruptions from this
#   define DEBUGGER_WAITFOR() ANG::logging::raiseSigstop()
#else
#   define DEBUGGER_BREAK() ((void)(0))
#   define DEBUGGER_WAITFOR() ((void)(0))
#endif

#if ANG_DEBUG
    namespace ANG::logging
    {
        using nanos = unsigned long long int;

        inline const char* formatTicksSinceBoot()
        {
            static char buffer[64];
            struct timespec t;
            bool success = clock_gettime(CLOCK_MONOTONIC, &t) == 0;
            nanos nano = success ? t.tv_nsec : 0;
            nanos sec = success ? t.tv_sec : 0;
            nanos min = sec / 60;
            nanos hour = min / 60;
            min %= 60;
            sec %= 60;
            std::snprintf(buffer, sizeof(buffer)/sizeof(buffer[0]), "%llu:%.2llu:%.2llu.%.9llu",  hour, min, sec, nano);
            return buffer;
        }
    }
#endif

namespace ANG::logging
{
    class RaiiTestSink : public std::stringstream
    {
    public:
        RaiiTestSink           () = default;
        RaiiTestSink           (const RaiiTestSink&) = delete;
        RaiiTestSink& operator=(const RaiiTestSink&) = delete;
        RaiiTestSink           (RaiiTestSink&& other) : std::stringstream(std::move(other)) {}
        RaiiTestSink& operator=(RaiiTestSink&& other) { swap(other); return *this; }
        ~RaiiTestSink          ();
    };

    std::ostream&                       writeAppendix(std::ostream&); ///< custom ostream modifier / data appender
    bool                                isDebuggerBreakEnabled(); ///< \returns always false on release builds
    void                                setUnittestExecution();
    [[nodiscard]] RaiiTestSink          enableTestSink();
    bool                                mute(bool=true); ///< mutes entire logging but messages are still generated because of that release builds still log everything and ignore calls to this \returns if it is indeed muted now
    std::string                         formatBinary(const std::string&);
    std::string                         currentExceptionTypeName();
    std::string                         demangleType(const char*);
    template<typename T> std::string    demangleType() { return demangleType(typeid(T).name()); }
    int                                 threadId();
    int&                                errno_copy();
#   if ANG_DEBUG
        std::string                     threadIdColored();
        void                            raiseSigstop();
#   endif

    inline std::string_view extractObjectType(std::string_view code)
    {
        if (code[0] == '"')
            return "control flow";
        if (code.starts_with("[&]()"))
            return "lambda evaluated";
        if (code.starts_with("[&"))
        {
            auto pos = code.find("](");
            if (pos != std::string::npos)
                return code.substr(2, pos-2);
        }
        return code;
    }

    class Omit ///< if accessed by multiple threads the state of it might get mixed which would in worst case skip the actual logging and thus omitting it a bit more/longer
    {
    public:
        friend std::ostream& operator<<(std::ostream&, Omit&); ///< appends \see Omit and \see Appendix

        Omit()              { reset(); }
        operator bool()     { return m & 0x8000; }
        Omit& toggle()      { m.fetch_and(0x7FFF); return *this; }
        Omit& operator++()  { m++; return *this; }
        int reset()         { int i = m; m = 0x8000; return i; }

    private:
        std::atomic_uint16_t m;
    };

    class Appendix ///< thread safe
    {
    public:
        static std::string_view current(std::string_view);
        template<std::size_t N> static std::string_view custom(std::string_view = std::string_view("get", 3)); ///< implementation missing here by intent \see LOG_APPENDIX_CUSTOM_ANT

        explicit Appendix(std::string_view msg) : m_prev(current(msg)) {}
        ~Appendix() { current(m_prev); }

        Appendix           (const Appendix&) = delete;
        Appendix& operator=(const Appendix&) = delete;
        Appendix           (Appendix&&) = delete;
        Appendix& operator=(Appendix&&) = delete;

    private:
         std::string_view m_prev;
    };

    class Sink : public std::stringstream
    {
    public:
        Sink();
        ~Sink();
    };

    struct HttpRequest
    {
        ///@{ forward available ctors of \see std::string_view
        HttpRequest(const std::string_view& data) : data(data) {}
        HttpRequest(const char* cstr, std::size_t count) : data(cstr, count) {}
        HttpRequest(const char* cstr) : data(cstr) {}
        template<class It, class End> HttpRequest(It first, End last) : data(first, last) {}
      //template<class R> HttpRequest(R&& r) : data(std::move(r)) {} TODO c++23 \attention to remove this form overload resolution here must be the same constraints applied as on \see std::basic_string_view(R&&) itself
        ///@}

        template<typename _CharT, typename _Traits> friend inline std::basic_ostream<_CharT, _Traits>& operator<<(std::basic_ostream<_CharT, _Traits>& os, HttpRequest http)
        {
            http.format<>(os, http.data);
            return os;
        }

        static std::string& format(std::string& target, std::string_view value)
        {
            HttpRequest(value).format<strappend>(target, value);
            return target;
        }

    private:
        struct streamout { streamout(auto& target, std::string_view value ) { target << value; } };
        struct strappend { strappend(auto& target, std::string_view value ) { target.append(value); } };

        template<typename append = streamout> void format(auto& target, std::string_view& data)
        {
            while (!data.empty())
            {
                auto pos = data.find('\n');
                if (pos == std::string::npos)
                {
                    append(target, data);
                    break;
                }
                if (pos && data[pos-1] == '\r') // found "something\r\n" turn into Graylog multi line message
                {
                    append(target, data.substr(0, pos-1));
                    append(target, "\n\t");
                }
                else if (pos+1 < data.length() && data[pos+1] == '\t') // found "something\n\t" which is already a Graylog multi line message
                    append(target, data.substr(0, pos+1));
                else
                {
                    append(target,data.substr(0, pos));
                    append(target, "\\n");
                }
                data.remove_prefix(pos+1);
            }
        }

        std::string_view data;
    };
    using EscapeNewLine = HttpRequest;
}

/// due to this anonymous namespace each translation unit will get its own template instantiated function
/// further will the line number used as template parameter ensures that each line will have its own function and thus a counter
namespace
{
#   ifndef ___LOG_OMIT_PERIOD
#       if ANG_DEBUG
#           define ___LOG_OMIT_PERIOD 0
#       else
#           define ___LOG_OMIT_PERIOD 300
#       endif
#   endif

    template<int T> ANG::logging::Omit& ___log_omit()
    {
        static ANG::logging::Omit omit;
        static std::time_t last = 0;
        if constexpr (___LOG_OMIT_PERIOD == 0)
            return omit.toggle();
        else if (!omit)
            return omit;
        else if ((last + ___LOG_OMIT_PERIOD) < std::time(nullptr))
        {
            last = std::time(nullptr);
            return omit.toggle();
        }
        else
            return ++omit;
    }

#   undef ___LOG_OMIT_PERIOD
}

#if __has_include(<google/protobuf/message.h>)
#   include <google/protobuf/message.h>
    inline void logProtobufHandler(google::protobuf::LogLevel level, const char* filename, int line, const std::string& message)
    {
        // get function pointer as the two calls need to use the same one thus they require to be on same line
        // but get a separate one for each line
        using funcptr = ANG::logging::Omit&(*)();
        funcptr lomit = &___log_omit<__LINE__>; // fallback case without default label in switch, so that [-Wswitch] could still be raised
        switch (level)
        {
        case google::protobuf::LOGLEVEL_INFO:   lomit = &___log_omit<__LINE__>; break;
        case google::protobuf::LOGLEVEL_WARNING:lomit = &___log_omit<__LINE__>; break;
        case google::protobuf::LOGLEVEL_ERROR:  lomit = &___log_omit<__LINE__>; break;
        case google::protobuf::LOGLEVEL_FATAL:  lomit = &___log_omit<__LINE__>; break;
        }
        if (lomit())
            return;
        auto sink = ANG::logging::Sink();
        switch (level)
        {
        case google::protobuf::LOGLEVEL_INFO:   sink << ___LOG_SEV_INF; break;
        case google::protobuf::LOGLEVEL_WARNING:sink << ___LOG_SEV_WRN; break;
        case google::protobuf::LOGLEVEL_ERROR:  sink << ___LOG_SEV_ERR; break;
        default: // fine as [-Wswitch] would be raised in switch above
        case google::protobuf::LOGLEVEL_FATAL:  sink << ___LOG_SEV_CRI; break;
        }
        sink << filename << ':' << line << ___LOG_COLOR_CYAN "\t" << __FUNCTION__ << "\t" ___LOG_COLOR_END;
#       if (ANG_DEBUG)
           sink << ANG::logging::threadIdColored();
#       else
           sink << ANG::logging::threadId();
#       endif
        sink << '\t' << message << lomit() << '\n';
    }
#endif

/// === container streaming ==================================================================================================
namespace ANG::logging
{
    template<typename T> struct ref { const T& m; };
#if __has_include(<google/protobuf/message.h>)
        /// for usage like
        ///     TRACE_OBJECT(protoBufObject)
        /// \attention may never be written into production application log
        template<typename T> std::string format(const T& val, typename std::enable_if<std::is_base_of_v<google::protobuf::Message, T>>::type* = 0)
        {
            std::string tmp = "\n{code:proto}\n";
            tmp += val.DebugString();
            tmp += "\n{code}";
            return tmp;
        }
        template<typename T> inline ref<T> format(const T& val, typename std::enable_if<!std::is_base_of_v<google::protobuf::Message, T>>::type* = 0) { return { val }; }
#   else
        template<typename T> inline ref<T> format(const T& val) { return { val }; }
#   endif

    /// for usage of
    ///     TRACE_OBJECT(this)
    ///     and other pointers
    template<typename _CharT, typename _Traits>
    inline void print(std::basic_ostream<_CharT, _Traits>& out, ref<void*> r)
    {
        out << r.m;
    }

    /// for usage of
    ///     TRACE_OBJECT("<?xml ...")
    void printXml(std::ostream&, std::string_view);
    template<typename _CharT, typename _Traits>
    inline void print(std::basic_ostream<_CharT, _Traits>& out, ref<std::string_view> r)
    {
        if constexpr (std::is_same_v<decltype(out), std::ostream&>)
        {
            bool isXml = r.m.compare(0, 5, "<?xml") == 0 || (r.m.length() > 2 && r.m[0] == '<' && std::isalpha(static_cast<unsigned char>(r.m[1])));
            if (isXml)
            {
                printXml(out, r.m);
                return;
            }
        }
        out << r.m;
    }

    template<typename _CharT, typename _Traits>
    inline void print(std::basic_ostream<_CharT, _Traits>& out, ref<std::string> r)
    {
        std::string_view tmp(r.m);
        print(out, ref<std::string_view>{tmp});
    }

    template<typename _CharT, typename _Traits>
    inline void print(std::basic_ostream<_CharT, _Traits>& out, ref<const std::string&> r)
    {
        std::string_view tmp(r.m);
        print(out, ref<std::string_view>{tmp});
    }

    //template<typename _CharT, typename _Traits>
    //inline void print(std::basic_ostream<_CharT, _Traits>& out, ref<const char*> r)
    //{
    //    std::string_view tmp(r.m);
    //    print(out, ref<std::string_view>{tmp});
    //}

    /// avoids program termination in case of
    ///     const char* text = nullptr;
    ///     TRACE_OBJECT(text);
    template<typename _CharT, typename _Traits>
    inline void print(std::basic_ostream<_CharT, _Traits>& out, ref<const _CharT*> r)
    {
        if (r.m)
        {
            std::string_view tmp(r.m);
            print(out, ref<std::string_view>{tmp});
        }
        else
            out << "<nullptr>";
    }

    template <typename _CharT, typename _Traits, typename T, typename = void>
    struct isOstreamDefined : std::false_type {};
    template<typename _CharT, typename _Traits, typename T>
    struct isOstreamDefined
        <
            _CharT,
            _Traits,
            T,
            std::void_t
            <
                decltype //< yields return type of operator<< below if it actually exists
                (                       // and if so this partial specialization gets valid
                    operator<<
                    (
                        std::declval<std::basic_ostream<_CharT, _Traits>&>(),
                        std::declval<T>()
                    )
                )
            >
        > : std::true_type {};
    template<typename _CharT, typename _Traits, typename T>
    inline constexpr bool isOstreamDefined_v = isOstreamDefined<_CharT, _Traits, T>::value;

    template<typename _CharT, typename _Traits, typename T>
    inline void print(std::basic_ostream<_CharT, _Traits>& out, ref<T> r)
    {
        if constexpr (isOstreamDefined_v<_CharT, _Traits, T>)
            operator<<(out, r.m);
        else if constexpr (std::is_enum_v<T>) ///< might actually \see std::is_scoped_enum_v<> from C++23 but maybe there essentially no difference in that case
            out << std::underlying_type_t<T>(r.m);
        else if constexpr (std::is_variant_v<T>)
            out << r.m;
        else if constexpr (std::is_tuple_v<T>)
            out << r.m;
        else
            out << r.m;
    }

///@{ TODO those (or variants) could replace all others
/// and in particular the MACROS below
/// further not even \see ref might be required anymore
    template<typename _CharT, typename _Traits, typename T0, typename ... Ts>
    inline void print(std::basic_ostream<_CharT, _Traits>& out, ref<std::variant<T0, Ts...>> r) ///< TODO add overload for \see std::tuple and \see std::pair
    {
        std::visit([&out](auto&& arg){ out << format(arg); }, r.m);
    }

    template<typename T> concept Streamable = requires(std::ostream& os, T value)
    {
        { os << value };
    };

    template<typename _CharT, typename _Traits, Streamable T>
    inline void print(std::basic_ostream<_CharT, _Traits>& out, T r)
    {
        out << r;
    }

    template<typename _CharT, typename _Traits, std::ranges::range T>
    inline void print(std::basic_ostream<_CharT, _Traits>& out, ref<T> r)
    {
        using namespace std::literals;
        std::string_view splitter = "{ "sv;
        for (const auto& current : r.m)
        {
            out << splitter;
            splitter = ", "sv;
            print(out, current);
        }
        if (splitter[0] == ',') // at least one element has been printed
            out << " }"sv;
    }
///@}

    template<typename _CharT, typename _Traits, typename T>
    inline std::basic_ostream<_CharT, _Traits>&
    operator<<(std::basic_ostream<_CharT, _Traits>& out, ref<T> r)
    {
        print(out, r);
        return out;
    }
}

#   define FUNCTION_T1(container, t1) /***************************************************************/ \
        namespace ANG::logging {                                                                        \
            template<typename _CharT, typename _Traits, t1 T>                                           \
            inline void print(std::basic_ostream<_CharT, _Traits>& out, ref<container<T>> r) {
#   define FUNCTION_T2(container, t1, t2) /***********************************************************/ \
        namespace ANG::logging {                                                                        \
            template<typename _CharT, typename _Traits, t1 T, t2 U>                                     \
            inline void print(std::basic_ostream<_CharT, _Traits>& out, ref<container<T, U>> r) {
#   define FUNCTION_T3(container, t1, t2, t3) /*******************************************************/ \
        namespace ANG::logging {                                                                        \
            template<typename _CharT, typename _Traits, t1 T, t2 U, t3 V>                               \
            inline void print(std::basic_ostream<_CharT, _Traits>& out, ref<container<T, U, V>> r) { /**/

#   define LOOP_BEGIN /***********************/ \
                bool first = true;              \
                out << '{';                     \
                for (const auto& current : r.m) \
                {                               \
                    if (!first)                 \
                        out << ", ";
#   define LOOP_END /*************************/ \
                    first = false;              \
                }                               \
                out << '}';
#   define LOOP_SIMPLE /**********************/ \
        LOOP_BEGIN                              \
        out << format(current);                 \
        LOOP_END
#   define LOOP_MAPPING /*********************/ \
        LOOP_BEGIN                              \
        out << format(std::get<0>(current))     \
            << '='                              \
            << format(std::get<1>(current));    \
        LOOP_END /******************************/

#if __has_include(<google/protobuf/message.h>)
#   include <google/protobuf/message.h>
    FUNCTION_T1(google::protobuf::RepeatedField, typename)
        LOOP_SIMPLE
    }}
#endif

#if __has_include(<google/protobuf/repeated_field.h>)
#   include <google/protobuf/repeated_field.h>
    namespace ANG::logging { template<typename _CharT, typename _Traits, typename T>
    inline void print(std::basic_ostream<_CharT, _Traits>& out, ref<google::protobuf::RepeatedPtrField<T>> r)
    {
        LOOP_SIMPLE
    }}
#endif

#if __has_include(<array>)
#   include <array>
    FUNCTION_T2(std::array, typename, std::size_t)
        LOOP_SIMPLE
    }}
#endif

#if __has_include(<vector>)
#   include <vector>
    FUNCTION_T2(std::vector, typename, typename)
        LOOP_SIMPLE
    }}
#endif

#if __has_include(<map>)
#   include <map>
    FUNCTION_T3(std::map, typename, typename, typename)
        LOOP_MAPPING
    }}

    FUNCTION_T3(std::multimap, typename, typename, typename)
        LOOP_MAPPING
    }}
#endif

#if __has_include(<unordered_map>)
#   include <unordered_map>
    FUNCTION_T3(std::unordered_map, typename, typename, typename)
        LOOP_MAPPING
    }}

    FUNCTION_T3(std::unordered_multimap, typename, typename, typename)
        LOOP_MAPPING
    }}
#endif

#if __has_include(<set>)
#   include <set>
    FUNCTION_T3(std::set, typename, typename, typename)
        LOOP_SIMPLE
    }}

    FUNCTION_T3(std::multiset, typename, typename, typename)
        LOOP_SIMPLE
    }}
#endif

#if __has_include(<unordered_set>)
#   include <unordered_set>
    FUNCTION_T3(std::unordered_set, typename, typename, typename)
        LOOP_SIMPLE
    }}

    FUNCTION_T3(std::unordered_multiset, typename, typename, typename)
        LOOP_SIMPLE
    }}
#endif

#if __has_include(<utility>)
#   include <utility>
    FUNCTION_T2(std::pair, typename, typename)
        out << '<';
        out << format(r.m.first);
        out << ", ";
        out << format(r.m.second);
        out << '>';
    }}
#endif

/// TODO add \see std::optional and \see std::variant

#ifdef FUNCTION_T2
#   undef FUNCTION_T2
#endif
#ifdef FUNCTION_T3
#   undef FUNCTION_T3
#endif
#ifdef LOOP_BEGIN
#   undef LOOP_BEGIN
#endif
#ifdef LOOP_END
#   undef LOOP_END
#endif

/// LCOV_EXCL_STOP
#endif
