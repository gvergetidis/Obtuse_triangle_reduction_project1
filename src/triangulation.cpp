#include "../include/CCDT.h"

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
    if(argc != 2){                                              //check the arguments
        std::cerr<<"Usage: "<<argv[0]<<" test_file_name\n";
        exit(1);
    }

    std::ifstream file(argv[1]);                                //open the file
    if (!file){
        std::cerr<<"could not open file: "<<argv[1]<<std::endl;
        exit(1);
    }

    //get the data of the file as a string
    std::string data((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());
    file.close();                                               //close the file

    //parse with json the data
    json::value jv = json::parse(data);
    json::object obj = jv.as_object();


    //variables and containers that store the data
    std::string id;
    int n_points;
    std::vector<int> X;
    std::vector<int> Y;
    std::vector<int> region_boundary;
    int n_constraints;
    std::vector<std::pair<int, int>> constraints;


    id = json::value_to<std::string>(obj["instance_uid"]);      //get the "instance_uid"
    n_points = json::value_to<int>(obj["num_points"]);          //get the "num_points"

    //get all the x coordinates in the X vector
    for (const auto& x : obj["points_x"].as_array()) {
        X.push_back(json::value_to<int>(x));
    }
    //get all the y coordinates in the Y vector
    for (const auto& y : obj["points_y"].as_array()) {
        Y.push_back(json::value_to<int>(y));
    }

    //get all the region boundary indices in the region_boundary vector
    for (const auto& boundary : obj["region_boundary"].as_array()) {
        region_boundary.push_back(json::value_to<int>(boundary));
    }

    //get the number of constraints 
    n_constraints = json::value_to<int>(obj["num_constraints"]);

    //get all the pairs of constraints in constraints vector
    for (const auto& constraint : obj["additional_constraints"].as_array()){  //take every constraint from the constraints array
        auto c = constraint.as_array();                 //the constraint contains 2 indices, so use it as array
        if (c.size() == 2){                             //check the number of elements in a constraint that should be 2
            int i = json::value_to<int>(c[0]);          //get the first index
            int j = json::value_to<int>(c[1]);          //get the second index
            constraints.emplace_back(i, j);             //emplace these 2 indices that form a constraint in the vector
        }else{                                          //handle this error
            std::cerr<<"The constarins have not the correct form\n";
            exit(1);
        }
    }

    //check that the number of points matches with the number of x and y coordinates
    if((X.size() != n_points) || (Y.size() != n_points)){
        std::cerr<<"Number of points_x is different than number of points_y \n";
        exit(1);
    }

    //check the number of constraints matches with the declared
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

    //create a vector that contains the points formed by X and Y coordinates
    std::vector<Point> points = {};
    for(int i = 0; i<n_points; i++) points.push_back(Point(X[i],Y[i]));

    std::cout<<"Points: ";
    for(auto const& p:points) std::cout<<"("<<p.x()<<","<<p.y()<<")"<<" ";
    std::cout<<std::endl;

    //create a vector that contains the points of the region boundary
    std::vector<Point> rbpoints = {};
    for(auto const& i: region_boundary){
        rbpoints.push_back(points[i]);
    }

    
    //create a vector that contains the region boundary edges that will be additionally constrained
    std::vector<std::pair<int, int>> RB_edges = {};
    for(int i=0; i<(region_boundary.size()-1); i++) RB_edges.emplace_back(region_boundary[i], region_boundary[i+1]);
    RB_edges.emplace_back(region_boundary[0], region_boundary[region_boundary.size()-1]);

    std::cout<<"Region boundary edges: ";
    for(auto const& e:RB_edges) std::cout << "(" << e.first << ", " << e.second << ") ";
    std::cout<<std::endl;

    //--------------------Create cdt---------------------------------------
    CDT cdt;
    for(const Point& p :points) cdt.insert(p);                          //insert the points in cdt
    // for(const auto& constraint:constraints){
    //     cdt.insert_constraint(points[constraint.first], points[constraint.second]);
    // }


    // for(const auto& e : RB_edges) cdt.insert_constraint(points[e.first],points[e.second]);
    
    for(const auto& p : rbpoints) cdt.region_boundary.push_back(p);     //insert the region boundary points in the cdt region boundary polygon
    CGAL::draw(cdt);

    Face_iterator it = cdt.faces_begin();
    Face_iterator beyond = cdt.faces_end();
    
    
    Face face;
    
    
    std::vector<std::string> steinerx = {};
    std::vector<std::string> steinery = {};

    CDT cdt2 = CDT(cdt);
    CDT cdt3 = CDT(cdt);
    CDT cdt4 = CDT(cdt);


    int limit , c1 , c2;
    c1=obtuse_count(cdt);
    std::cout <<"Before" << c1 << std::endl;


    //---------------------------------Flipping--------------------------------
    for (auto e : cdt.finite_edges()){                              //for every edge
        std::cout<<"-------------------------\n";
        

        //check if the edge is constrained and if it is, continue to the next edge
        if (cdt.is_constrained(e)) continue;

        Face_handle f1 = e.first;                                   //get the face containing the edge
        int index = e.second;                                       //get the index of the edge
        Face_handle f2 = f1->neighbor(index);                       //get the neighboor that shares this edge

        //check if the 2 neighboors sharing this edge are finite so that the edge can be flipped inside them
        //if any of them is not continue to the next edge
        if(cdt.is_infinite(f1) || cdt.is_infinite(f2)) continue;

        //if none of the 2 faces that share the edge is obtuse, there is no reason to flip and we continue to the next edge
        if(!obtuse_face(f1) && !obtuse_face(f2)) continue;

        Polygon_2 polygon;                                          //create a polygon with the points of these 2 faces
        //get the points in counterclockwise order
        Point p0 = f1->vertex(index)->point();
        Point p1 = f1->vertex(CDT::ccw(index))->point();
        Point p2 = f2->vertex(f2->index(f1))->point(); 
        Point p3 = f1->vertex(CDT::cw(index))->point();

        //form the polygon
        polygon.push_back(p0);
        polygon.push_back(p1);
        polygon.push_back(p2);
        polygon.push_back(p3);

        
        //check if the polygon id convex and if it is continue to the next edge
        if(!polygon.is_convex()){
            std::cout<<"The polygon is not convex"<<std::endl;
            continue;
        }

        //check if an endpoint of the edge and the points where the edge will be flipped are collinear
        //if they are, the dont form a valid traingulation and we must continue to the next edge
        if(CGAL::collinear(p3,p0,p2) || CGAL::collinear(p1,p0,p2)){
            std::cout<<"Collinear"<<std::endl;
            continue;
        }
        
        //if we reached here we satisfy all the constraints and we can perform flip 
        std::cout << "Flipped" << std::endl;
        cdt.flip(f1, index);
        
    }

     c2=obtuse_count(cdt);
     std::cout <<"After flip" << c2 << std::endl;
     CGAL::draw(cdt);


    
    //----------------------------Middle Point-------------------
    limit = 20;
    for(int i=0; i<limit; i++){                                 //iterate limit times
        for(auto f = cdt.finite_faces_begin(); f != cdt.finite_faces_end(); f++){   //for every face
            Point m;
            if(obtuse_face(f)){                                 //if the face is obtuse
                m = get_middlepoint(f);                         //get the mid point
                cdt.insert_no_flip(m);                          //insert the point
                points.push_back(m);                            //put the new point in the points vector
                break;
            }
         }
     }
     c2=obtuse_count(cdt);
     std::cout <<"Before" << c1 << std::endl;
     std::cout <<"After middle steiner "<< c2 << std::endl; 
     CGAL::draw(cdt);

     
    
    //-----------------------------Circum-center-Centroid------------------------------------------
    c1=obtuse_count(cdt2);
    limit = 20;
    for(int i=0; i<limit; i++){                                 //iterate limit times
        for(auto f = cdt2.finite_faces_begin(); f != cdt2.finite_faces_end(); f++){   //for every face
            Point ce;
            if(obtuse_face(f)){                                 //if the face is obtuse
                ce = get_circumcenter_centroid(cdt2, f);         //get the circumcenter or centroid point
                cdt2.insert_no_flip(ce);                        //insert the point
                points.push_back(ce);                           //put the new point in the points vector
                break;
            }
        }
     }
     c2=obtuse_count(cdt2);
     std::cout <<"Before" << c1 << std::endl;
     std::cout <<"After centroid/circumsenter steiner "<< c2 << std::endl; 
     CGAL::draw(cdt2);



   //-----------------------------Polygon center-----------------------------------------------
    c1=cdt3.obtuse_angles_count();
    limit = 10;
    for(int i=0; i<limit; i++){                                 //iterate limit times
        polygon_center(cdt3);                                   //perform the procedure of polygon center
    }
    c2=cdt3.obtuse_angles_count();
    std::cout <<"Before" << c1 << std::endl;
    std::cout <<"After polygon steiner "<< c2 << std::endl; 
    CGAL::draw(cdt3);

    //-----------------------------Projection------------------------------------------------------
    c1=obtuse_count(cdt4);
    std::cout <<"CDT4"<<std::endl; 
    CGAL::draw(cdt4);
    limit = 20;
    for(int i=0; i<limit; i++){                                 //iterate limit times
        for(auto f = cdt4.finite_faces_begin(); f != cdt4.finite_faces_end(); f++){
            Point proj;
            if(obtuse_face(f)){  //PUT REG BOUNDRY
                proj = get_projection(f);                       //get the projection point
                cdt4.insert_no_flip(proj);                      //insert the point
                points.push_back(proj);                         //put the new point in the points vector

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

            if(points[i] == p1){
                i1 = i;
            }
            if(points[i] == p2){
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