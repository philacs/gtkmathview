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

#ifndef MathMLTextNode_hh
#define MathMLTextNode_hh

#include "Area.hh"
#include "String.hh"
#include "MathMLFrame.hh"

class MathMLTextNode: public MathMLFrame
{
protected:
  MathMLTextNode(void);
  virtual ~MathMLTextNode();

public:
  virtual AreaRef format(class MathFormattingContext&) = 0;
#if 0
  virtual void 	   Setup(class RenderingEnvironment&) = 0;
  virtual void 	   DoLayout(const class FormattingContext&) = 0;
  virtual void 	   SetPosition(const scaled&, const scaled&);
  virtual void 	   Render(const DrawingArea&) = 0;
#endif

  virtual String   GetRawContent(void) const { return String(); }
  virtual unsigned GetLogicalContentLength(void) const { return 0; }

  virtual bool 	   IsText(void) const;  
#if 0
  virtual bool 	   IsInside(const scaled&, const scaled&) const;
#endif
  virtual bool     HasDecimalPoint(void) const;
  void             SetSpacing(int);
  void             AddSpacing(int);
  int              GetSpacing(void) const { return spacing; }

#if 0
  virtual scaled   GetLeftEdge(void) const;
  virtual scaled   GetRightEdge(void) const;
  virtual scaled   GetDecimalPointEdge(void) const;
#endif

protected:
  int spacing : 13;     // overall spacing after the node
};

#endif // MathMLTextNode_hh