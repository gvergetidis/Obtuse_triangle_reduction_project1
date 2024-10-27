#include "../include/CCDT.h"

//function that determines if a face is obtuse
bool obtuse_face(Face_handle face){
    //take all the points of the face
    Point a = face->vertex(0)->point();
    Point b = face->vertex(1)->point();
    Point c = face->vertex(2)->point();

    //get the angles that the points create
    Angle A = angle(b,a,c);
    Angle B = angle(a,b,c);
    Angle C = angle(a,c,b);
    
    //check if any angle is obtuse and return the corresponding value 
    if(A == CGAL::OBTUSE) return true;
    if(B == CGAL::OBTUSE) return true;
    if(C == CGAL::OBTUSE) return true;

    return false;
}


//function that returns the index of the vertex that creates an obtuse angle in the face
int get_obtuse_ind(Face_handle facep){
    //take all the points of the face
    Point a = facep->vertex(0)->point();
    Point b =  facep->vertex(1)->point();
    Point c =  facep->vertex(2)->point();

    //get the angles that the points create
    Angle A = angle(b,a,c);
    Angle B = angle(a,b,c);
    Angle C = angle(a,c,b);

    //find which angle is obtuse and return the corresponding index
    if(A == CGAL::OBTUSE) return 0;
    else if(B == CGAL::OBTUSE) return 1;
    else if(C == CGAL::OBTUSE) return 2;
    else return -1;                 //if there is not any obtuse angle return -1
}

//given function for computing the rational, it is changed in order to retun the values as strings
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


    std::cout << "*******" << std::endl;
    std::cout << result.first << std::endl;
    std::cout << "*******" << std::endl;
    return result;
}


//function that given a face returns the projection of the obtuse angle vertex to the opposite segment
Point get_projection(Face_handle facep){
    int obtuseInd = get_obtuse_ind(facep);          //get the index of the obtuse angle
    Point p1 = facep->vertex(obtuseInd)->point();   //the point of this vertex
    //the 2 points that form a segment opposite the the obtuse angle 
    Point p2 = facep->vertex(CDT::ccw(obtuseInd))->point();
    Point p3 = facep->vertex(CDT::cw(obtuseInd))->point();

    Line l(p2,p3);                                  //form a line with these 2 points
    Point proj = l.projection(p1);                  //find the projection point of the obtuse angle to this line
    return proj;                                    //return the point
}

//function that given a face returns the middlepoint of the obtuse angle vertex opposite segment
Point get_middlepoint(Face_handle facep){
    int obtuseInd = get_obtuse_ind(facep);          //get the index of the obtuse angle
    Point p1 = facep->vertex(obtuseInd)->point();   //the point of this vertex
    //the 2 points that form a segment opposite the the obtuse angle
    Point p2 = facep->vertex(CDT::ccw(obtuseInd))->point();
    Point p3 = facep->vertex(CDT::cw(obtuseInd))->point();

    Point m = CGAL::midpoint(p2,p3);                //compute the midpoint
    return m;                                       //return the midpoint
}

//function that given a face returns the centroid of the face
Point get_centroid(Face_handle facep){
    //get the face points
    int obtuseInd = get_obtuse_ind(facep);
    Point p1 = facep->vertex(obtuseInd)->point();
    Point p2 = facep->vertex(CDT::ccw(obtuseInd))->point();
    Point p3 = facep->vertex(CDT::cw(obtuseInd))->point();
    //compute centroid point
    Point ce = CGAL::centroid(p1,p2,p3);
    return ce;                                      //return the point
}

//function that given a face returns the circumcenter of the face
Point get_circumcenter(Face_handle facep){
    //get the face points
    int obtuseInd = get_obtuse_ind(facep);
    Point p1 = facep->vertex(obtuseInd)->point();
    Point p2 = facep->vertex(CDT::ccw(obtuseInd))->point();
    Point p3 = facep->vertex(CDT::cw(obtuseInd))->point();
    //compute circumcenter point
    Point circ = CGAL::circumcenter(p1,p2,p3);
    return circ;                                    //return the point
}

//function that given a face returns the circumcenter or centroid of the face based on region boundary condition
Point get_circumcenter_centroid(CDT cdt,Face_handle facep){
    Point steiner = get_circumcenter(facep);        //get the centroid
    if(!cdt.point_in_region_boundary(steiner)){     //if circumcenter is outside region boundary
        steiner = get_centroid(facep);              //get the centroid
    }
    return steiner;                                 //return the corresponding steiner point
}

//function that given a face and a cdt, determines if the face has constraints in cdt
bool face_has_constraint(CDT cdt,Face_handle facep){
    //get the edges of the face
    Edge e1 = Edge(facep, 0);
    Edge e2 = Edge(facep, 1);
    Edge e3 = Edge(facep, 2);
    //check if any of these edges is constrained
    return (cdt.is_constrained(e1) || cdt.is_constrained(e2) || cdt.is_constrained(e3));
}


//function that given a cdt seraches for a polygon formed by obtuse neighboor faces 
//and adds a steiner point in the center of this polygon
void polygon_center(CDT &cdt){   
    bool found = false;                         //boolean variable that indicates if we found a polygon
    std::vector <Point> polv;                   //vector that stores the points of the polygon  
    for(auto fit = cdt.finite_faces_begin(); fit != cdt.finite_faces_end(); ++fit){     //iterate all the faces
        if(obtuse_face(fit) && !face_has_constraint(cdt , fit)){  //search for obtuse faces and faces that dont have constraints
            //try to form the poygon, by getting all the potential points of the obtuse face
            //and its potential obtuse neighboors in counterclockwise order
            //the way that we store the points forms a counterclockwise order of points

            polv.push_back(fit->vertex(1)->point());        //store this point

            Face_handle neigh0 = fit->neighbor(0);          //get the neighboor 0
            //check if neighboor 0 is obtuse, finite and has not constraints
            if(obtuse_face(neigh0) && (!cdt.is_infinite(neigh0)) && !face_has_constraint(cdt , neigh0)){
                //store the point of neighboor 0 that is opposite the initial obtuse face
                polv.push_back(neigh0->vertex(neigh0->index(fit))->point());
                found = true;                                    //indicate that a polygon is found and can be formed
            }

            polv.push_back(fit->vertex(2)->point());            //store this point

            Face_handle neigh1 = fit->neighbor(1);              //get neighboor 1
            //check if neighboor 1 is obtuse, finite and has not constraints
            if(obtuse_face(neigh1) && (!cdt.is_infinite(neigh1)) && !face_has_constraint(cdt , neigh1)){
                //store the point of neighboor 1 that is opposite the initial obtuse face
                polv.push_back(neigh1->vertex(neigh1->index(fit))->point());    
                found = true;                                    //indicate that a polygon is found and can be formed

            }

            polv.push_back(fit->vertex(0)->point());            //store this point

            Face_handle neigh2 = fit->neighbor(2);              //get neighboor 2
            //check if neighboor 2 is obtuse, finite and has not constraints
            if(obtuse_face(neigh2) && (!cdt.is_infinite(neigh2)) && !face_has_constraint(cdt , neigh2)){
                //store the point of neighboor 2 that is opposite the initial obtuse face
                polv.push_back(neigh2->vertex(neigh2->index(fit))->point());
                found = true;                                   //indicate that a polygon is found and can be formed
            }
            
            //form a polygon from the points that we collected
            Polygon_2 polyg;
            for(auto p : polv){
                polyg.push_back(p);                     
            }
            //check if the polygon is convex, if not the polygon we want is not found 
            if(!polyg.is_convex()){
                found = false;
            }            
                    
            if (found){                                        //if the polygon is found and all the constraints are satisfied         
                //remove the points of the polygon from cdt
                for(auto p : polv){
                    cdt.remove(cdt.insert(p));
                    std::cout<<"("<<p.x()<<","<<p.y()<<") ";                        
                }

                //compute the centroid of the polygon
                Point p = CGAL::centroid(polv.begin(), polv.end(), CGAL::Dimension_tag<0>());
                cdt.insert_no_flip(p);                          //insert the center point in cdt

                std::vector<std::pair<Point,Point>> cons;       //vector to store the constraints between the polygon points

                for (int i = 0; i < polv.size(); i++){          //create and store these constraints
                    const Point& p1 = polv[i];                  //get a point
                    const Point& p2 = polv[(i+1) % polv.size()]; //get the next point (use mod to get the last constraint from last point to first point)  
                    cons.push_back(std::make_pair(p1,p2));
                            
                }

                //insert the removed points as constraints 
                for(int i=0; i<cons.size(); i++) cdt.insert_constraint(cons[i].first, cons[i].second);

                //remove the constraints that were inserted
                for(int i=0; i<cons.size(); i++){               //for every constraint
                    for(auto e: cdt.constrained_edges()){       //search all the constrained edges
                        Face_handle f = e.first;
                        int index = e.second;
                        //get the 2 endpoints of the edge
                        Point p1 = f->vertex(CDT::ccw(index))->point();
                        Point p2 = f->vertex(CDT::cw(index))->point();
                        //if the 2 endpoints of this edge are the points of the constraint, we have located the constrained
                        //edge and we can remove the constraint
                        if((p1 == cons[i].first && p2 == cons[i].second) || (p1 == cons[i].second && p2 == cons[i].first)){
                            cdt.remove_constraint(e.first,e.second);
                        }

                    }
                }
            }
            else polv.clear();          //if the polygon is not found clear the polygon point vector and prepare for new search 

        }
        if(found) break;                //if a polygon was found and all the necessary steps were taken we we exit the loop
    }
}