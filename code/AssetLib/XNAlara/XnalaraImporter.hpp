#ifndef XNALARA_FILE_IMPORTER_H
#define XNALARA_FILE_IMPORTER_H

// C++
#include <sstream>
#include <vector>

/* assimp 5.0.1
 * License: BSD
 */
#include <assimp/BaseImporter.h>
#include <assimp/IOSystem.hpp>
#include <assimp/material.h>
#include <assimp/scene.h>

// Local Project
#include "read_bin_xps.hpp"

namespace assimpXnalara {

/* @class  XnalaraImporter
 * @brief  Imports Xnalara a .xps/.mesh/.mesh.ascii file
 */
class XnalaraImporter : public Assimp::BaseImporter {
public:
  XnalaraImporter();

  ~XnalaraImporter();

public:
  /* @brief  Returns whether the class can handle the format of the given file.
   * @remark See BaseImporter::CanRead() for details.
   */
  bool CanRead(const std::string &pFile, Assimp::IOSystem *pIOHandler,
               bool checkSig) const;

private:
  /* @brief  Appends the supported extension.
   */
  const aiImporterDesc *GetInfo() const;

  /* @brief  File import implementation.
   */
  void InternReadFile(const std::string &pFile, aiScene *pScene,
                      Assimp::IOSystem *pIOHandler);
};

// ------------------------------------------------------------------------------------------------

} // namespace assimpXnalara

#endif
