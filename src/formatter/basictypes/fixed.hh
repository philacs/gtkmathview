// Copyright (C) 2000-2003, Luca Padovani <luca.padovani@cs.unibo.it>.
//
// This file is part of GtkMathView, a Gtk widget for MathML.
// 
// GtkMathView is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// GtkMathView is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GtkMathView; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// For details, see the GtkMathView World-Wide-Web page,
// http://helm.cs.unibo.it/mml-widget, or send a mail to
// <luca.padovani@cs.unibo.it>

#ifndef __fixed_hh__
#define __fixed_hh__

#include <limits>

template<typename T = int, int precision = std::numeric_limits<T>::digits / 2>
class fixed
{
public:
  fixed(void) { value = 0; }
  fixed(T v) { value = v; }
  fixed(float v) { value = static_cast<T>(v * (static_cast<T>(1) << precision)); }
  fixed(double v) { value = static_cast<T>(v * (static_cast<T>(1) << precision)); }

  static fixed ofInt(int v) { return fixed(static_cast<T>(v) << precision); }
  static fixed zero(void) { return fixed(T(0)); }
  static fixed half(void) { return fixed(static_cast<T>(1) << (precision - 1)); }
  static fixed one(void) { return fixed(static_cast<T>(1) << precision); }
  static fixed min(void) { return fixed(std::numeric_limits<T>::min()); }
  static fixed max(void) { return fixed(std::numeric_limits<T>::max()); }
  
  template<typename S, int p> friend fixed<S,p> abs(const fixed<S,p>&);
  template<typename S, int p> friend fixed<S,p> trunc(const fixed<S,p>&);
  template<typename S, int p> friend fixed<S,p> round(const fixed<S,p>&);
  template<typename S, int p> friend fixed<S,p> ceil(const fixed<S,p>&);
  template<typename S, int p> friend fixed<S,p> floor(const fixed<S,p>&);
  template<typename S, int p> friend fixed<S,p> min(const fixed<S,p>&, const fixed<S,p>&);
  template<typename S, int p> friend fixed<S,p> max(const fixed<S,p>&, const fixed<S,p>&);

  template<typename S, int p> friend fixed<S,p> operator+(const fixed<S,p>&, const fixed<S,p>&);
  template<typename S, int p> friend fixed<S,p> operator-(const fixed<S,p>&, const fixed<S,p>&);
  template<typename S, int p> friend fixed<S,p> operator-(const fixed<S,p>&);
  template<typename S, int p> friend fixed<S,p> operator*(const fixed<S,p>&, int);
  template<typename S, int p> friend fixed<S,p> operator*(const fixed<S,p>&, float);
  template<typename S, int p> friend fixed<S,p> operator*(const fixed<S,p>&, double);
  template<typename S, int p> friend fixed<S,p> operator*(int, const fixed<S,p>&);
  template<typename S, int p> friend fixed<S,p> operator*(float, const fixed<S,p>&);
  template<typename S, int p> friend fixed<S,p> operator*(double, const fixed<S,p>&);
  template<typename S, int p> friend fixed<S,p> operator/(const fixed<S,p>&, int);
  template<typename S, int p> friend fixed<S,p> operator/(const fixed<S,p>&, float);
  template<typename S, int p> friend fixed<S,p> operator/(const fixed<S,p>&, double);
  template<typename S, int p> friend fixed<S,p>& operator+=(fixed<S,p>&, const fixed<S,p>&);
  template<typename S, int p> friend fixed<S,p>& operator-=(fixed<S,p>&, const fixed<S,p>&);
  template<typename S, int p> friend fixed<S,p>& operator*=(fixed<S,p>&, int);
  template<typename S, int p> friend fixed<S,p>& operator*=(fixed<S,p>&, float);
  template<typename S, int p> friend fixed<S,p>& operator*=(fixed<S,p>&, double);
  template<typename S, int p> friend fixed<S,p>& operator/=(fixed<S,p>&, int);
  template<typename S, int p> friend fixed<S,p>& operator/=(fixed<S,p>&, float);
  template<typename S, int p> friend fixed<S,p>& operator/=(fixed<S,p>&, double);
  template<typename S, int p> friend bool operator==(const fixed<S,p>&, const fixed<S,p>&);
  template<typename S, int p> friend bool operator!=(const fixed<S,p>&, const fixed<S,p>&);
  template<typename S, int p> friend bool operator<(const fixed<S,p>&, const fixed<S,p>&);
  template<typename S, int p> friend bool operator>(const fixed<S,p>&, const fixed<S,p>&);
  template<typename S, int p> friend bool operator<=(const fixed<S,p>&, const fixed<S,p>&);
  template<typename S, int p> friend bool operator>=(const fixed<S,p>&, const fixed<S,p>&);

  static int getPrecision(void) { return precision; }
  int toInt(void) const { return value >> precision; }
  float toFloat(void) const { return static_cast<float>(value) / fixed::one().value; }
  double toDouble(void) const { return static_cast<double>(value) / fixed::one().value; }
  T getValue(void) const { return value; }
  
private:
  T value;
};

template <typename T, int p>
inline fixed<T,p>
abs(const fixed<T,p>& f)
{ 
  return fixed<T,p>(std::abs(f.value));
}

template <typename T, int p>
inline fixed<T,p>
trunc(const fixed<T,p>& f)
{
  if (f.value > 0) return fixed<T,p>(f.value & ~(fixed<T,p>::one().value - 1));
  else return fixed<T,p>(f.value | (fixed<T,p>::one().value - 1));
}

template <typename T, int p>
inline fixed<T,p>
round(const fixed<T,p>& f)
{
  if (f.value > 0) return fixed<T,p>::trunc(f + fixed<T,p>::half());
  else return fixed<T,p>::trunc(f - fixed<T,p>::half());
}

template <typename T, int p>
inline fixed<T,p>
ceil(const fixed<T,p>& f)
{
  if (f.value > 0) return fixed<T,p>::round(f + fixed<T,p>::half());
  else return fixed<T,p>::trunc(f);
}

template <typename T, int p>
inline fixed<T,p>
floor(const fixed<T,p>& f)
{
  if (f.value > 0) return fixed<T,p>::trunc(f);
  else return fixed<T,p>::round(f - fixed<T,p>::half());
}

template <typename T, int p>
inline fixed<T,p>
min(const fixed<T,p>& f1, const fixed<T,p>& f2)
{
  return fixed<T,p>(std::min(f1.value, f2.value));
}

template <typename T, int p>
inline fixed<T,p>
max(const fixed<T,p>& f1, const fixed<T,p>& f2)
{
  return fixed<T,p>(std::max(f1.value, f2.value));
}

template <typename T, int p> 
inline fixed<T,p> 
operator+(const fixed<T,p>& f1, const fixed<T,p>& f2)
{
  return fixed<T,p>(f1.value + f2.value);
}

template <typename T, int p> 
inline fixed<T,p>
operator-(const fixed<T,p>& f1, const fixed<T,p>& f2)
{
  return fixed<T,p>(f1.value - f2.value);
}

template <typename T, int p> 
inline fixed<T,p> 
operator-(const fixed<T,p>& f)
{ 
  return fixed<T,p>(-f.value);
}

template <typename T, int p> 
inline fixed<T,p> 
operator*(const fixed<T,p>& f, int v) 
{ 
  return fixed<T,p>(f.value * v);
}

template <typename T, int p> fixed<T,p> 
operator*(const fixed<T,p>& f, float v)
{
  return fixed<T,p>(static_cast<T>(f.value * v));
}

template <typename T, int p> 
inline fixed<T,p>
operator*(const fixed<T,p>& f, double v) 
{ 
  return fixed<T,p>(static_cast<T>(f.value * v));
}

template <typename T, int p> 
inline fixed<T,p> 
operator/(const fixed<T,p>& f, int v)
{ 
  return fixed<T,p>(f.value / v);
}

template <typename T, int p> 
inline fixed<T,p> 
operator/(const fixed<T,p>& f, float v)
{
  return fixed<T,p>(static_cast<T>(f.value / v));
}

template <typename T, int p> 
inline fixed<T,p> 
operator/(const fixed<T,p>& f, double v)
{
  return fixed<T,p>(static_cast<T>(f.value / v));
}

template <typename T, int p> 
inline fixed<T,p> 
operator*(int v, const fixed<T,p>& f) 
{ 
  return fixed<T,p>(f.value * v);
}

template <typename T, int p> fixed<T,p> 
operator*(float v, const fixed<T,p>& f)
{
  return fixed<T,p>(static_cast<T>(f.value * v));
}

template <typename T, int p> 
inline fixed<T,p>
operator*(double v, const fixed<T,p>& f) 
{ 
  return fixed<T,p>(static_cast<T>(f.value * v));
}

template<typename S, int p> 
inline fixed<S,p>& 
operator+=(fixed<S,p>& f1, const fixed<S,p>& f2)
{
  f1.value += f2.value;
  return f1;
}

template<typename S, int p> 
inline fixed<S,p>& 
operator-=(fixed<S,p>& f1, const fixed<S,p>& f2)
{
  f1.value -= f2.value;
  return f1;
}

template<typename S, int p>
inline fixed<S,p>& 
operator*=(fixed<S,p>& f, int v)
{
  f.value *= v;
  return f;
}

template<typename T, int p> 
inline fixed<T,p>& 
operator*=(fixed<T,p>& f, float v)
{
  f.value = static_cast<T>(f.value * v);
  return f;
}

template<typename T, int p> 
inline fixed<T,p>& 
operator*=(fixed<T,p>& f, double v)
{
  f.value = static_cast<T>(f.value * v);
  return f;
}

template<typename T, int p> 
inline fixed<T,p>& 
operator/=(const fixed<T,p>& f, int v)
{
  f.value /= v;
  return f;
}

template<typename T, int p> 
inline fixed<T,p>& 
operator/=(const fixed<T,p>& f, float v)
{
  f.value = static_cast<T>(f.value / v);
  return f;
}

template<typename T, int p> 
inline fixed<T,p>& 
operator/=(const fixed<T,p>& f, double v)
{
  f.value = static_cast<T>(f.value / v);
  return f;
}

template <typename T, int p> 
inline bool 
operator==(const fixed<T,p>& f1, const fixed<T,p>& f2)
{
  return f1.value == f2.value;
}

template <typename T, int p> 
inline bool 
operator!=(const fixed<T,p>& f1, const fixed<T,p>& f2)
{
  return f1.value != f2.value;
}

template <typename T, int p> 
inline bool 
operator<(const fixed<T,p>& f1, const fixed<T,p>& f2)
{
  return f1.value < f2.value;
}

template <typename T, int p> 
inline bool 
operator>(const fixed<T,p>& f1, const fixed<T,p>& f2)
{ 
  return f1.value > f2.value;
}

template <typename T, int p> 
inline bool operator<=(const fixed<T,p>& f1, const fixed<T,p>& f2)
{
  return f1.value <= f2.value;
}

template <typename T, int p>
inline bool
operator>=(const fixed<T,p>& f1, const fixed<T,p>& f2)
{
  return f1.value >= f2.value;
}

#endif // __fixed_hh__

