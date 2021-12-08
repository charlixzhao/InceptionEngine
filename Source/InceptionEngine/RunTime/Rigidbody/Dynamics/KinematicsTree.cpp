

#include "KinematicsTree.h"


namespace inceptionengine::dynamics
{
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

         
            /*
            Mat6x6d Y = PlukerTransformInvAdj(Geoms[j].first.transpose(), Geoms[j].second) *
                PlukerTransformInvAdj(Joints[j].E(), (Vec3d() << 0.0f, 0.0f, 0.0f).finished());*/

            IA[Parents[j]] += X.transpose() * Ia * X;


            Vec6d Pa = PA[j] + Ia * (Skew6(Bodies[j].V) * S) * Joints[j].qd + U * (jointTorques[j] - ST * PA[j]);

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
            Joints[i].qdd = D[i] * (jointTorques[i] - t1);
            Bodies[i].a = X * Bodies[Parents[i]].a + S * Joints[i].qdd + (Skew6(Bodies[i].V) * S) * Joints[i].qd;
        }

        
	}
}