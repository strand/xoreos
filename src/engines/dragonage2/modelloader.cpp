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
 *  Dragon Age 2 model loader.
 */

#include "src/common/system.h"
#include "src/common/error.h"
#include "src/common/readstream.h"

#include "src/aurora/types.h"

#include "src/graphics/aurora/model_dragonage.h"

#include "src/engines/dragonage2/modelloader.h"

namespace Engines {

namespace DragonAge2 {

Graphics::Aurora::Model *DragonAge2ModelLoader::load(const Common::UString &resref,
		Graphics::Aurora::ModelType type, const Common::UString &UNUSED(texture)) {

	return new Graphics::Aurora::Model_DragonAge(resref, type);
}

} // End of namespace DragonAge2

} // End of namespace Engines
