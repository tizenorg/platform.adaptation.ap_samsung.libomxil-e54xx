Name: libomxil-e54xx
Summary: OpenMAX IL for e54xx
Version: 1.0.0
License: Apache-2.0
Group: Development/Libraries
Release: 4
Source: %{name}-%{version}.tar.gz
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires: kernel-headers
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(mm-common)
ExclusiveArch: %arm aarch64
%description
implementation of OpenMAX IL for e54xx-v4l2


%package devel
Summary: OpenMAX IL for e54xx-v4l2 (Developement)
Group: Development/Libraries
Requires: %{name} = %{version}-%{release}

%description devel
development package for libomxil-e54xx-v4l2

%prep
%setup -q

%build
./autogen.sh

export CFLAGS+="\
%ifnarch aarch64
 -mfpu=neon\
%endif
 -DUSE_PB\
 -DUSE_DMA_BUF\
 -DUSE_H264_PREPEND_SPS_PPS\
 -DGST_EXT_TIME_ANALYSIS\
 -DKERNEL_HEADER_MODIFICATION"


%ifnarch aarch64
%configure --prefix=%{_prefix} --disable-static --enable-dlog --enable-neon
%else
%configure --prefix=%{_prefix} --disable-static --enable-dlog --disable-neon
%endif

#make %{?jobs:-j%jobs}
make


%install
rm -rf %{buildroot}
%make_install


%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%manifest libomxil-e54xx.manifest
%{_libdir}/*.so*
%{_libdir}/omx/*.so


%files devel
/usr/include/*
%{_libdir}/pkgconfig/*

