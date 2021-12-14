
#include "IE_PCH.h"
#include "KinematicsTree.h"


namespace inceptionengine::dynamics
{
    KinematicsTree::KinematicsTree()
    {
        N = 1;
        Geoms.resize(1, { Mat3x3d::Identity(), Vec3d::Identity() });
        Bodies.resize(1);
        //Bodies[0].a = (SpatialAccel() << 0.0f, 0.0f, 0.0f, 0.0, 9.81f, 0.0f).finished();

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
       // Bodies[0].a = (SpatialAccel() << 0.0f, 0.0f, 0.0f, 0.0, 9.81f, 0.0f).finished();
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



    }

    void KinematicsTree::InitializeComputationBuffer()
    {
        xd = Vec::Zero(N * (4 + 3));

        bodyVels.resize(N, SpatialVel::Zero());
        bodyAccels.resize(N, SpatialAccel::Zero());
        bodyAccels[0] = (SpatialAccel() << 0.0f, 0.0f, 0.0f, 0.0, 9.81f, 0.0f).finished();
        IA.resize(N);
        PA.resize(N);
        D.resize(N);
        X.resize(N);
        P.resize(4, 3);
    }

    void dynamics::KinematicsTree::SetJointPdot(int i, Vec4d const& p, Vec3d const& qd)
    {
        int base = i * 4;        
        P << -p[1], -p[2], -p[3],
            p[0], -p[3], p[2],
            p[3], p[0], -p[1],
            -p[2], p[1], p[0];

        xd.block<4, 1>(base, 0) = 0.5 * P * qd;

    }

    void dynamics::KinematicsTree::SetJointQddot(int i, Vec3d const& qdd)
    {
        int base = N * 4 + i * 3;
        xd.block<3, 1>(base, 0) = qdd;
    }
    

    Vec KinematicsTree::f(Vec const& x, std::vector<Vec3d> const& jointTorques, std::vector<SpatialForce> const& ext)
    {
        
        for (int i = 1; i < N; i++)
        {
            //compute velocity propagation
            int pBase = 4 * i;
            int qDotBase = 4 * N + 3 * i;

            // i^X_\lambda(i)
            X[i] = PlukerTransform(Joint::E(x.block<4, 1>(pBase, 0), Joints[i].Type), Joints[i].r()) *
                PlukerTransform(Geoms[i].first.transpose(), Geoms[i].second);

            bodyVels[i] = X[i] * bodyVels[Parents[i]] + Joints[i].S * x.block<3, 1>(qDotBase, 0);
            IA[i] = Io[i];
            PA[i] = Skew6Adj(bodyVels[i]) * Io[i] * bodyVels[i] - ext[i];

            SetJointPdot(i, x.block<4, 1>(pBase, 0), x.block<3, 1>(qDotBase, 0));
        }

        for (int j = N - 1; j >= 1; j--)
        {
            int qDotBase = 4 * N + 3 * j;
            //compute articulated body inertia
            auto const& S = Joints[j].S;
            auto const ST = Joints[j].ST;
            auto XjT = X[j].transpose();

            D[j] = (ST * IA[j] * S).inverse();

            Mat U = IA[j] * S * D[j];

            Mat6x6d Ia = IA[j] - U * ST * IA[j];

            IA[Parents[j]] += XjT * Ia * X[j];

            Vec3d torque = (jointTorques.size() != 0) ? jointTorques[j] : Vec3d::Zero();

            Vec6d Pa = PA[j] + Ia * (Skew6(bodyVels[j]) * S) * x.block<3, 1>(qDotBase, 0) + U * (torque - ST * PA[j]);

            //X.tranpose() = (\lambda(i)^X_i)*, which transform force from i to \lambda(i) 
            PA[Parents[j]] += XjT * Pa;
        }


        for (int i = 1; i < N; i++)
        {
            int qDotBase = 4 * N + 3 * i;
            auto const& S = Joints[i].S;
            auto const& ST = Joints[i].ST;
 
            auto t1 = ST * (IA[i] * (X[i] * bodyAccels[Parents[i]] + Skew6(bodyVels[i]) * S * x.block<3, 1>(qDotBase, 0)) + PA[i]);
            Vec3d torque = (jointTorques.size() != 0) ? jointTorques[i] : Vec3d::Zero();
            Vec qdd = D[i] * (torque - t1);
            SetJointQddot(i, qdd);
            bodyAccels[i] = X[i] * bodyAccels[Parents[i]] + S * qdd + (Skew6(bodyVels[i]) * S) * x.block<3, 1>(qDotBase, 0);
        }

        return xd;
    }


    
    /*

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

        
	}*/
}