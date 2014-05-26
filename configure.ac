
AC_INIT([tms32_reader], [1.0], [info@centum.pl],[tms32_reader], [http://centum.pl/])
AC_PREREQ([2.61])
AM_INIT_AUTOMAKE([1.10 no-define])
LT_INIT
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AC_PROG_CXX
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CC_STDC
AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_ARG_ENABLE(debug,
AS_HELP_STRING([--enable-debug],
               [enable debugging, default: no]),
[case "${enableval}" in
             yes) debug=true ;;
             no)  debug=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
esac],
[debug=false])

AM_CONDITIONAL(DEBUG, test x"$debug" = x"true")

AC_OUTPUT
