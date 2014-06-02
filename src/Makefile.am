bin_PROGRAMS = tms32_reader
tms32_reader_SOURCES = tms32_reader.cpp unilib.cpp TFloatSignal.cpp TTms32Reader.cpp extalg.cpp TMatlabSave.cpp
tms32_reader_CXXFLAGS = -Wunused-result -fpermissive
tms32_reader_CPPFLAGS = -Wunused-result -I$(top_srcdir)/include
tms32_reader_LDFLAGS = $(MAT_LIBS)
#-L/usr/local/lib -lmatio 
#$(AM_LDFLAGS)
# $(LIBFOOLDFLAGS)
#ACLOCAL_AMFLAGS = '-I m4'

if DEBUG
AM_CFLAGS = -g3 -O0
AM_CXXFLAGS = -g3 -O0 -Wunused-result -fpermissive
else
AM_CFLAGS = -O2
AM_CXXFLAGS = -O2 -Wunused-result -fpermissive
endif


