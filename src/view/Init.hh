// Copyright (C) 2000-2007, Luca Padovani <padovani@sti.uniurb.it>.
//
// This file is part of GtkMathView, a flexible, high-quality rendering
// engine for MathML documents.
// 
// GtkMathView is free software; you can redistribute it and/or modify it
// either under the terms of the GNU Lesser General Public License version
// 3 as published by the Free Software Foundation (the "LGPL") or, at your
// option, under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation (the "GPL").  If you do not
// alter this notice, a recipient may use your version of this file under
// either the GPL or the LGPL.
//
// GtkMathView is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the LGPL or
// the GPL for more details.
// 
// You should have received a copy of the LGPL and of the GPL along with
// this program in the files COPYING-LGPL-3 and COPYING-GPL-2; if not, see
// <http://www.gnu.org/licenses/>.

#ifndef __Init_hh__
#define __Init_hh__

#include <unistd.h>

#include "AbstractLogger.hh"
#include "Configuration.hh"
#include "MathMLOperatorDictionary.hh"

inline bool
fileExists(const String& fileName)
{
  if (access (fileName.c_str(), F_OK) == 0)
    return true;

  return false;
}

template <typename MathView> SmartPtr<Configuration>
initConfiguration(SmartPtr<AbstractLogger>& logger, const char* confPath)
{
  SmartPtr<Configuration> configuration = Configuration::create();

  bool res = false;
  if (fileExists(MathView::getDefaultConfigurationPath()))
    res |= MathView::loadConfiguration(logger, configuration, MathView::getDefaultConfigurationPath());

  for (std::vector<String>::const_iterator p = Configuration::getConfigurationPaths().begin();
       p != Configuration::getConfigurationPaths().end();
       p++)
    if (fileExists(*p))
      res |= MathView::loadConfiguration(logger, configuration, *p);
    else
      logger->out(LOG_WARNING, "configuration file %s explicitly specified but not found", p->c_str());

  if (fileExists("gtkmathview.conf.xml"))
    res |= MathView::loadConfiguration(logger, configuration, "gtkmathview.conf.xml");

  if (confPath != NULL)
    {
    if (fileExists(confPath))
      res |= MathView::loadConfiguration(logger, configuration, confPath);
    else
      logger->out(LOG_WARNING, "configuration file %s explicitly specified but not found", confPath);
    }

  if (!res) logger->out(LOG_WARNING, "could not load configuration file");
  logger->setLogLevel(LogLevelId(configuration->getInt(logger, "logger/verbosity", 1)));
  String confVersion = configuration->getString(logger, "version", "<undefined>");
  if (confVersion != Configuration::getBinaryVersion())
    logger->out(LOG_WARNING, "configuration file version (%s) differs from binary version (%s)",
		    confVersion.c_str(), Configuration::getBinaryVersion());
  return configuration;
}

template <typename MathView> SmartPtr<MathMLOperatorDictionary>
initOperatorDictionary(const SmartPtr<AbstractLogger>& logger, const SmartPtr<Configuration> configuration)
{
  SmartPtr<MathMLOperatorDictionary> dictionary = MathMLOperatorDictionary::create();
  std::vector<String> paths = configuration->getStringList("dictionary/path");
  if (!paths.empty())
    for (std::vector<String>::const_iterator dit = paths.begin();
	 dit != paths.end();
	 dit++)
      {
	if (fileExists(*dit))
	  {
	    logger->out(LOG_DEBUG, "loading dictionary `%s'", (*dit).c_str());
	    if (!MathView::loadOperatorDictionary(logger, dictionary, (*dit).c_str()))
	      logger->out(LOG_WARNING, "could not load `%s'", (*dit).c_str());
	  }
	else
	  logger->out(LOG_WARNING, "dictionary `%s' does not exist", (*dit).c_str());
      }
  else
    {
      bool res = false;
      if (fileExists(MathView::getDefaultOperatorDictionaryPath()))
	res |= MathView::loadOperatorDictionary(logger, dictionary, MathView::getDefaultOperatorDictionaryPath());
      if (fileExists("config/dictionary.xml"))
	res |= MathView::loadOperatorDictionary(logger, dictionary, "config/dictionary.xml");
    }

  return dictionary;
}

#endif // __Init_hh__
