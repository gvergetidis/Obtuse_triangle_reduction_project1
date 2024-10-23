#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
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
//typedef CGAL::Constrained_Delaunay_triangulation_2<K, CGAL::Default, Itag> CDT; old
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


/*Point project_point(const Point& P, const Point& A, const Point& B) {
    // Create segment AB
    Segment AB(A, B);

    // Vector from A to B
    Vector AB_vec = B - A;

    // Vector from A to P
    Vector AP_vec = P - A;

    // Projection scalar
    double t = (AP_vec * AB_vec) / AB_vec.squared_length();

    // Compute the projected point
    if (t < 0.0) {
        // Projection falls outside the segment, closest to A
        return A;
    } else if (t > 1.0) {
        // Projection falls outside the segment, closest to B
        return B;
    } else {
        // Projection is within the segment
        return A + t * AB_vec;
    }
}*/

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


int main() {
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



    std:: vector<Point> points = {
         Point(0,0), Point(4, 0), Point(12,-6), Point(12,-3),
         Point(20,3), Point(16, 3), Point(12, 6), Point(8, 3),
         Point(8, 0), Point(0, 6), Point(12,0), Point(12,3)
         
     };
     for(const Point& p :points){
         cdt.insert(p);
     }


     std::vector<std::pair<int,int>> constraints = {
         {0,1} ,{1,2}, {2,3},{3,4},{4,5},{5,6},{6,7},
         {7,8},{8,9},{9,0},{3,8},{5,10},{11,10}
     };
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
    
    
    int c1=obtuse_count(cdt);
    std::cout <<"Before" << c1 << std::endl;

    //return 0 ;
    //Edge flips
    //terate through finite edges and flip if possible
    

    //Steiner points in the middle of the edge

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
    

    int c2;// = obtuse_count(cdt);
    //std::cout <<"after" << c2 << std::endl;
    //CGAL::draw(cdt);
    //return 0;
    
    /*
    it = cdt.faces_begin();
    beyond = cdt.faces_end();
    while (it != beyond) {
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

        // std::cout << A << std::endl;
        // std::cout << B << std::endl;
        // std::cout << C << std::endl;





        if(A== CGAL::OBTUSE){
            // Point p = CGAL::circumcenter(a,b,c);
            // Face_handle f = cdt.locate(p);
            // Point af = f->vertex(0)->point();
            // Point bf = f->vertex(1)->point();
            // Point cf = f->vertex(2)->point();
            // std::cout <<"FACE THAT IS THE P"<< std::endl; 
            // std::cout << af << std::endl; 
            // std::cout << bf << std::endl; 
            // std::cout << cf << std::endl; 
            // if(a!=af || b!=bf || c!=cf){
            //     Point centroid = CGAL::centroid(a,b,c);
            //     centroids.push_back()
            // }
            Point centroid = CGAL::centroid(a,b,c);

            std::cout << "A" << std::endl;
            Point m= CGAL::midpoint(b,c);
            Face_handle neigh = face.neighbor(0);
            Point d = neigh->vertex(neigh->index(facep))->point();
            Point polygon_centre = CGAL::centroid(a,b,c,d);
            centers.push_back(polygon_centre);

            std::cout << m << std::endl;
            midpoints.push_back(m);
            circs.push_back(centroid);
            c1++;

        }
        if(B== CGAL::OBTUSE){

            Point centroid = CGAL::centroid(a,b,c);
            // Face_handle f = cdt.locate(p);
            // Point af = f->vertex(0)->point();
            // Point bf = f->vertex(1)->point();
            // Point cf = f->vertex(2)->point();
            // std::cout <<"FACE THAT IS THE P"<< std::endl; 
            // std::cout << af << std::endl; 
            // std::cout << bf << std::endl; 
            // std::cout << cf << std::endl; 


            std::cout << "B" << std::endl;
            Point m= CGAL::midpoint(a,c);
            Face_handle neigh = face.neighbor(1);
            Point d = neigh->vertex(neigh->index(facep))->point();
            Point polygon_centre = CGAL::centroid(a,b,c,d);
            centers.push_back(polygon_centre);


            std::cout << m << std::endl;

            midpoints.push_back(m);
            circs.push_back(centroid);
            c1++;
        }
        if(C== CGAL::OBTUSE){

            Point centroid = CGAL::centroid(a,b,c);
            // Face_handle f = cdt.locate(p);
            // Point af = f->vertex(0)->point();
            // Point bf = f->vertex(1)->point();
            // Point cf = f->vertex(2)->point();
            // std::cout <<"FACE THAT IS THE P"<< std::endl; 
            // std::cout << af << std::endl; 
            // std::cout << bf << std::endl; 
            // std::cout << cf << std::endl; 

            std::cout << "C" << std::endl;
            Point m= CGAL::midpoint(b,a);
            Face_handle neigh = face.neighbor(2);
            Point d = neigh->vertex(neigh->index(facep))->point();
            Point polygon_centre = CGAL::centroid(a,b,c,d);
            centers.push_back(polygon_centre);

            std::cout << m << std::endl;
            midpoints.push_back(m);
            circs.push_back(centroid);
            c1++;
        }
        ++it;        // advance the iterator
    }

    // for(const Point& m :midpoints){
    //     cdt.insert(m);
    // }
    // cdt.insert_no_flip(midpoints.at(0));

    // for(const Point& c :circs){
    //     cdt.insert_no_flip(c);
    // }
    ///cdt.insert_no_flip(circs.at(0));
    
    cdt.insert_no_flip(centers.at(0));*/



    int limit = 20;
    for(int i=0; i<limit ;i++){
        //projs = {};
        std::cout<<"----------------------------------\n";
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

            Vertex_handle va = face.vertex(0);
            Vertex_handle vb = face.vertex(1);
            Vertex_handle vc = face.vertex(2);

            
            std::cout <<"FACE"<< std::endl; 
            std::cout << a << std::endl; 
            std::cout << b << std::endl; 
            std::cout << c << std::endl; 

            Angle A = angle(b,a,c);
            Angle B = angle(a,b,c);
            Angle C = angle(a,c,b);
            if(A== CGAL::OBTUSE){
                obtuse = true;
  

                std::cout << "A" << std::endl;
                Face_handle neigh = face.neighbor(0);

                Line l(b,c);
                Point proj = l.projection(a);

                //CGAL::_projection()

                if(CGAL::collinear(proj,b,c)) std::cout<<" colinear\n";
                else std::cout<<" no colinear\n"; 
                cdt.insert_no_flip(proj);
                //std::cout<<"inserted point "<<proj<<std::endl;

             

                //if (cdt.is_infinite(neigh))  projs.push_back(proj);


                
            }
            else if(B== CGAL::OBTUSE){
                obtuse = true;

                
                std::cout << "B" << std::endl;
                Face_handle neigh = face.neighbor(1);

                Line l(a,c);
                Point proj = l.projection(b);
                cdt.insert_no_flip(proj);
                //std::cout<<"inserted point "<<proj<<std::endl;
                if(CGAL::collinear(proj,a,c)) std::cout<<"colinear\n";
                else std::cout<<" no colinear\n";
                //if (cdt.is_infinite(neigh))  projs.push_back(proj); 

                
            }
            else if(C== CGAL::OBTUSE){
                obtuse = true;

                std::cout << "C" << std::endl;
                Face_handle neigh = face.neighbor(2);

                Line l(a,b);
                Point proj = l.projection(c);
                cdt.insert_no_flip(proj);
                //std::cout<<"inserted point "<<proj<<std::endl;
                if(CGAL::collinear(proj,b,a)) std::cout<<"colinear\n";
                else std::cout<<" no colinear\n";
                //if (cdt.is_infinite(neigh))  projs.push_back(proj);
                
            }
            if(obtuse){
                break;
            }
           
            ++it;        // advance the iterator
        }
        //for(auto p : projs) cdt.insert_no_flip(p);
    }


    c2=obtuse_count(cdt);
    
    std::cout <<"After"<< c2 << std::endl; 

    CGAL::draw(cdt);

    return 0;



    it = cdt.faces_begin();
    beyond = cdt.faces_end();
    while (it != beyond) {
        face = *it;  // get face
        Point a = face.vertex(0)->point();
        Point b = face.vertex(1)->point();
        Point c = face.vertex(2)->point();
        
        // std::cout <<"FACE"<< std::endl; 
        // std::cout << a << std::endl; 
        // std::cout << b << std::endl; 
        // std::cout << c << std::endl; 

        Angle A = angle(b,a,c);
        Angle B = angle(a,b,c);
        Angle C = angle(a,c,b);

        // std::cout << A << std::endl;
        // std::cout << B << std::endl;
        // std::cout << C << std::endl;

        if(A== CGAL::OBTUSE){
            std::cout << "A" << std::endl;
            c2++;
        }
        if(B== CGAL::OBTUSE){
            std::cout << "B" << std::endl;
            c2++;
        }
        if(C== CGAL::OBTUSE){
            std::cout << "C" << std::endl;
            c2++;
        }
        ++it;        // advance the iterator
    }
    std::cout <<"Before" << c1 << std::endl; 
    std::cout <<"After"<< c2 << std::endl; 
    CGAL::draw(cdt);


    //return 0;
    // // Create a property tree to hold the parsed JSON
     pt::ptree root;

    // // Example JSON string
     /*std::string json_data = R"(
     {
        "name": "John",
         "age": 30,
         "is_student": false,
         "courses": {
             "math": "A",
             "science": "B"
         },
         "hobbies": ["reading", "hiking", "coding"]
     })";*/

    // // Read the JSON string into the property tree
     //std::istringstream json_stream(json_data);
     pt::read_json("data.json", root);

    // // Accessing simple data
     std::string name = root.get<std::string>("name");
     int age = root.get<int>("age");
     bool is_student = root.get<bool>("is_student");

    // // Accessing nested objects
     std::string math_grade = root.get<std::string>("courses.math");
     std::string science_grade = root.get<std::string>("courses.science");

    // // Accessing arrays
     for (const auto& hobby : root.get_child("hobbies")) {
         std::cout << "Hobby: " << hobby.second.get_value<std::string>() << std::endl;
     }

    // // Printing parsed data
     std::cout << "Name: " << name << "\nAge: " << age << "\nIs Student: " << std::boolalpha << is_student << std::endl;
     std::cout << "Math Grade: " << math_grade << "\nScience Grade: " << science_grade << std::endl;
    


    /*std::ifstream file("data.json");
    if (!file.is_open()) {
        std::cerr << "Failed to open file 'data.json'" << std::endl;
        return 1;
    }

    // Parse the JSON data
    std::stringstream buffer;
    buffer << file.rdbuf();
    json::value parsed_data = json::parse(buffer.str());

    // Access the data
    json::object obj = parsed_data.as_object();

    std::string name = obj["name"].as_string().c_str();
    int age = obj["age"].as_int64();
    bool is_student = obj["is_student"].as_bool();
    
    // Accessing nested arrays (courses)
    json::array courses = obj["courses"].as_array();
    std::cout << "Name: " << name << std::endl;
    std::cout << "Age: " << age << std::endl;
    std::cout << "Is Student: " << (is_student ? "Yes" : "No") << std::endl;
    std::cout << "Courses: " << std::endl;

    for (const auto& course : courses) {
        json::object course_obj = course.as_object();
        std::string title = course_obj["title"].as_string().c_str();
        std::string grade = course_obj["grade"].as_string().c_str();
        std::cout << "  - " << title << ": " << grade << std::endl;
    }

    // Accessing a nested object (address)
    json::object address = obj["address"].as_object();
    std::string street = address["street"].as_string().c_str();
    std::string city = address["city"].as_string().c_str();
    std::string zip_code = address["zip_code"].as_string().c_str();

    std::cout << "Address: " << street << ", " << city << ", " << zip_code << std::endl;

        */
    return 0;
}
