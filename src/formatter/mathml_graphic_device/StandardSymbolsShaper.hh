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

#ifndef __StandardSymbolsShaper_hh__
#define __StandardSymbolsShaper_hh__

#include <functional>

#include "HashMap.hh"
#include "Shaper.hh"

class StandardSymbolsShaper : public Shaper
{
public:
  virtual void registerShaper(const SmartPtr<class ShaperManager>&, unsigned);
  virtual void unregisterShaper(const SmartPtr<class ShaperManager>&, unsigned);
  virtual void shape(class ShapingContext&) const;

  struct HStretchyChar
  {
    Char16 ch;
    Char8 normal;
    Char8 left;
    Char8 glue;
    Char8 right;
  };
  
  struct VStretchyChar
  {
    Char16 ch;
    Char8 normal;
    Char8 top;
    Char8 glue;
    Char8 middle;
    Char8 bottom;
  };

protected:
  virtual void registerChar(const SmartPtr<class ShaperManager>&, unsigned, Char16, Char8);
  virtual void registerStretchyCharH(const SmartPtr<class ShaperManager>&, unsigned, const HStretchyChar&, Char8);
  virtual void registerStretchyCharV(const SmartPtr<class ShaperManager>&, unsigned, const VStretchyChar&, Char8);
  virtual AreaRef createGlyphArea(const SmartPtr<class AreaFactory>&, Char8, const scaled&) const = 0;
  AreaRef getGlyphArea(const SmartPtr<class AreaFactory>&, Char8, const scaled&) const;

  AreaRef shapeChar(const class ShapingContext&) const;
  AreaRef shapeStretchyCharV(const class ShapingContext&) const;
  AreaRef shapeStretchyCharH(const class ShapingContext&) const;

  struct CachedAreaKey
  {
    CachedAreaKey(unsigned g, const scaled& s) : glyph(g), size(s) { }

    unsigned glyph;
    scaled size;

    bool operator==(const CachedAreaKey& k) const
    { return glyph == k.glyph && size == k.size; }
  };

  struct CachedAreaKeyHash
  {
    bool operator()(const CachedAreaKey& key) const
    { return key.glyph ^ key.size.getValue(); }
  };

  typedef HASH_MAP_NS::hash_map<CachedAreaKey,AreaRef,CachedAreaKeyHash> AreaCache;

  mutable AreaCache areaCache;
};

#endif // __StandardSymbolsShaper_hh__
