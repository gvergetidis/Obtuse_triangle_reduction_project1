#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/draw_constrained_triangulation_2.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <string>

namespace pt = boost::property_tree;

#include <cassert>
#include <iostream>
#include<vector>
#include <CGAL/enum.h>
#include<CCDT.h>
#include<CGAL/Line_2.h>

typedef CGAL::Exact_predicates_tag Itag;
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CCDT<K, CGAL::Default, Itag> CDT;
typedef CDT::Point Point;
typedef CDT::Edge Edge;
typedef K::Line_2 Line;
typedef CDT::Point Point;
typedef CDT::Face Face;
typedef CDT::Face_iterator Face_iterator;
typedef CGAL::Angle Angle;
typedef CDT::Face_handle Face_handle;
typedef CDT::Vertex_handle Vertex_handle;
typedef K::Point_2 Point_2;
typedef CGAL::Polygon_2<K> Polygon_2;
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_traits_2.h>


bool is_obtuse_triangle(const Point& A, const Point& B, const Point& C){
    // Check the angles at each vertex
    if (CGAL::angle(B, A, C) == CGAL::OBTUSE || CGAL::angle(A, B, C) == CGAL::OBTUSE || CGAL::angle(A, C, B) == CGAL::OBTUSE) {
        return true; // The triangle is obtuse
    }
    return false; // The triangle is not obtuse
}

bool my_angle(Face_handle face){
    Point a = face->vertex(0)->point();
    Point b = face->vertex(1)->point();
    Point c = face->vertex(2)->point();

    Angle A = angle(b,a,c);
    Angle B = angle(a,b,c);
    Angle C = angle(a,c,b);

    // std::cout << A << std::endl;
    // std::cout << B << std::endl;
    // std::cout << C << std::endl;

    if(A== CGAL::OBTUSE){
        return true;
    }
    if(B== CGAL::OBTUSE){
        return true;
    }
    if(C== CGAL::OBTUSE){
        return true;
    }

    return false;
}
int obtuse_count(CDT cdt){
    int c2=0;
    Face_iterator it = cdt.finite_faces_begin();
    Face_iterator beyond = cdt.finite_faces_end();
    while (it != beyond) {
        Face face = *it;  // get face
        Point a = face.vertex(0)->point();
        Point b = face.vertex(1)->point();
        Point c = face.vertex(2)->point();
        if(is_obtuse_triangle(a,b,c)){
            c2++;
        }
        ++it;        // advance the iterator
    }
    return c2;
}
Point project_point(const Point& A, const Point& B, const Point& P) {
    // Vector AB
    double ABx = B.x() - A.x();
    double ABy = B.y() - A.y();
    // Vector AP
    double APx = P.x() - A.x();
    double APy = P.y() - A.y();
    // Dot product of AB and AP
    double dot_product = ABx * APx + ABy * APy;
    // Length squared of AB
    double length_squared = ABx * ABx + ABy * ABy;
    // Calculate the parameter t
    double t = dot_product / length_squared;
    // Projection point Q
    double Qx = A.x() + t * ABx;
    double Qy = A.y() + t * ABy;
    return Point(Qx, Qy);
}

int main(){
    CDT cdt;
    
    std:: vector<Point> points = {
        Point(632,1588), Point(1330, 1097), Point(3051,470), Point(5040,1077),
        Point(5883,2766), Point(8130, 3629), Point(9280, 2836), Point(9613, 4963),
        Point(9422, 6363), Point(8996, 7327), Point(8020, 7611), Point(8467, 9720),
        Point(6735,9183), Point(4674,7865), Point(2519, 7692), Point(973,9797),
        Point(1205,6005), Point(1929, 5812), Point(3203, 6301),Point(5345,2923)
    };
    for(const Point& p :points){
        cdt.insert(p);
    }
    std::vector<std::pair<int,int>> constraints = {
        {3,4} ,{5,6}, {9,10},{10,11},{11,12},{12,13},{13,14},
        {14,15},{15,16},{18,19},{19,0}
    };
    for(const auto& constraint:constraints){
        cdt.insert_constraint(points[constraint.first], points[constraint.second]);
    }


    // cdt.insert_constraint(Point(0, 0), Point(4, 0));
    // cdt.insert_constraint(Point(4, 0), Point(4, 4));
    // cdt.insert_constraint(Point(4, 4), Point(0, 4));
    // cdt.insert_constraint(Point(0, 4), Point(0, 0));
    // // // Insert some internal points to create more triangles
    // cdt.insert(Point(1, 1));
    // cdt.insert(Point(2, 2));
    // cdt.insert(Point(3, 1));
    // Face_handle fh = cdt.locate(Point(1, 1));
    // bool is_inside = fh->is_in_domain()
    // std::cout << is_inside << std::endl;
    CGAL::draw(cdt);

    //Edge flips
    //terate through finite edges and flip if possible
    // for (auto e : cdt.finite_edges()) {
    //     // Check if the edge can be flipped
    //     if (cdt.is_flipable(e.first,e.second)) {
    //         std::cout << "Flipped edge" << std::endl;
    //         cdt.flip(e.first,e.second);  // Perform the flip operation
    //     }
    // }
    std::cout<< "---------------------------------------------------BEFORE FLIPS COUNT" << obtuse_count(cdt) << std::endl;

    for (auto e : cdt.finite_edges()) {
        Edge edge = e;
        // Get the two faces sharing this edge
        Face_handle f1 = edge.first;
        int index = edge.second;
        Face_handle f2 = f1->neighbor(index);
        // Check if the edge is internal (not on the boundary) and not constrained
        Polygon_2 polygon;
    
        // Add points to the polygon
        Point a = f1->vertex(0)->point();
        Point b = f1->vertex(1)->point();
        Point c = f1->vertex(2)->point();
        Point a2 = f2->vertex(0)->point();
        Point b2= f2->vertex(1)->point();
        Point c2 = f2->vertex(2)->point();

        polygon.push_back(a);
        polygon.push_back(b);
        polygon.push_back(c);
        polygon.push_back(a2);
        polygon.push_back(b2);
        polygon.push_back(c2);
        if(!polygon.is_convex()){
            std::cout<<"The polygon is not convex"<<std::endl;
        }
        // Check if the polygon is convex
        if (!cdt.is_constrained(edge) && polygon.is_convex()) {
            // Ensure that the edge is internal
            if (!cdt.is_infinite(f1) && !cdt.is_infinite(f2)) {
                // Perform the flip
                if(my_angle(f1)== true  && my_angle(f2)==true){
                    cdt.flip(f1, index);
                }
            } else {
                std::cerr << "Edge is on the boundary, not flippable" << std::endl;
            }
        } else {
            std::cerr << "Edge is constrained, not flippable" << std::endl;
        }
    }
    
    std::cout << "Edge flipping completed." << std::endl;
    std::cout<< "-------------------------AFTER FLIPS COUNT" << obtuse_count(cdt) << std::endl;
    CGAL::draw(cdt);

    Face_iterator it = cdt.finite_faces_begin();
    Face_iterator beyond = cdt.finite_faces_end();
    Face face;
    Face_handle neighbor;

    //Loop with iterations
    std:: vector<Point> cent={};
    int limit = 1;
    for(int i=0; i<limit ;i++){
        // cent = {};
        std::cout<< "Size of sent " <<cent.size()<<std::endl;
        it = cdt.finite_faces_begin();
        beyond = cdt.finite_faces_end();
        std::vector<std::tuple<Face_handle, Face_handle,Edge,Point>> polyinfo = {};
        while (it != beyond) {
            bool obtuse = false;
            std::cout<<"hiiiiiiiiiiiiiiiii\n";
            face = *it;  // get face

            Face_handle facep = it;
            Point a = face.vertex(0)->point();
            Point b = face.vertex(1)->point();
            Point c = face.vertex(2)->point();

            std::cout <<"FACE"<< std::endl; 
            std::cout << a << std::endl; 
            std::cout << b << std::endl; 
            std::cout << c << std::endl; 

            Angle A = angle(b,a,c);
            Angle B = angle(a,b,c);
            Angle C = angle(a,c,b);
            int ind ;
            Point p;
            if(A== CGAL::OBTUSE){
                ind = 0;
                p = project_point(b,c,a);
            }else if(B== CGAL::OBTUSE){
                ind = 1;
                p = project_point(a,c,b);
            }else if(C== CGAL::OBTUSE){
                ind = 2;
                p = project_point(a,b,c);
            }
            if(A== CGAL::OBTUSE || B== CGAL::OBTUSE || C== CGAL::OBTUSE){
                Face_handle neigh = face.neighbor(ind);
                if(cdt.is_infinite(neigh)){
                    Point proj = p;
                    cent.push_back(p);
                }
                else{
                    Point ce = CGAL::centroid(a,b,c);
                    // if(!is_obtuse_triangle(a,ce,c) && !is_obtuse_triangle(c,ce,b) && !is_obtuse_triangle(b,ce,a)){
                    cent.push_back(ce);
                    // }
                    // cent.push_back(ce);
                }


            }
            ++it;        // advance the iterator
        }
        for(const Point& ce :cent){
            cdt.insert_no_flip(ce);
        }
        cent.clear();
    }


    // cdt.insert_no_flip(cent.at(0));
    std::cout<< "---------------------------COUNT: " << obtuse_count(cdt) << std::endl;
    
    CGAL::draw(cdt);


}