#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/draw_constrained_triangulation_2.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <boost/json.hpp>
#include <fstream>
#include <stdexcept>
namespace json = boost::json;
#include <utility>
#include <string>
namespace pt = boost::property_tree;
#include <cassert>
#include <iostream>
#include<vector>
#include <CGAL/enum.h>
#include "CCDT.h"
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_traits_2.h>

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
typedef K::Segment_2 Segment;
typedef K::Vector_2 Vector;


#include <fstream>
//#include <boost/json/src.hpp> // Include the Boost.JSON implementation
//namespace json = boost::json;

typedef CGAL::Exact_predicates_tag Itag;
typedef CGAL::Exact_predicates_exact_constructions_kernel K;
// typedef CGAL::Constrained_Delaunay_triangulation_2<K, CGAL::Default, Itag> CDT;
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


bool is_obtuse(Face face){
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

#include <gmp.h>

void printface(Point a , Point b ,Point c ){
    std::cout << a << std::endl; 
    std::cout << b << std::endl; 
    std::cout << c << std::endl;
}


std::pair<std::string, std::string> print_rational(const K::FT& coord) {
    const auto exact_coord = CGAL::exact(coord);

    // Convert the exact coordinate to a GMP rational (mpq_t)
    const mpq_t* gmpq_ptr = reinterpret_cast<const mpq_t*>(&exact_coord);

    // Declare GMP integers to hold the numerator and denominator
    mpz_t num, den;
    mpz_init(num);
    mpz_init(den);

    // Extract the numerator and denominator using GMP functions
    mpq_get_num(num, *gmpq_ptr);  // Get the numerator
    mpq_get_den(den, *gmpq_ptr);  // Get the denominator

    // Convert numerator and denominator to strings (because mpz_t is arbitrary precision)
    char* num_str = mpz_get_str(nullptr, 10, num);
    char* den_str = mpz_get_str(nullptr, 10, den);

    // Create a pair of strings to return
    std::pair<std::string, std::string> result(num_str, den_str);

    // Free the dynamically allocated strings
    free(num_str);
    free(den_str);

    // Clear GMP integers
    mpz_clear(num);
    mpz_clear(den);

    return result;
}

bool in_regionboundry(std::vector<Point> points , Point p){
    Polygon_2 polygon;
    for(const Point& p :points){
        polygon.push_back(p);
    }
    if(polygon.bounded_side(p)== CGAL::ON_UNBOUNDED_SIDE){
        std::cout<<"OUT OF THE REGION BOUNDARY"<<std::endl;
        return false;
    }
    return true;
}
int get_obtuse(Face_handle facep){
    Point a = facep->vertex(0)->point();
    Point b =  facep->vertex(1)->point();
    Point c =  facep->vertex(2)->point();

    Vertex_handle va =  facep->vertex(0);
    Vertex_handle vb =  facep->vertex(1);
    Vertex_handle vc =  facep->vertex(2);

    Angle A = angle(b,a,c);
    Angle B = angle(a,b,c);
    Angle C = angle(a,c,b);
    if(A== CGAL::OBTUSE){
        std::cout << "A" << std::endl;
        return 0;
    }
    else if(B== CGAL::OBTUSE){
        std::cout << "B" << std::endl;
        return 1;
    }
    else if(C== CGAL::OBTUSE){
        std::cout << "C" << std::endl;
        return 2;
    }
    return -1;
}

Point get_projection(Face_handle facep){
    int obtuseInd = get_obtuse(facep);
    Point p1 = facep->vertex(obtuseInd)->point();
    Point p2 = facep->vertex(CDT::ccw(obtuseInd))->point();
    Point p3 = facep->vertex(CDT::cw(obtuseInd))->point();
    Line l(p2,p3);
    Point proj = l.projection(p1);
    return proj;
}

Point get_middlepoint(Face_handle facep){
    int obtuseInd = get_obtuse(facep);
    Point p1 = facep->vertex(obtuseInd)->point();
    Point p2 = facep->vertex(CDT::ccw(obtuseInd))->point();
    Point p3 = facep->vertex(CDT::cw(obtuseInd))->point();
    Point m = CGAL::midpoint(p2,p3);
    return m;
}

Point get_centroid(Face_handle facep){
    int obtuseInd = get_obtuse(facep);
    Point p1 = facep->vertex(obtuseInd)->point();
    Point p2 = facep->vertex(CDT::ccw(obtuseInd))->point();
    Point p3 = facep->vertex(CDT::cw(obtuseInd))->point();
    Point ce = CGAL::centroid(p1,p2,p3);
    return ce;
}

Point get_circumcenter(Face_handle facep){
    int obtuseInd = get_obtuse(facep);
    Point p1 = facep->vertex(obtuseInd)->point();
    Point p2 = facep->vertex(CDT::ccw(obtuseInd))->point();
    Point p3 = facep->vertex(CDT::cw(obtuseInd))->point();
    Point circ = CGAL::circumcenter(p1,p2,p3);
    return circ;
}

Point get_circumcenter_centroid(Face_handle facep, std::vector<Point> points){
    Point steiner = get_circumcenter(facep);
    if(!in_regionboundry(points,steiner)){
        steiner = get_centroid(facep);
    }
    return steiner;
}

bool get_polygon_centre(Face_handle facep, CDT cdt){
    int obtuseInd = get_obtuse(facep);
    Point p1 = facep->vertex(obtuseInd)->point();
    Point p2 = facep->vertex(CDT::ccw(obtuseInd))->point();
    Point p3 = facep->vertex(CDT::cw(obtuseInd))->point();
    Face_handle neigh = facep->neighbor(obtuseInd);
    if(!cdt.is_infinite(neigh)){ //check if neighbour is finite
        Point p4 = neigh->vertex(neigh->index(facep))->point(); //get the opposite of p1 the point d
        //points of neighbour
        Point na = neigh->vertex(0)->point();
        Point nb = neigh->vertex(1)->point();
        Point nc = neigh->vertex(2)->point();
        //get the centroid of the polygon abcd
        Point polygon_centre = CGAL::centroid(p1,p2,p3,p4);

        //Check if the polygon_centre is in the edge (the 3 points are collinear)
        if(!CGAL::collinear(p2,polygon_centre,p3)){ //the 3 points are not collinear
            Face_handle f = cdt.locate(polygon_centre); //find the face that polygon_centre is in
            Point af = f->vertex(0)->point();
            Point bf = f->vertex(1)->point();
            Point cf = f->vertex(2)->point();
            std::cout <<"FACE THAT IS THE P"<< std::endl; 
            std::cout << af << std::endl; 
            std::cout << bf << std::endl; 
            std::cout << cf << std::endl;

            //if(a==af && b==bf & c==cf){ //case 1 the polygon center is in the existing triangle
            if(f == facep){
                std::cout<<"IN tringle1"<< std::endl;
                // cdt.insert_no_flip(polygon_centre);
                // Edge edge(f, neigh->index(facep));
                // cdt.flip(edge.first , edge.second);
                
                Edge edge(f, (facep->index(neigh))%3);
                std::cout<<"number ----------------- "<<facep->index(neigh)<<std::endl;
                cdt.insert_no_flip(polygon_centre);
                if(!cdt.is_constrained(edge)) cdt.flip(edge.first , edge.second);
                //polyinfo.push_back(make_tuple(facep,neigh,edge,polygon_centre));
            }
            //else if(na==af && nb==bf && nc==cf){  //case 2 the polygon center is in the new triangle
            else if(f == neigh){
                    std::cout<<"IN tringle2"<< std::endl;
                // cdt.insert_no_flip(polygon_centre);
                // Edge edge(facep, 0);
                // cdt.flip(edge.first , edge.second);

                Edge edge(facep, 0);
                //polyinfo.push_back(make_tuple(facep,neigh,edge,polygon_centre));
                cdt.insert_no_flip(polygon_centre);
                if(!cdt.is_constrained(edge)) cdt.flip(edge.first , edge.second);
            }else{ //case 3 polygon center is outside of the polygon 
                std::cout<<"Outside"<< std::endl;
                return true;
            }
        }
        else{ //the 3 points are collinear
            std::cout<<"Collinear"<< std::endl;
            return true;
        }
        // centers.push_back(polygon_centre);
        //std::cout << m << std::endl;
        // midpoints.push_back(m);
        // circs.push_back(centroid);
    }
    else{
        std::cout<<"Infinite neighbour"<<std::endl;
        return true;
    }
}

int main(int argc, char* argv[]){


    //---------------INPUT---------------------------------------------------------------
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

    std::vector<Point> rbpoints = {};
    for(auto const& i: region_boundary){
        rbpoints.push_back(points.at(i));
    }

    

    std::vector<std::pair<int, int>> RB_edges = {};
    for(int i=0; i<(region_boundary.size()-1); i++) RB_edges.emplace_back(region_boundary.at(i), region_boundary.at(i+1));
    RB_edges.emplace_back(region_boundary.at(0), region_boundary.at(region_boundary.size()-1));

    std::cout<<"Region boundary edges: ";
    for(auto const& e:RB_edges) std::cout << "(" << e.first << ", " << e.second << ") ";
    std::cout<<std::endl;

    //--------------------Create cdt---------------------------------------
    CDT cdt;
    std::vector<Point> startpoints ={};
    for(const Point& p :points){
        cdt.insert(p);
        startpoints.push_back(p);
    }
    for(const auto& constraint:constraints){
        cdt.insert_constraint(points[constraint.first], points[constraint.second]);
    }
    CGAL::draw(cdt);

    Face_iterator it = cdt.faces_begin();
    Face_iterator beyond = cdt.faces_end();
    Face face;

    Face_handle neighbor;
    std:: vector<Point> midpoints= {};
    std:: vector<Point> circs = {};
    std:: vector<Point> centers = {};
    
    std::vector<std::string> steinerx = {};
    std::vector<std::string> steinery = {};


    
    int c1=obtuse_count(cdt);
    std::cout <<"Before" << c1 << std::endl;

    //Edge flips
    //terate through finite edges and flip if possible
    
    //-----------TESTING REG BOUNDRY--------------------------
    // std::cout<< in_regionboundry(rbpoints , Point(100,70))<<std::endl;
    // std::cout<< in_regionboundry(rbpoints , Point(0,0))<<std::endl;
    // std::cout<< in_regionboundry(rbpoints , Point(25,25))<<std::endl;


    //---------------------------------Flipping--------------------------------
    for(int k=0; k<0; k++){
        for (auto e : cdt.finite_edges()) {
            std::cout<<"-------------------------\n";
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

            int ind = f2->index(f1);
            Point a2 = f2->vertex(ind)->point();
            Point b2= f2->vertex(f2->cw(ind))->point();
            Point c2 = f2->vertex(f2->ccw(ind))->point();
            
            Angle an = angle(b2,a2,c2);
            

            polygon.push_back(a);
            polygon.push_back(b);
            polygon.push_back(c);
            polygon.push_back(a2);
            polygon.push_back(b2);
            polygon.push_back(c2);

            std::cout<<a<<std::endl;
            std::cout<<b<<std::endl;
            std::cout<<c<<std::endl;
            std::cout<<"--------\n";
            std::cout<<a2<<std::endl;
            std::cout<<b2<<std::endl;
            std::cout<<c2<<std::endl;

            if(!polygon.is_convex()){
                std::cout<<"The polygon is not convex"<<std::endl;
            }
            // Check if the polygon is convex
            if (!cdt.is_constrained(edge)) {
                // Ensure that the edge is internal
                if (!cdt.is_infinite(f1) && !cdt.is_infinite(f2)) {
                    // Perform the flip
                    if(my_angle(f1)== true  && my_angle(f2)==true ){
                        cdt.flip(f1, index);
                        break;
                    }
                } else {
                    std::cerr << "Edge is on the boundary, not flippable" << std::endl;
                }
            } else {
                std::cerr << "Edge is constrained, not flippable" << std::endl;
            }
        }
    }

    CDT cdt2 = CDT(cdt);
    CDT cdt3 = CDT(cdt);
    CDT cdt4 = CDT(cdt);

    //----------------------------Middle Point-------------------
    int limit = 20;
    for(int i=0; i<limit ;i++){
        it = cdt.finite_faces_begin();
        beyond = cdt.finite_faces_end();
        std::vector<std::tuple<Face_handle, Face_handle,Edge,Point>> polyinfo = {};
        while (it != beyond) {
            bool obtuse = false;
            face = *it;  // get face
            Face_handle facep = it;
            Point m;
            if(is_obtuse(*facep)){
                m = get_middlepoint(facep);
                cdt.insert_no_flip(m);
                points.push_back(m);
                break;
            }
            ++it;        // advance the iterator
        }
    }
    int c2=obtuse_count(cdt);
    std::cout <<"Before" << c1 << std::endl;
    std::cout <<"After middle steiner "<< c2 << std::endl; 
    CGAL::draw(cdt);
    
    //-----------------------------Circum-center-Centroid------------------------------------------
    c1=obtuse_count(cdt2);
    limit = 20;
    for(int i=0; i<limit ;i++){
        it = cdt2.finite_faces_begin();
        beyond = cdt2.finite_faces_end();
        std::vector<std::tuple<Face_handle, Face_handle,Edge,Point>> polyinfo = {};
        while (it != beyond) {
            bool obtuse = false;
            face = *it;  // get face
            Face_handle facep = it;
            Point ce;
            if(is_obtuse(*facep)){
                ce = get_circumcenter_centroid(facep , startpoints);
                cdt2.insert_no_flip(ce);
                points.push_back(ce);
                break;
            }
            ++it;        // advance the iterator
        }
    }
    c2=obtuse_count(cdt2);
    std::cout <<"Before" << c1 << std::endl;
    std::cout <<"After centroid/circumsenter steiner "<< c2 << std::endl; 
    CGAL::draw(cdt2);

    //-----------------------------Polygon center--------------------------------------------------
    c1=obtuse_count(cdt3);
    limit = 20;
    for(int i=0; i<limit ;i++){
        it = cdt3.finite_faces_begin();
        beyond = cdt3.finite_faces_end();
        std::vector<std::tuple<Face_handle, Face_handle,Edge,Point>> polyinfo = {};
        while (it != beyond) {
            bool obtuse = false;
            face = *it;  // get face
            Face_handle facep = it;
            if(is_obtuse(*facep)){
                bool cont = get_polygon_centre(facep,cdt);
                if(cont){
                    it++;
                    continue;
                }
                else{
                    break;
                }
            }
            ++it;        // advance the iterator
        }
    }
    c2=obtuse_count(cdt3);
    std::cout <<"Before" << c1 << std::endl;
    std::cout <<"After centroid/circumsenter steiner "<< c2 << std::endl; 
    CGAL::draw(cdt3);
    
    //-----------------------------Projection------------------------------------------------------
    c1=obtuse_count(cdt4);
    std::cout <<"CDT4"<<std::endl; 
    CGAL::draw(cdt4);
    limit = 20;
    for(int i=0; i<limit ;i++){
        it = cdt4.finite_faces_begin();
        beyond = cdt4.finite_faces_end();
        std::vector<std::tuple<Face_handle, Face_handle,Edge,Point>> polyinfo = {};
        while (it != beyond) {
            bool obtuse = false;
            face = *it;  // get face
            Face_handle facep = it;
            Point proj;
            if(is_obtuse(*facep)){
                proj = get_projection(facep);
                cdt4.insert_no_flip(proj);
                points.push_back(proj);
                std::pair<std::string, std::string> xpair = print_rational(proj.x());
                std::pair<std::string, std::string> ypair = print_rational(proj.y());
                steinerx.push_back(xpair.first + "/" + xpair.second);
                steinery.push_back(ypair.first + "/" + ypair.second);
                break;
            }
            ++it;        // advance the iterator
        }
    }
    c2=obtuse_count(cdt4);
    std::cout <<"Before" << c1 << std::endl;
    std::cout <<"After projection"<< c2 << std::endl; 
    CGAL::draw(cdt4);




    //EXPORTING--------------------------------------------------------
    boost::property_tree::ptree root; //empty root of the tree

    // Add "content_type" and "instance_uid"
    root.put("content_type", "CG_SHOP_2025_Solution");
    root.put("instance_uid", id); 
    


    boost::property_tree::ptree steinerx_nodes; //create a steiner points node
    for (auto &x : steinerx){

        pt::ptree steinerx_node;
        steinerx_node.put("", x); //create a node of a steiner point

        // Add this node to the list.
        steinerx_nodes.push_back(std::make_pair("", steinerx_node)); //add this node to the steiner nodes
    }
    root.add_child("steiner_points_x", steinerx_nodes); //add to root


    boost::property_tree::ptree steinery_nodes; //create a steiner points node
    for (auto &y : steinery){

        pt::ptree steinery_node;
        std::cout<<y<<std::endl;
        steinery_node.put("", y); //create a node of a steiner point

        // Add this node to the list.
        steinery_nodes.push_back(std::make_pair("", steinery_node)); //add this node to the steiner nodes
    }
    root.add_child("steiner_points_y", steinery_nodes); //add to root

    
    std::vector<std::pair<int, int>> edges = {};
    for (const Edge& e : cdt.finite_edges()) {
        CDT::Face_handle face = e.first;
        int index = e.second; 
        CDT::Vertex_handle vh1 = face->vertex((index + 1) % 3);
        CDT::Vertex_handle vh2 = face->vertex((index + 2) % 3);
        Point p1 = vh1->point();
        Point p2 = vh2->point();
        int i1 = -1;
        int i2 = -1;
        for(int i=0 ; i<points.size() ; i++){
            // std::cout<<"Point in Vector " << points.at(i) << std::endl;
            // std::cout<< "P1 " << p1 << std::endl;
            // std::cout<< "P2 " << p2 << std::endl;

            if(points.at(i) == p1){
                i1 = i;
            }
            if(points.at(i) == p2){
                i2 = i;
            }
        }
        if( i1 == -1 || i2 == -1){
            // std::cout<<i1 << i2 <<std::endl;
            // std::cout<<i1 << i2 <<std::endl;
            std::cout<<"NOT FOUND"<<std::endl;
            break;
        }
        edges.push_back(std::make_pair(i1,i2));
    }


    // Add "edges" array of arrays
    boost::property_tree::ptree edges_node; //outer node
    for (const struct std::pair<int, int> e : edges) {
        boost::property_tree::ptree edge_node; //inner node
        
        boost::property_tree::ptree edge_val1_node; //first node of the edge
        edge_val1_node.put("", e.first);
        edge_node.push_back(std::make_pair("", edge_val1_node));

        boost::property_tree::ptree edge_val2_node; //second node of the edge
        edge_val2_node.put("", e.second);
        edge_node.push_back(std::make_pair("", edge_val2_node));
        
        edges_node.push_back(std::make_pair("", edge_node));
    }
    root.add_child("edges", edges_node);
    
    std::ostringstream oss;
    boost::property_tree::write_json(oss, root, true);  // 'false' to avoid pretty-printing

    // Get the JSON string from the string stream
    std::string json_str = oss.str();

    // OPTIONAL: Replace escaped forward slashes "\/" with "/"
    size_t pos = 0;
    while ((pos = json_str.find("\\/", pos)) != std::string::npos) {
        json_str.replace(pos, 2, "/");
        pos += 1;  // Move past the replaced character
    }

    // Save JSON to a file
    std::ofstream fileout("out/output.json");
    if (fileout.is_open()) {
        // boost::property_tree::write_json(fileout, root);
        fileout << json_str;
        fileout.close();
        std::cout << "JSON saved to output.json" << std::endl;
    } else {
        std::cerr << "Unable to open file for writing!" << std::endl;
    }

}