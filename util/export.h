
#ifndef EASY3D_UTIL_EXPORT_H
#define EASY3D_UTIL_EXPORT_H

#ifdef EASY3D_UTIL_STATIC_DEFINE
#  define EASY3D_UTIL_EXPORT
#  define EASY3D_UTIL_NO_EXPORT
#else
#  ifndef EASY3D_UTIL_EXPORT
#    ifdef easy3d_util_EXPORTS
        /* We are building this library */
#      define EASY3D_UTIL_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define EASY3D_UTIL_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef EASY3D_UTIL_NO_EXPORT
#    define EASY3D_UTIL_NO_EXPORT 
#  endif
#endif

#ifndef EASY3D_UTIL_DEPRECATED
#  define EASY3D_UTIL_DEPRECATED __declspec(deprecated)
#endif

#ifndef EASY3D_UTIL_DEPRECATED_EXPORT
#  define EASY3D_UTIL_DEPRECATED_EXPORT EASY3D_UTIL_EXPORT EASY3D_UTIL_DEPRECATED
#endif

#ifndef EASY3D_UTIL_DEPRECATED_NO_EXPORT
#  define EASY3D_UTIL_DEPRECATED_NO_EXPORT EASY3D_UTIL_NO_EXPORT EASY3D_UTIL_DEPRECATED
#endif

/* NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if) */
#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef EASY3D_UTIL_NO_DEPRECATED
#    define EASY3D_UTIL_NO_DEPRECATED
#  endif
#endif

#endif /* EASY3D_UTIL_EXPORT_H */
