//#include<SelfIntersection.h>
//#include<CGAL/IO/STL/STL_reader.h>
//#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
//#include <CGAL/Surface_mesh.h>
//#include <CGAL/Polygon_mesh_processing/self_intersections.h>
//#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>
//#include <CGAL/Polygon_mesh_processing/stitch_borders.h> 
//#include <iostream>
//#include <iterator>
//#include <string>
//#include <vector>
//typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
//typedef CGAL::Surface_mesh<K::Point_3>                      Mesh;
//typedef boost::graph_traits<Mesh>::face_descriptor          face_descriptor;
//namespace PMP = CGAL::Polygon_mesh_processing;
//
//void SelfIntersection::mergeNearPoint(const std::string& process_file_name, const std::string& output_file_name)
//{
//    Mesh mesh;
//    PMP::IO::read_polygon_mesh(process_file_name, mesh);
//
//    bool intersecting = PMP::does_self_intersect<CGAL::Parallel_if_available_tag>(mesh, CGAL::parameters::vertex_point_map(get(CGAL::vertex_point, mesh)));
//    if (intersecting)
//    {
//        std::vector<std::pair<face_descriptor, face_descriptor> > intersected_tris;
//        PMP::self_intersections<CGAL::Parallel_if_available_tag>(faces(mesh), mesh, std::back_inserter(intersected_tris));
//        if (intersected_tris.size() != 0)
//        {
//            CGAL::Polygon_mesh_processing::stitch_borders(mesh);
//        }
//    }
//    else
//    {
//        CGAL::IO::write_polygon_mesh(output_file_name, mesh);
//    }
//}