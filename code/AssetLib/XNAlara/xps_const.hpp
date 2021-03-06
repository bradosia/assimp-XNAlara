#ifndef XNALARA_CONSTANTS_H
#define XNALARA_CONSTANTS_H

#define ASSIMP_XNALARA_IMPORTER_DEBUG 1
#define ASSIMP_XNALARA_READ_BIN_DEBUG 1
#define ASSIMP_XNALARA_READ_BIN_HEADER_DEBUG 1
#define ASSIMP_XNALARA_READ_BIN_BONES_DEBUG 1
#define ASSIMP_XNALARA_READ_BIN_BONES_DEBUG_2 0
#define ASSIMP_XNALARA_READ_BIN_MESHES_DEBUG 0
#define ASSIMP_XNALARA_READ_BIN_MESHES_DEBUG_2 0
#define ASSIMP_XNALARA_READ_BIN_MESHES_DEBUG_3 0
#define ASSIMP_XNALARA_READ_BIN_MESHES_DEBUG_4 0
#define ASSIMP_XNALARA_READ_BIN_MESHES_DEBUG_5 0

// C++
#include <string>

namespace assimpXnalara {

const int MAGIC_NUMBER = 323232;
const int XPS_VERSION_MAYOR = 3;
const int XPS_VERSION_MINOR = 15;
const std::string XNA_ARAL = "XNAaraL";
const int SETTINGS_LEN = 1080;
const int LIMIT = 128;
const int STRLEN = 275;

const int ROUND_MULTIPLE = 4;

const std::string ENCODING_READ = "utf-8-sig";
const std::string ENCODING_WRITE = "utf-8";

const std::string BACK_FACE_CULLING = "backFaceCulling";
const std::string ALWAYS_FORCE_CULLING = "alwaysForceCulling";
const std::string MODEL_CAST_SHADOWS = "modelCastShadows";
const std::string TANGENT_SPACE_RED = "TangentSpaceRed";
const std::string TANGENT_SPACE_GREEN = "TangentSpaceGreen";
const std::string TANGENT_SPACE_BLUE = "TangentSpaceBlue";
const std::string GLOSS = "gloss";
const std::string HAS_BONE_DIRECTIONS = "hasBoneDirections";

}

#endif
