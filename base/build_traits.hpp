#pragma once

#include <type_traits>

namespace jkgm {
    enum class build { release, debug };
    enum class architecture { x86 };
    enum class platform { windows };
    enum class platform_family { win32 };

    class build_traits {
    public:
#ifdef BUILD_RELEASE
        static constexpr jkgm::build build = jkgm::build::release;
#elif BUILD_DEBUG
        static constexpr jkgm::build build = jkgm::build::debug;
#else
#error "build_traits does not support this build type"
#endif

#ifdef ARCHITECTURE_X86
        static constexpr jkgm::architecture architecture = jkgm::architecture::x86;
#else
#error "build_traits does not support this architecture type"
#endif

#ifdef PLATFORM_WINDOWS
        static constexpr jkgm::platform platform = jkgm::platform::windows;
#else
#error "build_traits does not support this platform type"
#endif

        static constexpr bool is_release = (build == jkgm::build::release);
        static constexpr bool is_debug = (build == jkgm::build::debug);

        static constexpr bool is_windows = (platform == jkgm::platform::windows);

        static constexpr platform_family family = platform_family::win32;
    };

    template <class T>
    struct win32_version {
        using type = T;
        static constexpr bool enabled = (build_traits::family == platform_family::win32);
    };

    namespace detail {
        template <class... T>
        struct count_enabled;

        template <class HeadT, class... TailT>
        struct count_enabled<HeadT, TailT...> {
            static constexpr int value = (HeadT::enabled ? 1 : 0) + count_enabled<TailT...>::value;
        };

        template <>
        struct count_enabled<> {
            static constexpr int value = 0;
        };

        template <class... T>
        struct select_enabled;

        template <class HeadT, class... TailT>
        struct select_enabled<HeadT, TailT...> {
            using type = std::conditional_t<HeadT::enabled,
                                            typename HeadT::type,
                                            typename select_enabled<TailT...>::type>;
        };

        template <>
        struct select_enabled<> {
            using type = void;
        };
    }

    template <class... T>
    struct specialization {
        static_assert(detail::count_enabled<T...>::value == 1,
                      "must provide exactly one active specialization");
        using type = typename detail::select_enabled<T...>::type;
    };

    template <class... T>
    using specialization_t = typename specialization<T...>::type;
}
