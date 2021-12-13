
#include "IE_PCH.h"

#include "GeomGen.h"
#include "GeomProcessing.h"


namespace inceptionengine
{
	SubMesh GeneratePlane(float scale, float level, std::string const& texture)
	{
		Vertex v1 = {}; v1.position = Vec4f(scale, level, scale, 1.0f); v1.texCoord = Vec3f(0.0f, 0.0f, 0.0f);
		Vertex v2 = {}; v2.position = Vec4f(-scale, level, scale, 1.0f); v2.texCoord = Vec3f(0.0f, 1.0f, 0.0f);
		Vertex v3 = {}; v3.position = Vec4f(-scale, level, -scale, 1.0f); v3.texCoord = Vec3f(1.0f, 1.0f, 0.0f);
		Vertex v4 = {}; v4.position = Vec4f(scale, level, -scale, 1.0f); v4.texCoord = Vec3f(1.0f, 0.0f, 0.0f);

		std::vector<Vertex> planeVertices = { v1, v2, v3, v4 };
		std::vector<uint32_t> planeIndices = { 0,2,1,2,0,3 };

		ComputeNormals(planeVertices, planeIndices);

		SubMesh plane;
		plane.vertices = planeVertices;
		plane.indices = planeIndices;
		plane.texturePath = texture;
		plane.shaderPath = { "EngineResource/shader/spv/vertex.spv","EngineResource/shader/spv/fragment.spv" };
		return plane;
	}


	SubMesh GenerateCuboid(float x, float y, float z, std::string const& texture)
	{
		Vertex v0 = {}; v0.position = Vec4f(0.0f, y / 2.0f, -z / 2.0f, 1.0f); v0.texCoord = Vec3f(0.0f, 1.0f, 0.0f);
		Vertex v1 = {}; v1.position = Vec4f(0.0f, y / 2.0f, z / 2.0f, 1.0f); v1.texCoord = Vec3f(1.0f, 1.0f, 0.0f);
		Vertex v2 = {}; v2.position = Vec4f(0.0f, -y / 2.0f, z / 2.0f, 1.0f); v2.texCoord = Vec3f(1.0f, 0.0f, 0.0f);
		Vertex v3 = {}; v3.position = Vec4f(0.0f, -y / 2.0f, -z / 2.0f, 1.0f); v3.texCoord = Vec3f(0.0f, 0.0f, 0.0f);
		Vertex v4 = {}; v4.position = Vec4f(x, y / 2.0f, -z / 2.0f, 1.0f); v4.texCoord = Vec3f(0.0f, 1.0f, 0.0f);
		Vertex v5 = {}; v5.position = Vec4f(x, y / 2.0f, z / 2.0f, 1.0f); v5.texCoord = Vec3f(1.0f, 1.0f, 0.0f);
		Vertex v6 = {}; v6.position = Vec4f(x, -y / 2.0f, z / 2.0f, 1.0f); v6.texCoord = Vec3f(1.0f, 0.0f, 0.0f);
		Vertex v7 = {}; v7.position = Vec4f(x, -y / 2.0f, -z / 2.0f, 1.0f); v7.texCoord = Vec3f(0.0f, 0.0f, 0.0f);

		std::vector<Vertex> cubeVertices = { v0, v1, v2, v3, v4,v5,v6,v7 };
		std::vector<uint32_t> cubeIndices = { 3,2,1,1,0,3,0,1,5,5,4,0,1,2,6,6,5,1,0,4,7,7,3,0,6,2,3,3,7,6,4,5,6,6,7,4 };

		ComputeNormals(cubeVertices, cubeIndices);

		SubMesh cube;
		cube.vertices = cubeVertices;
		cube.indices = cubeIndices;
		cube.texturePath = texture;
		cube.shaderPath = { "EngineResource/shader/spv/vertex.spv","EngineResource/shader/spv/fragment.spv" };
		return cube;
	}


	void Subdivide(std::vector<Vec3f>& pos, std::vector<uint32_t>& indices)
	{
		auto constexpr invalid = std::numeric_limits<uint32_t>().max();

		std::vector<uint32_t> newIndices;

		std::vector<std::vector<uint32_t>> table;
		table.resize(pos.size());
		for (auto& r : table) r.resize(pos.size(), invalid);

		auto getMidpoint = [&pos, &table, invalid](uint32_t i0, uint32_t i1) -> uint32_t
		{
			if (table[i0][i1] != invalid || table[i1][i0] != invalid)
			{
				return (table[i0][i1] != invalid) ? table[i0][i1] : table[i1][i0];
			}
			else
			{
				uint32_t m01 = pos.size();
				pos.push_back((pos[i0] + pos[i1]) / 2.0f);
				table[i0][i1] = m01;
				return m01;
			}
		};

		for (int i = 0; i < indices.size(); i += 3)
		{
			uint32_t i0 = indices[i];
			uint32_t i1 = indices[i + 1];
			uint32_t i2 = indices[i + 2];
			uint32_t m01 = getMidpoint(i0, i1);
			uint32_t m02 = getMidpoint(i0, i2);
			uint32_t m12 = getMidpoint(i1, i2);
			std::vector<uint32_t> face = { i0,m01,m02,i1,m12,m01,i2,m02,m12,m02,m01,m12 };
			std::move(face.begin(), face.end(), std::back_inserter(newIndices));
		}

		indices = newIndices;
	}

	SubMesh GenerateSphere(float r, std::string const& texture)
	{
		std::vector<uint32_t> indices =
		{
			0, 4, 1,
			0, 9, 4,
				9, 5, 4,
				4, 5, 8,
				4, 8, 1,
				8, 10, 1,
				8, 3, 10,
				5, 3, 8,
				5, 2, 3,
				2, 7, 3,
				7, 10, 3,
				7, 6, 10,
				7, 11, 6,
				11, 0, 6,
				0, 1, 6,
				6, 1, 10,
				9, 0, 11,
				9, 11, 2,
				9, 2, 5,
				7, 2, 11
		};

		float X = 0.525731112119133606f;
		float Z = 0.850650808352039932f;

		std::vector<Vec3f> pos =
		{
			Vec3f(-X, 0.0f, -Z),
				 Vec3f(X, 0.0f, -Z),
				 Vec3f(-X, 0.0f, Z),
				 Vec3f(X, 0.0f, Z),
				 Vec3f(0.0f, Z, -X),
				 Vec3f(0.0f, Z, X),
				 Vec3f(0.0f, -Z, -X),
				 Vec3f(0.0f, -Z, X),
				 Vec3f(Z, X, 0.0f),
				 Vec3f(-Z, X, 0.0f),
				 Vec3f(Z, -X, 0.0f),
				 Vec3f(-Z, -X, 0.0f)
		};

		Subdivide(pos, indices);
		Subdivide(pos, indices);
		Subdivide(pos, indices);

		for (auto& p : pos) p = r * NormalizeVec(p);

		std::vector<Vertex> vertices;
		vertices.resize(pos.size());
		for (int i = 0; i < vertices.size(); i++) vertices[i].position = Vec4f(pos[i], 1.0f);

		ComputeNormals(vertices, indices);

		for (auto& v : vertices)
		{
			v.texCoord[0] = std::asin(v.normal[0]) / PI + 0.5f;
			v.texCoord[1] = std::asin(v.normal[1]) / PI + 0.5f;
		}


		SubMesh sphere;
		sphere.vertices = vertices;
		sphere.indices = indices;
		sphere.texturePath = texture;
		sphere.shaderPath = { "EngineResource/shader/spv/vertex.spv","EngineResource/shader/spv/fragment.spv" };
		return sphere;
	}
}