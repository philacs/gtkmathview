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

#ifndef __SpaceShaper_hh__
#define __SpaceShaper_hh__

#include "Shaper.hh"

class SpaceShaper : public Shaper
{
protected:
  SpaceShaper(void) { }
  virtual ~SpaceShaper() { }

public:
  static SmartPtr<SpaceShaper> create(void)
  { return new SpaceShaper(); }

  virtual void registerShaper(const SmartPtr<class ShaperManager>&, unsigned);
  virtual void unregisterShaper(const SmartPtr<class ShaperManager>&, unsigned);
  virtual unsigned shape(const class MathFormattingContext&, class ShapingResult&) const;

protected:
  static unsigned shapeFixedSpace(const class MathFormattingContext&, ShapingResult&, const GlyphSpec&);
  static unsigned shapeContextSensitiveSpace(const class MathFormattingContext&, ShapingResult&, const GlyphSpec&);

  static void pushSpace(const class MathFormattingContext&, ShapingResult&, int);
  static void shapeFunctionApplication(const class MathFormattingContext&, class ShapingResult&);
  static void shapeInvisibleTimes(const class MathFormattingContext&, class ShapingResult&);
};

#endif // __SpaceShaper_hh__