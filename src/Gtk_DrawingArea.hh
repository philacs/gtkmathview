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

#ifndef Gtk_DrawingArea_hh
#define Gtk_DrawingArea_hh

#include <gtk-2.0/gtk/gtk.h>

#include "AFont.hh"
#include "DrawingArea.hh"
#include "GraphicsContext.hh"

class Gtk_DrawingArea : public DrawingArea {
public:
  Gtk_DrawingArea(const GraphicsContextValues&, const scaled&, const scaled&, GtkWidget*,
		  const RGBColor&, const RGBColor&);
  virtual ~Gtk_DrawingArea();

  virtual const GraphicsContext* GetGC(const GraphicsContextValues&) const;

  virtual void DrawLine(const GraphicsContext*, const scaled&, const scaled&, const scaled&, const scaled&) const;
  virtual void DrawRectangle(const GraphicsContext*, const scaled&, const scaled&, const scaled&, const scaled&) const;
  virtual void DrawChar(const GraphicsContext*, const AFont*, const scaled&, const scaled&, char) const;
  virtual void DrawString(const GraphicsContext*, const AFont*, const scaled&, const scaled&, const char*, unsigned) const;
  virtual void Clear(const GraphicsContext*, const scaled&, const scaled&, const scaled&, const scaled&) const;
  virtual void Update(const scaled&, const scaled&, const scaled&, const scaled&) const;
  virtual void Update(void) const;

  virtual void Realize(void);

  void SetPixmap(GdkPixmap*);
  GdkPixmap* GetPixmap(void) const { return gdk_pixmap; }

protected:
  GtkWidget* gtk_widget;
  GdkPixmap* gdk_pixmap;
};

#define TO_GTK_DRAWINGAREA(area) (dynamic_cast<Gtk_DrawingArea*>(area))

#endif
