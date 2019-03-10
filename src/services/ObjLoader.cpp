#include "ObjLoader.hpp"
#include "../mesh/Mesh.hpp"
#include "../document/MeshItem.hpp"
#include <tiny_obj_loader.h>
#include <QDir>
#include <QFileInfo>
#include <QtDebug>
#include "../support/Debug.hpp"

using namespace glm;

namespace Lattice::Services {

std::vector<SP<Document::MeshItem>> ObjLoader::load(const QString &filePathString) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> objShapes;
    std::vector<tinyobj::material_t> objMaterials;

    QFileInfo fileInfo(filePathString);
    auto parentPathString = fileInfo.dir().path() + "/";

    std::string warn;
    std::string err;
    bool ret = tinyobj::LoadObj(&attrib, &objShapes, &objMaterials, &warn, &err, filePathString.toUtf8().data(), parentPathString.toUtf8().data());

    if (!warn.empty()) {
        qWarning() << warn.c_str();
    }
    if (!err.empty()) {
        qWarning() << err.c_str();
    }
    if (!ret) {
        return {};
    }

    std::vector<SP<Document::MeshItem>> items;

    auto loadImage = [&](const std::string& name) {
        if (name.empty()) {
            return QImage();
        }
        auto path = parentPathString + QString::fromStdString(name);
        return QImage(path);
    };

    for (auto& objShape : objShapes) {
        auto item = makeShared<Document::MeshItem>();
        item->setName(objShape.name);

        // TODO: use index_t as key
        std::unordered_map<int, SP<Mesh::MeshVertex>> vertexForIndices;
        std::unordered_map<std::pair<int, int>, SP<Mesh::MeshUVPoint>> uvPointForIndices;

        std::vector<SP<Mesh::MeshMaterial>> materials;

        // Add materials
        for (auto& objMaterial : objMaterials) {
            vec3 diffuse(objMaterial.diffuse[0], objMaterial.diffuse[1], objMaterial.diffuse[2]);
            auto diffuseImage = loadImage(objMaterial.diffuse_texname);

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

            std::vector<SP<Mesh::MeshUVPoint>> uvPoints;

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                tinyobj::index_t idx = objShape.mesh.indices[index_offset + v];

                auto vertex = [&] {
                    auto vertexIt = vertexForIndices.find(idx.vertex_index);
                    if (vertexIt != vertexForIndices.end()) {
                        return vertexIt->second;
                    }
                    tinyobj::real_t vx = attrib.vertices[3*idx.vertex_index+0];
                    tinyobj::real_t vy = attrib.vertices[3*idx.vertex_index+1];
                    tinyobj::real_t vz = attrib.vertices[3*idx.vertex_index+2];
                    //tinyobj::real_t nx = attrib.normals[3*idx.normal_index+0];
                    //tinyobj::real_t ny = attrib.normals[3*idx.normal_index+1];
                    //tinyobj::real_t nz = attrib.normals[3*idx.normal_index+2];
                    // Optional: vertex colors
                    // tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
                    // tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
                    // tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];
                    glm::vec3 pos(vx, vy, vz);
                    auto vertex = item->mesh()->addVertex(pos);
                    vertexForIndices.insert({idx.vertex_index, vertex});
                    return vertex;
                }();

                auto uvPoint = [&] {
                    auto uvPointIt = uvPointForIndices.find({idx.vertex_index, idx.texcoord_index});
                    if (uvPointIt != uvPointForIndices.end()) {
                        return uvPointIt->second;
                    }
                    tinyobj::real_t tx = attrib.texcoords[2*idx.texcoord_index+0];
                    tinyobj::real_t ty = attrib.texcoords[2*idx.texcoord_index+1];
                    glm::vec2 uv(tx, ty);
                    auto uvPoint = item->mesh()->addUVPoint(vertex, uv);

                    uvPointForIndices.insert({{idx.vertex_index, idx.texcoord_index}, uvPoint});
                    return uvPoint;
                }();

                uvPoints.push_back(uvPoint);
            }
            index_offset += fv;

            auto materialID = objShape.mesh.material_ids[f];
            if (materialID >= 0) {
                auto material = materials.at(materialID);
                item->mesh()->addFace(uvPoints, material);
            } else {
                // TODO: add default material
            }

        }

        items.push_back(item);
    }

    return items;
}

} // namespace Lattice
