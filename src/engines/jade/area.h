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

/** @file engines/jade/area.h
 *  An area.
 */

#ifndef ENGINES_JADE_AREA_H
#define ENGINES_JADE_AREA_H

#include <list>
#include <map>

#include "common/ustring.h"
#include "common/mutex.h"

#include "aurora/lytfile.h"
#include "aurora/visfile.h"

#include "events/types.h"
#include "events/notifyable.h"

namespace Engines {

namespace Jade {

class Room;

/** A Jade area. */
class Area : public Events::Notifyable {
public:
	Area();
	~Area();

	void load(const Common::UString &resRef);

	const Common::UString &getName();

	void show();
	void hide();

	void addEvent(const Events::Event &event);
	void processEventQueue();

	void removeFocus();


protected:
	void notifyCameraMoved();


private:
	typedef std::list<Room *> RoomList;


	bool _loaded;

	Common::UString _resRef;

	bool _visible;

	Aurora::LYTFile _lyt;
	Aurora::VISFile _vis;

	RoomList _rooms;

	std::list<Events::Event> _eventQueue;

	Common::Mutex _mutex;


	void loadLYT();
	void loadVIS();

	void loadRooms();

	void unload();
};

} // End of namespace Jade

} // End of namespace Engines

#endif // ENGINES_JADE_AREA_H