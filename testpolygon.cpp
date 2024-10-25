#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/draw_constrained_triangulation_2.h>


#include <string>

#include <boost/json.hpp>
#include <fstream>
#include <stdexcept>
namespace json = boost::json;

#include <cassert>
#include <iostream>
#include<vector>
#include <CGAL/enum.h>
#include "CCDT.h"
#include <string.h>
#include <CGAL/centroid.h>


typedef CGAL::Exact_predicates_tag Itag;
typedef CGAL::Exact_predicates_exact_constructions_kernel K;
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


typedef K::Segment_2 Segment;
typedef K::Vector_2 Vector;


#include <fstream>
//#include <boost/json/src.hpp> // Include the Boost.JSON implementation
//namespace json = boost::json;

typedef CGAL::Exact_predicates_tag Itag;
typedef CGAL::Exact_predicates_exact_constructions_kernel K;
//typedef CGAL::Constrained_Delaunay_triangulation_2<K, CGAL::Default, Itag> CDT;
typedef CDT::Point Point;
typedef CDT::Edge Edge;

typedef CDT::Face Face;
typedef CDT::Face_iterator Face_iterator;
typedef CGAL::Angle Angle;
typedef CDT::Face_handle Face_handle;

//using namespace CGAL;
typedef CGAL::Polygon_2<K> Polygon_2;
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_traits_2.h>
#include <algorithm>
#include <cmath>

Point compute_centroid(const std::vector<Point>& points) {
    double sum_x = 0.0, sum_y = 0.0;
    for (const auto& p : points) {
        sum_x += CGAL::to_double(p.x());
        sum_y += CGAL::to_double(p.y());
    }
    int n = points.size();
    return Point(sum_x / n, sum_y / n);
}

// Comparator function to sort points in counterclockwise order
bool counterclockwise_compare(const Point& a, const Point& b, const Point& centroid) {
    // Calculate angles relative to the centroid
    double angle_a = std::atan2(CGAL::to_double(a.y() - centroid.y()), CGAL::to_double(a.x() - centroid.x()));
    double angle_b = std::atan2(CGAL::to_double(b.y() - centroid.y()), CGAL::to_double(b.x() - centroid.x()));

    // Sort by angle
    return angle_a < angle_b;
}

// Function to sort points in counterclockwise order
void sort_points_counterclockwise(std::vector<Point>& points) {
    // Compute the centroid of the points
    Point centroid = compute_centroid(points);

    // Sort using the custom comparator
    std::sort(points.begin(), points.end(),
              [&centroid](const Point& a, const Point& b) {
                  return counterclockwise_compare(a, b, centroid);
              });
}


bool my_angle(Face face){
    Point a = face.vertex(0)->point();
        Point b = face.vertex(1)->point();
        Point c = face.vertex(2)->point();
        

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


int main(int argc, char* argv[]){

    if(argc != 2){
        std::cerr<<"Usage: "<<argv[0]<<" test_file_name\n";
        exit(1);
    }

    std::ifstream file(argv[1]);
    if (!file){
        std::cerr<<"could not open file: "<<argv[1]<<std::endl;
        exit(1);
    }
    std::string file_content((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());
    file.close();

    // Parse the JSON content
    json::value jv = json::parse(file_content);
    json::object obj = jv.as_object();


    std::string id;
    int n_points;
    std::vector<int> X;
    std::vector<int> Y;
    std::vector<int> region_boundary;
    int n_constraints;
    std::vector<std::pair<int, int>> constraints;


    id = json::value_to<std::string>(obj["instance_uid"]);
    n_points = json::value_to<int>(obj["num_points"]);

    // Extract arrays of points
    for (const auto& x : obj["points_x"].as_array()) {
        X.push_back(json::value_to<int>(x));
    }
    for (const auto& y : obj["points_y"].as_array()) {
        Y.push_back(json::value_to<int>(y));
    }

    // Extract region boundary
    for (const auto& boundary : obj["region_boundary"].as_array()) {
        region_boundary.push_back(json::value_to<int>(boundary));
    }

    // Extract additional constraints
    n_constraints = json::value_to<int>(obj["num_constraints"]);
    for (const auto& constraint : obj["additional_constraints"].as_array()) {
        auto arr = constraint.as_array();
        if (arr.size() == 2){
            int first = json::value_to<int>(arr[0]);
            int second = json::value_to<int>(arr[1]);
            constraints.emplace_back(first, second);
        } else {
            std::cerr<<"The constarins have not the correct form\n";
            exit(1);
        }
    }


    if((X.size() != n_points) || (Y.size() != n_points)){
        std::cerr<<"Number of points_x is different than number of points_y \n";
        exit(1);
    }

    if(constraints.size() != n_constraints){
        std::cerr<<"Number of constraints is different than declared number\n";
        exit(1);
    }

        
       

        // Output parsed data
        std::cout << "Instance UID: " << id << "\n";
        std::cout << "Number of Points: " << n_points << "\n";

        std::cout << "Points X: ";
        for (const auto& x : X) {
            std::cout << x << " ";
        }
        std::cout << "\n";

        std::cout << "Points Y: ";
        for (const auto& y : Y) {
            std::cout << y << " ";
        }
        std::cout << "\n";

        std::cout << "Region Boundary: ";
        for (const auto& boundary : region_boundary) {
            std::cout << boundary << " ";
        }
        std::cout << "\n";

        std::cout << "Number of Constraints: " << n_constraints << "\n";

        std::cout << "Additional Constraints: ";
        for (const auto& constraint : constraints) {
            std::cout << "(" << constraint.first << ", " << constraint.second << ") ";
        }
        std::cout << "\n";


        std::vector<Point> points = {};
        for(int i = 0; i<n_points; i++) points.push_back(Point(X.at(i),Y.at(i)));

        std::cout<<"Points: ";
        for(auto const& p:points) std::cout<<"("<<p.x()<<","<<p.y()<<")"<<" ";
        std::cout<<std::endl;

        std::vector<std::pair<int, int>> RB_edges = {};
        for(int i=0; i<(region_boundary.size()-1); i++) RB_edges.emplace_back(region_boundary.at(i), region_boundary.at(i+1));
        RB_edges.emplace_back(region_boundary.at(0), region_boundary.at(region_boundary.size()-1));

        std::cout<<"Region boundary edges: ";
        for(auto const& e:RB_edges) std::cout << "(" << e.first << ", " << e.second << ") ";
        std::cout<<std::endl;


    //return 0;
    

    CDT cdt;
     /*std:: vector<Point> points = {
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
     }*/


    /*cdt.insert_constraint(Point(0, 0), Point(4, 0));
    cdt.insert_constraint(Point(4, 0), Point(4, 4));
    cdt.insert_constraint(Point(4, 4), Point(0, 4));
    cdt.insert_constraint(Point(0, 4), Point(0, 0));

    // // Insert some internal points to create more triangles
    cdt.insert(Point(1, 1));
    cdt.insert(Point(2, 2));
    cdt.insert(Point(3, 1));*/

    
    // Face_handle fh = cdt.locate(Point(1, 1));
    // bool is_inside = fh->is_in_domain()
    // std::cout << is_inside << std::endl;



    /*points = {
         Point(0,0), Point(4, 0), Point(12,-6), Point(12,-3),
         Point(20,3), Point(16, 3), Point(12, 6), Point(8, 3),
         Point(8, 0), Point(0, 6), Point(12,0), Point(12,3)
         
     };*/
     for(const Point& p :points){
         cdt.insert(p);
     }


    /*constraints = {
         {0,1} ,{1,2}, {2,3},{3,4},{4,5},{5,6},{6,7},
         {7,8},{8,9},{9,0},{3,8},{5,10},{11,10}
     };
     for(const auto& constraint:constraints){
         cdt.insert_constraint(points[constraint.first], points[constraint.second]);
     }*/
    CGAL::draw(cdt);

    
    
   
    Face_iterator it = cdt.faces_begin();
    Face_iterator beyond = cdt.faces_end();
    Face face;

    Face_handle neighbor;
    std:: vector<Point> midpoints= {};
    std:: vector<Point> circs = {};
    std:: vector<Point> centers = {};
    
    
    int c1=obtuse_count(cdt);

    


    std::cout <<"Before" << c1 << std::endl;



    

    int c2;// = obtuse_count(cdt);
    int limit = 1;
    for(int i=0; i<limit ;i++){
        bool found = false;
        std::vector <Point> pol;  
        for(auto fit = cdt.finite_faces_begin(); fit != cdt.finite_faces_end(); ++fit){
                if(my_angle(fit)){

                    Face_handle neigh0 = fit->neighbor(0);

                    pol.push_back(fit->vertex(1)->point());

                    if(my_angle(neigh0) && (!cdt.is_infinite(neigh0))){
                        std::cout<<"HI neig0"<<std::endl;
                        pol.push_back(neigh0->vertex(neigh0->index(fit))->point());
                        found = true;
                    }

                    pol.push_back(fit->vertex(2)->point());

                    Face_handle neigh1 = fit->neighbor(1);
                    if(my_angle(neigh1) && (!cdt.is_infinite(neigh1))){
                        std::cout<<"HI neig1"<<std::endl;
                        pol.push_back(neigh1->vertex(neigh1->index(fit))->point());
                        found = true;

                    }

                    pol.push_back(fit->vertex(0)->point());

                    Face_handle neigh2 = fit->neighbor(2);
                    if(my_angle(neigh2) && (!cdt.is_infinite(neigh2))){
                        std::cout<<"HI neig2"<<std::endl;
                        pol.push_back(neigh2->vertex(neigh2->index(fit))->point());
                        found = true;
                    }
                    
                    
                    if (found){
                        for(auto p : pol) {
                            //if(v->is_valid()) cdt.remove(v);
                            std::cout<<"("<<p.x()<<","<<p.y()<<") ";                        

                        }
                        std::cout<<std::endl;
                        /*std::sort(pol.begin(), pol.end());
                        auto last = std::unique(pol.begin(), pol.end());
                        pol.erase(last, pol.end());

                        sort_counterclockwise(pol);*/

                        //remove duplicates
                        for (auto it = pol.begin(); it != pol.end(); ++it) {
                            auto next_it = it + 1;
                            while (next_it != pol.end()) {
                                if (*it == *next_it) {
                                    next_it = pol.erase(next_it);
                                    } else {
                                        ++next_it;
                                    }
                                }
                        }

                        // sort_points_counterclockwise(pol);

                        for(auto p : pol) {
                            cdt.remove(cdt.insert(p));
                            std::cout<<"("<<p.x()<<","<<p.y()<<") ";                        
                        }

                        
                        Point p = CGAL::centroid(pol.begin(), pol.end(), CGAL::Dimension_tag<0>());
                        cdt.insert_no_flip(p);

                        for (size_t i = 0; i < pol.size(); ++i) {
                            const Point& p1 = pol[i];
                            const Point& p2 = pol[(i + 1) % pol.size()]; // Loop back to the first point
                            cdt.insert_constraint(p1, p2);
                        }
                    }
                    else pol.clear();

                }
                if (found) break;




        }


    }


    c2=obtuse_count(cdt);
    
    std::cout <<"After"<< c2 << std::endl; 

    CGAL::draw(cdt);

    //return 0;



    


    //return 0;








	
    


    
}
