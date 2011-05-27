/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/nwn/scriptfuncs.h
 *  NWN script functions.
 */

#ifndef ENGINES_NWN_SCRIPTFUNCS_H
#define ENGINES_NWN_SCRIPTFUNCS_H

namespace Aurora {
	namespace NWScript {
		class Object;
		class FunctionContext;
	}
}

namespace Engines {

namespace NWN {

class Module;

class Object;
class Creature;
class Area;

class ScriptFunctions {
public:
	ScriptFunctions();
	~ScriptFunctions();

	void setModule(Module *module = 0);

private:
	Module *_module;

	void registerFunctions();

	Common::UString floatToString(float f, int width = 18, int decimals = 9);
	int32 random(int min, int max, int32 n = 1);

	Aurora::NWScript::Object *getPC();

	Common::UString gTag(const Aurora::NWScript::Object *o);

	Module   *convertModule  (Aurora::NWScript::Object *o);
	Area     *convertArea    (Aurora::NWScript::Object *o);
	Object   *convertObject  (Aurora::NWScript::Object *o);
	Creature *convertCreature(Aurora::NWScript::Object *o);
	Creature *convertPC      (Aurora::NWScript::Object *o);

	void random(Aurora::NWScript::FunctionContext &ctx);
	void printString(Aurora::NWScript::FunctionContext &ctx);
	void printFloat(Aurora::NWScript::FunctionContext &ctx);
	void floatToString(Aurora::NWScript::FunctionContext &ctx);
	void printInteger(Aurora::NWScript::FunctionContext &ctx);
	void printObject(Aurora::NWScript::FunctionContext &ctx);
	void assignCommand(Aurora::NWScript::FunctionContext &ctx);
	void delayCommand(Aurora::NWScript::FunctionContext &ctx);
	void executeScript(Aurora::NWScript::FunctionContext &ctx);

	void actionMoveToObject(Aurora::NWScript::FunctionContext &ctx);

	void getArea(Aurora::NWScript::FunctionContext &ctx);

	void getItemPossessor(Aurora::NWScript::FunctionContext &ctx);
	void getItemPossessedBy(Aurora::NWScript::FunctionContext &ctx);

	void getNearestCreature(Aurora::NWScript::FunctionContext &ctx);

	void getDistanceToObject(Aurora::NWScript::FunctionContext &ctx);
	void getIsObjectValid(Aurora::NWScript::FunctionContext &ctx);

	void getLocalInt(Aurora::NWScript::FunctionContext &ctx);
	void getLocalFloat(Aurora::NWScript::FunctionContext &ctx);
	void getLocalString(Aurora::NWScript::FunctionContext &ctx);
	void getLocalObject(Aurora::NWScript::FunctionContext &ctx);
	void setLocalInt(Aurora::NWScript::FunctionContext &ctx);
	void setLocalFloat(Aurora::NWScript::FunctionContext &ctx);
	void setLocalString(Aurora::NWScript::FunctionContext &ctx);
	void setLocalObject(Aurora::NWScript::FunctionContext &ctx);

	void intToString(Aurora::NWScript::FunctionContext &ctx);

	void d2(Aurora::NWScript::FunctionContext &ctx);
	void d3(Aurora::NWScript::FunctionContext &ctx);
	void d4(Aurora::NWScript::FunctionContext &ctx);
	void d6(Aurora::NWScript::FunctionContext &ctx);
	void d8(Aurora::NWScript::FunctionContext &ctx);
	void d10(Aurora::NWScript::FunctionContext &ctx);
	void d12(Aurora::NWScript::FunctionContext &ctx);
	void d20(Aurora::NWScript::FunctionContext &ctx);
	void d100(Aurora::NWScript::FunctionContext &ctx);

	void getRacialType(Aurora::NWScript::FunctionContext &ctx);

	void getAbilityScore(Aurora::NWScript::FunctionContext &ctx);

	void getHitDice(Aurora::NWScript::FunctionContext &ctx);

	void getTag(Aurora::NWScript::FunctionContext &ctx);

	void getWaypointByTag(Aurora::NWScript::FunctionContext &ctx);

	void getObjectByTag(Aurora::NWScript::FunctionContext &ctx);

	void getIsPC(Aurora::NWScript::FunctionContext &ctx);

	void getNearestObjectByTag(Aurora::NWScript::FunctionContext &ctx);

	void getPCSpeaker(Aurora::NWScript::FunctionContext &ctx);

	void getModule(Aurora::NWScript::FunctionContext &ctx);

	void getName(Aurora::NWScript::FunctionContext &ctx);

	void beginConversation(Aurora::NWScript::FunctionContext &ctx);

	void objectToString(Aurora::NWScript::FunctionContext &ctx);

	void setCustomToken(Aurora::NWScript::FunctionContext &ctx);

	void getMaster(Aurora::NWScript::FunctionContext &ctx);

	void setLocked(Aurora::NWScript::FunctionContext &ctx);
	void getLocked(Aurora::NWScript::FunctionContext &ctx);

	void getClassByPosition(Aurora::NWScript::FunctionContext &ctx);
	void getLevelByPosition(Aurora::NWScript::FunctionContext &ctx);
	void getLevelByClass(Aurora::NWScript::FunctionContext &ctx);

	void getGender(Aurora::NWScript::FunctionContext &ctx);

	void getAttemptedAttackTarget(Aurora::NWScript::FunctionContext &ctx);

	void addJournalQuestEntry(Aurora::NWScript::FunctionContext &ctx);

	void sendMessageToPC(Aurora::NWScript::FunctionContext &ctx);
	void getAttemptedSpellTarget(Aurora::NWScript::FunctionContext &ctx);

	void getIsDay(Aurora::NWScript::FunctionContext &ctx);
	void getIsNight(Aurora::NWScript::FunctionContext &ctx);
	void getIsDawn(Aurora::NWScript::FunctionContext &ctx);
	void getIsDusk(Aurora::NWScript::FunctionContext &ctx);

	void getGold(Aurora::NWScript::FunctionContext &ctx);

	void musicBackgroundPlay(Aurora::NWScript::FunctionContext &ctx);
	void musicBackgroundStop(Aurora::NWScript::FunctionContext &ctx);

	void musicBackgroundChangeDay(Aurora::NWScript::FunctionContext &ctx);
	void musicBackgroundChangeNight(Aurora::NWScript::FunctionContext &ctx);

	void isInConversation(Aurora::NWScript::FunctionContext &ctx);

	void getCurrentAction(Aurora::NWScript::FunctionContext &ctx);

	void getFirstPC(Aurora::NWScript::FunctionContext &ctx);
	void getNextPC(Aurora::NWScript::FunctionContext &ctx);

	void musicBackgroundGetDayTrack(Aurora::NWScript::FunctionContext &ctx);
	void musicBackgroundGetNightTrack(Aurora::NWScript::FunctionContext &ctx);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_SCRIPTFUNCS_H
