#pragma once

// TESTAPP_ROOT_PATH is defined by the build as a string literal holding the
//  absolute path of the TestApp directory, so resources load regardless of
//  the working directory the app is launched from
#define PATH_FROM_TESTAPP_ROOT(x) TESTAPP_ROOT_PATH x
