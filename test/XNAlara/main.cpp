// C++
#include <iomanip>
#include <iostream>
#include <stack>

/* assimp 5.0.1
 * License: BSD
 */
#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h>       // Output data structure

/* assimp-xnalara 1.0
 * License: MIT
 */
#include <code/AssetLib/XNAlara/XnalaraImporter.hpp>

/* Usage:
 * cd build
 * ./assimp-xnalara/example_xnalara.exe "path/to/model.mesh"
 */
int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "ERROR: no input file\nTry: ./example-xnalara inputFile.xps"
              << std::endl;
    return 1;
  }
  std::string fileName = argv[1];
  /* Register xnalara importer
   * In the future, this importer may be integrated into assimp
   */
  assimpXnalara::XnalaraImporter xnalaraImporter;
  Assimp::Importer importer;
  importer.RegisterLoader(&xnalaraImporter);

  /* This demonstrates that the .mesh extension is already used by another
   * importer. If a .mesh file was naively imported using assimp, the wrong
   * importer will be selected.
   */
  Assimp::BaseImporter *meshImporter = importer.GetImporter("mesh");
  if (meshImporter) {
    std::cout << ".mesh importer TAKEN!" << std::endl;
    std::cout << "Name: " << meshImporter->GetInfo()->mName
              << "\nFile Extensions: "
              << meshImporter->GetInfo()->mFileExtensions << std::endl;
  } else {
    std::cout << ".mesh importer FREE!" << std::endl;
  }

  /* The .xps extension importer must specifically be selected.
   * XPS, mesh, and mesh.ascii actually have different sub-importers, but that
   * will be handled internally
   * XPS = binary with header
   * mesh = binary without header
   * mesh.ascii = ascii without header
   */
  Assimp::BaseImporter *xpsImporter = importer.GetImporter("xps");
  if (!xpsImporter) {
    std::cout << "ERROR: .xps importer does not exist" << std::endl;
    return 1;
  }
  std::cout << ".xps importer exists!" << std::endl;
  std::cout << "Name: " << xpsImporter->GetInfo()->mName
            << "\nFile Extensions: " << xpsImporter->GetInfo()->mFileExtensions
            << std::endl;

  std::shared_ptr<aiScene> scenePtr(
      xpsImporter->ReadFile(&importer, fileName, importer.GetIOHandler()));

  if (!scenePtr) {
    std::cout << "ERROR: could not import!" << std::endl;
    return 2;
  }

  std::cout << "Import finished!" << std::endl;
  std::cout << "Displaying bone node hierarchy:" << std::endl;

  std::unordered_map<std::string, bool> visited;
  std::stack<std::pair<aiNode *, unsigned int>> nodeStack;
  nodeStack.push({scenePtr->mRootNode, 0});
  std::string boneNameId =
      std::string(scenePtr->mRootNode->mName.data).append("0");
  visited.insert({boneNameId, true});
  // depth first traversal
  while (!nodeStack.empty()) {
    std::pair<aiNode *, unsigned int> &pairCurrent = nodeStack.top();
    aiNode *nodeCurrent = pairCurrent.first;
    unsigned int depth = pairCurrent.second;
    // pop must come after data copy
    nodeStack.pop();
    unsigned int depthChild = depth + 1;
    unsigned int n = nodeCurrent->mNumChildren;
    std::cout << std::setfill(' ') << std::setw(depth)
              << "name=" << nodeCurrent->mName.C_Str() << " children=" << n
              << " depth=" << depth << std::endl;
    for (unsigned int i = 0; i < n; i++) {
      std::string boneNameId =
          std::string(nodeCurrent->mChildren[i]->mName.data)
              .append("|")
              .append(std::to_string(depthChild));
      if (visited.find(boneNameId) == visited.end()) {
        visited.insert({boneNameId, true});
        nodeStack.push({nodeCurrent->mChildren[i], depthChild});
      } else {
        std::cout << std::setfill(' ') << std::setw(depth) << "CYCLE! "
                  << boneNameId << std::endl;
      }
    }
  }

  return 0;
}
