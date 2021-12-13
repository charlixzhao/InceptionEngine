#include "IE_PCH.h"

#include "GeomProcessing.h"

namespace inceptionengine
{

	void ComputeNormals(std::vector<Vertex>& vertices, std::vector<uint32_t> const& indices)
	{
		for (int i = 0; i < indices.size(); i += 3)
		{
			// Get the face normal
			auto vector1 = vertices[indices[i + 1]].position - vertices[indices[i]].position;
			auto vector2 = vertices[indices[i + 2]].position - vertices[indices[i]].position;
			auto faceNormal = CrossProduct(vector1, vector2);
			faceNormal = NormalizeVec(faceNormal);

			// Add the face normal to the 3 vertices normal touching this face
			vertices[indices[i]].normal += faceNormal;
			vertices[indices[i + 1]].normal += faceNormal;
			vertices[indices[i + 2]].normal += faceNormal;
		}

		// Normalize vertices normal
		for (auto& v : vertices) v.normal = NormalizeVec(v.normal);
	}
}

