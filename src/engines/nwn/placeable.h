/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  NWN placeable.
 */

#ifndef ENGINES_NWN_PLACEABLE_H
#define ENGINES_NWN_PLACEABLE_H

#include "src/aurora/types.h"

#include "src/engines/nwn/situated.h"

namespace Engines {

namespace NWN {

class Tooltip;

class Placeable : public Situated {
public:
	/** The state of a placeable. */
	enum State {
		kStateDefault     = 0, ///< Default.
		kStateOpen        = 1, ///< Open.
		kStateClosed      = 2, ///< Closed.
		kStateDestroyed   = 3, ///< Destroyed.
		kStateActivated   = 4, ///< Activated.
		kStateDeactivated = 5  ///< Deactivated.
	};

	/** Load from a placeable instance. */
	Placeable(const Aurora::GFF3Struct &placeable);
	~Placeable();


	void hide(); ///< Hide the placeable's model.


	void enter(); ///< The cursor entered the placeable.
	void leave(); ///< The cursor left the placeable.

	/** (Un)Highlight the placeable. */
	void highlight(bool enabled);

	/** The placeable was clicked. */
	bool click(Object *triggerer = 0);

	/** Is the placeable open? */
	bool isOpen() const;

	/** Play a placable animation. */
	void playAnimation(Animation animation);
	virtual void playAnimation(const Common::UString &animation = "", bool restart = true, int32 loopCount = 0) { Situated::playAnimation(animation, restart, loopCount); }

protected:
	/** Load placeable-specific properties. */
	void loadObject(const Aurora::GFF3Struct &gff);
	/** Load appearance-specific properties. */
	void loadAppearance();

private:
	State _state; ///< The current state of the placeable.

	Tooltip *_tooltip; ///< The tooltip displayed over the placeable.

	/** Load from a placeable instance. */
	void load(const Aurora::GFF3Struct &placeable);

	void createTooltip(); ///< Create the tooltip.
	void showTooltip();   ///< Show the tooltip.
	void hideTooltip();   ///< Hide the tooltip.

	/** Sync the model's state with the placeable's state. */
	void setModelState();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_PLACEABLE_H
