/*
 * Copyright (c) 2006-2015, Visualization and Multimedia Lab,
 *                          University of Zurich <http://vmml.ifi.uzh.ch>,
 *                          Eyescale Software GmbH,
 *                          Blue Brain Project, EPFL
 *
 * This file is part of VMMLib <https://github.com/VMML/vmmlib/>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.  Redistributions in binary
 * form must reproduce the above copyright notice, this list of conditions and
 * the following disclaimer in the documentation and/or other materials provided
 * with the distribution.  Neither the name of the Visualization and Multimedia
 * Lab, University of Zurich nor the names of its contributors may be used to
 * endorse or promote products derived from this software without specific prior
 * written permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef VMMLIB__MATH__HPP
#define VMMLIB__MATH__HPP

#include <cmath>

#ifndef M_E
#define M_E 2.71828182845904523536028747135266250 /* e              */
#endif
#ifndef M_LOG2E
#define M_LOG2E 1.44269504088896340735992468100189214 /* log2(e)        */
#endif
#ifndef M_LOG10E
#define M_LOG10E 0.434294481903251827651128918916605082 /* log10(e)       */
#endif
#ifndef M_LN2
#define M_LN2 0.693147180559945309417232121458176568 /* loge(2)        */
#endif
#ifndef M_LN10
#define M_LN10 2.30258509299404568401799145468436421 /* loge(10)       */
#endif
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288 /* pi             */
#endif
#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923132169163975144 /* pi/2           */
#endif
#ifndef M_PI_4
#define M_PI_4 0.785398163397448309615660845819875721 /* pi/4           */
#endif
#ifndef M_1_PI
#define M_1_PI 0.318309886183790671537767526745028724 /* 1/pi           */
#endif
#ifndef M_2_PI
#define M_2_PI 0.636619772367581343075535053490057448 /* 2/pi           */
#endif
#ifndef M_2_SQRTPI
#define M_2_SQRTPI 1.12837916709551257389615890312154517 /* 2/sqrt(pi)     */
#endif
#ifndef M_SQRT2
#define M_SQRT2 1.41421356237309504880168872420969808 /* sqrt(2)        */
#endif
#ifndef M_SQRT1_2
#define M_SQRT1_2 0.707106781186547524400844362104849039 /* 1/sqrt(2)      */
#endif

namespace vmml {

namespace math {
// helpers for certain cmath functions

template <class T> inline T squared(const T a) {
  return (a == 0.0) ? 0.0 : a * a;
}

/*
 * Computes (a2 + b2)1/2 without destructive underflow or overflow.
 */
template <class T> inline T pythag(T a, T b) {
  a = std::abs(a);
  b = std::abs(b);
  if (a > b)
    return a * std::sqrt(1.0 + squared(b / a));
  else
    return (b == 0.0) ? 0.0 : b * sqrt(1.0 + squared(a / b));
}

template <class T> inline T sign(T a, T b) {
  return (b >= 0.0) ? std::abs(a) : -std::abs(a);
}

template <typename T> struct abs_less {
  T operator()(const T &a, const T &b) { return std::abs(a) < std::abs(b); }
};

template <typename T> struct abs_greater {
  T operator()(const T &a, const T &b) { return std::abs(a) > std::abs(b); }
};

} // namespace math

} // namespace vmml

#endif
