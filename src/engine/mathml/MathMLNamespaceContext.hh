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

#ifndef __MathMLNamespaceContext_hh__
#define __MathMLNamespaceContext_hh__

#include "NamespaceContext.hh"
#include "MathFormattingContext.hh"

class MathMLNamespaceContext : public NamespaceContext
{
protected:
  MathMLNamespaceContext(const SmartPtr<class MathMLElementFactory>&,
			 const SmartPtr<class MathGraphicDevice>&);
  virtual ~MathMLNamespaceContext();

public:
  static SmartPtr<MathMLNamespaceContext> create(const SmartPtr<class MathMLElementFactory>& factory,
						 const SmartPtr<class MathGraphicDevice>& device)
  { return new MathMLNamespaceContext(factory, device); }

  unsigned getDefaultFontSize(void) const { return defaultFontSize; }
  void setDefaultFontSize(unsigned);

  SmartPtr<class MathMLElementFactory> getFactory(void) const;
  SmartPtr<class MathGraphicDevice> getGraphicDevice(void) const;
  MathFormattingContext& getFormattingContext(void) const { return context; }

  virtual SmartPtr<class Element> construct(const DOM::Element&) const;
  virtual SmartPtr<const class Area> format(const SmartPtr<class Element>&) const;

private:
  SmartPtr<class MathMLElementFactory> factory;
  SmartPtr<class MathGraphicDevice> device;
  mutable MathFormattingContext context;
  unsigned defaultFontSize;
};

#endif // __MathMLNamespaceContext_hh__