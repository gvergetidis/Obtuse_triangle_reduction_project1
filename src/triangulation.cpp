#include "../include/CCDT.h"
#define limit 20  //the limit of the number of loops for each method 

int main(int argc, char* argv[]){
    //---------------INPUT---------------------------------------------------------------
    if(argc != 2){                                              //check the arguments
        std::cout<<"Usage: "<<argv[0]<<" test_file_name\n";
        exit(1);
    }

    std::ifstream file(argv[1]);                                //open the file
    if(!file){
        std::cout<<"could not open file: "<<argv[1]<<std::endl;
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
    for(const auto& x : obj["points_x"].as_array()){
        X.push_back(json::value_to<int>(x));
    }
    //get all the y coordinates in the Y vector
    for(const auto& y : obj["points_y"].as_array()){
        Y.push_back(json::value_to<int>(y));
    }

    //get all the region boundary indices in the region_boundary vector
    for(const auto& boundary : obj["region_boundary"].as_array()){
        region_boundary.push_back(json::value_to<int>(boundary));
    }

    //get the number of constraints 
    n_constraints = json::value_to<int>(obj["num_constraints"]);

    //get all the pairs of constraints in constraints vector
    for(const auto& constraint : obj["additional_constraints"].as_array()){  //take every constraint from the constraints array
        auto c = constraint.as_array();                 //the constraint contains 2 indices, so use it as array
        if (c.size() == 2){                             //check the number of elements in a constraint that should be 2
            int i = json::value_to<int>(c[0]);          //get the first index
            int j = json::value_to<int>(c[1]);          //get the second index
            constraints.emplace_back(i, j);             //emplace these 2 indices that form a constraint in the vector
        }else{                                          //handle this error
            std::cout<<"The constarins have not the correct form\n";
            exit(1);
        }
    }

    //check that the number of points matches with the number of x and y coordinates
    if((X.size() != n_points) || (Y.size() != n_points)){
        std::cout<<"Number of points_x is different than number of points_y \n";
        exit(1);
    }

    //check the number of constraints matches with the declared
    if(constraints.size() != n_constraints){
        std::cout<<"Number of constraints is different than declared number\n";
        exit(1);
    }

    //create a vector that contains the points formed by X and Y coordinates
    std::vector<Point> points = {};
    for(int i = 0; i<n_points; i++) points.push_back(Point(X[i],Y[i]));

    //create a vector that contains the points of the region boundary
    std::vector<Point> rbpoints = {};
    for(auto const& i: region_boundary){
        rbpoints.push_back(points[i]);
    }

    
    //create a vector that contains the region boundary edges that will be additionally constrained
    std::vector<std::pair<int, int>> RB_edges = {};
    for(int i=0; i<(region_boundary.size()-1); i++) RB_edges.emplace_back(region_boundary[i], region_boundary[i+1]);
    RB_edges.emplace_back(region_boundary[0], region_boundary[region_boundary.size()-1]);

    //--------------------Create cdt---------------------------------------
    CDT cdt , cdt1 , cdt2 , cdt3 , cdt4 , cdt5;
    for(const Point& p :points) cdt.insert(p);                          //insert the points in cdt
    //add the constaint edges
    for(const auto& constraint:constraints){
        cdt.insert_constraint(points[constraint.first], points[constraint.second]);
    }

    //insert the region boundary points in the cdt region boundary polygon
    for(const auto& e : RB_edges) cdt.insert_constraint(points[e.first],points[e.second]);
    for(const auto& p : rbpoints) cdt.region_boundary.push_back(p);  

    CGAL::draw(cdt);

    Face_iterator it = cdt.faces_begin();
    Face_iterator beyond = cdt.faces_end();
    Face face;
    
    std::vector<std::string> steinerx = {};
    std::vector<std::string> steinery = {};

    cdt1 = CDT(cdt); //cdt copy that will do the flips

    int cbefore , cafter; //obtuse angles counts before and after each operation
    cbefore=cdt1.obtuse_angles_count();
    std::cout <<"Before flips: " << cbefore << std::endl;

    //---------------------------------Flipping--------------------------------
    for(auto e : cdt1.finite_edges()){                              //for every edge
        //check if the edge is constrained and if it is, continue to the next edge
        if (cdt1.is_constrained(e)) continue;

        Face_handle f1 = e.first;                                   //get the face containing the edge
        int index = e.second;                                       //get the index of the edge
        Face_handle f2 = f1->neighbor(index);                       //get the neighboor that shares this edge

        //check if the 2 neighboors sharing this edge are finite so that the edge can be flipped inside them
        //if any of them is not continue to the next edge
        if(cdt1.is_infinite(f1) || cdt1.is_infinite(f2)) continue;

        //check if the 2 neighboors belong in the region boundary
        if(!cdt1.face_in_region_boundary(f1) || !cdt1.face_in_region_boundary(f2)) continue;

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
            continue;
        }

        //check if an endpoint of the edge and the points where the edge will be flipped are collinear
        //if they are, the dont form a valid traingulation and we must continue to the next edge
        if(CGAL::collinear(p3,p0,p2) || CGAL::collinear(p1,p0,p2)){
            continue;
        }
        
        //if we reached here we satisfy all the constraints and we can perform flip 
        cdt1.flip(f1, index);
        
    }

     cafter=cdt1.obtuse_angles_count();
     std::cout <<"After flips: " << cafter << std::endl;
     CGAL::draw(cdt1);
    

    if(cafter<cbefore){ //if the flips reduced the obtuse angles use the copy of the cdt with the flips
        cdt2 = CDT(cdt1); //cdt for the midle point method
        cdt3 = CDT(cdt1); //cdt for the centroid point method
        cdt4 = CDT(cdt1); //cdt for the polygon center method
        cdt5 = CDT(cdt1); //cdt for the projection point method
    }
    else{  //if the flips did not help use the cdt without the flips
        cdt2 = CDT(cdt); //cdt for the midle point method
        cdt3 = CDT(cdt); //cdt for the centroid point method
        cdt4 = CDT(cdt); //cdt for the polygon center method
        cdt5 = CDT(cdt); //cdt for the projection point method
    }


    
    //----------------------------Middle Point-------------------
    cbefore=cdt2.obtuse_angles_count();
    std::cout <<"Before middle steiner: " << cbefore << std::endl;

    for(int i=0; i<limit; i++){                                 //iterate limit times
        for(auto f = cdt2.finite_faces_begin(); f != cdt2.finite_faces_end(); f++){   //for every face
            Point m;
            if(obtuse_face(f) && cdt2.face_in_region_boundary(f)){  //if the face is obtuse and belongs in the region boundry
                m = get_middlepoint(f);                         //get the mid point
                cdt2.insert_no_flip(m);                          //insert the point
                points.push_back(m);                            //put the new point in the points vector
                break;
            }
         }
     }
     cafter=cdt2.obtuse_angles_count();
     std::cout <<"After middle steiner: "<< cafter << std::endl; 
     CGAL::draw(cdt2);

     
    
    //-----------------------------Circum-center-Centroid------------------------------------------
    cbefore=cdt3.obtuse_angles_count();
    std::cout <<"Before centroid/circumcenter steiner: " << cbefore << std::endl;

    for(int i=0; i<limit; i++){                                 //iterate limit times
        for(auto f = cdt3.finite_faces_begin(); f != cdt3.finite_faces_end(); f++){   //for every face
            Point ce;
            if(obtuse_face(f) && cdt3.face_in_region_boundary(f)){ //if the face is obtuse and belongs in the region boundry
                ce = get_circumcenter_centroid(cdt3, f);         //get the circumcenter or centroid point
                cdt3.insert_no_flip(ce);                        //insert the point
                points.push_back(ce);                           //put the new point in the points vector
                break;
            }
        }
     }
     cafter=cdt3.obtuse_angles_count();
     std::cout <<"After centroid/circumcenter steiner: "<< cafter << std::endl; 
     CGAL::draw(cdt3);



   //-----------------------------Polygon center-----------------------------------------------
    cbefore=cdt4.obtuse_angles_count();
    std::cout <<"Before polygon steiner: " << cbefore << std::endl;
    for(int i=0; i<limit; i++){                                 //iterate limit times
        polygon_center(cdt4);                                   //perform the procedure of polygon center
    }
    cafter=cdt4.obtuse_angles_count();
    std::cout <<"After polygon steiner: "<< cafter << std::endl; 
    CGAL::draw(cdt4);

    //-----------------------------Projection------------------------------------------------------
    cbefore=cdt5.obtuse_angles_count();
    std::cout <<"Before projection steiner: " << cbefore << std::endl;

    for(int i=0; i<limit; i++){                                 //iterate limit times
        for(auto f = cdt5.finite_faces_begin(); f != cdt5.finite_faces_end(); f++){
            Point proj;
            if(obtuse_face(f) && cdt5.face_in_region_boundary(f)){  //if the face is obtuse and belongs in the region boundry
                proj = get_projection(f);                       //get the projection point
                cdt5.insert_no_flip(proj);                      //insert the point
                points.push_back(proj);                         //put the new point in the points vector

                std::pair<std::string, std::string> xpair = print_rational(proj.x());  //save the Numerator
                std::pair<std::string, std::string> ypair = print_rational(proj.y());  //save the Denominator
                //the special sting with /end is used so we can identify that 
                //this string shoud be an int and input it correctly in the json output
                if(xpair.second == "1"){ //if the Denominator is 1 
                    steinerx.push_back(xpair.first + "/end"); //save in the sting the special sting with /end
                }else{
                    steinerx.push_back(xpair.first + "/" + xpair.second); //save the rational number
                }

                if(ypair.second == "1"){//if the Denominator is 1 
                    steinery.push_back(ypair.first + "/end"); //save in the sting the special sting with /end
                }else{
                    steinery.push_back(ypair.first + "/" + ypair.second);//save the rational number
                }       
                break;
            }
        }
    }
    cafter=cdt5.obtuse_angles_count();
    std::cout <<"After projection steiner: "<< cafter <<std::endl; 
    CGAL::draw(cdt5);

    //---Exporting--------------------------------------------------------
    std::vector<std::pair<int, int>> edges = {}; //the vector with the edges that will be exported
    for(const Edge& e : cdt5.finite_edges()){
        CDT::Face_handle face = e.first; //get the face of the edge
        int index = e.second;            //get the index of the edge
        //find the two points of the edge using the functions cw ,ccw
        CDT::Vertex_handle vh1 = face->vertex((index + 1) % 3);  
        CDT::Vertex_handle vh2 = face->vertex((index + 2) % 3);
        Point p1 = vh1->point();
        Point p2 = vh2->point();
        //initalize the indexes of the points to -1
        int i1 = -1;
        int i2 = -1;
        //search the points for the 2 edge points
        for(int i=0 ; i<points.size() ; i++){
            //if found a point of the edge equal to a point of the points vector save its index
            if(points[i] == p1){
                i1 = i;
            }
            if(points[i] == p2){
                i2 = i;
            }
        }
        if( i1 == -1 || i2 == -1){
            std::cout<<"NOT FOUND"<<std::endl;
            break;
        }
        //seve the edge as a pair odf the 2 indexes of the points
        edges.push_back(std::make_pair(i1,i2));
    }

    // Construct JSON output
    json::object obj1; //root object obj1
    obj1["content_type"] = "CG_SHOP_2025_Solution";  //add the content type
    obj1["instance_uid"] = id;  //add the instance uid 

    json::array json_x, json_y;
    for(const auto& x : steinerx){ //for every x coordinate of the steiner points
        //check if x needs to be pushed as int or string example: is "12/33" string or is 4 int
        if(x.find("/end") != std::string::npos){ //if the x string contains /end
            int num = std::stoi(x.substr(0, x.find("/"))); //get the nominator save it in number
            json_x.push_back(json::value(num));  //push the number as int
        }else{
            json_x.push_back(json::value(x)); //push the rational number as string
        }
    }
    for(const auto& y : steinery){
        //check if y needs to be pushed as int or string example: is "12/33" string or is 4 int
        if(y.find("/end") != std::string::npos){//if the y string contains /end
            int num = std::stoi(y.substr(0, y.find("/")));//get the nominator save it in number
            json_y.push_back(json::value(num)); //push the number as int
        }else{
            json_y.push_back(json::value(y)); //push the rational number as string
        }
    }
    //add the x and y steiner coordinates
    obj1["steiner_points_x"] = json_x;
    obj1["steiner_points_y"] = json_y;

    json::array j_edges;
    for(const auto& edge : edges){ //for every edge add a pair of the 2 indexes
        j_edges.push_back({edge.first, edge.second});
    }
    obj1["edges"] = j_edges; //add the edges

    std::string file_path = "out/output.json";
    std::string json_str = json::serialize(obj1); //serialize the obj1
    std::ofstream file_output(file_path);  //the file will be outputes in the specific path
    if(file_output.is_open()){ //check if file is open
        file_output << json_str; //write the sreialized string
        file_output.close();  //close the file
        std::cout << "JSON saved to "+file_path << std::endl;
    }else{
        std::cout << "Unable to open file for writing" << std::endl;
    }
    return 0;
}