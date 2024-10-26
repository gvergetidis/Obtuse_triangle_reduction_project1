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
    std::vector<Point> startpoints ={};
    for(const Point& p :points){
        cdt.insert(p);
        startpoints.push_back(p);
    }
    for(const auto& constraint:constraints){
        cdt.insert_constraint(points[constraint.first], points[constraint.second]);
    }


    for(const auto& e : RB_edges) cdt.insert_constraint(points[e.first],points[e.second]);
    for(const auto& p : rbpoints) cdt.region_boundary.push_back(p);
    CGAL::draw(cdt);


    // if(cdt.point_in_region_boundary(Point(192,520))) std::cout<<"in region boundary\n";
    // else std::cout<<"not in region boundary\n";
    // CDT cdt5 = CDT(cdt);
    // CGAL::draw(cdt5);
    // if(cdt5.point_in_region_boundary(Point(192,520))) std::cout<<"in region boundary\n";
    // else std::cout<<"not in region boundary\n";


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
    for(int k=0; k<1; k++){
        for (auto e : cdt.finite_edges()) {
            std::cout<<"-------------------------\n";
            Edge edge = e;
            // Get the two faces sharing this edge
            Face_handle f1 = edge.first;
            int index = edge.second;
            Face_handle f2 = f1->neighbor(index);

            // Check if the edge is internal (not on the boundary) and not constrained
            Polygon_2 polygon;
            Point p0 = f1->vertex(index)->point();
            Point p1 = f1->vertex(CDT::ccw(index))->point();
            Point p2 = f2->vertex(f2->index(f1))->point();
            Point p3 = f1->vertex(CDT::cw(index))->point();

            polygon.push_back(p0);
            polygon.push_back(p1);
            polygon.push_back(p2);
            polygon.push_back(p3);

            if(!polygon.is_convex()){
                std::cout<<"The polygon is not convex"<<std::endl;
                continue;
            }
            // Check if the polygon is convex
            if (!cdt.is_constrained(edge)) {
                // Ensure that the edge is internal
                if (!cdt.is_infinite(f1) && !cdt.is_infinite(f2)) {
                    // Perform the flip
                    if(obtuse_face(f1)== true  || obtuse_face(f2)==true ){
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

    c1=obtuse_count(cdt);
    std::cout <<"After flip" << c1 << std::endl;

    CGAL::draw(cdt);
    return 0;
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
            if(obtuse_face(facep)){
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
            if(obtuse_face(facep)){
                ce = get_circumcenter_centroid(cdt, facep);
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

   //-----------------------------Polygon--------------------------------------------------
    
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
            if(obtuse_face(facep)){
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
    c2=cdt4.obtuse_angles_count();
    std::cout <<"Before" << c1 << std::endl;
    std::cout <<"After projection"<< c2 <<" "<<obtuse_count(cdt4)<< std::endl; 
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