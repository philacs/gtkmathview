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

#include "VerticalLayout.hh"
#include "MathMLSpaceElement.hh"

void
VerticalLayout::SetSpacing(const scaled& rs, const scaled& rms)
{
  rowSpacing = rs;
  rowMinSpacing = rms;
}

void
VerticalLayout::Add(const SmartPtr<MathMLElement>& el)
{
  if (content.empty())
    content.push_back(HorizontalLayout::create());

  SmartPtr<HorizontalLayout> row = content.back();
  row->Add(el);

  if (SmartPtr<MathMLSpaceElement> space = smart_cast<MathMLSpaceElement>(el))
    {
      if (space->GetBreakability() == T_NEWLINE ||
	  space->GetBreakability() == T_INDENTINGNEWLINE)
	content.push_back(HorizontalLayout::create());
    }
}

void
VerticalLayout::RemoveAll()
{
  content.clear();
}

scaled
VerticalLayout::GetRowSpacing(const std::vector< SmartPtr<HorizontalLayout> >::const_iterator& p0,
			      const std::vector< SmartPtr<HorizontalLayout> >::const_iterator& p1) const
{
  if (p0 + 1 == content.begin() || p1 == content.end())
    return 0;
  else
    {
      scaled s = rowSpacing;
      if (s - ((*p0)->GetBoundingBox().depth + (*p1)->GetBoundingBox().height) < rowMinSpacing)
	s = rowMinSpacing + ((*p0)->GetBoundingBox().depth + (*p1)->GetBoundingBox().height);
      return s;
    }
}

void
VerticalLayout::SetPosition(const scaled& x0, const scaled& y0)
{
  scaled y = y0;
  for (std::vector< SmartPtr<HorizontalLayout> >::const_iterator p = content.begin();
       p != content.end();
       p++)
    {
      (*p)->SetPosition(x0, y);
      y += GetRowSpacing(p, p + 1);
    }
}

BoundingBox
VerticalLayout::GetBoundingBox() const
{
  BoundingBox box;
  for (std::vector< SmartPtr<HorizontalLayout> >::const_iterator p = content.begin();
       p != content.end();
       p++)
    {
      BoundingBox pBox = (*p)->GetBoundingBox();
      if (p == content.begin()) box.set(0, pBox.height, 0);
      if (p + 1 == content.end()) box.depth += pBox.depth;
      if (p != content.begin()) box.depth += GetRowSpacing(p - 1, p);
      box.width = std::max(box.width, pBox.width);
    }
  return box;
}

BoundingBox
VerticalLayout::GetLinearBoundingBox() const
{
  BoundingBox box;
  for (std::vector< SmartPtr<HorizontalLayout> >::const_iterator p = content.begin();
       p != content.end();
       p++)
    box.append((*p)->GetLinearBoundingBox());
  return box;
}

scaled
VerticalLayout::GetExitBaseline() const
{
  scaled bl = 0;
  for (std::vector< SmartPtr<HorizontalLayout> >::const_iterator p = content.begin();
       p != content.end();
       p++)
    bl += GetRowSpacing(p, p + 1);
  return bl;
}