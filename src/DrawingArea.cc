// Copyright (C) 2000, Luca Padovani <luca.padovani@cs.unibo.it>.
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
// http://cs.unibo.it/~lpadovan/mml-widget, or send a mail to
// <luca.padovani@cs.unibo.it>

#include <config.h>

#include <functional>
#include <algorithm>

#include <assert.h>
#include <stddef.h>

#include "DrawingArea.hh"

// definition of local adaptors
struct DeleteGraphicsContextAdaptor
  : public std::unary_function<const GraphicsContext*,void>
{
  void operator()(const GraphicsContext* gc) const
  { delete gc; }
};

DrawingArea::DrawingArea(const GraphicsContextValues& v, const scaled& xMargin, const scaled& yMargin,
			 const RGBColor& f, const RGBColor& b)
{
  width = height = 0;

  mx = xMargin;
  my = yMargin;

  x = mx;
  y = my;

  x0 = y0 = 0;

  defaultValues = v;
  selectionForeground = f;
  selectionBackground = b;
}

DrawingArea::~DrawingArea()
{  
  ReleaseGCs();
}

const GraphicsContext*
DrawingArea::GetGC(const GraphicsContextValues& values, unsigned mask) const
{
  GraphicsContextValues v = defaultValues;

  if ((mask & GC_MASK_FOREGROUND) != 0) v.foreground = values.foreground;
  if ((mask & GC_MASK_BACKGROUND) != 0) v.background = values.background;
  if ((mask & GC_MASK_LINE_STYLE) != 0) v.lineStyle = values.lineStyle;
  if ((mask & GC_MASK_LINE_WIDTH) != 0) v.lineWidth = values.lineWidth;

  for (std::vector<const GraphicsContext*>::iterator i = poolGC.begin();
       i != poolGC.end();
       i++)
    {
      assert(*i);
      if ((*i)->GetValues().Equals(v)) return *i;
    }

  const GraphicsContext* gc = GetGC(v);
  poolGC.push_back(gc);

  return gc;
}

void
DrawingArea::ReleaseGCs()
{
  std::for_each(poolGC.begin(), poolGC.end(), DeleteGraphicsContextAdaptor());
  poolGC.resize(0);
}

void
DrawingArea::MoveTo(const scaled& x1, const scaled& y1) const
{
  x = x1;
  y = y1;
}

void
DrawingArea::DrawLineTo(const GraphicsContext* gc, const scaled& x1, const scaled& y1) const
{
  DrawLine(gc, x, y, x1, y1);
}

void
DrawingArea::DrawLineToDelta(const GraphicsContext* gc, const scaled& dx, const scaled& dy) const
{
  DrawLine(gc, x, y, x + dx, y + dy);
}

void
DrawingArea::DrawRectangle(const GraphicsContext* gc,
			   const scaled& x, const scaled& y, const BoundingBox& box) const
{
  DrawRectangle(gc, x, y - box.height, box.width, box.verticalExtent());
}

void
DrawingArea::DrawRectangle(const GraphicsContext* gc, const Rectangle& rect) const
{
  DrawRectangle(gc, rect.x, rect.y, rect.width, rect.height);
}

void
DrawingArea::DrawBoundingBox(const GraphicsContext* gc,
			     const scaled& x, const scaled& y,
			     const BoundingBox& box,
			     bool drawExtra) const
{
  scaled width = box.width;
  scaled height = box.verticalExtent();
  scaled ascent = box.height;
  DrawRectangle(gc, x, y - ascent, width, height);
  if (drawExtra) {
    DrawLine(gc, x, y, x + box.width, y);
    DrawLine(gc, x, y - box.height, x + box.width, y - box.height);
    DrawLine(gc, x, y + box.depth, x + box.width, y + box.depth);
  }
}

void
DrawingArea::DrawString(const GraphicsContext* gc, const AFont* font,
			const scaled& x, const scaled& y, const String& text) const
{
  DrawString(gc, font, x, y, text.c_str(), text.length());
}

void
DrawingArea::Clear(const GraphicsContext* gc,
		   const scaled& x, const scaled& y, const BoundingBox& box) const
{
  Clear(gc, x, y - box.height, box.width, box.verticalExtent());
}

void
DrawingArea::Clear(const GraphicsContext* gc, const Rectangle& rect) const
{
  Clear(gc, rect.x, rect.y, rect.width, rect.height);
}

void
DrawingArea::Update(const Rectangle& rect) const
{
  Update(rect.x, rect.y, rect.width, rect.height);
}

void
DrawingArea::Update() const
{
  Update(0, 0, width, height);
}

void
DrawingArea::SetSize(const scaled& w, const scaled& h)
{
  assert(w != scaled(0));
  assert(h != scaled(0));

  width = w;
  height = h;
}

#include "scaledAux.hh"

bool
DrawingArea::Exposed(const scaled& x, const scaled& y, const scaled& w, const scaled& h) const
{
  //std::cout << "AREA " << x0 << "," << y0 << "," << width << "," << height << std::endl;
  //std::cout << "RECT " << x << "," << y << "," << w << "," << h << std::endl;
  //return (x >= x0 || x + w <= x0 + width) && (y >= y0 || y + h <= y0 + height);
  return (x + w >= x0 && x0 + width >= x && y + h >= y0 && y0 + height >= y);
}
