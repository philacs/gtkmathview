// Copyright (C) 2000-2004, Luca Padovani <luca.padovani@cs.unibo.it>.
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
// http://helm.cs.unibo.it/mml-widget/, or send a mail to
// <lpadovan@cs.unibo.it>

#ifndef __SearchingContext_hh__
#define __SearchingContext_hh__

#include "Rectangle.hh"

class SearchingContext
{
public:
  SearchingContext(const scaled& x, const scaled& y) : x0(x), y0(y) { }
  ~SearchingContext() { }

  bool accept(const Rectangle& rect) const
  { return rect.isInside(x0, y0); }

  bool accept(const BoundingBox& box, const scaled& x, const scaled& y) const
  { return accept(Rectangle(x, y, box));  }

  bool accept(AreaRef area, const scaled& x, const scaled& y) const
  { return accept(area->box(), x, y); }

  scaled getX(void) const { return x0; }
  scaled getY(void) const { return y0; }

  void setResult(AreaRef area) { result = area; }
  AreaRef getResult(void) const { return result; }

protected:
  scaled x0;
  scaled y0;

  AreaRef result;
};

#endif // __SearchingContext_hh__