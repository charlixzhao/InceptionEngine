
#include "IE_PCH.h"

#include "FbxImporter.h"
#include "Serialization/Serializer.h"
#include "EngineGlobals/RenderGlobals.h"
#include "PathHelper.h"

#define FBXSDK_SHARED
#include "External/fbxsdk/include/fbxsdk.h"

namespace inceptionengine::fbximport
{
    /*
    declaration of helper functions
    */

    void ImportMesh(FbxScene* pFbxScene, ImportScene& importScene);

    void ImportSkeleton(FbxScene* pFbxScene, ImportScene& importScene);

    void ImportAnimations(FbxScene* pFbxScene, ImportScene& importScene);

    void SerializeImportScene(std::string const& savePath, ImportScene const& importScene);

    Matrix4x4f ConvertMatrix(FbxAMatrix const& mat)
    {
        Vec4f col1 = Vec4f(mat.GetRow(0)[0], mat.GetRow(0)[1], mat.GetRow(0)[2], mat.GetRow(0)[3]);
        Vec4f col2 = Vec4f(mat.GetRow(1)[0], mat.GetRow(1)[1], mat.GetRow(1)[2], mat.GetRow(1)[3]);
        Vec4f col3 = Vec4f(mat.GetRow(2)[0], mat.GetRow(2)[1], mat.GetRow(2)[2], mat.GetRow(2)[3]);
        Vec4f col4 = Vec4f(mat.GetRow(3)[0], mat.GetRow(3)[1], mat.GetRow(3)[2], mat.GetRow(3)[3]);

        return Matrix4x4f(col1, col2, col3, col4); 
    }

    /*
    Implementation
    */

	void Import(std::string const& filePath, ImportScene& importScene)
	{
        std::cout << "import begin\n";
		fbxsdk::FbxManager* fbxsdkManager = fbxsdk::FbxManager::Create();

        fbxsdk::FbxIOSettings* fbxIOSetting = fbxsdk::FbxIOSettings::Create(fbxsdkManager, IOSROOT);
        fbxsdkManager->SetIOSettings(fbxIOSetting);

        fbxsdk::FbxImporter* fbxsdkImporter = fbxsdk::FbxImporter::Create(fbxsdkManager, "");

        if (!fbxsdkImporter->Initialize(PathHelper::GetAbsolutePath(filePath).c_str(), -1, fbxsdkManager->GetIOSettings()))
        {
            std::cerr << "Error returned: " << fbxsdkImporter->GetStatus().GetErrorString() << std::endl;;
            throw std::runtime_error("");
        }

        FbxScene* pFbxScene = fbxsdk::FbxScene::Create(fbxsdkManager, "myScene");

        fbxsdkImporter->Import(pFbxScene);

        fbxsdkImporter->Destroy();


        FbxAxisSystem OurAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
        OurAxisSystem.ConvertScene(pFbxScene);
        FbxSystemUnit::cm.ConvertScene(pFbxScene);


        ImportSkeleton(pFbxScene, importScene);
        ImportMesh(pFbxScene, importScene);
        ImportAnimations(pFbxScene, importScene);

        SerializeImportScene("", importScene);
	}

    Vec4f ConvertFbxTranslation(fbxsdk::FbxVector4 const& fbxvec)
    {
        return Vec4f(fbxvec.mData[0], fbxvec.mData[1], fbxvec.mData[2], 1.0f);
    }

    FbxAMatrix GetGeometryTransformation(FbxNode* pNode)
    {
        const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
        const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
        const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

        return FbxAMatrix(lT, lR, lS);
    }

    void ImportMeshVertexPosition()
    {

    }

    void ImportMeshVertexUV()
    {

    }

    int FindVertexAgrminBoneWeight(Vertex const& vertex)
    {
        int argmin = 0;
        float min = vertex.boneWeights[0];
        for (int i = 1; i < 4; i++)
        {
            if (vertex.boneWeights[i] < min)
            {
                min = vertex.boneWeights[i];
                argmin = i;
            }
        }
        return argmin;
    }

    void AddSkinToVertex(Vertex& vertex, int boneID, float weight)
    {
        int argminWeight = FindVertexAgrminBoneWeight(vertex);
        vertex.affectedBonesID[argminWeight] = boneID + AnimPoseOffsetInUBuffer;
        vertex.boneWeights[argminWeight] = weight;
    }

    void NormalizeVertexBoneWeights(Vertex& vertex)
    {
        float sum = vertex.boneWeights[0] + vertex.boneWeights[1] + vertex.boneWeights[2] + vertex.boneWeights[3];
        assert(sum > 0.000001f && "vertex as no skin!");
        for (int i = 0; i < 4; i++)
        {
            vertex.boneWeights[i] = vertex.boneWeights[i] / sum;
        }
    }

    void ImportMesh(FbxScene* pFbxScene, ImportScene& importScene)
    {
       fbxsdk::FbxNode* pRootNode = pFbxScene->GetRootNode();
       std::queue<fbxsdk::FbxNode*> nodeQueue;
       nodeQueue.push(pRootNode);
       while (!nodeQueue.empty())
       {
           fbxsdk::FbxNode* curNode = nodeQueue.front();
           nodeQueue.pop();

           for (int i = 0; i < curNode->GetChildCount(); i++)
           {
               nodeQueue.push(curNode->GetChild(i));
           }

           if (curNode->GetNodeAttribute() != nullptr && curNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::EType::eMesh)
           {
               int meshIndex = importScene.mesh.mSubMeshes.size();
               importScene.mesh.mSubMeshes.push_back({});
               
               std::cout << "Import Mesh " << curNode->GetName() << std::endl;

               FbxMesh* pFbxMesh = curNode->GetMesh();
               assert(pFbxMesh != nullptr);

               /*
               import vertex positions
               */
               auto axisMat = pFbxMesh->GetNode()->EvaluateGlobalTransform();//.Inverse();
               auto geoMat = GetGeometryTransformation(curNode);

               int ctrlPointsCount = pFbxMesh->GetControlPointsCount();

               for (int ctrlPointIndex = 0; ctrlPointIndex < ctrlPointsCount; ctrlPointIndex++)
               {
                   fbxsdk::FbxVector4 ctrlPoint = pFbxMesh->GetControlPointAt(ctrlPointIndex);

                   Vertex vertex;
                   vertex.position = ConvertMatrix(axisMat * geoMat) * Vec4f(ctrlPoint[0], ctrlPoint[1], ctrlPoint[2], 1.0f);
  
                   importScene.mesh.mSubMeshes[meshIndex].vertices.push_back(vertex);
               }
               
               /*
               import indices
               */
               int triangleCount = pFbxMesh->GetPolygonCount();
               for (int triangleIndex = 0; triangleIndex < triangleCount; triangleIndex++)
               {
                   assert(pFbxMesh->GetPolygonSize(triangleIndex) == 3);

                   importScene.mesh.mSubMeshes[meshIndex].indices.push_back(pFbxMesh->GetPolygonVertex(triangleIndex, 0));
                   importScene.mesh.mSubMeshes[meshIndex].indices.push_back(pFbxMesh->GetPolygonVertex(triangleIndex, 1));
                   importScene.mesh.mSubMeshes[meshIndex].indices.push_back(pFbxMesh->GetPolygonVertex(triangleIndex, 2));
               }


               /*
               import vertex uv coords
               */

               FbxStringList uvSetNameList;
               pFbxMesh->GetUVSetNames(uvSetNameList);

               for (int uvSetIndex = 0; uvSetIndex < uvSetNameList.GetCount(); uvSetIndex++)
               {
                   //get uvSetIndex-th uv set
                   const char* uvSetName = uvSetNameList.GetStringAt(uvSetIndex);
                   std::cout << "find uv set: " << uvSetName << std::endl;
                   const FbxGeometryElementUV* lUVElement = pFbxMesh->GetElementUV(uvSetName);

                   if (!lUVElement)
                       continue;

                   // only support mapping mode eByPolygonVertex and eByControlPoint
                   if (lUVElement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex &&
                       lUVElement->GetMappingMode() != FbxGeometryElement::eByControlPoint)
                   {
                       throw std::runtime_error("");
                   }

                   //index array, where holds the index referenced to the uv data
                   const bool lUseIndex = lUVElement->GetReferenceMode() != FbxGeometryElement::eDirect;
                   const int lIndexCount = (lUseIndex) ? lUVElement->GetIndexArray().GetCount() : 0;

                   //iterating through the data by polygon
                   const int lPolyCount = pFbxMesh->GetPolygonCount();

                   if (lUVElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
                   {
                       for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex)
                       {
                           // build the max index array that we need to pass into MakePoly
                           const int lPolySize = pFbxMesh->GetPolygonSize(lPolyIndex);
                           assert(lPolySize == 3);
                           for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
                           {
                               FbxVector2 lUVValue;

                               //get the index of the current vertex in control points array
                               int lPolyVertIndex = pFbxMesh->GetPolygonVertex(lPolyIndex, lVertIndex);

                               //the UV index depends on the reference mode
                               int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyVertIndex) : lPolyVertIndex;

                               lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);

                               //assert(lUVValue[0] >= 0.0f && lUVValue[0] <= 1.0f);
                               //assert(lUVValue[1] >= 0.0f && lUVValue[1] <= 1.0f);

                               importScene.mesh.mSubMeshes[meshIndex].vertices[lPolyVertIndex].texCoord = Vec3f(lUVValue.mData[0], 1.0f - lUVValue.mData[1], 0.0f);
                           
                           }
                       }
                   }
                   else if (lUVElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
                   {
                       int lPolyIndexCounter = 0;
                       for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex)
                       {
                           // build the max index array that we need to pass into MakePoly
                           const int lPolySize = pFbxMesh->GetPolygonSize(lPolyIndex);
                           for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
                           {
                               if (lPolyIndexCounter < lIndexCount)
                               {
                                   FbxVector2 lUVValue;

                                   //the UV index depends on the reference mode
                                   int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyIndexCounter) : lPolyIndexCounter;
                                   int ctrlPointIndex = pFbxMesh->GetPolygonVertex(lPolyIndex, lVertIndex);


                                   lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);

                                   //assert(lUVValue[0] >= 0.0f && lUVValue[0] <= 1.0f);
                                   //assert(lUVValue[1] >= 0.0f && lUVValue[1] <= 1.0f);

                                   importScene.mesh.mSubMeshes[meshIndex].vertices[ctrlPointIndex].texCoord = Vec3f(lUVValue.mData[0], 1.0f - lUVValue.mData[1], 0.0f);

                                   lPolyIndexCounter++;
                               }
                           }
                       }
                   }
               }

               /*
               Import Normals
               */

               
               //get the normal element
               FbxGeometryElementNormal* lNormalElement = pFbxMesh->GetElementNormal();
               if (lNormalElement)
               {
                   //mapping mode is by control points. The mesh should be smooth and soft.
                   //we can get normals by retrieving each control point
                   if (lNormalElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
                   {
                       //Let's get normals of each vertex, since the mapping mode of normal element is by control point
                       for (int lVertexIndex = 0; lVertexIndex < pFbxMesh->GetControlPointsCount(); lVertexIndex++)
                       {
                           int lNormalIndex = 0;
                           //reference mode is direct, the normal index is same as vertex index.
                           //get normals by the index of control vertex
                           if (lNormalElement->GetReferenceMode() == FbxGeometryElement::eDirect)
                               lNormalIndex = lVertexIndex;

                           //reference mode is index-to-direct, get normals by the index-to-direct
                           if (lNormalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
                               lNormalIndex = lNormalElement->GetIndexArray().GetAt(lVertexIndex);

                           //Got normals of each vertex.
                           FbxVector4 lNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);

                           Vec4f adjustedNormal = ConvertMatrix(axisMat * geoMat) * Vec4f(lNormal[0], lNormal[1], lNormal[2], 0.0f);
                           importScene.mesh.mSubMeshes[meshIndex].vertices[lVertexIndex].normal = Vec3f(adjustedNormal);
       
                       }//end for lVertexIndex
                   }//end eByControlPoint

                   //mapping mode is by polygon-vertex.
                   //we can get normals by retrieving polygon-vertex.
                   else if (lNormalElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
                   {
                       int lIndexByPolygonVertex = 0;
                       //Let's get normals of each polygon, since the mapping mode of normal element is by polygon-vertex.
                       for (int lPolygonIndex = 0; lPolygonIndex < pFbxMesh->GetPolygonCount(); lPolygonIndex++)
                       {
                           //get polygon size, you know how many vertices in current polygon.
                           int lPolygonSize = pFbxMesh->GetPolygonSize(lPolygonIndex);
                           //retrieve each vertex of current polygon.
                           for (int i = 0; i < lPolygonSize; i++)
                           {
                               int lNormalIndex = 0;
                               //reference mode is direct, the normal index is same as lIndexByPolygonVertex.
                               if (lNormalElement->GetReferenceMode() == FbxGeometryElement::eDirect)
                                   lNormalIndex = lIndexByPolygonVertex;

                               //reference mode is index-to-direct, get normals by the index-to-direct
                               if (lNormalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
                                   lNormalIndex = lNormalElement->GetIndexArray().GetAt(lIndexByPolygonVertex);

                               //Got normals of each polygon-vertex.

                               FbxVector4 lNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);
                               FBXSDK_printf("normals for polygon[%d]vertex[%d]: %f %f %f %f \n",
                                             lPolygonIndex, i, lNormal[0], lNormal[1], lNormal[2], lNormal[3]);
                               //add your custom code here, to output normals or get them into a list, such as KArrayTemplate<FbxVector4>
                               int ctrlPointIndex = pFbxMesh->GetPolygonVertex(lPolygonIndex, i);

                               Vec4f adjustedNormal = ConvertMatrix(axisMat * geoMat) * Vec4f(lNormal[0], lNormal[1], lNormal[2], 0.0f);
                               importScene.mesh.mSubMeshes[meshIndex].vertices[ctrlPointIndex].normal = Vec3f(adjustedNormal);

                               lIndexByPolygonVertex++;
                           }//end for i //lPolygonSize
                       }//end for lPolygonIndex //PolygonCount

                   }//end eByPolygonVertex
               }//end if lNormalElement
               

               /*
               import skin data
               */
               int skinCount = pFbxMesh->GetDeformerCount(FbxDeformer::eSkin);
               std::cout << "The model has " << skinCount << " skin\n";
               if (skinCount > 0)
               {
                   for (auto& vertex : importScene.mesh.mSubMeshes[meshIndex].vertices)
                   {
                       vertex.boneWeights[0] = 0.0f;
                   }
               }

               for (int skinLoop = 0; skinLoop < skinCount; skinLoop++)
               {
                   FbxSkin* pFbxSkin = reinterpret_cast<FbxSkin*>(pFbxMesh->GetDeformer(skinLoop, FbxDeformer::eSkin));
                   assert(pFbxSkin != nullptr);
                   int clustercount = pFbxSkin->GetClusterCount();
                   for (int clusterLoop = 0; clusterLoop < pFbxSkin->GetClusterCount(); clusterLoop++)
                   {
                       FbxCluster* pFbxCluster = pFbxSkin->GetCluster(clusterLoop);
                       int boneID = importScene.mesh.mSkeleton.GetBoneID(pFbxCluster->GetLink()->GetName());
                       assert(boneID != -1);
                       for (int affectCtrlPointLoop = 0; affectCtrlPointLoop < pFbxCluster->GetControlPointIndicesCount(); affectCtrlPointLoop++)
                       {
                           int affectCtrlPointIndex = (pFbxCluster->GetControlPointIndices())[affectCtrlPointLoop];
                           float affectCtrlPointWeight = static_cast<float>(pFbxCluster->GetControlPointWeights()[affectCtrlPointLoop]);
                           AddSkinToVertex(importScene.mesh.mSubMeshes[meshIndex].vertices[affectCtrlPointIndex], boneID, affectCtrlPointWeight);
                       }
                   }
               }

               for (auto& vertex : importScene.mesh.mSubMeshes[meshIndex].vertices)
               {
                   NormalizeVertexBoneWeights(vertex);
               }
               


               /*
               set default texture and shader path
               */
               importScene.mesh.mSubMeshes[meshIndex].texturePath = "StandAloneResource\\maria\\maria_diffuse.BMP";
               //importScene.mesh.mSubMeshes[meshIndex].texturePath = "D:\\InceptionEngine\\EngineResource\\model\\hornet\\hornet_base.BMP";
               importScene.mesh.mSubMeshes[meshIndex].shaderPath =
               {
                   "EngineResource\\shader\\spv\\vertex.spv",
                   "EngineResource\\shader\\spv\\fragment.spv"
               };

           }
       }

    }

    void ImportSkeleton(FbxScene* pFbxScene, ImportScene& importScene)
    {
        Skeleton& skeleton = importScene.mesh.mSkeleton;

        std::vector<FbxNode*> boneNodes;
        fbxsdk::FbxNode* pRootNode = pFbxScene->GetRootNode();
        std::queue<fbxsdk::FbxNode*> nodeQueue;
        nodeQueue.push(pRootNode);

        while (!nodeQueue.empty())
        {
            fbxsdk::FbxNode* curNode = nodeQueue.front();
            nodeQueue.pop();

            for (int i = 0; i < curNode->GetChildCount(); i++)
            {
                nodeQueue.push(curNode->GetChild(i));
            }
            if (curNode->GetNodeAttribute() != nullptr && curNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::EType::eSkeleton)
            {
                boneNodes.push_back(curNode);
            }
        }

        for (int boneIndex = 0; boneIndex < boneNodes.size(); boneIndex++)
        {
            skeleton.mNameToIDMap.insert(std::pair<std::string, int>(boneNodes[boneIndex]->GetName(), boneIndex));
        }

        for (int boneIndex = 0; boneIndex < boneNodes.size(); boneIndex++)
        {
            Skeleton::Bone bone;
            bone.name = boneNodes[boneIndex]->GetName();
            bone.ID = skeleton.GetBoneID(bone.name);
            bone.parentID = skeleton.GetBoneID(boneNodes[boneIndex]->GetParent()->GetName());
            bone.bindPose = ConvertMatrix(boneNodes[boneIndex]->EvaluateGlobalTransform());
            bone.bindPoseInv = Inverse(bone.bindPose);

            skeleton.mBones.push_back(bone);
        }

       // pFbxScene->SetCurrentAnimationStack();
        FbxAnimStack* pAnimStack = pFbxScene->GetSrcObject<FbxAnimStack>(0);
        double duration = pAnimStack->GetLocalTimeSpan().GetDuration().GetSecondDouble();
        importScene.animation.mDuration = static_cast<float>(duration);
        double sampleRate = 30.0;
        for (double time = 0.0; time <= duration; time += (1.0 / sampleRate))
        {
            FbxTime fbxTime;
            fbxTime.SetSecondDouble(time);
            std::vector<Matrix4x4f> frame;
            for (int boneIndex = 0; boneIndex < boneNodes.size(); boneIndex++)
            {
                frame.push_back(ConvertMatrix(boneNodes[boneIndex]->EvaluateGlobalTransform(fbxTime)));
            }
            importScene.animation.mBoneTransforms.push_back(std::move(frame));
        }
    }

    void ImportAnimations(FbxScene* pFbxScene, ImportScene& importScene)
    {

    }

    void SerializeImportScene(std::string const& savePath, ImportScene const& importScene)
    {
        Serializer::Serailize<Animation>(importScene.animation, "C:\\Users\\Xiaoxiang Zhao\\Desktop\\tests\\dance.anim");
        Serializer::Serailize<SkeletalMesh>(importScene.mesh, "C:\\Users\\Xiaoxiang Zhao\\Desktop\\tests\\maria.mesh");
    }
}