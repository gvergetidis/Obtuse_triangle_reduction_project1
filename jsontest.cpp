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
int main(){
    // Create a property tree to hold the parsed JSON
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

    std::string filename = "tests/instance_6.json";

    pt::read_json(filename, root);

    std::string instance_uid = root.get<std::string>("instance_uid");
    int num_points = root.get<int>("num_points"); 
    std::vector<int>xarr= {};
    std::vector<int>yarr= {};
    for (const auto& point : root.get_child("points_x")) {
        xarr.push_back(point.second.get_value<int>());
    }
    for (const auto& point : root.get_child("points_y")) {
        yarr.push_back(point.second.get_value<int>());
    }


    std:: vector<Point> points = {};
    for(int i=0 ;i<xarr.size() ; i++){
        int x = xarr.at(i);
        int y = yarr.at(i);
        points.push_back(Point(x, y));
    }

    //get region boundry
    std::vector<int>reg= {};
    for (const auto& point : root.get_child("region_boundary")) {
        reg.push_back(point.second.get_value<int>());
    }


    int num_constraints = root.get<int>("num_constraints");
    std::vector<std::pair<int,int>> constraints = {};
    if(num_constraints >0){
        for (const auto& constr : root.get_child("additional_constraints")) {
            int x = constr.second.get_child("0").get_value<int>();
            std::cout<< "x"<< x <<std::endl;
            int y = constr.second.get_child("1").get_value<int>();
            std::cout<< "y"<< y <<std::endl;
            constraints.push_back(std::make_pair(x, y));
        }
    }


    // int x = 0;
    // for (pt::ptree::value_type &row : root.get_child("additional_constraints")){
    //     int y = 0;
    //     for (pt::ptree::value_type &cell : row.second){
    //         matrix[x][y] = cell.second.get_value<int>();
    //         y++;
    //     }
    //     x++;
    // }
    
    
    
    CDT cdt;
    for(const Point& p :points){
        cdt.insert(p);
    }
    for(const auto& constraint:constraints){
        cdt.insert_constraint(points[constraint.first], points[constraint.second]);
    }
    CGAL::draw(cdt);



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