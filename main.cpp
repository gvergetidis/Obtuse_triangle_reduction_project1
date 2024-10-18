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


int main() {
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
    for (auto e : cdt.finite_edges()) {
        // Check if the edge can be flipped
        if (cdt.is_flipable(e.first,e.second)) {
            std::cout << "Flipped edge" << std::endl;
            cdt.flip(e.first,e.second);  // Perform the flip operation
        }
    }
    std::cout << "Edge flipping completed." << std::endl;

    //Steiner points in the middle of the edge
    typedef CDT::Point Point;
    typedef CDT::Face Face;
    typedef CDT::Face_iterator Face_iterator;
    typedef CGAL::Angle Angle;

    Face_iterator it = cdt.finite_faces_begin();
    Face_iterator beyond = cdt.finite_faces_end();
    Face face;
    typedef CDT::Face_handle Face_handle;
    Face_handle neighbor;
    typedef CDT::Vertex_handle Vertex_handle;
    std:: vector<Point> midpoints= {};
    std:: vector<Point> circs = {};
    std:: vector<Point> centers = {};
    std:: vector<Point> projs = {};
    std:: vector<Edge> edges= {};
    int c1 = 0;
    int c2 = 0;
    
    
    // c1=0;
    // it = cdt.finite_faces_begin();
    // beyond = cdt.finite_faces_end();
    // while (it != beyond) {
    //     face = *it;  // get face
    //     Point a = face.vertex(0)->point();
    //     Point b = face.vertex(1)->point();
    //     Point c = face.vertex(2)->point();
        
    //     // std::cout <<"FACE"<< std::endl; 
    //     // std::cout << a << std::endl; 
    //     // std::cout << b << std::endl; 
    //     // std::cout << c << std::endl; 

    //     Angle A = angle(b,a,c);
    //     Angle B = angle(a,b,c);
    //     Angle C = angle(a,c,b);

    //     // std::cout << A << std::endl;
    //     // std::cout << B << std::endl;
    //     // std::cout << C << std::endl;

    //     if(A== CGAL::OBTUSE){
    //         std::cout << "A" << std::endl;
    //         c1++;
    //     }
    //     if(B== CGAL::OBTUSE){
    //         std::cout << "B" << std::endl;
    //         c1++;
    //     }
    //     if(C== CGAL::OBTUSE){
    //         std::cout << "C" << std::endl;
    //         c1++;
    //     }
    //     ++it;        // advance the iterator
    // }
    // std::cout <<"Before"<< c1 << std::endl; 







    // int limit = 10;
    // for(int i=0; i<limit ;i++){
    //     projs = {};
    //     std::cout<<"----------------------------------\n";
    //     it = cdt.finite_faces_begin();
    //     beyond = cdt.finite_faces_end();
    //     std::vector<std::tuple<Face_handle, Face_handle,Edge,Point>> polyinfo = {};
    //     while (it != beyond) {
    //         bool obtuse = false;
    //         std::cout<<"hiiiiiiiiiiiiiiiii\n";
    //         face = *it;  // get face
    //         Face_handle facep = it;
    //         Point a = face.vertex(0)->point();
    //         Point b = face.vertex(1)->point();
    //         Point c = face.vertex(2)->point();

    //         Vertex_handle va = face.vertex(0);
    //         Vertex_handle vb = face.vertex(1);
    //         Vertex_handle vc = face.vertex(2);

            
    //         std::cout <<"FACE"<< std::endl; 
    //         std::cout << a << std::endl; 
    //         std::cout << b << std::endl; 
    //         std::cout << c << std::endl; 

    //         Angle A = angle(b,a,c);
    //         Angle B = angle(a,b,c);
    //         Angle C = angle(a,c,b);
    //         if(A== CGAL::OBTUSE){
    //             obtuse = true;
  

    //             std::cout << "A" << std::endl;
    //             Face_handle neigh = face.neighbor(0);

    //             Line l(b,c);
    //             Point proj = project_point(b,c,a);
    //             //cdt.insert_no_flip(proj);
    //             //std::cout<<"inserted point "<<proj<<std::endl;
    //             projs.push_back(proj);


                
    //         }
    //         else if(B== CGAL::OBTUSE){
    //             obtuse = true;

                
    //             std::cout << "B" << std::endl;
    //             Face_handle neigh = face.neighbor(0);

    //             Line l(a,c);
    //             Point proj = project_point(a,c,b);//l.projection(b);
    //             //cdt.insert_no_flip(proj);
    //             //std::cout<<"inserted point "<<proj<<std::endl;
    //             projs.push_back(proj);

                
    //         }
    //         else if(C== CGAL::OBTUSE){
    //             obtuse = true;

    //             std::cout << "C" << std::endl;
    //             Face_handle neigh = face.neighbor(0);

    //             Line l(a,b);
    //             Point proj = project_point(a,b,c);//l.projection(c);
    //             //cdt.insert_no_flip(proj);
    //             //std::cout<<"inserted point "<<proj<<std::endl;
    //             projs.push_back(proj);
                
    //         }
    //         /*if(obtuse){
    //             break;
    //         }*/
           
    //         ++it;        // advance the iterator
    //     }
    //     for(auto p : projs) cdt.insert_no_flip(p);
    // }


    // c2=0;
    // it = cdt.finite_faces_begin();
    // beyond = cdt.finite_faces_end();
    // while (it != beyond) {
    //     face = *it;  // get face
    //     Point a = face.vertex(0)->point();
    //     Point b = face.vertex(1)->point();
    //     Point c = face.vertex(2)->point();
        
         

    //     Angle A = angle(b,a,c);
    //     Angle B = angle(a,b,c);
    //     Angle C = angle(a,c,b);

    //     // std::cout << A << std::endl;
    //     // std::cout << B << std::endl;
    //     // std::cout << C << std::endl;

    //     if(A== CGAL::OBTUSE){
    //         /*std::cout <<"FACE"<< std::endl; 
    //         std::cout << a << std::endl; 
    //         std::cout << b << std::endl; 
    //         std::cout << c << std::endl;
    //         std::cout << "A" << std::endl;*/
    //         c2++;
    //     }
    //     if(B== CGAL::OBTUSE){
    //         /*std::cout <<"FACE"<< std::endl; 
    //         std::cout << a << std::endl; 
    //         std::cout << b << std::endl; 
    //         std::cout << c << std::endl;
    //         std::cout << "B" << std::endl;*/
    //         c2++;
    //     }
    //     if(C== CGAL::OBTUSE){
    //         /*std::cout <<"FACE"<< std::endl; 
    //         std::cout << a << std::endl; 
    //         std::cout << b << std::endl; 
    //         std::cout << c << std::endl;
    //         std::cout << "C" << std::endl;*/
    //         c2++;
    //     }
    //     ++it;        // advance the iterator
    // }
    // std::cout <<"After"<< c2 << std::endl; 

    // CGAL::draw(cdt);



    // return 0;



    //Loop with iterations
    int limit = 100;
    for(int i=0; i<limit ;i++){
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
                // Point centroid = CGAL::centroid(a,b,c); method2
                // Point m= CGAL::midpoint(b,c); method1

                std::cout << "A" << std::endl;
                Face_handle neigh = face.neighbor(0);

                /*bool exists = std::any_of(polyinfo.begin(), polyinfo.end(), [facep,neigh](const auto& t) {
                return std::get<0>(t) == facep || std::get<1>(t) == facep || std::get<0>(t) == neigh || std::get<1>(t) == neigh;
                });

                if(exists){
                    it++;
                    continue;
                }*/

                if(!cdt.is_infinite(neigh)){ //check if neighbour is finite
                    Point d = neigh->vertex(neigh->index(facep))->point(); //get the opposite of a the point d
                    //points of neighbour
                    Point na = neigh->vertex(0)->point();
                    Point nb = neigh->vertex(1)->point();
                    Point nc = neigh->vertex(2)->point();
                    //get the centroid of the polygon abcd
                    Point polygon_centre = CGAL::centroid(a,b,c,d);

                    //Check if the polygon_centre is in the edge (the 3 points are collinear)
                    if(!CGAL::collinear(b,polygon_centre,c)){ //the 3 points are not collinear
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
                            obtuse=false;
                            it++;
                            continue;
                        }
                    }
                    else{ //the 3 points are collinear
                        std::cout<<"Collinear"<< std::endl;
                        obtuse=false;
                        it++;
                        continue;
                    }
                    // centers.push_back(polygon_centre);
                    //std::cout << m << std::endl;
                    // midpoints.push_back(m);
                    // circs.push_back(centroid);
                }
                else{
                    std::cout<<"Infinite neighbour"<<std::endl;
                    obtuse=false;
                    it++;
                    continue;
                }
                c1++;
            }
            if(B== CGAL::OBTUSE){
                obtuse = true;
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

                /*bool exists = std::any_of(polyinfo.begin(), polyinfo.end(), [facep,neigh](const auto& t) {
                return std::get<0>(t) == facep || std::get<1>(t) == facep || std::get<0>(t) == neigh || std::get<1>(t) == neigh;
                });

                if(exists){
                    it++;
                    continue;
                }*/

                if(!cdt.is_infinite(neigh)){

                    Point d = neigh->vertex(neigh->index(facep))->point();
                    Point na = neigh->vertex(0)->point();
                    Point nb = neigh->vertex(1)->point();
                    Point nc = neigh->vertex(2)->point();

                    Point polygon_centre = CGAL::centroid(a,b,c,d);
                    //Check if the polygon_centre is in the edge (the 3 points are collinear
                    if(!CGAL::collinear(a,polygon_centre,c)){ //the 3 points are not collinear
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
                            cdt.insert_no_flip(polygon_centre);
                            if(!cdt.is_constrained(edge)) cdt.flip(edge.first , edge.second);
                        
                            //polyinfo.push_back(make_tuple(facep,neigh,edge,polygon_centre));

                        }
                        //else if(na==af && nb==bf && nc==cf){  //case 2 the polygon center is in the new triangle
                        else if(f == neigh){
                            std::cout<<"IN tringle2"<< std::endl;
                            // cdt.insert_no_flip(polygon_centre);
                            // Edge edge(facep, 1);
                            // cdt.flip(edge.first , edge.second);

                            Edge edge(facep, 1);
                            cdt.insert_no_flip(polygon_centre);
                            if(!cdt.is_constrained(edge)) cdt.flip(edge.first , edge.second);
                            //polyinfo.push_back(make_tuple(facep,neigh,edge,polygon_centre));

                        }else{ //case 3 polygon center is outside of the polygon 
                            std::cout<<"Outside"<< std::endl;
                            obtuse=false;
                            it++;
                            continue;
                        }   
                        
                    }
                    else{
                        std::cout<<"Collinear"<< std::endl;
                        obtuse=false;
                        it++;
                        continue;
                    }
                    centers.push_back(polygon_centre);
                    midpoints.push_back(m);
                    circs.push_back(centroid);
                }
                else{
                    std::cout<<"Infinite neighbour"<<std::endl;
                    obtuse=false;
                    it++;
                    continue;
                }
                c1++;
            }
            if(C== CGAL::OBTUSE){
                obtuse = true;
                // Point centroid = CGAL::centroid(a,b,c);
                // Face_handle f = cdt.locate(p);
                // Point af = f->vertex(0)->point();
                // Point bf = f->vertex(1)->point();
                // Point cf = f->vertex(2)->point();
                // std::cout <<"FACE THAT IS THE P"<< std::endl; 
                // std::cout << af << std::endl; 
                // std::cout << bf << std::endl; 
                // std::cout << cf << std::endl; 
                std::cout << "C" << std::endl;
                // Point m= CGAL::midpoint(b,a);

                Face_handle neigh = face.neighbor(2);

                /*bool exists = std::any_of(polyinfo.begin(), polyinfo.end(), [facep,neigh](const auto& t) {
                return std::get<0>(t) == facep || std::get<1>(t) == facep || std::get<0>(t) == neigh || std::get<1>(t) == neigh;
                });

                if(exists){
                    it++;
                    continue;
                }*/

                if(!cdt.is_infinite(neigh)){
                    Point d = neigh->vertex(neigh->index(facep))->point();
                    Point na = neigh->vertex(0)->point();
                    Point nb = neigh->vertex(1)->point();
                    Point nc = neigh->vertex(2)->point();

                    Point polygon_centre = CGAL::centroid(a,b,c,d);
                    centers.push_back(polygon_centre);
                    //Check if the polygon_centre is in the edge (the 3 points are collinear
                    if(!CGAL::collinear(a,polygon_centre,b)){ //the 3 points are not collinear
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
                            cdt.insert_no_flip(polygon_centre);
                            if(!cdt.is_constrained(edge)) cdt.flip(edge.first , edge.second);
                            //polyinfo.push_back(make_tuple(facep,neigh,edge,polygon_centre));

                        }
                        //else if(na==af && nb==bf && nc==cf){  //case 2 the polygon center is in the new triangle
                        else if(f == neigh){
                            std::cout<<"IN tringle2"<< std::endl;
                            // cdt.insert_no_flip(polygon_centre);
                            // Edge edge(facep, 2);
                            // cdt.flip(edge.first , edge.second);

                            Edge edge(facep, 2);
                            cdt.insert_no_flip(polygon_centre);
                            if(!cdt.is_constrained(edge)) cdt.flip(edge.first , edge.second);
                            //polyinfo.push_back(make_tuple(facep,neigh,edge,polygon_centre));

                        }else{ //case 3 polygon center is outside of the polygon 
                            std::cout<<"Outside"<< std::endl;
                            obtuse=false;
                            it++;
                            continue;
                        }   
                        
                    }
                    else{
                        std::cout<<"Collinear"<< std::endl;
                        obtuse=false;
                        it++;
                        continue;
                    }
                    centers.push_back(polygon_centre);
                    // std::cout << m << std::endl;
                    // midpoints.push_back(m);
                    // circs.push_back(centroid);
                    c1++;
                }else{
                    std::cout<<"Infinite neighbour"<<std::endl;
                    obtuse=false;
                    it++;
                    continue;
                }
            }
            std::cout<<"------------------"<< std::endl;
            if(obtuse){
                break;
            }
            ++it;        // advance the iterator
        }
        std::cout<<"-------------------------------------NUM:" << polyinfo.size()<<std::endl;
        /*for(const auto& t :polyinfo){
            Face_handle f1 = std::get<0>(t);
            Face_handle f2 = std::get<1>(t);

            Edge e = std::get<2>(t);
            Point p = std::get<3>(t);
            if(!cdt.is_infinite(f1) && !cdt.is_infinite(f2) && !cdt.is_constrained(e)) {
                cdt.insert_no_flip(p);
                cdt.flip(e.first , e.second);
            }
        }*/
        /*if(!cdt.is_valid()){
            std::cout<<"--------NOT VALID----------\n";
            break;
        }*/
    
    }
    // for(const Point& m :midpoints){
    //     cdt.insert(m);
    // }
    // cdt.insert_no_flip(midpoints.at(0));

    // for(const Point& c :circs){
    //     cdt.insert_no_flip(c);
    // }
    ///cdt.insert_no_flip(circs.at(0));
    // cdt.insert_no_flip(centers.at(0));

    




    c2=0;
    it = cdt.finite_faces_begin();
    beyond = cdt.finite_faces_end();
    while (it != beyond) {
        face = *it;  // get face
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
            /*std::cout <<"FACE"<< std::endl; 
            std::cout << a << std::endl; 
            std::cout << b << std::endl; 
            std::cout << c << std::endl;
            std::cout << "A" << std::endl;*/
            c2++;
        }
        if(B== CGAL::OBTUSE){
            /*std::cout <<"FACE"<< std::endl; 
            std::cout << a << std::endl; 
            std::cout << b << std::endl; 
            std::cout << c << std::endl;
            std::cout << "B" << std::endl;*/
            c2++;
        }
        if(C== CGAL::OBTUSE){
            /*std::cout <<"FACE"<< std::endl; 
            std::cout << a << std::endl; 
            std::cout << b << std::endl; 
            std::cout << c << std::endl;
            std::cout << "C" << std::endl;*/
            c2++;
        }
        ++it;        // advance the iterator
    }
    std::cout <<"After"<< c2 << std::endl; 

    CGAL::draw(cdt);



	
    // // Create a property tree to hold the parsed JSON
    // pt::ptree root;

    // // Example JSON string
    // std::string json_data = R"(
    // {
    //     "name": "John",
    //     "age": 30,
    //     "is_student": false,
    //     "courses": {
    //         "math": "A",
    //         "science": "B"
    //     },
    //     "hobbies": ["reading", "hiking", "coding"]
    // })";

    // // Read the JSON string into the property tree
    // std::istringstream json_stream(json_data);
    // pt::read_json(json_stream, root);

    // // Accessing simple data
    // std::string name = root.get<std::string>("name");
    // int age = root.get<int>("age");
    // bool is_student = root.get<bool>("is_student");

    // // Accessing nested objects
    // std::string math_grade = root.get<std::string>("courses.math");
    // std::string science_grade = root.get<std::string>("courses.science");

    // // Accessing arrays
    // for (const auto& hobby : root.get_child("hobbies")) {
    //     std::cout << "Hobby: " << hobby.second.get_value<std::string>() << std::endl;
    // }

    // // Printing parsed data
    // std::cout << "Name: " << name << "\nAge: " << age << "\nIs Student: " << std::boolalpha << is_student << std::endl;
    // std::cout << "Math Grade: " << math_grade << "\nScience Grade: " << science_grade << std::endl;


    return 0;
}
