Name:       libomxil-e54xx
Summary:    OpenMAX IL for Exynos libomxil-e54xx series
Version:    1.1.0
Release:    3
Group:      Development/Libraries
License:    LGPLv2+
ExclusiveArch: %arm
Source: %{name}-%{version}.tar.gz

%description
OpenMAX IL for Exynos s5pc2xx series

%prep
%setup -q

%build

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/%{_libdir}
mkdir -p %{buildroot}/%{_libdir}/omx
%if "%{?profile}"=="tv"
install -m 644 lib/tv/lib*.so* %{buildroot}/%{_libdir}/
install -m 644 lib/tv/omx/lib*.so* %{buildroot}/%{_libdir}/omx/
%endif
%if "%{?profile}"=="mobile"
install -m 644 lib/mobile/lib*.so* %{buildroot}/%{_libdir}/
install -m 644 lib/mobile/omx/lib*.so* %{buildroot}/%{_libdir}/omx/
%endif
mkdir -p %{buildroot}/usr/share/license
cp LICENSE.APLv2 %{buildroot}/usr/share/license/%{name}

%post

%postun

%files
%manifest libomxil-e54xx.manifest
%defattr(-,root,root,-)
%{_libdir}/lib*.so*
%{_libdir}/omx/lib*.so*
/usr/share/license/%{name}

