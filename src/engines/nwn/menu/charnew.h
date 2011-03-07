/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/charnew.h
 *  The new character creator.
 */

#ifndef ENGINES_NWN_MENU_CHARNEW_H
#define ENGINES_NWN_MENU_CHARNEW_H

#include "engines/nwn/menu/gui.h"

namespace Engines {

namespace NWN {

/** The NWN character creator. */
class CharNewMenu : public GUI {
public:
	CharNewMenu(Module &module);
	~CharNewMenu();

protected:
	void callbackActive(Widget &widget);

private:
	Module *_module;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_MENU_CHARNEW_H