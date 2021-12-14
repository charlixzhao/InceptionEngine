
#include "IE_PCH.h"
#include "KinematicsTree.h"


namespace inceptionengine::dynamics
{
    KinematicsTree::KinematicsTree()
    {
        N = 1;
        Geoms.resize(1, { Mat3x3d::Identity(), Vec3d::Identity() });
        Bodies.resize(1);
        Bodies[0].a = (SpatialAccel() << 0.0f, 0.0f, 0.0f, 0.0, 9.81f, 0.0f).finished();

        Io.resize(1, SpatialInertia::Identity());
        Parents.resize(1, -1);
        Joints.resize(1);
        Joints[0].SetType(JointType::Fixed);
    }

    KinematicsTree::KinematicsTree(int n, int m)
    {
        N = n;
        Geoms.resize(n);
        Bodies.resize(n);
        Io.resize(n, SpatialInertia::Identity());
        Parents.resize(n, -1);

        Joints.resize(m);
        Bodies[0].a = (SpatialAccel() << 0.0f, 0.0f, 0.0f, 0.0, 9.81f, 0.0f).finished();
    }

    KinematicsTree::~KinematicsTree() = default;

    void KinematicsTree::AddCuboidLink(JointType jointType, float x, float y, float z, float r, int parent, Vec3d const& offset)
    {
        N += 1;
        Bodies.push_back(ConstructCuboid(x, y, z));
        Parents.push_back(parent);
        Io.push_back(ParallelInertia(Bodies.back().Ic, (dynamics::Vec3d() << r + x / 2.0f, 0.0f, 0.0f).finished(), Bodies.back().m));
        Joints.push_back(Joint());
        Joints.back().SetType(jointType);
        Geoms.push_back({ dynamics::Mat3x3d::Identity(), offset });


        //only for test
        if (N == 2)
        {
            float theta = -std::numbers::pi / 12;
            Joints.back().q[0] = std::cos(theta);
            Joints.back().q[3] = std::sin(theta);
        }

    }

    void KinematicsTree::ForwardDynamics(std::vector<Vec3d> const& jointTorques, std::vector<SpatialForce> const& ext)
	{
        std::vector<Mat6x6d> IA;
        IA.resize(N);
        std::vector<Vec6d> PA;
        PA.resize(N);

        std::vector<Mat> D;

        D.resize(N);

        for (int i = 1; i < N; i++)
        {
            //compute velocity propagation

            // i^X_\lambda(i)
            Mat6x6d X = PlukerTransform(Joints[i].E(), Joints[i].r()) *
                PlukerTransform(Geoms[i].first.transpose(), Geoms[i].second);

            Bodies[i].V = X * Bodies[Parents[i]].V + Joints[i].S * Joints[i].qd;
            IA[i] = Io[i];
            PA[i] = Skew6Adj(Bodies[i].V) * Io[i] * Bodies[i].V - ext[i];
        }



        for (int j = N - 1; j >= 1; j--)
        {
            //compute articulated body inertia
            auto const& S = Joints[j].S;
            auto const ST = S.transpose();
            D[j] = (ST * IA[j] * S).inverse();
            Mat U = IA[j] * S * D[j];
   
            Mat6x6d Ia = IA[j] - U * ST * IA[j];
 
            Mat6x6d X = PlukerTransform(Joints[j].E(), Joints[j].r()) *
                PlukerTransform(Geoms[j].first.transpose(), Geoms[j].second);

            IA[Parents[j]] += X.transpose() * Ia * X;

            Vec3d torque = (jointTorques.size() != 0) ? jointTorques[j] : Vec3d::Zero();
            Vec6d Pa = PA[j] + Ia * (Skew6(Bodies[j].V) * S) * Joints[j].qd + U * (torque - ST * PA[j]);

            //X.tranpose() = (\lambda(i)^X_i)*, which transform force from i to \lambda(i) 
            PA[Parents[j]] += X.transpose() * Pa;
        }

      
        for (int i = 1; i < N; i++)
        {
            auto const& S = Joints[i].S;
            auto const ST = S.transpose();
            Mat6x6d X = PlukerTransform(Joints[i].E(), Joints[i].r()) *
                PlukerTransform(Geoms[i].first.transpose(), Geoms[i].second);

            auto t1 = ST * (IA[i] * (X * Bodies[Parents[i]].a + Skew6(Bodies[i].V) * S * Joints[i].qd) + PA[i]);
            Vec3d torque = (jointTorques.size() != 0) ? jointTorques[i] : Vec3d::Zero();
            Joints[i].qdd = D[i] * (torque - t1);
            Bodies[i].a = X * Bodies[Parents[i]].a + S * Joints[i].qdd + (Skew6(Bodies[i].V) * S) * Joints[i].qd;
        }

        
	}

    void KinematicsTree::Step(float dt, float threadshold)
    {
        for (auto& j : Joints) j.Step(dt, threadshold);
    }
}