#include "RemeshMesh.h"
#include <vcg/space/point3.h>
#include <cmath>
#include <vcg/complex/algorithms/isotropic_remeshing.h>
#include <vcg/complex/algorithms/smooth.h>
#include <vcg/simplex/face/pos.h>
#include "vcg/complex/append.h"
#include <assert.h>
#include <vcg/complex/algorithms/closest.h>
#include <vcg/complex/algorithms/intersection.h>
class RemeshMesh::PImpl
{
public:
	PImpl(CMeshO mesh)
	{
		mesh_ = mesh;
		mesh_.vert.EnableMark();
		mesh_.face.EnableMark();
		mesh_.vert.EnableVFAdjacency();
		mesh_.face.EnableFFAdjacency();
		vcg::tri::UnMarkAll(mesh_);
		vcg::tri::InitFaceIMark(mesh_);
		vcg::tri::InitVertexIMark(mesh_);
		//vcg::tri::InitEdgeIMark(mesh_);
		//vcg::tri::InitTetraIMark(mesh_);

		par_.Iterations = 10;
		par_.Adaptive = false;
		par_.SelectedOnly = true;
		par_.SplitFlag = true;
		par_.CollapseFlag = true;
		par_.SmoothFlag = true;
		par_.SwapFlag = true;
		par_.ReprojectFlag = true;
		par_.FeatureAngleDeg = 30.0f;
		par_.MaxSurfDist = 0.073973;
		par_.CheckSurfDist = true;
		par_.TargetLength = 0.073973;
		select_face_.clear();
	}
	~PImpl() {}
public:
	CMeshO mesh_;
	CMeshO output_mesh_;
	std::set<CFaceO*> select_face_;
	InterFaceParam par_;
public:
	void selectFace();
	void ringNeighborhoodFF(CFaceO* f, std::set<CFaceO*>& new_point);
	void IsotropicRemeshing();
};

RemeshMesh::RemeshMesh(CMeshO mesh)
{
	impl_.reset(new PImpl(mesh));
}

RemeshMesh::~RemeshMesh()
{
}


void RemeshMesh::getMesh(CMeshO& result)
{
	result = impl_->mesh_;
}

void RemeshMesh::apply()
{
	if (impl_->par_.SelectedOnly)
	{
		//assert(vcg::tri::HasVFAdjacency(impl_->mesh_) == false);
		impl_->mesh_.face.EnableVFAdjacency();
		impl_->selectFace();
	}

	impl_->IsotropicRemeshing();
}

void RemeshMesh::PImpl::selectFace()
{
	vcg::tri::Clean<CMeshO>::RemoveDuplicateVertex(mesh_);
	vcg::tri::Clean<CMeshO>::RemoveUnreferencedVertex(mesh_);
	vcg::tri::Allocator<CMeshO>::CompactEveryVector(mesh_);
	vcg::tri::UpdateTopology<CMeshO>::FaceFace(mesh_);
	vcg::tri::UpdateTopology<CMeshO>::VertexFace(mesh_);
	vcg::tri::RequirePerVertexMark(mesh_);
	vcg::tri::RequirePerFaceMark(mesh_);

	std::vector<CFaceO*> intersectPairs;
	vcg::tri::Clean<CMeshO>::SelfIntersections(mesh_, intersectPairs);
	for (auto itor : intersectPairs)
	{
		itor->SetS();
		select_face_.insert(itor);
	}
	for (auto& itor : mesh_.face)
	{
		CVertexO* v0 = itor.V(0);
		CVertexO* v1 = itor.V(1);
		CVertexO* v2 = itor.V(2);

		vcg::Point3f edge1 = v1->P() - v0->P();
		vcg::Point3f edge2 = v2->P() - v0->P();
		vcg::Point3f edge3 = v2->P() - v1->P();
		float triangle_area = vcg::Norm(edge1 ^ edge2)*0.5;

		float e_len1 = edge1.Norm();
		float e_len2 = edge2.Norm();
		float e_len3 = edge3.Norm();
		float r_circum = e_len1 * e_len2 * e_len3 / (4 * triangle_area);

		float min_edge = std::min(e_len1, std::min(e_len2, e_len3));
		float rate = r_circum / min_edge;
		if (rate > 2.0 || triangle_area < 10 * std::pow(10, -5))
		{
			itor.SetS();
			//std::cout << mesh_.imark << std::endl;
			//std::cout << itor.IMark() << std::endl;
			//itor.IMark() = mesh_.imark;
			select_face_.insert(&itor);
		}
	}
	
	for (int i = 0; i < 3; i++)
	{
		std::set<CFaceO*> new_face;
		for (auto it : select_face_)
		{
			ringNeighborhoodFF(it, new_face);
		}
		if (new_face.size() == 0)
			break;
		select_face_.clear();
		select_face_ = new_face;
	}

#if 0
	CMeshO mm = mesh_;
	for (auto& itor : mm.face)
	{
		if (itor.IsS())
		{
			itor.SetD();
		}
	}
	vcg::tri::Clean<CMeshO>::RemoveDuplicateFace(mm);
	vcg::tri::Clean<CMeshO>::RemoveUnreferencedVertex(mm);
	vcg::tri::Allocator<CMeshO>::CompactEveryVector(mm);
	vcg::tri::io::ExporterOBJ<CMeshO>::Save(mm, "D:/data/Remeshing/selectFace.obj", 1);
#endif

}

void RemeshMesh::PImpl::ringNeighborhoodFF(CFaceO* f, std::set<CFaceO*>& new_face)
{
	// 获取当前面的索引与位置
	for (int i = 0; i < 3; ++i) 
	{
		auto neighbor_f = f->FFp(i);
		if (neighbor_f != nullptr)
		{
			neighbor_f->SetS();
			//f->IMark() = mesh_.imark;
			new_face.insert(neighbor_f);
			//std::cout << neighbor_f->Index() << std::endl;
		}
	}
}

void RemeshMesh::PImpl::IsotropicRemeshing()
{
	/*vcg::tri::Clean<CMeshO>::RemoveDuplicateVertex(mesh_);
	vcg::tri::Clean<CMeshO>::RemoveUnreferencedVertex(mesh_);
	vcg::tri::Allocator<CMeshO>::CompactEveryVector(mesh_);*/
	mesh_.face.EnableFFAdjacency();
	vcg::tri::UpdateTopology<CMeshO>::FaceFace(mesh_);
	mesh_.vert.EnableVFAdjacency();
	mesh_.face.EnableVFAdjacency();
	vcg::tri::UpdateTopology<CMeshO>::VertexFace(mesh_);
	mesh_.face.EnableWedgeTexCoord();
	mesh_.face.EnableColor();
	mesh_.face.EnableQuality();
	mesh_.face.EnableCurvatureDir();
	mesh_.face.EnableMark();
	mesh_.vert.EnableMark();
	mesh_.vert.EnableCurvatureDir();
	mesh_.vert.EnableRadius();
	mesh_.vert.EnableTexCoord();

	vcg::tri::Clean<CMeshO>::RemoveDuplicateVertex(mesh_);
	vcg::tri::Clean<CMeshO>::RemoveUnreferencedVertex(mesh_);
	vcg::tri::Allocator<CMeshO>::CompactEveryVector(mesh_);

	vcg::tri::UpdateBounding<CMeshO>::Box(mesh_);
	if (mesh_.fn > 0) {
		vcg::tri::UpdateNormal<CMeshO>::PerFaceNormalized(mesh_);
		vcg::tri::UpdateNormal<CMeshO>::PerVertexAngleWeighted(mesh_);
	}

	vcg::tri::IsotropicRemeshing<CMeshO>::Params params;
	params.SetTargetLen(par_.TargetLength);
	params.SetFeatureAngleDeg(par_.FeatureAngleDeg);
	params.maxSurfDist = par_.MaxSurfDist;
	params.iter = par_.Iterations;
	params.adapt = par_.Adaptive;
	params.selectedOnly = par_.SelectedOnly;
	params.splitFlag = par_.SplitFlag;
	params.collapseFlag = par_.CollapseFlag;
	params.swapFlag = par_.SwapFlag;
	params.smoothFlag = par_.SmoothFlag;
	params.projectFlag = par_.ReprojectFlag;
	params.surfDistCheck = par_.CheckSurfDist;


	//std::cout << mesh_.imark << std::endl;
	CMeshO toProjectCopy = mesh_;
	toProjectCopy.face.EnableMark();
	vcg::tri::IsotropicRemeshing<CMeshO>::Do(mesh_, toProjectCopy, params);
}