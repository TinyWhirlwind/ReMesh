#ifndef REMESHMESH_H
#define REMESHMESH_H

#include <vcg/complex/complex.h>
#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/isotropic_remeshing.h>
#include <vcg/complex/algorithms/update/flag.h>
#include "mymesh.h"
//class Face;
//class Vertex;
//class UsedTypes :public vcg::UsedTypes<vcg::Use<Vertex>::AsVertexType, vcg::Use<Face>::AsFaceType> {};
//class Vertex :public vcg::Vertex<UsedTypes, vcg::vertex::VFAdj, vcg::vertex::Coord3f, vcg::vertex::Normal3f, vcg::vertex::Color4b, vcg::vertex::BitFlags, vcg::face::Mark> {};
//class Face :public vcg::Face<UsedTypes, vcg::face::VertexRef, vcg::face::Normal3f, vcg::face::FFAdj, vcg::face::VFAdj, vcg::face::BitFlags, vcg::face::Mark> {};
//class Mesh :public vcg::tri::TriMesh<std::vector<Vertex>, std::vector<Face>> {};

struct InterFaceParam
{
	float TargetLength;
	float FeatureAngleDeg;
	float MaxSurfDist;
	int Iterations;
	bool Adaptive;
	bool SelectedOnly;
	bool SplitFlag;
	bool CollapseFlag;
	bool SwapFlag;
	bool SmoothFlag;
	bool  ReprojectFlag;
	bool CheckSurfDist;
};
class RemeshMesh
{
public:
	RemeshMesh(CMeshO mesh);
	~RemeshMesh();

public:;
	void apply();
	void getMesh(CMeshO& result);
private:
	class PImpl;
	std::shared_ptr<PImpl> impl_;
};
#endif