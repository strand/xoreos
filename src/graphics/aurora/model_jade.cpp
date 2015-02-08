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

/** @file graphics/aurora/model_jade.cpp
 *  Loading MDL/MDX files found in Jade Empire
 */

// Disable the "unused variable" warnings while most stuff is still stubbed
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"

#include "common/error.h"
#include "common/maths.h"
#include "common/stream.h"

#include "aurora/types.h"
#include "aurora/resman.h"

#include "graphics/aurora/model_jade.h"

enum NodeType {
	kNodeTypeNode             = 0x00000001,
	kNodeTypeLight            = 0x00000003,
	kNodeTypeEmitter          = 0x00000005,
	kNodeTypeCamera           = 0x00000009,
	kNodeTypeReference        = 0x00000011,
	kNodeTypeTrimesh          = 0x00000021,
	kNodeTypeSkin             = 0x00000061,
	kNodeTypeAABB             = 0x00000221,
	kNodeTypeWeaponTrail      = 0x00000821,
	kNodeTypeGob              = 0x00001001,
	kNodeTypeCloth            = 0x00004021,
	kNodeTypeCollisionSphere  = 0x00006001,
	kNodeTypeCollisionCapsule = 0x0000A001,
	kNodeTypeDanglyBone       = 0x00020001,
	kNodeTypeCollisionLozenge = 0x00022001,
	kNodeTypeUnknown          = 0x00040001
};

enum NodeTypeFeature {
	kNodeTypeHasHeader = 0x00000001,
	kNodeTypeHasMesh   = 0x00000020,
	kNodeTypeHasSkin   = 0x00000040,
	kNodeTypeHasAABB   = 0x00000200
};

enum NodeFlag {
	kNodeFlagsAnimatedUV         = 1 << 0,
	kNodeFlagsLightmapped        = 1 << 1,
	kNodeFlagsBackgroundGeometry = 1 << 2,
	kNodeFlagsBeaming            = 1 << 3,
	kNodeFlagsRender             = 1 << 4
};

namespace Graphics {

namespace Aurora {

Model_Jade::ParserContext::ParserContext(const Common::UString &name,
                                         const Common::UString &t) :
	mdl(0), mdx(0), state(0), texture(t) {

	try {

		if (!(mdl = ResMan.getResource(name, ::Aurora::kFileTypeMDL)))
			throw Common::Exception("No such MDL \"%s\"", name.c_str());
		if (!(mdx = ResMan.getResource(name, ::Aurora::kFileTypeMDX)))
			throw Common::Exception("No such MDX \"%s\"", name.c_str());

	} catch (...) {
		delete mdl;
		delete mdx;
		throw;
	}
}

Model_Jade::ParserContext::~ParserContext() {
	delete mdl;
	delete mdx;

	clear();
}

void Model_Jade::ParserContext::clear() {
	for (std::list<ModelNode_Jade *>::iterator n = nodes.begin(); n != nodes.end(); ++n)
		delete *n;
	nodes.clear();

	delete state;
	state = 0;
}


Model_Jade::Model_Jade(const Common::UString &name, ModelType type, const Common::UString &texture) :
	Model(type) {

	_fileName = name;

	ParserContext ctx(name, texture);

	load(ctx);

	finalize();
}

Model_Jade::~Model_Jade() {
}

void Model_Jade::load(ParserContext &ctx) {
	/* Magic and version number:
	 *
	 * - First byte must be 0x00
	 * - Third byte version:
	 *   - Upper bit PC (1) / Xbox (0)
	 *   - Lower 7 bits version number
	 *
	 * We only support version 7 of the PC version.
	 */
	uint32 version = ctx.mdl->readUint32BE();
	if (version != 0x00008700)
		throw Common::Exception("Unsupported MDL: 0x%08X", version);

	ctx.offModelData = 20;

	// Size of the MDL file, without the 20 byte header
	ctx.mdlSize = ctx.mdl->readUint32LE();

	// Size of the vertices part of the MDX file
	ctx.mdxSizeVertices = ctx.mdl->readUint32LE();
	// Size of the faces part of the MDX file
	ctx.mdxSizeFaces    = ctx.mdl->readUint32LE();
	// Size of a third part of the MDX file, always 0?
	ctx.mdxSize3        = ctx.mdl->readUint32LE();

	if (ctx.mdxSize3 != 0)
		warning("Model_Jade: Model \"%s\" mdxSize3 == %d", _fileName.c_str(), ctx.mdxSize3);

	ctx.mdl->skip(8); // Function pointers

	_name.readFixedASCII(*ctx.mdl, 32);

	uint32 nodeHeadPointer = ctx.mdl->readUint32LE();
	uint32 nodeCount       = ctx.mdl->readUint32LE();

	ctx.mdl->skip(24); // Unknown
	ctx.mdl->skip( 4); // Pointer to the MDL file

	uint8 type = ctx.mdl->readByte();

	ctx.mdl->skip(3); // Padding
	ctx.mdl->skip(4); // Unknown
	ctx.mdl->skip(4); // Reference count

	ctx.mdl->skip(12); // TODO: Animation Header Pointer Array

	ctx.mdl->skip(4); // Pointer to the super model

	float boundingMin[3], boundingMax[3];

	boundingMin[0] = ctx.mdl->readIEEEFloatLE();
	boundingMin[1] = ctx.mdl->readIEEEFloatLE();
	boundingMin[2] = ctx.mdl->readIEEEFloatLE();

	boundingMax[0] = ctx.mdl->readIEEEFloatLE();
	boundingMax[1] = ctx.mdl->readIEEEFloatLE();
	boundingMax[2] = ctx.mdl->readIEEEFloatLE();

	float radius = ctx.mdl->readIEEEFloatLE();

	ctx.mdl->skip(4); // Unknown

	float scale = ctx.mdl->readIEEEFloatLE();

	Common::UString superModelName;

	superModelName.readFixedASCII(*ctx.mdl, 32);

	ctx.mdl->skip( 4); // Pointer to some node
	ctx.mdl->skip(12); // Unknown
	ctx.mdl->skip( 4); // Pointer to the MDX file

	uint32 nameOffset, nameCount;
	readArrayDef(*ctx.mdl, nameOffset, nameCount);

	std::vector<uint32> nameOffsets;
	readArray(*ctx.mdl, ctx.offModelData + nameOffset, nameCount, nameOffsets);

	readStrings(*ctx.mdl, nameOffsets, ctx.offModelData, ctx.names);

	newState(ctx);

	ModelNode_Jade *rootNode = new ModelNode_Jade(*this);
	ctx.nodes.push_back(rootNode);

	ctx.mdl->seek(ctx.offModelData + nodeHeadPointer);
	rootNode->load(ctx);

	addState(ctx);
}

void Model_Jade::readStrings(Common::SeekableReadStream &mdl,
		const std::vector<uint32> &offsets, uint32 offset,
		std::vector<Common::UString> &strings) {

	uint32 pos = mdl.pos();

	strings.resize(offsets.size());
	for (uint32 i = 0; i < offsets.size(); i++) {
		mdl.seekTo(offsets[i] + offset);
		strings[i].readASCII(mdl);
	}

	mdl.seekTo(pos);
}

void Model_Jade::newState(ParserContext &ctx) {
	ctx.clear();

	ctx.state = new State;
}

void Model_Jade::addState(ParserContext &ctx) {
	if (!ctx.state || ctx.nodes.empty()) {
		ctx.clear();
		return;
	}

	for (std::list<ModelNode_Jade *>::iterator n = ctx.nodes.begin();
	     n != ctx.nodes.end(); ++n) {

		ctx.state->nodeList.push_back(*n);
		ctx.state->nodeMap.insert(std::make_pair((*n)->getName(), *n));

		if (!(*n)->getParent())
			ctx.state->rootNodes.push_back(*n);
	}

	_stateList.push_back(ctx.state);
	_stateMap.insert(std::make_pair(ctx.state->name, ctx.state));

	if (!_currentState)
		_currentState = ctx.state;

	ctx.state = 0;

	ctx.nodes.clear();
}


ModelNode_Jade::ModelNode_Jade(Model &model) : ModelNode(model) {
}

ModelNode_Jade::~ModelNode_Jade() {
}

void ModelNode_Jade::load(Model_Jade::ParserContext &ctx) {
	uint32 type = ctx.mdl->readUint32LE();

	// Node number in tree order
	uint16 nodeNumber1 = ctx.mdl->readUint16LE();

	// Sequentially node number as found in the file
	uint16 nodeNumber2 = ctx.mdl->readUint16LE();

	if (nodeNumber2 < ctx.names.size())
		_name = ctx.names[nodeNumber2];

	ctx.mdl->skip(4); // Pointer to the MDL file
	ctx.mdl->skip(4); // Pointer to the parent Model

	_position   [0] = ctx.mdl->readIEEEFloatLE();
	_position   [1] = ctx.mdl->readIEEEFloatLE();
	_position   [2] = ctx.mdl->readIEEEFloatLE();
	_orientation[3] = Common::rad2deg(acos(ctx.mdl->readIEEEFloatLE()) * 2.0);
	_orientation[0] = ctx.mdl->readIEEEFloatLE();
	_orientation[1] = ctx.mdl->readIEEEFloatLE();
	_orientation[2] = ctx.mdl->readIEEEFloatLE();

	uint32 childrenOffset = ctx.mdl->readUint32LE();
	uint32 childrenCount  = ctx.mdl->readUint32LE();

	float scale           = ctx.mdl->readIEEEFloatLE();
	float maxAnimDistance = ctx.mdl->readIEEEFloatLE();

	std::vector<uint32> children;
	Model::readArray(*ctx.mdl, ctx.offModelData + childrenOffset, childrenCount, children);

	if (type & kNodeTypeHasMesh) {
		readMesh(ctx);
	}

	for (std::vector<uint32>::const_iterator child = children.begin(); child != children.end(); ++child) {
		ModelNode_Jade *childNode = new ModelNode_Jade(*_model);
		ctx.nodes.push_back(childNode);

		childNode->setParent(this);

		ctx.mdl->seek(ctx.offModelData + *child);
		childNode->load(ctx);
	}
}

void ModelNode_Jade::readMesh(Model_Jade::ParserContext &ctx) {
	ctx.mdl->skip(52); // Unknown

	uint32 transparencyHint = ctx.mdl->readUint32LE();
	uint16 flags            = ctx.mdl->readUint16LE();

	_shadow = ctx.mdl->readUint16LE();

	_render  = flags & kNodeFlagsRender;
	_beaming = flags & kNodeFlagsBeaming;

	_hasTransparencyHint = true;
	_transparencyHint    = (transparencyHint == 1);

	Common::UString texture;
	texture.readFixedASCII(*ctx.mdl, 32);

	uint32 indexCount = ctx.mdl->readUint32LE();

	// Offset of the face indices into the MDL. If 0, use faceOffsetMDX.
	uint32 faceOffsetMDL = ctx.mdl->readUint32LE();

	ctx.mdl->skip(4); // Unknown

	// Type of the mesh:
	// - 0: Point list?
	// - 1: Line list?
	// - 2: Line strip?
	// - 3: Triangle list
	// - 4: Triangle strip
	// - 5: Triangle fan
	// - 6: ???
	uint32 meshType = ctx.mdl->readUint32LE();

	ctx.mdl->skip(12); // Unknown

	uint32 mdxStructSize = ctx.mdl->readUint32LE();

	ctx.mdl->skip(52); // Unknown

	uint16 vertexCount  = ctx.mdl->readUint16LE();
	uint16 textureCount = ctx.mdl->readUint16LE();

	uint32 vertexOffset = ctx.mdl->readUint32LE();
	ctx.mdl->skip(4); // Unknown

	uint32 materialID      = ctx.mdl->readUint32LE();
	uint32 materialGroupID = ctx.mdl->readUint32LE();

	_selfIllum[0] = ctx.mdl->readIEEEFloatLE();
	_selfIllum[1] = ctx.mdl->readIEEEFloatLE();
	_selfIllum[2] = ctx.mdl->readIEEEFloatLE();

	_alpha = ctx.mdl->readIEEEFloatLE();

	float textureWCoords = ctx.mdl->readIEEEFloatLE();

	ctx.mdl->skip(4); // Unknown

	// Offset of the face indices into the MDX. If 0, use faceOffsetMDL.
	uint32 faceOffsetMDX = ctx.mdl->readUint32LE();

	ctx.mdl->skip(4); // Unknown


	if ((vertexCount == 0) || (indexCount == 0))
		return;


	// Load textures
	// If no texture is given, we load Texture0 from the material for now

	if (texture.empty())
		texture = readMaterialTexture(materialID);

	std::vector<Common::UString> textures;

	textureCount = texture.empty() ? 0 : 1;
	if (textureCount > 0)
		textures.push_back(texture);

	loadTextures(textures);


	// Read vertices

	std::vector<float> vertices;
	vertices.reserve(vertexCount * 3);

	std::vector<float> texCoords;
	texCoords.reserve(vertexCount * 2);

	uint32 mdxMinStructSize = 12 + textureCount * 8;
	if (mdxStructSize < mdxMinStructSize) {
		warning("ModelNode_Jade \"%s\".\"%s\": mdxStructSize too small (%d < %d)",
		        _model->getName().c_str(), _name.c_str(), mdxStructSize, mdxMinStructSize);
		return;
	}

	// TODO: Figure out the correct layout of the vertex struct
	for (uint32 i = 0; i < vertexCount; i++) {
		ctx.mdx->seek(vertexOffset + i * mdxStructSize);

		vertices.push_back(ctx.mdx->readIEEEFloatLE());
		vertices.push_back(ctx.mdx->readIEEEFloatLE());
		vertices.push_back(ctx.mdx->readIEEEFloatLE());

		// HACK!
		if      (mdxStructSize == 24)
			ctx.mdx->skip(4);
		else if (mdxStructSize == 28)
			ctx.mdx->skip(8);
		else if (mdxStructSize == 32)
			ctx.mdx->skip(4);
		else if (mdxStructSize == 36)
			ctx.mdx->skip(8);
		else if (mdxStructSize == 48)
			ctx.mdx->skip(4);
		else if (mdxStructSize == 52)
			ctx.mdx->skip(8);

		if (textureCount > 0) {
			texCoords.push_back(ctx.mdx->readIEEEFloatLE());
			texCoords.push_back(ctx.mdx->readIEEEFloatLE());
		}
	}


	// Read face indices

	std::vector<uint16> indices;

	if      (faceOffsetMDL != 0)
		readPlainIndices  (*ctx.mdl, indices, faceOffsetMDL + ctx.offModelData, indexCount);
	else if (faceOffsetMDX != 0)
		readChunkedIndices(*ctx.mdx, indices, faceOffsetMDX, indexCount);

	unfoldFaces(indices, meshType);
	if (indices.empty())
		return;


	// Create the VertexBuffer / IndexBuffer

	GLsizei vpsize = 3;
	GLsizei vnsize = 0;
	GLsizei vtsize = 2;
	uint32 vertexSize = (vpsize + vnsize + vtsize * textureCount) * sizeof(float);
	_vertexBuffer.setSize(vertexCount, vertexSize);

	float *vertexData = (float *) _vertexBuffer.getData();
	VertexDecl vertexDecl;

	VertexAttrib vp;
	vp.index = VPOSITION;
	vp.size = vpsize;
	vp.type = GL_FLOAT;
	vp.stride = vertexSize;
	vp.pointer = vertexData;
	vertexDecl.push_back(vp);

	for (uint16 t = 0; t < textureCount; t++) {
		VertexAttrib vt;
		vt.index = VTCOORD + t;
		vt.size = vtsize;
		vt.type = GL_FLOAT;
		vt.stride = vertexSize;
		vt.pointer = vertexData + vpsize + vnsize + vtsize * t;
		vertexDecl.push_back(vt);
	}

	_vertexBuffer.setVertexDecl(vertexDecl);

	float *v = vertexData;
	for (uint32 i = 0; i < vertexCount; i++) {
		// Position
		*v++ = vertices[i * 3 + 0];
		*v++ = vertices[i * 3 + 1];
		*v++ = vertices[i * 3 + 2];

		// Texture coordinates
		if (textureCount != 0) {
			*v++ = texCoords[i * 2 + 0];
			*v++ = texCoords[i * 2 + 1];
		}
	}

	_indexBuffer.setSize(indices.size(), sizeof(uint16), GL_UNSIGNED_SHORT);

	uint16 *f = (uint16 *) _indexBuffer.getData();
	memcpy(f, &indices[0], indices.size() * sizeof(uint16));

	createBound();
}

void ModelNode_Jade::readPlainIndices(Common::SeekableReadStream &stream, std::vector<uint16> &indices,
                                      uint32 offset, uint32 count) {

	stream.seek(offset);

	indices.resize(count);
	for (std::vector<uint16>::iterator i = indices.begin(); i != indices.end(); ++i)
		*i = stream.readUint16LE();
}

void ModelNode_Jade::readChunkedIndices(Common::SeekableReadStream &stream, std::vector<uint16> &indices,
                                        uint32 offset, uint32 count) {

	stream.seek(offset);

	uint32 stopValue = stream.readUint32LE();
	stream.skip(4); // Unknown

	indices.reserve(count);

	while (count > 0) {
		uint32 chunk = stream.readUint32LE();
		if (chunk == stopValue)
			break;

		uint32 chunkLength = ((chunk >> 16) & 0x1FFF) / 2;
		uint32 toRead = MIN(chunkLength, count);

		for (uint32 i = 0; i < toRead; i++)
			indices.push_back(stream.readUint16LE());

		count -= toRead;
	}
}

/** Unfolds triangle strips / fans into triangle lists. */
void ModelNode_Jade::unfoldFaces(std::vector<uint16> &indices, uint32 meshType) {
	switch (meshType) {
		case 0: // Point list?
		case 1: // Line list?
		case 2: // Line strip?
		case 6: // ???
		default:
			warning("ModelNode_Jade \"%s\".\"%s\": Unsupported mesh type %d",
			        _model->getName().c_str(), _name.c_str(), meshType);
			indices.clear();
			break;

		case 3: // Triangle list
			break;

		case 4: // Triangle strip
			unfoldTriangleStrip(indices);
			break;

		case 5: // Triangle fan
			unfoldTriangleFan(indices);
			break;
	}
}

void ModelNode_Jade::unfoldTriangleStrip(std::vector<uint16> &indices) {
	if (indices.size() < 3) {
		indices.clear();
		return;
	}

	std::vector<uint16> unfolded;
	unfolded.reserve((indices.size() - 2) * 3);

	for (uint i = 0; i < indices.size() - 2; i++) {
		if (i & 1) {
			unfolded.push_back(indices[i]);
			unfolded.push_back(indices[i + 2]);
			unfolded.push_back(indices[i + 1]);
		} else {
			unfolded.push_back(indices[i]);
			unfolded.push_back(indices[i + 1]);
			unfolded.push_back(indices[i + 2]);
		}
	}

	indices.swap(unfolded);
}

void ModelNode_Jade::unfoldTriangleFan(std::vector<uint16> &indices) {
	if (indices.size() < 3) {
		indices.clear();
		return;
	}

	std::vector<uint16> unfolded;
	unfolded.reserve((indices.size() - 2) * 3);

	for (uint i = 1; i < indices.size() - 1; i++) {
		unfolded.push_back(indices[0]);
		unfolded.push_back(indices[i]);
		unfolded.push_back(indices[i + 1]);
	}

	indices.swap(unfolded);
}

/** Opens the resource for the materialID and parses it to return the first texture.
 *
 *  TODO: Proper material support.
 */
Common::UString ModelNode_Jade::readMaterialTexture(uint32 materialID) {
	if (materialID == 0)
		return "";

	Common::UString mabFile = Common::UString::sprintf("%d", materialID);
	Common::SeekableReadStream *mab = ResMan.getResource(mabFile, ::Aurora::kFileTypeMAB);
	if (!mab)
		return "";

	Common::UString texture;

	try {
		uint32 size = mab->readUint32LE();
		if (size != 292)
			throw Common::Exception("Invalid size in binary material %s.mab", mabFile.c_str());

		mab->skip(96);

		texture.readFixedASCII(*mab, 32);

	} catch (Common::Exception &e) {
		delete mab;

		Common::printException(e, "WARNING: ");
		return "";
	}

	delete mab;

	if (texture == "NULL")
		texture.clear();

	return texture;
}

} // End of namespace Aurora

} // End of namespace Graphics