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
#include <assert.h>

#include "Globals.hh"
#include "MathMLAttribute.hh"
#include "MathMLParseFile.hh"
#include "OperatorDictionary.hh"
#include "MathMLAttributeList.hh"
#include "MathMLAttributeSignature.hh"

#if defined(HAVE_GMETADOM)

void
getAttribute(const DOM::Element& node, const MathMLAttributeSignature& signature,
	     const SmartPtr<MathMLAttributeList>& aList)
{
  assert(aList);

  DOM::GdomeString attrVal = node.getAttribute(toDOMString(signature.name));
  if (attrVal.empty()) return;

  aList->set(MathMLAttribute::create(signature, fromDOMString(attrVal)));
}

#endif // HAVE_GMETADOM

//

OperatorDictionary::OperatorDictionary()
{
}

OperatorDictionary::~OperatorDictionary()
{
  Unload();
}

#if defined(HAVE_GMETADOM)

bool
OperatorDictionary::Load(const char* fileName)
{
  try {
    DOM::Document doc = MathMLParseFile(fileName, true);

    DOM::Element root = doc.get_documentElement();
    if (!root) {
      Globals::logger(LOG_WARNING, "operator dictionary `%s': parse error", fileName);
      return false;
    }

    if (root.get_nodeName() != "dictionary") {
      Globals::logger(LOG_WARNING, "operator dictionary `%s': could not find root element", fileName);
      return false;
    }

    for (DOM::Node op = root.get_firstChild(); op; op = op.get_nextSibling()) {
      if (op.get_nodeType() == DOM::Node::ELEMENT_NODE && op.get_nodeName() == "operator") {
	DOM::Element elem = op;
	String opName = fromDOMString(elem.getAttribute("name"));

	if (!opName.empty()) {
	  SmartPtr<MathMLAttributeList> defaults = MathMLAttributeList::create();

	  getAttribute(op, ATTRIBUTE_SIGNATURE(Operator, form), defaults);
	  getAttribute(op, ATTRIBUTE_SIGNATURE(Operator, fence), defaults);
	  getAttribute(op, ATTRIBUTE_SIGNATURE(Operator, separator), defaults);
	  getAttribute(op, ATTRIBUTE_SIGNATURE(Operator, lspace), defaults);
	  getAttribute(op, ATTRIBUTE_SIGNATURE(Operator, rspace), defaults);
	  getAttribute(op, ATTRIBUTE_SIGNATURE(Operator, stretchy), defaults);
	  //getAttribute(op, ATTRIBUTE_SIGNATURE(Operator, direction), defaults);
	  getAttribute(op, ATTRIBUTE_SIGNATURE(Operator, symmetric), defaults);
	  getAttribute(op, ATTRIBUTE_SIGNATURE(Operator, maxsize), defaults);
	  getAttribute(op, ATTRIBUTE_SIGNATURE(Operator, minsize), defaults);
	  getAttribute(op, ATTRIBUTE_SIGNATURE(Operator, largeop), defaults);
	  getAttribute(op, ATTRIBUTE_SIGNATURE(Operator, movablelimits), defaults);
	  getAttribute(op, ATTRIBUTE_SIGNATURE(Operator, accent), defaults);

	  FormDefaults& formDefaults = items[opName];
	  if (elem.getAttribute("form") == "prefix")
	    if (formDefaults.prefix)
	      Globals::logger(LOG_WARNING, "duplicate `prefix' form for operator `%s' in dictionary (ignored)",
			      opName.c_str());
	    else
	      formDefaults.prefix = defaults;
	  else if (elem.getAttribute("form") == "infix")
	    if (formDefaults.prefix)
	      Globals::logger(LOG_WARNING, "duplicate `infix' form for operator `%s' in dictionary (ignored)",
			      opName.c_str());
	    else
	      formDefaults.infix = defaults;
	  else if (elem.getAttribute("form") == "postfix")
	    if (formDefaults.prefix)
	      Globals::logger(LOG_WARNING, "duplicate `postfix' form for operator `%s' in dictionary (ignored)",
			      opName.c_str());
	    else
	      formDefaults.postfix = defaults;
	  else
	    Globals::logger(LOG_WARNING, 
			    "invalid `form' attribute for entry `%s' in operator dictionary (ignored)",
			    opName.c_str());
	} else {
	  Globals::logger(LOG_WARNING, "operator dictionary `%s': could not find operator name", fileName);
	}
      } else if (!DOM::nodeIsBlank(op)) {
	std::string s_name = op.get_nodeName();
	Globals::logger(LOG_WARNING, "operator dictionary `%s': unknown element `%s'", fileName, s_name.c_str());
      }
    }

    return true;
  } catch (DOM::DOMException) {
    return false;
  }
}

#endif // HAVE_GMETADOM

void
OperatorDictionary::Unload()
{
}

void
OperatorDictionary::Search(const String& opName,
			   SmartPtr<MathMLAttributeList>& prefix,
			   SmartPtr<MathMLAttributeList>& infix,
			   SmartPtr<MathMLAttributeList>& postfix) const
{
  prefix = infix = postfix = 0;

  Dictionary::const_iterator p = items.find(opName);
  if (p != items.end())
    {
      prefix = (*p).second.prefix;
      infix = (*p).second.infix;
      postfix = (*p).second.postfix;
    }
}

