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

#include <config.h>

#include <cassert>

#include "MathMLDummyElement.hh"
#include "RenderingEnvironment.hh"
#include "FormattingContext.hh"

MathMLDummyElement::MathMLDummyElement(const SmartPtr<class MathMLView>& view)
  : MathMLElement(view)
{
}

MathMLDummyElement::~MathMLDummyElement()
{
}

void
MathMLDummyElement::Setup(RenderingEnvironment& env)
{
  if (DirtyAttribute())
    {
      background = env.GetBackgroundColor();
      scaled size = env.ToScaledPoints(env.GetFontAttributes().size);
      box.set(size, size, 0);
      ResetDirtyAttribute();
    }
}

void
MathMLDummyElement::DoLayout(const FormattingContext& ctxt)
{
  if (DirtyLayout(ctxt)) ResetDirtyLayout(ctxt);
}

void
MathMLDummyElement::Render(const DrawingArea& area)
{
  if (Exposed(area))
    {
      RenderBackground(area);
      if (fGC[Selected()] == NULL)
	{
	  GraphicsContextValues values;
	  values.foreground = Selected() ? area.GetSelectionForeground() : RGBColor::RED();
	  values.lineStyle = LINE_STYLE_SOLID;
	  fGC[Selected()] = area.GetGC(values, GC_MASK_FOREGROUND | GC_MASK_LINE_STYLE);
	}

      area.DrawRectangle(fGC[Selected()], GetX(), GetY(), GetBoundingBox());

      ResetDirty();
    }
}
