
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

dnl AC_ARG_ENABLE([mat],
dnl     AS_HELP_STRING([--enable-mat], [Enable save to MAT format (requires MATIO installed),default: no]))

dnl AS_IF([test "x$enable_mat" = "xyes"], [
dnl   AC_CHECK_HEADERS(matio.h)
dnl   AC_CHECK_LIB(matio, mat_close, LIBMATIO=-lmatio; HAVE_MATIO=yes,
dnl 	HAVE_MATIO=no;AC_MSG_ERROR([MATIO library required for --enable-mat]))
dnl   AC_SUBST(HAVE_MATIO)
 dnl  ])

dnl Checks for zlib library. -- from libxml2 configure.in
_cppflags="${CPPFLAGS}"
_ldflags="${LDFLAGS}"  

AC_ARG_WITH(mat,
[  --with-mat[[=DIR]]       use MATIO lib/headers in DIR],[
  if test "$withval" != "no" -a "$withval" != "yes"; then
    MAT_DIR=$withval
    CPPFLAGS="${CPPFLAGS} -I$withval/include"
    LDFLAGS="${LDFLAGS} -L$withval/lib"
  fi
])
if test "$with_mat" = "no"; then   
    echo "Disabling Matlab/Octave support"
else
    echo "Enabling Matlab/Octave support"
    AC_CHECK_HEADERS(matio.h,
        AC_CHECK_LIB(hdf5, H5_init_library,

        AC_CHECK_LIB(matio, Mat_CreateVer,[
            AC_DEFINE(HAVE_MATIO,[],[Do we have MATIO])
            if test "x${MAT_DIR}" != "x"; then
                MAT_CFLAGS="-I${MAT_DIR}/include"
                MAT_LIBS="-L${MAT_DIR}/lib -lmatio -lhdf5"
                LDFLAGS="$LDFLAGS $MAT_LIBS"
            else
                MAT_LIBS="-L/usr/local/lib -lmatio -lhdf5"
            fi],[],-lhdf5),
        AC_CHECK_LIB(matio, Mat_CreateVer,[
            AC_DEFINE(HAVE_MATIO,[],[Do we have MATIO])
            if test "x${MAT_DIR}" != "x"; then
                MAT_CFLAGS="-I${MAT_DIR}/include"
                MAT_LIBS="-L${MAT_DIR}/lib -lmatio"
                LDFLAGS="$LDFLAGS $MAT_LIBS"
            else
                MAT_LIBS="-L/usr/local/lib -lmatio"
            fi],[],[]),[]
))
fi

#
#AC_CHECK_LIB (library, function, [action-if-found], [action-if-not-found], [other-libraries
CPPFLAGS=${_cppflags}
LDFLAGS=${_ldflags}


AC_SUBST([MAT_CFLAGS])
AC_SUBST([MAT_LIBS])


AC_OUTPUT
