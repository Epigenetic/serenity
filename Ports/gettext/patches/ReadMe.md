# Patches for gettext on SerenityOS

## `0001-Stub-out-some-wctype-functions.patch`

Stub out some wctype functions


## `0002-libtool-Enable-shared-library-support-for-SerenityOS.patch`

libtool: Enable shared library support for SerenityOS

For some odd reason, libtool handles the configuration for shared
libraries entirely statically and in its configure script. If no
shared library support is "present", building shared libraries is
disabled entirely.

Fix that by just adding the appropriate configuration options for
`serenity`. This allows us to finally create dynamic libraries
automatically using libtool, without having to manually link the
static library into a shared library.

This patch here is a bit more elaborate for other ports, as libintl's
configure includes the code for detecting dynamic linker characteristics
twice, and it also queries the C++ compiler for shared library support.


