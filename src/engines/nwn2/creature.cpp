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

/** @file engines/nwn2/creature.cpp
 *  NWN2 creature.
 */

#include "common/util.h"
#include "common/maths.h"

#include "aurora/types.h"
#include "aurora/locstring.h"
#include "aurora/gfffile.h"
#include "aurora/2dafile.h"
#include "aurora/2dareg.h"

#include "graphics/aurora/model.h"

#include "engines/aurora/util.h"
#include "engines/aurora/model.h"

#include "engines/nwn2/util.h"
#include "engines/nwn2/creature.h"

namespace Engines {

namespace NWN2 {

Creature::Creature() : Object(kObjectTypeCreature) {
	init();
}

Creature::Creature(const Aurora::GFFStruct &creature) : Object(kObjectTypeCreature) {
	init();

	load(creature);
}

Creature::~Creature() {
	delete _model;
}

void Creature::init() {
	_gender  = kGenderNone;
	_race    = kRaceInvalid;
	_subRace = kSubRaceInvalid;

	_isPC = false;
	_isDM = false;

	_age = 0;

	_xp = 0;

	_baseHP    = 0;
	_bonusHP   = 0;
	_currentHP = 0;

	_hitDice = 0;

	_goodEvil = 0;
	_lawChaos = 0;

	_appearanceID = Aurora::kFieldIDInvalid;

	_appearanceHead  = 0;
	_appearanceMHair = 0;
	_appearanceFHair = 0;

	_armorVisualType = 0;
	_armorVariations = 0;

	_model = 0;

	for (int i = 0; i < kAbilityMAX; i++)
		_abilities[i] = 0;
}

void Creature::show() {
	if (_model)
		_model->show();
}

void Creature::hide() {
	if (_model)
		_model->hide();
}

void Creature::setPosition(float x, float y, float z) {
	Object::setPosition(x, y, z);
	Object::getPosition(x, y, z);

	if (_model)
		_model->setPosition(x, y, z);
}

void Creature::setOrientation(float x, float y, float z) {
	Object::setOrientation(x, y, z);
	Object::getOrientation(x, y, z);

	if (_model)
		_model->setRotation(x, z, -y);
}

const Common::UString &Creature::getFirstName() const {
	return _firstName;
}

const Common::UString &Creature::getLastName() const {
	return _lastName;
}

uint32 Creature::getGender() const {
	return _gender;
}

void Creature::setGender(uint32 gender) {
	_gender = gender;
}

bool Creature::isFemale() const {
	// Male and female are hardcoded.  Other genders (none, both, other)
	// count as male when it comes to tokens in text strings.

	return _gender == Aurora::kGenderFemale;
}

uint32 Creature::getRace() const {
	return _race;
}

void Creature::setRace(uint32 race) {
	_race = race;
}

uint32 Creature::getSubRace() const {
	return _subRace;
}

void Creature::setSubRace(uint32 subRace) {
	_subRace = subRace;
}

bool Creature::isPC() const {
	return _isPC;
}

bool Creature::isDM() const {
	return _isDM;
}

uint32 Creature::getAge() const {
	return _age;
}

uint32 Creature::getXP() const {
	return _xp;
}

int32 Creature::getCurrentHP() const {
	return _currentHP + _bonusHP;
}

int32 Creature::getMaxHP() const {
	return _baseHP + _bonusHP;
}

void Creature::loadModel() {
	if (_model)
		return;

	if (_appearanceID == Aurora::kFieldIDInvalid) {
		warning("Creature \"%s\" has no appearance", _tag.c_str());
		return;
	}

	const Aurora::TwoDARow &appearance = TwoDAReg.get("appearance").getRow(_appearanceID);

	Common::UString modelBody = appearance.getString("NWN2_Model_Body");
	if (modelBody.empty()) {
		warning("Creature \"%s\" has no body", _tag.c_str());
		return;
	}

	// Male/Female
	modelBody.replaceAll('?', isFemale() ? 'F' : 'M');

	// Prefix for armor model parts
	const Aurora::TwoDARow &armorVisual = TwoDAReg.get("armorvisualdata").getRow(_armorVisualType);
	Common::UString armorPrefix = armorVisual.getString("Prefix");

	// Model for the main body part
	modelBody = Common::UString::sprintf("%s_%s_BODY%02d", modelBody.c_str(),
	                                     armorPrefix.c_str(), _armorVariations + 1);

	_model = loadModelObject(modelBody);

	// Positioning

	float x, y, z;

	getPosition(x, y, z);
	setPosition(x, y, z);

	getOrientation(x, y, z);
	setOrientation(x, y, z);
}

void Creature::unloadModel() {
	hide();

	delete _model;
	_model = 0;
}

void Creature::load(const Aurora::GFFStruct &creature) {
	Common::UString temp = creature.getString("TemplateResRef");

	Aurora::GFFFile *utc = 0;
	if (!temp.empty()) {
		try {
			utc = new Aurora::GFFFile(temp, Aurora::kFileTypeUTC, MKTAG('U', 'T', 'C', ' '));
		} catch (...) {
		}
	}

	load(creature, utc ? &utc->getTopLevel() : 0);

	delete utc;
}

void Creature::load(const Aurora::GFFStruct &instance, const Aurora::GFFStruct *blueprint) {
	// General properties

	if (blueprint)
		loadProperties(*blueprint); // Blueprint
	loadProperties(instance);    // Instance

	// Position

	setPosition(instance.getDouble("XPosition"),
	            instance.getDouble("YPosition"),
	            instance.getDouble("ZPosition"));

	// Orientation

	float bearingX = instance.getDouble("XOrientation");
	float bearingY = instance.getDouble("YOrientation");

	float o[3];
	Common::vector2orientation(bearingX, bearingY, o[0], o[1], o[2]);

	setOrientation(o[0], o[1], o[2]);
}

void Creature::loadProperties(const Aurora::GFFStruct &gff) {
	// Tag

	_tag = gff.getString("Tag", _tag);

	// Name

	if (gff.hasField("FirstName")) {
		Aurora::LocString firstName;
		gff.getLocString("FirstName", firstName);

		_firstName = createDisplayName(firstName.getString());
	}

	if (gff.hasField("LastName")) {
		Aurora::LocString lastName;
		gff.getLocString("LastName", lastName);

		_lastName = createDisplayName(lastName.getString());
	}

	_name = _firstName + " " + _lastName;
	_name.trim();

	// Description

	if (gff.hasField("Description")) {
		Aurora::LocString description;
		gff.getLocString("Description", description);

		_description = description.getString();
	}

	// Conversation

	_conversation = gff.getString("Conversation", _conversation);

	// Sound Set

	_soundSet = gff.getUint("SoundSetFile", Aurora::kFieldIDInvalid);

	// Gender
	_gender = gff.getUint("Gender", _gender);

	// Race
	_race = gff.getUint("Race", _race);

	// Subrace
	_subRace = gff.getUint("Subrace", _subRace);

	// PC and DM
	_isPC = gff.getBool("IsPC", _isPC);
	_isDM = gff.getBool("IsDM", _isDM);

	// Age
	_age = gff.getUint("Age", _age);

	// Experience
	_xp = gff.getUint("Experience", _xp);

	// Abilities
	_abilities[kAbilityStrength]     = gff.getUint("Str", _abilities[kAbilityStrength]);
	_abilities[kAbilityDexterity]    = gff.getUint("Dex", _abilities[kAbilityDexterity]);
	_abilities[kAbilityConstitution] = gff.getUint("Con", _abilities[kAbilityConstitution]);
	_abilities[kAbilityIntelligence] = gff.getUint("Int", _abilities[kAbilityIntelligence]);
	_abilities[kAbilityWisdom]       = gff.getUint("Wis", _abilities[kAbilityWisdom]);
	_abilities[kAbilityCharisma]     = gff.getUint("Cha", _abilities[kAbilityCharisma]);

	// Classes
	loadClasses(gff, _classes, _hitDice);

	// Skills
	if (gff.hasField("SkillList")) {
		_skills.clear();

		const Aurora::GFFList &skills = gff.getList("SkillList");
		for (Aurora::GFFList::const_iterator s = skills.begin(); s != skills.end(); ++s) {
			const Aurora::GFFStruct &skill = **s;

			_skills.push_back(skill.getSint("Rank"));
		}
	}

	// Feats
	if (gff.hasField("FeatList")) {
		_feats.clear();

		const Aurora::GFFList &feats = gff.getList("FeatList");
		for (Aurora::GFFList::const_iterator f = feats.begin(); f != feats.end(); ++f) {
			const Aurora::GFFStruct &feat = **f;

			_feats.push_back(feat.getUint("Feat"));
		}
	}

	// Deity
	_deity = gff.getString("Deity", _deity);

	// Health
	if (gff.hasField("HitPoints")) {
		_baseHP    = gff.getSint("HitPoints");
		_bonusHP   = gff.getSint("MaxHitPoints", _baseHP) - _baseHP;
		_currentHP = gff.getSint("CurrentHitPoints", _baseHP);
	}

	// Alignment

	_goodEvil = gff.getUint("GoodEvil", _goodEvil);
	_lawChaos = gff.getUint("LawfulChaotic", _lawChaos);

	// Appearance

	_appearanceID = gff.getUint("Appearance_Type", _appearanceID);

	_appearanceHead  = gff.getUint("Appearance_Head" , _appearanceHead);
	_appearanceMHair = gff.getUint("Appearance_Hair" , _appearanceMHair);
	_appearanceFHair = gff.getUint("Appearance_FHair", _appearanceFHair);

	_armorVisualType = gff.getUint("ArmorVisualType", _armorVisualType);
	_armorVariations = gff.getUint("Variation"      , _armorVariations);
}

void Creature::loadClasses(const Aurora::GFFStruct &gff,
                           std::vector<Class> &classes, uint8 &hitDice) {

	if (!gff.hasField("ClassList"))
		return;

	classes.clear();
	hitDice = 0;

	const Aurora::GFFList &cClasses = gff.getList("ClassList");
	for (Aurora::GFFList::const_iterator c = cClasses.begin(); c != cClasses.end(); ++c) {
		classes.push_back(Class());

		const Aurora::GFFStruct &cClass = **c;

		classes.back().classID = cClass.getUint("Class");
		classes.back().level   = cClass.getUint("ClassLevel");

		hitDice += classes.back().level;
	}
}

void Creature::getClass(uint32 position, uint32 &classID, uint16 &level) const {
	if (position >= _classes.size()) {
		classID = kClassInvalid;
		level   = 0;
		return;
	}

	classID = _classes[position].classID;
	level   = _classes[position].level;
}

uint16 Creature::getClassLevel(uint32 classID) const {
	for (std::vector<Class>::const_iterator c = _classes.begin(); c != _classes.end(); ++c)
		if (c->classID == classID)
			return c->level;

	return 0;
}

const Common::UString &Creature::getDeity() const {
	return _deity;
}

uint8 Creature::getGoodEvil() const {
	return _goodEvil;
}

uint8 Creature::getLawChaos() const {
	return _lawChaos;
}

uint8 Creature::getHitDice() const {
	return _hitDice;
}

uint8 Creature::getAbility(Ability ability) const {
	assert((ability >= 0) && (ability < kAbilityMAX));

	return _abilities[ability];
}

int8 Creature::getSkillRank(uint32 skill) const {
	if (skill >= _skills.size())
		return -1;

	return _skills[skill];
}

bool Creature::hasFeat(uint32 feat) const {
	for (std::vector<uint32>::const_iterator f = _feats.begin(); f != _feats.end(); ++f)
		if (*f == feat)
			return true;

	return false;
}

} // End of namespace NWN2

} // End of namespace Engines