AC_INIT([dtalk],[0.1])

AM_INIT_AUTOMAKE([-Wall])

AC_CONFIG_SRCDIR([src/dtalk_s/dtalk_s.c])

AC_CONFIG_FILES([Makefile
                src/dtalk_s/Makefile
                src/dtalk_c/Makefile])

AC_SEARCH_LIBS([pthread_create], [pthread], [], [AC_MSG_ERROR([pthread lib not found])])
AC_SEARCH_LIBS([dlopen], [dl],   [], [AC_MSG_ERROR([dl lib not found])])
AC_SEARCH_LIBS([sqlite3_open],   [sqlite3], [], [AC_MSG_ERROR([sqlite3 lib not found])])

AC_CONFIG_HEADERS([config.h])

AC_PROG_CC

AM_PROG_CC_C_O

AC_ISC_POSIX
AC_HEADER_STDC

AC_OUTPUT
