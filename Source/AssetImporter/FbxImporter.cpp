
#include "FbxImporter.h"

#include "Math/Math.h"
#include "Serialization/Serializer.h"
#include "PathHelper.h"

#include "EngineGlobals/RenderGlobals.h"
#include "RunTime/SkeletalMesh/SkeletalMesh.h"
#include "RunTime/SkeletalMesh/Skeleton.h"
#include "RunTime/Animation/Animation.h"


#define FBXSDK_SHARED
#include <fbxsdk.h>

#include <vector>
#include <set>
#include <cassert>
#include <iostream>
#include <fstream>


namespace inceptionengine::fbximport
{
    Matrix4x4f ConvertMatrix(FbxAMatrix const& mat);
    FbxScene* CreateFbxScene(std::string const& filePath);
    void Import(std::string const& filePath, std::string const& outputFolder);
    FbxAMatrix GetGeometryTransformation(FbxNode* pNode);
    void ImportMeshVertexPosition(FbxMesh* pFbxMesh, SubMesh& mesh);
    void ImportMeshIndices(FbxMesh* pFbxMesh, SubMesh& mesh);
    void ImportMeshVertexUV(FbxMesh* pFbxMesh, SubMesh& mesh);
    void ImportMeshVertexNormal(FbxMesh* pFbxMesh, SubMesh& mesh);
    void ImportMeshSkin(FbxMesh* pFbxMesh, SubMesh& mesh, Skeleton const& skeleton);
    int FindVertexAgrminBoneWeight(Vertex const& vertex);
    void AddSkinToVertex(Vertex& vertex, int boneID, float weight);
    void NormalizeVertexBoneWeights(Vertex& vertex);
    void ImportSkeletalMesh(FbxScene* pFbxScene, std::vector<FbxNode*> const& meshNodes, SkeletalMesh& mesh, Skeleton const& skeleton);
    void FillDataArray(FbxScene* pFbxScene, std::vector<FbxNode*>& meshNodes, std::vector<FbxNode*>& boneNodes);
    void ImportSkeletonHierachy(FbxScene* pFbxScene, std::vector<FbxNode*> const& boneNodes, Skeleton& skeleton);
    void ImportSkeletonBindPose(FbxScene* pFbxScene, std::vector<FbxNode*> const& meshNodes, Skeleton& skeleton);
    void ImportAnimations(FbxScene* pFbxScene, std::vector<FbxNode*> const& boneNodes, Skeleton const& skeleton, std::vector<Animation>& animations);
    std::vector<std::string> GetFileLines(std::string const& fileName);
    void FillBoneVelocities(Animation& anim, float h);
    std::vector<Matrix4x4f> GetBonesGlobalTransforms(std::vector<Matrix4x4f> const& localTransforms, std::shared_ptr<Skeleton const> skeleton);


    std::vector<std::string> GetFileLines(std::string const& fileName)
    {
        std::ifstream in(fileName.c_str());
        if (!in)
        {
            throw std::runtime_error("Can't open file\n");
        }
        std::vector<std::string> result;
        std::string str;
        while (std::getline(in, str))
        {
            if (str.size() > 0)
            {
                result.push_back(str);
            }
        }
        in.close();
        return result;
    }

    Matrix4x4f ConvertMatrix(FbxAMatrix const& mat)
    {
        Vec4f col1 = Vec4f(mat.GetRow(0)[0], mat.GetRow(0)[1], mat.GetRow(0)[2], mat.GetRow(0)[3]);
        Vec4f col2 = Vec4f(mat.GetRow(1)[0], mat.GetRow(1)[1], mat.GetRow(1)[2], mat.GetRow(1)[3]);
        Vec4f col3 = Vec4f(mat.GetRow(2)[0], mat.GetRow(2)[1], mat.GetRow(2)[2], mat.GetRow(2)[3]);
        Vec4f col4 = Vec4f(mat.GetRow(3)[0], mat.GetRow(3)[1], mat.GetRow(3)[2], mat.GetRow(3)[3]);

        return Matrix4x4f(col1, col2, col3, col4);
    }

    //
    FbxScene* CreateFbxScene(std::string const& filePath)
    {
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


        FbxAxisSystem OurAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
        OurAxisSystem.ConvertScene(pFbxScene);
        FbxSystemUnit::cm.ConvertScene(pFbxScene);

        FbxGeometryConverter geoConverter(fbxsdkManager);
        bool rc = geoConverter.SplitMeshesPerMaterial(pFbxScene, true);
        bool rc2 = geoConverter.Triangulate(pFbxScene, true);
        assert(rc);
        assert(rc2);

        fbxsdkImporter->Destroy();


        return pFbxScene;
    }


    void ReorderAnimations(std::vector<Animation>& animations,
        std::string pathToExistingSkeleton,
        Skeleton const& importedSkeleton,
        std::shared_ptr<Skeleton> existingSkeleton)
    {
        std::cout << "Inconsistency found. Inter bone pair config if want to reorder animation to match skeleton: ";

        std::string bonePairConfig;
        std::getline(std::cin, bonePairConfig);
        std::vector<std::string> bonePairFile = GetFileLines(PathHelper::GetAbsolutePath(bonePairConfig));
        assert(bonePairFile.size() % 2 == 0);
        std::unordered_map<std::string, std::string> bonePairs;
        for (int i = 0; i < bonePairFile.size(); i += 2)
        {
            bonePairs.insert({ bonePairFile[i], bonePairFile[i + 1] });
        }

        std::set<std::string> unmatchedBones;
        for (auto& anim : animations)
        {
            Animation reordered_anim;
            reordered_anim.mDuration = anim.mDuration;
            reordered_anim.mName = anim.mName;
            reordered_anim.mPathToSkeleton = pathToExistingSkeleton;
            reordered_anim.mBoneTransforms.resize(anim.mBoneTransforms.size());
            for (int i = 0; i < reordered_anim.mBoneTransforms.size(); i++)
            {
                reordered_anim.mBoneTransforms[i].resize(existingSkeleton->GetBoneNumber());

                for (int j = 0; j < existingSkeleton->GetBoneNumber(); j++)
                {
                    std::string boneName = existingSkeleton->mBones[j].name;
                    int correspondBoneID = -1;
                    if (bonePairs.find(boneName) != bonePairs.end())
                    {
                        correspondBoneID = importedSkeleton.GetBoneID(bonePairs.at(boneName));
                        if (correspondBoneID == -1)
                        {
                            throw std::runtime_error("Error in pair config\n");
                        }
                    }

                    if (correspondBoneID != -1)
                    {
                        reordered_anim.mBoneTransforms[i][j] = anim.mBoneTransforms[i][correspondBoneID];
                    }
                    else
                    {
                        unmatchedBones.insert(boneName);
                        reordered_anim.mBoneTransforms[i][j] = existingSkeleton->mBones[j].lclRefPose;
                    }
                }
            }

            anim = std::move(reordered_anim);
        }
        for (auto& boneName : unmatchedBones)
        {
            std::cout << "Bone " << boneName << " does not find a correpondent\n";
        }
    }

    void Import(std::string const& filePath, std::string const& outputFolder)
    {
        std::cout << "Import begin, please wait.\n";
        std::cout << "...\n";
        
        FbxScene* pFbxScene = CreateFbxScene(filePath);
        std::vector<FbxNode*> boneNodes;
        std::vector<FbxNode*> meshNodes;
        FillDataArray(pFbxScene, meshNodes, boneNodes);

        Skeleton skeleton;
        ImportSkeletonHierachy(pFbxScene, boneNodes, skeleton);
        ImportSkeletonBindPose(pFbxScene, meshNodes, skeleton);

        SkeletalMesh skmesh;
        ImportSkeletalMesh(pFbxScene, meshNodes, skmesh, skeleton);

        std::vector<Animation> animations;
        ImportAnimations(pFbxScene, boneNodes, skeleton, animations);


        if (skeleton.mBones.size() > 0)
        {
            std::cout << "The mesh has skeleton, so it's a skeletal mesh\n";
            std::string useExsitingSkeleton;
            std::cout << "Do you want to use an existing skeleton?(y/n)\n";
            std::getline(std::cin, useExsitingSkeleton);
            if (useExsitingSkeleton == "y")
            {
                std::string pathToSkeleton;
                std::cout << "Enter the path to the skeleton file, without extension: ";
                std::getline(std::cin, pathToSkeleton);
                pathToSkeleton += ".ie_skeleton";
                std::shared_ptr<Skeleton> existingSkeleton = Serializer::Deserailize<Skeleton>(PathHelper::GetAbsolutePath(pathToSkeleton));
                if (skeleton <= *existingSkeleton)
                {
                    std::cout << "Successfully use an existing skeleton\n";
                    skmesh.mPathToSkeleton = pathToSkeleton;
                    for (auto& anim : animations) anim.mPathToSkeleton = pathToSkeleton;
                }
                else
                {
                    ReorderAnimations(animations, pathToSkeleton, skeleton, existingSkeleton);
                }
            }
            else
            {
                std::string skeletonFileName;
                std::cout << "Enter a name for the skeleton file, without extension: ";
                std::getline(std::cin, skeletonFileName);
                Serializer::Serailize<Skeleton>(skeleton, PathHelper::GetAbsolutePath(outputFolder) + "\\" + skeletonFileName + ".ie_skeleton");
                std::cout << "Import skelelton done\n";
                std::string pathToSkeleton = outputFolder + "\\" + skeletonFileName + ".ie_skeleton";
                skmesh.mPathToSkeleton = pathToSkeleton;
                for (auto& anim : animations) anim.mPathToSkeleton = pathToSkeleton;
            }
        }
        else
        {
            std::cout << "The mesh has no skeleton, so it's static mesh\n";
        }

        for (auto& anim : animations) FillBoneVelocities(anim, 1.0f / SampleRate);

       
        std::string meshFileName;
        std::cout << "Enter a name for the mesh file, without extension, empty if don't serialize it: ";
        std::getline(std::cin, meshFileName);
        if(meshFileName != "") Serializer::Serailize<SkeletalMesh>(skmesh, PathHelper::GetAbsolutePath(outputFolder) + "\\" + meshFileName + ".ie_skmesh");
        std::cout << "Import mesh done\n";

        for (int animIndex = 0; animIndex < animations.size(); animIndex++)
        {
            std::cout << "Find animation " << animations[animIndex].mName << std::endl;
            std::string animFileName;
            std::cout << "Enter a name for this animation file, without extension, empty if don't serialize it: ";
            std::getline(std::cin, animFileName);
            if(animFileName != "") Serializer::Serailize<Animation>(animations[animIndex], PathHelper::GetAbsolutePath(outputFolder) + "\\" + animFileName + ".ie_anim");
        }
        std::cout << "Import animation done\n";

    }



    FbxAMatrix GetGeometryTransformation(FbxNode* pNode)
    {
        const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
        const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
        const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

        return FbxAMatrix(lT, lR, lS);
    }

    void ImportMeshVertexPosition(FbxMesh* pFbxMesh, SubMesh& mesh)
    {
        //auto axisMat = pFbxMesh->GetNode()->EvaluateGlobalTransform();//.Inverse();
        //auto geoMat = GetGeometryTransformation(pFbxMesh->GetNode());

        int ctrlPointsCount = pFbxMesh->GetControlPointsCount();

        for (int ctrlPointIndex = 0; ctrlPointIndex < ctrlPointsCount; ctrlPointIndex++)
        {
            fbxsdk::FbxVector4 ctrlPoint = pFbxMesh->GetControlPointAt(ctrlPointIndex);

            Vertex vertex;
            //vertex.position = ConvertMatrix(axisMat * geoMat) * Vec4f(ctrlPoint[0], ctrlPoint[1], ctrlPoint[2], 1.0f);
            vertex.position = Vec4f(ctrlPoint[0], ctrlPoint[1], ctrlPoint[2], 1.0f);

            mesh.vertices.push_back(vertex);
        }

    }





    void ImportMeshIndices(FbxMesh* pFbxMesh, SubMesh& mesh)
    {
        int triangleCount = pFbxMesh->GetPolygonCount();
        for (int triangleIndex = 0; triangleIndex < triangleCount; triangleIndex++)
        {
            assert(pFbxMesh->GetPolygonSize(triangleIndex) == 3);

            mesh.indices.push_back(pFbxMesh->GetPolygonVertex(triangleIndex, 0));
            mesh.indices.push_back(pFbxMesh->GetPolygonVertex(triangleIndex, 1));
            mesh.indices.push_back(pFbxMesh->GetPolygonVertex(triangleIndex, 2));
        }
    }


    void ImportMeshVertexUV(FbxMesh* pFbxMesh, SubMesh& mesh)
    {
        FbxStringList uvSetNameList;
        pFbxMesh->GetUVSetNames(uvSetNameList);

        for (int uvSetIndex = 0; uvSetIndex < uvSetNameList.GetCount(); uvSetIndex++)
        {
            //get uvSetIndex-th uv set
            const char* uvSetName = uvSetNameList.GetStringAt(uvSetIndex);
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

                        mesh.vertices[lPolyVertIndex].texCoord = Vec3f(lUVValue.mData[0], 1.0f - lUVValue.mData[1], 0.0f);

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

                            mesh.vertices[ctrlPointIndex].texCoord = Vec3f(lUVValue.mData[0], 1.0f - lUVValue.mData[1], 0.0f);

                            lPolyIndexCounter++;
                        }
                    }
                }
            }
        }
    }


    void ImportMeshVertexNormal(FbxMesh* pFbxMesh, SubMesh& mesh)
    {
        auto axisMat = pFbxMesh->GetNode()->EvaluateGlobalTransform();//.Inverse();
        auto geoMat = GetGeometryTransformation(pFbxMesh->GetNode());

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
                    mesh.vertices[lVertexIndex].normal = Vec3f(adjustedNormal);

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
                        //FBXSDK_printf("normals for polygon[%d]vertex[%d]: %f %f %f %f \n",
                         //             lPolygonIndex, i, lNormal[0], lNormal[1], lNormal[2], lNormal[3]);
                        //add your custom code here, to output normals or get them into a list, such as KArrayTemplate<FbxVector4>
                        int ctrlPointIndex = pFbxMesh->GetPolygonVertex(lPolygonIndex, i);

                        Vec4f adjustedNormal = ConvertMatrix(axisMat * geoMat) * Vec4f(lNormal[0], lNormal[1], lNormal[2], 0.0f);
                        mesh.vertices[ctrlPointIndex].normal = Vec3f(adjustedNormal);

                        lIndexByPolygonVertex++;
                    }//end for i //lPolygonSize
                }//end for lPolygonIndex //PolygonCount

            }//end eByPolygonVertex
        }//end if lNormalElement
    }






    void ImportMeshSkin(FbxMesh* pFbxMesh, SubMesh& mesh, Skeleton const& skeleton)
    {
        if (skeleton.mBones.size() == 0)
            return;

        int skinCount = pFbxMesh->GetDeformerCount(FbxDeformer::eSkin);
        if (skinCount > 0)
        {
            for (auto& vertex : mesh.vertices)
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
                int boneID = skeleton.GetBoneID(pFbxCluster->GetLink()->GetName());
                assert(boneID != -1);
                for (int affectCtrlPointLoop = 0; affectCtrlPointLoop < pFbxCluster->GetControlPointIndicesCount(); affectCtrlPointLoop++)
                {
                    int affectCtrlPointIndex = (pFbxCluster->GetControlPointIndices())[affectCtrlPointLoop];
                    float affectCtrlPointWeight = static_cast<float>(pFbxCluster->GetControlPointWeights()[affectCtrlPointLoop]);
                    AddSkinToVertex(mesh.vertices[affectCtrlPointIndex], boneID, affectCtrlPointWeight);
                }
            }
        }

        for (auto& vertex : mesh.vertices)
        {
            NormalizeVertexBoneWeights(vertex);
        }
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


    void ImportSkeletalMesh(FbxScene* pFbxScene, std::vector<FbxNode*> const& meshNodes, SkeletalMesh& mesh, Skeleton const& skeleton)
    {
        for (int meshIndex = 0; meshIndex < meshNodes.size(); meshIndex++)
        {
            int attributeCount = meshNodes[meshIndex]->GetNodeAttributeCount();
            for (int submeshIndex = 0; submeshIndex < attributeCount; submeshIndex++)
            {
                FbxMesh* pFbxMesh = reinterpret_cast<FbxMesh*>(meshNodes[meshIndex]->GetNodeAttributeByIndex(submeshIndex));
                if (pFbxMesh)
                {
                    mesh.mSubMeshes.push_back({});
                    mesh.mSubMeshes.back().mName = pFbxMesh->GetName();

                    ImportMeshVertexPosition(pFbxMesh, mesh.mSubMeshes.back());
                    ImportMeshIndices(pFbxMesh, mesh.mSubMeshes.back());
                    ImportMeshVertexUV(pFbxMesh, mesh.mSubMeshes.back());
                    ImportMeshVertexNormal(pFbxMesh, mesh.mSubMeshes.back());
                    ImportMeshSkin(pFbxMesh, mesh.mSubMeshes.back(), skeleton);

                    std::cout << "Find submesh in the fbx. The name of the submesh is " << pFbxMesh->GetName() << std::endl;
                    std::cout << "Enter a texture path for the submesh, empty to use default texture: ";
                    std::getline(std::cin, mesh.mSubMeshes.back().texturePath);
                    if (mesh.mSubMeshes.back().texturePath == "")
                    {
                        mesh.mSubMeshes.back().texturePath = "EngineResource\\texture\\DefaultTexture.png";
                    }
                }
            }

            /*
            FbxMesh* pFbxMesh = meshNodes[meshIndex]->GetMesh();

            assert(pFbxMesh != nullptr);

            mesh.mSubMeshes.push_back({});
            mesh.mSubMeshes.back().mName = pFbxMesh->GetName();

            ImportMeshVertexPosition(pFbxMesh, mesh.mSubMeshes.back());
            ImportMeshIndices(pFbxMesh, mesh.mSubMeshes.back());
            ImportMeshVertexUV(pFbxMesh, mesh.mSubMeshes.back());
            ImportMeshVertexNormal(pFbxMesh, mesh.mSubMeshes.back());
            ImportMeshSkin(pFbxMesh, mesh.mSubMeshes.back(), skeleton);

            std::cout << "Find submesh in the fbx. The name of the submesh is " << pFbxMesh->GetName() << std::endl;
            std::cout << "Enter a texture path for the submesh, empty to use default texture: ";
            std::getline(std::cin, mesh.mSubMeshes.back().texturePath);*/
        }
    }

    void FillDataArray(FbxScene* pFbxScene, std::vector<FbxNode*>& meshNodes, std::vector<FbxNode*>& boneNodes)
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
                meshNodes.push_back(curNode);
            }
            else if (curNode->GetNodeAttribute() != nullptr && curNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::EType::eSkeleton)
            {
                boneNodes.push_back(curNode);
            }
        }
    }

    void ImportSkeletonHierachy(FbxScene* pFbxScene, std::vector<FbxNode*> const& boneNodes, Skeleton& skeleton)
    {
        for (int boneIndex = 0; boneIndex < boneNodes.size(); boneIndex++)
        {
            skeleton.mNameToIDMap.insert(std::pair<std::string, int>(boneNodes[boneIndex]->GetName(), boneIndex));
        }

        for (int boneIndex = 0; boneIndex < boneNodes.size(); boneIndex++)
        {
            Skeleton::Bone bone;
            bone.name = boneNodes[boneIndex]->GetName();
            bone.ID = skeleton.GetBoneID(bone.name);
            FbxNode* parentNode = boneNodes[boneIndex]->GetParent();
            if (parentNode->GetNodeAttribute() != nullptr && parentNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::EType::eSkeleton)
            {
                bone.parentID = skeleton.GetBoneID(boneNodes[boneIndex]->GetParent()->GetName());
            
            }
            else
            {
                bone.parentID = -1;
            }
            bone.bindPose = Matrix4x4f(1.0f); //ConvertMatrix(boneNodes[boneIndex]->EvaluateGlobalTransform());
            bone.bindPoseInv = Matrix4x4f(1.0f);  //Inverse(bone.bindPose);
            bone.lclRefPose = ConvertMatrix(boneNodes[boneIndex]->EvaluateGlobalTransform());

            skeleton.mBones.push_back(bone);
        }
        std::vector<Matrix4x4f> globalRefPose;
        for (auto const& bone : skeleton.mBones)
        {
            globalRefPose.push_back(bone.lclRefPose);
        }
        for (int i = 0; i < skeleton.mBones.size(); i++)
        {
            if (skeleton.mBones[i].parentID != -1)
            {
                skeleton.mBones[i].lclRefPose = Inverse(globalRefPose[skeleton.mBones[i].parentID]) * globalRefPose[i];
            }
            else
            {
                skeleton.mBones[i].lclRefPose = globalRefPose[i];
            }
        }
    }


    void ImportSkeletonBindPose(FbxScene* pFbxScene, std::vector<FbxNode*> const& meshNodes, Skeleton& skeleton)
    {
        for (int i = 0; i < meshNodes.size(); i++)
        {
            FbxNode* inNode = meshNodes[i];
            FbxMesh* currMesh = inNode->GetMesh();
            unsigned int numOfDeformers = currMesh->GetDeformerCount();

            FbxAMatrix geometryTransform = GetGeometryTransformation(inNode);

            for (unsigned int deformerIndex = 0; deformerIndex < numOfDeformers; ++deformerIndex)
            {
                // There are many types of deformers in Maya, 
                // We are using only skins, so we see if this is a skin 
                FbxSkin* currSkin = reinterpret_cast<FbxSkin*>(currMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));

                if (!currSkin) { continue; }

                unsigned int numOfClusters = currSkin->GetClusterCount();
                for (unsigned int clusterIndex = 0; clusterIndex < numOfClusters; ++clusterIndex)
                {
                    FbxCluster* currCluster = currSkin->GetCluster(clusterIndex);
                    std::string currJointName = currCluster->GetLink()->GetName();
                    unsigned int currJointIndex = skeleton.GetBoneID(currJointName);
                    assert(currJointIndex != -1);
                    FbxAMatrix transformMatrix;
                    FbxAMatrix transformLinkMatrix;
                    currCluster->GetTransformMatrix(transformMatrix);
                    currCluster->GetTransformLinkMatrix(transformLinkMatrix);

                    skeleton.mBones[currJointIndex].bindPoseInv = ConvertMatrix(transformLinkMatrix.Inverse() * transformMatrix * geometryTransform);
                    skeleton.mBones[currJointIndex].bindPose = Inverse(skeleton.mBones[currJointIndex].bindPoseInv);
                }
            }
        }
    }

    template<typename T>
    std::vector<T> ComputeVelocities(std::vector<T> const& pos, float h)
    {
        std::vector<T> vels;
        vels.reserve(pos.size());
        vels.push_back((pos[1] - pos[0]) / h);
        vels.push_back(ThreePointStencil(pos[2], pos[0], h));
        for (int i = 2; i < pos.size() - 2; i++)
        {
            vels.push_back(FivePointStencil(pos[i + 2], pos[i + 1], pos[i - 1], pos[i - 2], h));
        }
        vels.push_back(ThreePointStencil(pos[pos.size() - 1], pos[pos.size() - 3], h));
        vels.push_back((pos[pos.size() - 1] - pos[pos.size() - 2]) / h);
        assert(pos.size() == vels.size());
        return vels;
    }

    void FillBoneVelocities(Animation& anim, float h)
    {
        auto skeleton = Serializer::Deserailize<Skeleton>(PathHelper::GetAbsolutePath(anim.mPathToSkeleton));
        size_t boneNumber = skeleton->GetBoneNumber();

        //compute global positions of bones
        std::vector<std::vector<Vec3f>> globalPositions;
        for (auto const& lclFrame : anim.mBoneTransforms)
        {
            std::vector<Vec3f> globalPosition;
            auto globalFrame = GetBonesGlobalTransforms(lclFrame, skeleton);
            for (auto const& boneTransform : globalFrame)
            {
                globalPosition.push_back(boneTransform[3]);
            }
            globalPositions.push_back(globalPosition);
        }

        //prepare space for velocities
        anim.mBoneVelocities.resize(anim.mBoneTransforms.size());
        for (auto& vel : anim.mBoneVelocities) { vel.resize(boneNumber); }

        //compute bone velocities
        for (int boneIndex = 0; boneIndex < boneNumber; boneIndex++)
        {
            std::vector<Vec3f> bonePositions;
            bonePositions.reserve(anim.mBoneTransforms.size());
            for (int i = 0; i < anim.mBoneTransforms.size(); i++)
            {
                bonePositions.push_back(globalPositions[i][boneIndex]);
            }
            std::vector<Vec3f> boneVelocities = ComputeVelocities(bonePositions, h);
            for (int i = 0; i < boneVelocities.size(); i++) { anim.mBoneVelocities[i][boneIndex] = boneVelocities[i]; }
        }
    }

    void ImportAnimations(FbxScene* pFbxScene, std::vector<FbxNode*> const& boneNodes, Skeleton const& skeleton, std::vector<Animation>& animations)
    {
        for (int animIndex = 0; animIndex < pFbxScene->GetSrcObjectCount<FbxAnimStack>(); animIndex++)
        {
            FbxAnimStack* pAnimStack = pFbxScene->GetSrcObject<FbxAnimStack>(animIndex);
            assert(pAnimStack != nullptr);
            pFbxScene->SetCurrentAnimationStack(pAnimStack);

            animations.push_back({});
            animations.back().mName = pAnimStack->GetName();

            double duration = pAnimStack->GetLocalTimeSpan().GetDuration().GetSecondDouble();
            animations.back().mDuration = static_cast<float>(duration);
            double constexpr sampleRate = static_cast<double>(SampleRate);
            std::vector<std::vector<Vec3f>> globalPositions;
            for (double time = 0.0; time <= duration; time += (1.0 / sampleRate))
            {
                FbxTime fbxTime;
                fbxTime.SetSecondDouble(time);
                std::vector<Matrix4x4f> globalFrame;
                for (int boneIndex = 0; boneIndex < boneNodes.size(); boneIndex++)
                {
                    globalFrame.push_back(ConvertMatrix(boneNodes[boneIndex]->EvaluateGlobalTransform(fbxTime)));
                }

                std::vector<Matrix4x4f> localFrame;
                localFrame.resize(globalFrame.size());
                for (auto const& bone : skeleton.mBones)
                {
                    if (bone.parentID == -1)
                    {
                        localFrame[bone.ID] = globalFrame[bone.ID];
                    }
                    else
                    {
                        localFrame[bone.ID] = Inverse(globalFrame[bone.parentID]) * globalFrame[bone.ID];
                    }
                }
                animations.back().mBoneTransforms.push_back(std::move(localFrame));
            }
        }
    }


    std::vector<Matrix4x4f> GetBonesGlobalTransforms(std::vector<Matrix4x4f> const& localTransforms, std::shared_ptr<Skeleton const> skeleton)
    {
        std::vector<Matrix4x4f> globalFinalPose;
        globalFinalPose.resize(localTransforms.size());
        for (auto const& bone : skeleton->mBones)
        {
            Matrix4x4f globalTransform = localTransforms[bone.ID];
            int parentID = bone.parentID;
            while (parentID != -1)
            {
                globalTransform = localTransforms[parentID] * globalTransform;
                parentID = skeleton->mBones[parentID].parentID;
            }
            globalFinalPose[bone.ID] = globalTransform;
        }

        return globalFinalPose;
    }
    
 

}
