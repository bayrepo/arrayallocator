%define version 0.1
%define release 1.alpha


Name:           inarray-allocator
Version:        %{version}
Release:        %{release}%{?dist}
Summary:        Headers of uthash, utlist, utarray, utringbuffer and utstring wich allocates data into array.
        
License:        MIT
URL:            https://github.com/bayrepo/arrayallocator
Source0:        %{name}-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:  CUnit-devel cmake >= 2.8

%description
Package contains the library with alternative malloc wich can allocate memory from pre-allocated array.
Library has the ut* utils which extended for use alternative allocator.

%package devel
Requires: %{name}=%{version}-%{release}
Summary: Headers of uthash, utlist, utarray, utringbuffer and utstring wich allocates data into array.

%description devel
Package contains the library with alternative malloc wich can allocate memory from pre-allocated array.
Library has the ut* utils which extended for use alternative allocator.

%prep
%setup -q


%build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr -DLIB_VERSION_MAJOR=%{version} -DLIB_VERSION_MINOR=%{release} ..
make %{?_smp_mflags}

%check
cd build
make unit-test

%install
rm -rf $RPM_BUILD_ROOT
cd build
make install DESTDIR=$RPM_BUILD_ROOT

cd ..
mkdir -p $RPM_BUILD_ROOT/%{_sysconfdir}/ld.so.conf.d/
mkdir -p $RPM_BUILD_ROOT/%{_datarootdir}/bayrepo
install -m 644 -D allocator.conf $RPM_BUILD_ROOT/%{_sysconfdir}/ld.so.conf.d/
install -m 644 -D LICENSE $RPM_BUILD_ROOT/%{_datarootdir}/bayrepo/

%clean
rm -rf $RPM_BUILD_ROOT

%post
/sbin/ldconfig

%postun
/sbin/ldconfig


%files
%defattr(-,root,root,-)
%{_datarootdir}/bayrepo/*
%{_libdir}/bayrepo/*
%{_sysconfdir}/ld.so.conf.d/*

%files devel
%defattr(-,root,root,-)
%{_includedir}/bayrepo/*

%changelog
* Sun Aug 13 2017 Alexey Berezhok <bayrepo.info@gmail.com> 0.1-1.alpha
- Initial commit