//////////////////////////////////////////////////////////////////////////////////////////
// A cross platform socket APIs, support ios & android & wp8 & window store
// universal app
//////////////////////////////////////////////////////////////////////////////////////////
/*
The MIT License (MIT)

Copyright (c) 2012-2020 HALX99

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef YASIO__SFMT_HPP
#define YASIO__SFMT_HPP

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string>
#include "yasio/compiler/feature_test.hpp"

namespace yasio
{
// yasio::format_traits: the vsnprintf/vswprintf wrapper
// !!! if '_BufferCount' had been sufficiently large, not counting the terminating null character.
template <typename _Elem> struct format_traits
{};
template <> struct format_traits<char>
{
  /* @pitfall: The behavior of vsnprintf are different at different standard implemetations
   **      VS2013/glibc-2.0: returns -1 when buffer insufficient
   **      VS2015+/glbc-2.1: returns the actural needed length when buffer insufficient
   ** see reference:
   **      http://www.cplusplus.com/reference/cstdio/vsnprintf/
   */
  static int format(char* const _Buffer, size_t const _BufferCount, char const* const _Format,
                    va_list _ArgList)
  {
    return vsnprintf(_Buffer, _BufferCount, _Format, _ArgList);
  }
};
template <> struct format_traits<wchar_t>
{
  /* see: http://www.cplusplus.com/reference/cwchar/vswprintf/ */
  static int format(wchar_t* const _Buffer, size_t const _BufferCount, wchar_t const* const _Format,
                    va_list _ArgList)
  {
    return vswprintf(_Buffer, _BufferCount, _Format, _ArgList);
  }
};

/*
 * --- This is a C++ universal sprintf in the future, works correct at all compilers.
 */
template <class _Elem, class _Traits = std::char_traits<_Elem>,
          class _Alloc = std::allocator<_Elem>>
inline std::basic_string<_Elem, _Traits, _Alloc> _strfmt(size_t n, const _Elem* format, ...)
{
  va_list args;
  std::basic_string<_Elem, _Traits, _Alloc> buffer(n, 0);

  va_start(args, format);
  int nret = format_traits<_Elem>::format(&buffer.front(), buffer.length() + 1, format, args);
  va_end(args);

  if (nret >= 0)
  {
    if ((unsigned int)nret < buffer.length())
    {
      buffer.resize(nret);
    }
    else if ((unsigned int)nret > buffer.length())
    { // handle return required length when buffer insufficient
      buffer.resize(nret);

      va_start(args, format);
      nret = format_traits<_Elem>::format(&buffer.front(), buffer.length() + 1, format, args);
      va_end(args);
    }
    // else equals, do nothing.
  }
  else
  { // handle return -1 when buffer insufficient
    do
    {
      buffer.resize(buffer.length() * 3 / 2);

      va_start(args, format);
      nret = format_traits<_Elem>::format(&buffer.front(), buffer.length() + 1, format, args);
      va_end(args);

    } while (nret < 0);

    buffer.resize(nret);
  }

  return buffer;
}

static auto constexpr strfmt = _strfmt<char>;
static auto constexpr wcsfmt = _strfmt<wchar_t>;

} // namespace yasio

#endif
