//#include "header.h"
#include "CCDT.h"
#include <gmp.h>
int obtuse_count(CDT cdt){
    int c2=0;
    Face_iterator it = cdt.finite_faces_begin();
    Face_iterator beyond = cdt.finite_faces_end();
    while (it != beyond) {
        
        if(obtuse_face(it)){
            c2++;
        }
        ++it;        // advance the iterator
    }
    return c2;
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
        rbpoints.push_back(points[i]);
    }

    

    std::vector<std::pair<int, int>> RB_edges = {};
    for(int i=0; i<(region_boundary.size()-1); i++) RB_edges.emplace_back(region_boundary[i], region_boundary[i+1]);
    RB_edges.emplace_back(region_boundary[0], region_boundary[region_boundary.size()-1]);

    std::cout<<"Region boundary edges: ";
    for(auto const& e:RB_edges) std::cout << "(" << e.first << ", " << e.second << ") ";
    std::cout<<std::endl;

    //--------------------Create cdt---------------------------------------
    CDT cdt;
    for(const Point& p :points){
        cdt.insert(p);
    }
    // for(const auto& constraint:constraints){
    //     cdt.insert_constraint(points[constraint.first], points[constraint.second]);
    // }


    // for(const auto& e : RB_edges) cdt.insert_constraint(points[e.first],points[e.second]);
    for(const auto& p : rbpoints) cdt.region_boundary.push_back(p);
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

    CDT cdt2 = CDT(cdt);
    CDT cdt3 = CDT(cdt);
    CDT cdt4 = CDT(cdt);


    int limit , c1 , c2;
    c1=obtuse_count(cdt);
    std::cout <<"Before" << c1 << std::endl;


    //---------------------------------Flipping--------------------------------
    // for (auto e : cdt.finite_edges()) {
    //     std::cout<<"-------------------------\n";
    //     Edge edge = e;
    //     // Get the two faces sharing this edge
    //     Face_handle f1 = edge.first;
    //     int index = edge.second;
    //     Face_handle f2 = f1->neighbor(index);

    //     // Check if the edge is internal (not on the boundary) and not constrained
    //     Polygon_2 polygon;
    //     Point p0 = f1->vertex(index)->point();
    //     Point p1 = f1->vertex(CDT::ccw(index))->point();
    //     Point p2 = f2->vertex(f2->index(f1))->point(); 
    //     Point p3 = f1->vertex(CDT::cw(index))->point();

    //     polygon.push_back(p0);
    //     polygon.push_back(p1);
    //     polygon.push_back(p2);
    //     polygon.push_back(p3);

    //     if(!polygon.is_convex()){
    //         std::cout<<"The polygon is not convex"<<std::endl;
    //         continue;
    //     }
    //     if(CGAL::collinear(p3,p0,p2) || CGAL::collinear(p1,p0,p2)){
    //         std::cout<<"Collinear"<<std::endl;
    //         continue;
    //     }
    //     // Check if the polygon is convex
    //     if (!cdt.is_constrained(edge)) {
    //         // Ensure that the edge is internal
    //         if (!cdt.is_infinite(f1) && !cdt.is_infinite(f2)) {
    //             // Perform the flip
    //             if(obtuse_face(f1)== true  || obtuse_face(f2)==true ){
    //                 std::cout << "Flipped" << std::endl;
    //                 cdt.flip(f1, index);
    //             }
    //         }else {
    //             std::cout << "Edge is on the boundary, not flippable" << std::endl;
    //         }
    //     } else {
    //         std::cout << "Edge is constrained, not flippable" << std::endl;
    //     }
    // }

    // c2=obtuse_count(cdt);
    // std::cout <<"After flip" << c2 << std::endl;
    // CGAL::draw(cdt);

    

    
    //----------------------------Middle Point-------------------
    // limit = 20;
    // for(int i=0; i<limit ;i++){
    //     it = cdt.finite_faces_begin();
    //     beyond = cdt.finite_faces_end();
    //     std::vector<std::tuple<Face_handle, Face_handle,Edge,Point>> polyinfo = {};
    //     while (it != beyond) {
    //         bool obtuse = false;
    //         face = *it;  // get face
    //         Face_handle facep = it;
    //         Point m;
    //         if(obtuse_face(facep)){
    //             m = get_middlepoint(facep);
    //             cdt.insert_no_flip(m);
    //             points.push_back(m);
    //             break;
    //         }
    //         ++it;        // advance the iterator
    //     }
    // }
    // c2=obtuse_count(cdt);
    // std::cout <<"Before" << c1 << std::endl;
    // std::cout <<"After middle steiner "<< c2 << std::endl; 
    // CGAL::draw(cdt);
    
    //-----------------------------Circum-center-Centroid------------------------------------------
    // c1=obtuse_count(cdt2);
    // limit = 20;
    // for(int i=0; i<limit ;i++){
    //     it = cdt2.finite_faces_begin();
    //     beyond = cdt2.finite_faces_end();
    //     std::vector<std::tuple<Face_handle, Face_handle,Edge,Point>> polyinfo = {};
    //     while (it != beyond) {
    //         bool obtuse = false;
    //         face = *it;  // get face
    //         Face_handle facep = it;
    //         Point ce;
    //         if(obtuse_face(facep)){
    //             ce = get_circumcenter_centroid(cdt, facep);
    //             cdt2.insert_no_flip(ce);
    //             points.push_back(ce);
    //             break;
    //         }
    //         ++it;        // advance the iterator
    //     }
    // }
    // c2=obtuse_count(cdt2);
    // std::cout <<"Before" << c1 << std::endl;
    // std::cout <<"After centroid/circumsenter steiner "<< c2 << std::endl; 
    // CGAL::draw(cdt2);

   //-----------------------------Polygon--------------------------------------------------
    // c1=cdt3.obtuse_angles_count();
    // limit = 10;
    // for(int i=0; i<limit ;i++){
    //     polygon_center(cdt3);
    // }
    // c2=cdt3.obtuse_angles_count();
    // std::cout <<"Before" << c1 << std::endl;
    // std::cout <<"After polygon steiner "<< c2 << std::endl; 
    // CGAL::draw(cdt3);

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
            if(obtuse_face(facep)){  //PUT REG BOUNDRY
                proj = get_projection(facep);
                cdt4.insert_no_flip(proj);
                points.push_back(proj);

                std::pair<std::string, std::string> xpair = print_rational(proj.x());
                std::pair<std::string, std::string> ypair = print_rational(proj.y());

                if(xpair.second == "1"){
                    steinerx.push_back(xpair.first + "/end");
                }else{
                    steinerx.push_back(xpair.first + "/" + xpair.second);
                }

                if(ypair.second == "1"){
                    steinery.push_back(ypair.first + "/end");
                }else{
                    std::cout<<ypair.first<<std::endl;
                    steinery.push_back(ypair.first + "/" + ypair.second);
                }
                    
                break;
            }
            ++it;        // advance the iterator
        }
    }
    c2=cdt4.obtuse_angles_count();
    std::cout <<"Before" << c1 << std::endl;
    std::cout <<"After projection"<< c2 <<" "<<obtuse_count(cdt4)<< std::endl; 
    CGAL::draw(cdt4);

    


    //EXPORTING--------------------------------------------------------
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

    // Construct JSON object
    json::object obj1;
    obj1["content_type"] = "CG_SHOP_2025_Solution";
    obj1["instance_uid"] = id;


    // Convert steiner_points_x and steiner_points_y to JSON arrays
    json::array json_steiner_points_x, json_steiner_points_y;
    for (const auto& x : steinerx){
        if (x.find("/end") != std::string::npos) {
            // Extract the part before "/1" and convert it to an integer
            int number = std::stoi(x.substr(0, x.find("/")));
            json_steiner_points_x.push_back(json::value(number));  // Add as integer
        } else {
            json_steiner_points_x.push_back(json::value(x));  // Add as string
        }
    }
    for (const auto& y : steinery) {
        if (y.find("/end") != std::string::npos) {
            // Extract the part before "/1" and convert it to an integer
            int number = std::stoi(y.substr(0, y.find("/")));
            json_steiner_points_y.push_back(json::value(number));  // Add as integer
        } else {
            json_steiner_points_y.push_back(json::value(y));  // Add as string
        }
    }
    obj1["steiner_points_x"] = json_steiner_points_x;
    obj1["steiner_points_y"] = json_steiner_points_y;

    // Convert edges to JSON array of arrays
    json::array json_edges;
    for (const auto& edge : edges) {
        json_edges.push_back({edge.first, edge.second});
    }
    obj1["edges"] = json_edges;

    // Output the JSON object as a string
    std::string json_str = json::serialize(obj1);
    std::ofstream fileout("out/output.json");
    if (fileout.is_open()) {
        fileout << json_str;
        fileout.close();
        std::cout << "JSON saved to output.json" << std::endl;
    } else {
        std::cerr << "Unable to open file for writing!" << std::endl;
    }
    return 0;
    
}