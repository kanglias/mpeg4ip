AM_CFLAGS = -D__TRACE_AND_STATS_ -D__DOUBLE_PRECISION_ -fexceptions @BILLS_CWARNINGS@ -D_OBSS_

AM_CXXFLAGS = -D__TRACE_AND_STATS_ -D__DOUBLE_PRECISION_ -D_REENTRANT -DNOCONTROLS -fexceptions @BILLS_CPPWARNINGS@ -D_OBSS_

INCLUDES = -I../include -I../main -I../pezw -I../wavelet -I../zte -I../ztq -I../../type -I../shape
noinst_LTLIBRARIES = libobj_vtc_wavelet.la

libobj_vtc_wavelet_la_SOURCES = \
	download_filter.cpp \
	download_filter.h \
	dwt_aux.cpp \
	dwt.cpp \
	dwt.h \
	dwtmask.cpp \
	idwt_aux.cpp \
	idwt.cpp \
	imagebox.cpp \
	wvtfilter.h \
	default.h \
	idwtmask.cpp

