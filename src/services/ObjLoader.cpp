#include "ObjLoader.hpp"
#include "../mesh/Mesh.hpp"
#include "../document/MeshObject.hpp"
#include "../document/Document.hpp"
#include "../document/ImageManager.hpp"
#include <tiny_obj_loader.h>
#include <QDir>
#include <QFileInfo>
#include <QtDebug>
#include "../support/Debug.hpp"
#include "../support/Hash.hpp"

using namespace glm;

namespace Lattice {
namespace Services {

std::vector<SP<Document::MeshObject>> ObjLoader::load(const SP<Document::Document> &document, const boost::filesystem::path &filePath) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> objShapes;
    std::vector<tinyobj::material_t> objMaterials;

    auto parentPath = filePath.parent_path();

    std::string warn;
    std::string err;
    bool ret = tinyobj::LoadObj(&attrib, &objShapes, &objMaterials, &warn, &err, filePath.string().data(), parentPath.string().data(), false);

    if (!warn.empty()) {
        qWarning() << warn.c_str();
    }
    if (!err.empty()) {
        qWarning() << err.c_str();
    }
    if (!ret) {
        return {};
    }

    std::vector<SP<Document::MeshObject>> objects;

    auto loadImage = [&](const std::string& name) -> Opt<SP<Document::Image>> {
        if (name.empty()) {
            return {};
        }
        auto path = parentPath / name;
        return document->imageManager()->openImage(path.string());
    };

    for (auto& objShape : objShapes) {
        auto object = makeShared<Document::MeshObject>();
        object->setName(objShape.name);

        Mesh::Mesh mesh;

        // TODO: use index_t as key
        std::unordered_map<std::pair<int, int>, Mesh::UVPointHandle> uvPointForIndices;

        std::vector<Document::Material> materials;

        // Add materials
        for (auto& objMaterial : objMaterials) {
            vec3 diffuse(objMaterial.diffuse[0], objMaterial.diffuse[1], objMaterial.diffuse[2]);
            auto diffuseImage = loadImage(objMaterial.diffuse_texname);

            Document::Material material;
            material.setBaseColor(diffuse);
            material.setBaseColorImage(diffuseImage);
            // TODO: set more values

            materials.push_back(material);
        }
        object->setMaterials(materials);

        for (size_t v = 0; v < attrib.vertices.size() / 3; ++v) {
            tinyobj::real_t vx = attrib.vertices[3*v+0];
            tinyobj::real_t vy = attrib.vertices[3*v+1];
            tinyobj::real_t vz = attrib.vertices[3*v+2];
            mesh.addVertex(glm::vec3(vx, vy, vz));
        }

        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < objShape.mesh.num_face_vertices.size(); f++) {
            size_t fv = objShape.mesh.num_face_vertices[f];

            std::vector<Mesh::UVPointHandle> uvPoints;

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                tinyobj::index_t idx = objShape.mesh.indices[index_offset + v];
                auto vertex = Mesh::VertexHandle(idx.vertex_index);

                auto uvPoint = [&] {
                    auto uvPointIt = uvPointForIndices.find({idx.vertex_index, idx.texcoord_index});
                    if (uvPointIt != uvPointForIndices.end()) {
                        return uvPointIt->second;
                    }
                    tinyobj::real_t tx = attrib.texcoords[2*idx.texcoord_index+0];
                    tinyobj::real_t ty = attrib.texcoords[2*idx.texcoord_index+1];
                    glm::vec2 uv(tx, ty);
                    auto uvPoint = mesh.addUVPoint(vertex, uv);

                    uvPointForIndices[{idx.vertex_index, idx.texcoord_index}] = uvPoint;
                    return uvPoint;
                }();

                uvPoints.push_back(uvPoint);
            }
            index_offset += fv;

            auto materialID = objShape.mesh.material_ids[f];
            if (materialID >= 0) {
                mesh.addFace(uvPoints, Mesh::MaterialHandle(materialID));
            } else {
                // TODO: add default material
            }
        }

        object->setMesh(std::move(mesh));
        objects.push_back(object);
    }

    return objects;
}

}
} // namespace Lattice
