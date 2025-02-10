#ifndef EASY3D_UTIL_RESOURCE_H
#define EASY3D_UTIL_RESOURCE_H

#include "../core/types.h"
#include <string>

namespace MV {


    /// \brief Resources used in Easy3D.
    /// \namespace MV::resource
    namespace resource {

        /// \brief Initializes the resource directory (that contains color maps, shaders, textures, fonts, etc.).
        /// \details \c Easy3D_RESOURCE_DIR is the default value, which is the directory coming with the Easy3D
        ///     distribution. In most cases you should use the default resource directory (unless you want to use
        ///     different resources).
        void initialize(const std::string& resource_dir = "");

        /// Returns the resource directory (containing color maps, shaders, textures, fonts, etc.)
        std::string directory();


        /// The vertices of a bunny model.
        /// \see bunny_indices
        extern const __declspec(dllexport) std::vector<vec3>           bunny_vertices;
        /// The triangle faces of a bunny model. Each consecutive 3 values denote the vertex indices of a triangle.
        /// \see bunny_vertices
        extern const __declspec(dllexport) std::vector<unsigned int>   bunny_indices;

    } // namespace resource

} // namespace MV


#endif // EASY3D_UTIL_RESOURCE_H
