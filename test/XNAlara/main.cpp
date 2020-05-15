#include <iostream>

/* assimp 5.0.1
 * License: BSD
 */
#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h>       // Output data structure

/* assimp-xnalara 1.0
 * License: MIT
 */
#include <code/AssetLib/XNAlara/read_bin_xps.hpp>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "ERROR: no input file\nTry: ./example-xnalara inputFile.xps"
              << std::endl;
    return 1;
  }
  std::string fileName = argv[0];
  Assimp::Importer importer;

  const aiScene *scene = importer.ReadFile(
      fileName, aiProcess_CalcTangentSpace | aiProcess_Triangulate |
                    aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

  if (!scene) {
    std::cout << "ERROR: could not import!" << std::endl;
    return 2;
  }

  return 0;
}
