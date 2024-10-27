#ifndef CGAL_CUSTOM_CONSTRAINED_DELAUNAY_TRIANGULATION_2_H
#define CGAL_CUSTOM_CONSTRAINED_DELAUNAY_TRIANGULATION_2_H

//include all the headers for the necessary libraries 
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/draw_constrained_triangulation_2.h>
#include <CGAL/enum.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_traits_2.h>
#include <CGAL/centroid.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/json.hpp>

#include <fstream>
//#include <utility>
#include <string>
#include <vector>
#include <gmp.h>
#include <iostream>


//typedef for the kernel, predicates and the original CDT class  
typedef CGAL::Exact_predicates_exact_constructions_kernel K;
typedef CGAL::Exact_predicates_tag Itag;
typedef CGAL::Constrained_Delaunay_triangulation_2<K,CGAL::Default, Itag> CDT1;

//do some typedefs for more convenience when refering to common used types 
typedef CDT1::Point Point;
typedef CDT1::Edge Edge;
typedef K::Line_2 Line;
typedef CDT1::Face Face;
typedef CDT1::Face_iterator Face_iterator;
typedef CGAL::Angle Angle;
typedef CDT1::Face_handle Face_handle;
typedef CDT1::Vertex_handle Vertex_handle;
typedef CGAL::Polygon_2<K> Polygon_2;

//namespaces for json
namespace json = boost::json;
namespace pt = boost::property_tree;


//prototypes of helpre functions 
bool obtuse_face(Face_handle);
//int get_obtuse_ind(Face_handle);
std::pair<std::string, std::string> print_rational(const K::FT& );
Point get_projection(Face_handle);
Point get_middlepoint(Face_handle);
Point get_centroid(Face_handle);
Point get_circumcenter(Face_handle);

//this is the custom class called CCDT
template <class Gt, class Tds = CGAL::Default, class Itag = CGAL::Default>
class CCDT
    : public CGAL::Constrained_Delaunay_triangulation_2<Gt, Tds, Itag> {
public:

    Polygon_2 region_boundary;                                  //polygon that refers to the region boundary of the triangulation(it contains the region boundary points) 

    using Base = CGAL::Constrained_Delaunay_triangulation_2<Gt, Tds, Itag>;
    using typename Base::Face_handle;
    using typename Base::Point;
    using typename Base::Vertex_handle;
    using typename Base::Locate_type;

    // Constructors
    CCDT(const Gt& gt = Gt())
        : Base(gt) {}

    CCDT(typename Base::List_constraints& lc, const Gt& gt = Gt())
        : Base(lc, gt) {}

    template <class InputIterator>
    CCDT(InputIterator it, InputIterator last, const Gt& gt = Gt())
        : Base(it, last, gt) {}

    //copy constructor that calls the base class copy consteuctor and the copies the region boundary
    CCDT(const CCDT& cdt2) : CDT1(cdt2), region_boundary(cdt2.region_boundary) {}

    // New insert method without flips
    Vertex_handle insert_no_flip(const Point& a, Face_handle start = Face_handle()) {
        // Call Ctr::insert without flip_around
        Vertex_handle va = this->Base::Ctr::insert(a, start); // Directly call Ctr::insert from the base
        return va;
    }

    // Another insert method with known location
    Vertex_handle insert_no_flip(const Point& a, Locate_type lt, Face_handle loc, int li) {
        Vertex_handle va = this->Base::Ctr::insert(a, lt, loc, li); // Directly call Ctr::insert from the base
        return va;
    }

    //method that determines if a point is inside or outside the region boundary
    bool point_in_region_boundary(Point p){
        if(this->region_boundary.bounded_side(p) == CGAL::ON_UNBOUNDED_SIDE) return false;
        return true;
    }

    //method that determines if a face is inside or outside the region boundary
    bool face_in_region_boundary(Face_handle f){
        //collect the face points
        Point p0 = f->vertex(0)->point();
        Point p1 = f->vertex(1)->point();
        Point p2 = f->vertex(2)->point();
        //check if all the points are in the region boundary
        return (point_in_region_boundary(p0) && point_in_region_boundary(p1) && point_in_region_boundary(p2));
    }

    //method that computes the number of obtuse angles in CDT
    int obtuse_angles_count(){
        int c = 0;
        for(auto f = this->finite_faces_begin(); f != this->finite_faces_end(); f++){   //iterate all the faces
            //for the faces that are in region boundary and are obtuse increase the count
            if(face_in_region_boundary(f) && obtuse_face(f)) c++;
        }
        return c;
    }
};
#endif

//typedef the new class name to CDT 
typedef CCDT<K, CGAL::Default, Itag> CDT;

//additional helper function that use CDT
Point get_circumcenter_centroid(CDT, Face_handle );
void polygon_center(CDT&);
