%define _topdir       %(pwd)
%define buildroot     %{_topdir}/BUILDROOT

Name:           Fractorium
Version:        1.0.0.15
Release:        1
Summary:        A fractal flame editor with GPU support

Group:          Graphics
BuildArch:      x86_64
License:        GPL
URL:            https://mfeemster@bitbucket.org/mfeemster/fractorium.git
BuildRoot:      %{buildroot}

%description
Fractorium

A fractal flame editor with GPU support.

http://fractorium.com/

The project is maintained on Bitbucket:

https://bitbucket.org/mfeemster/fractorium

News and updates at the above or on Matt Feemster's DeviantArt page:

http://mfeemster.deviantart.com/


%files
%attr(0755, root, root) "/usr/bin/Fractorium-x86_64.AppImage"
/usr/bin/emberanimate
/usr/bin/embergenome
/usr/bin/emberrender
/usr/bin/fractorium
%attr(0644, root, root) "/usr/share/applications/fractorium.desktop"
%attr(0644, root, root) "/usr/share/fractorium/fractorium.png"

%changelog

