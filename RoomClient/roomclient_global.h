#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(ROOMCLIENT_LIB)
#  define ROOMCLIENT_EXPORT Q_DECL_EXPORT
# else
#  define ROOMCLIENT_EXPORT Q_DECL_IMPORT
# endif
#else
# define ROOMCLIENT_EXPORT
#endif
