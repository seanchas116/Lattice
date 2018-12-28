#include "ObjLoader.hpp"
#include "../document/Mesh.hpp"
#include "../document/MeshItem.hpp"
#include <tiny_obj_loader.h>
#include <boost/filesystem.hpp>
#include <QtDebug>
#include "../support/Debug.hpp"

using namespace glm;

namespace Lattice {

std::vector<SP<MeshItem>> ObjLoader::load(const std::string &filePathString) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> objShapes;
    std::vector<tinyobj::material_t> objMaterials;

    boost::filesystem::path filePath(filePathString);
    auto parentPathString = filePath.parent_path().string() + "/";

    std::string err;
    bool ret = tinyobj::LoadObj(&attrib, &objShapes, &objMaterials, &err, filePathString.c_str(), parentPathString.c_str());

    if (!err.empty()) {
        qWarning() << err.c_str();
    }
    if (!ret) {
        return {};
    }

    std::vector<SP<MeshItem>> items;

    auto loadImage = [&](const std::string& name) {
        if (name.empty()) {
            return QImage();
        }
        auto path = parentPathString + name;
        return QImage(QString::fromStdString(path));
    };

    for (auto& objShape : objShapes) {
        auto item = std::make_shared<MeshItem>();
        item->setName(objShape.name);

        // TODO: use index_t as key
        std::unordered_map<int, SP<MeshVertex>> vertexForIndex;

        std::vector<SP<MeshMaterial>> materials;

        // Add materials
        for (auto& objMaterial : objMaterials) {
            vec3 diffuse(objMaterial.diffuse[0], objMaterial.diffuse[1], objMaterial.diffuse[2]);
            qDebug() << "diffuse: " << diffuse;

            auto diffuseImage = loadImage(objMaterial.diffuse_texname);
            qDebug() << "diffuse image: " << diffuseImage;

            auto material = item->mesh()->addMaterial();
            material->setBaseColor(diffuse);
            material->setBaseColorImage(diffuseImage);

            // TODO: set more values

            materials.push_back(material);
        }
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < objShape.mesh.num_face_vertices.size(); f++) {
            size_t fv = objShape.mesh.num_face_vertices[f];

            std::vector<SP<MeshVertex>> vertices;

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                tinyobj::index_t idx = objShape.mesh.indices[index_offset + v];

                if (vertexForIndex.find(idx.vertex_index) == vertexForIndex.end()) {
                    tinyobj::real_t vx = attrib.vertices[3*idx.vertex_index+0];
                    tinyobj::real_t vy = attrib.vertices[3*idx.vertex_index+1];
                    tinyobj::real_t vz = attrib.vertices[3*idx.vertex_index+2];
                    //tinyobj::real_t nx = attrib.normals[3*idx.normal_index+0];
                    //tinyobj::real_t ny = attrib.normals[3*idx.normal_index+1];
                    //tinyobj::real_t nz = attrib.normals[3*idx.normal_index+2];
                    //tinyobj::real_t tx = attrib.texcoords[2*idx.texcoord_index+0];
                    //tinyobj::real_t ty = attrib.texcoords[2*idx.texcoord_index+1];
                    // Optional: vertex colors
                    // tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
                    // tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
                    // tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];
                    vertexForIndex[idx.vertex_index] = item->mesh()->addVertex(glm::vec3(vx, vy, vz));
                }
                vertices.push_back(vertexForIndex[idx.vertex_index]);
            }
            index_offset += fv;

            auto materialID = objShape.mesh.material_ids[f];
            if (materialID >= 0) {
                auto material = materials[materialID];
                item->mesh()->addFace(vertices, material);
            } else {
                // TODO: add default material
            }

        }

        items.push_back(item);
    }

    return items;
}

} // namespace Lattice
