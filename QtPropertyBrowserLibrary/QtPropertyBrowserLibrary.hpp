#ifndef QTPROPERTYBROWSERLIBRARY_HPP
#define QTPROPERTYBROWSERLIBRARY_HPP

#include <QtCore/QtGlobal>

#if defined(QTPROPERTYBROWSER_LIBRARY)
#  define QTPROPERTYBROWSERSHAREDLIB_EXPORT Q_DECL_EXPORT
#else
#  define QTPROPERTYBROWSERSHAREDLIB_EXPORT Q_DECL_IMPORT
#endif

#endif // QTPROPERTYBROWSERLIBRARY_HPP
