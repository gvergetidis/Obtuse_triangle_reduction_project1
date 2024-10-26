#include "CCDT.h"

bool obtuse_face(Face_handle face){
    Point a = face->vertex(0)->point();
    Point b = face->vertex(1)->point();
    Point c = face->vertex(2)->point();

    Angle A = angle(b,a,c);
    Angle B = angle(a,b,c);
    Angle C = angle(a,c,b);
    
    if(A == CGAL::OBTUSE) return true;
    if(B == CGAL::OBTUSE) return true;
    if(C == CGAL::OBTUSE) return true;

    return false;
}


int get_obtuse_ind(Face_handle facep){
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



Point get_projection(Face_handle facep){
    int obtuseInd = get_obtuse_ind(facep);
    Point p1 = facep->vertex(obtuseInd)->point();
    Point p2 = facep->vertex(CDT::ccw(obtuseInd))->point();
    Point p3 = facep->vertex(CDT::cw(obtuseInd))->point();
    Line l(p2,p3);
    Point proj = l.projection(p1);
    return proj;
}

Point get_middlepoint(Face_handle facep){
    int obtuseInd = get_obtuse_ind(facep);
    Point p1 = facep->vertex(obtuseInd)->point();
    Point p2 = facep->vertex(CDT::ccw(obtuseInd))->point();
    Point p3 = facep->vertex(CDT::cw(obtuseInd))->point();
    Point m = CGAL::midpoint(p2,p3);
    return m;
}

Point get_centroid(Face_handle facep){
    int obtuseInd = get_obtuse_ind(facep);
    Point p1 = facep->vertex(obtuseInd)->point();
    Point p2 = facep->vertex(CDT::ccw(obtuseInd))->point();
    Point p3 = facep->vertex(CDT::cw(obtuseInd))->point();
    Point ce = CGAL::centroid(p1,p2,p3);
    return ce;
}

Point get_circumcenter(Face_handle facep){
    int obtuseInd = get_obtuse_ind(facep);
    Point p1 = facep->vertex(obtuseInd)->point();
    Point p2 = facep->vertex(CDT::ccw(obtuseInd))->point();
    Point p3 = facep->vertex(CDT::cw(obtuseInd))->point();
    Point circ = CGAL::circumcenter(p1,p2,p3);
    return circ;
}

Point get_circumcenter_centroid(CDT cdt,Face_handle facep){
    Point steiner = get_circumcenter(facep);
    if(!cdt.point_in_region_boundary(steiner)){
        steiner = get_centroid(facep);
    }
    return steiner;
}

bool face_has_constraint(CDT cdt,Face_handle facep){
    Edge e1 = Edge(facep, 0);
    Edge e2 = Edge(facep, 1);
    Edge e3 = Edge(facep, 2);
    return (cdt.is_constrained(e1) || cdt.is_constrained(e2) || cdt.is_constrained(e3));
}

void polygon_center(CDT cdt){   
    bool found = false;
    std::vector <Point> polv;  
    for(auto fit = cdt.finite_faces_begin(); fit != cdt.finite_faces_end(); ++fit){
        if(obtuse_face(fit) && face_has_constraint(cdt , fit)){

            Face_handle neigh0 = fit->neighbor(0);

            polv.push_back(fit->vertex(1)->point());

            if(obtuse_face(neigh0) && (!cdt.is_infinite(neigh0)) && !face_has_constraint(cdt , neigh0)){
                std::cout<<"HI neig0"<<std::endl;
                polv.push_back(neigh0->vertex(neigh0->index(fit))->point());
                found = true;
            }

            polv.push_back(fit->vertex(2)->point());

            Face_handle neigh1 = fit->neighbor(1);
            if(obtuse_face(neigh1) && (!cdt.is_infinite(neigh1)) && !face_has_constraint(cdt , neigh1)){
                std::cout<<"HI neig1"<<std::endl;
                polv.push_back(neigh1->vertex(neigh1->index(fit))->point());
                found = true;

            }

            polv.push_back(fit->vertex(0)->point());

            Face_handle neigh2 = fit->neighbor(2);
            if(obtuse_face(neigh2) && (!cdt.is_infinite(neigh2)) && !face_has_constraint(cdt , neigh2)){
                std::cout<<"HI neig2"<<std::endl;
                polv.push_back(neigh2->vertex(neigh2->index(fit))->point());
                found = true;
            }
            
            Polygon_2 polyg;
            for(auto p : polv) {
                polyg.push_back(p);                     
            } 
            if(!polyg.is_convex()){
                found = false;
            }            
                    
            if (found){
                for(auto p : polv) {
                    //if(v->is_valid()) cdt.remove(v);
                    std::cout<<"("<<p.x()<<","<<p.y()<<") ";                        

                }
                std::cout<<std::endl;
                       

                        /*//remove duplicates
                        for (auto it = pol.begin(); it != pol.end(); ++it) {
                            auto next_it = it + 1;
                            while (next_it != pol.end()) {
                                if (*it == *next_it) {
                                    next_it = pol.erase(next_it);
                                    } else {
                                        ++next_it;
                                    }
                                }
                        }*/
                  
                
                for(auto p : polv) {
                    cdt.remove(cdt.insert(p));
                    std::cout<<"("<<p.x()<<","<<p.y()<<") ";                        
                }

                        
                Point p = CGAL::centroid(polv.begin(), polv.end(), CGAL::Dimension_tag<0>());
                cdt.insert_no_flip(p);

                std::vector<std::pair<Point,Point>> cons;

                for (size_t i = 0; i < polv.size(); ++i) {
                    const Point& p1 = polv[i];
                    const Point& p2 = polv[(i + 1) % polv.size()]; // Loop back to the first point
                    cons.push_back(std::make_pair(p1,p2));
                            
                }

                for(int i=0; i<cons.size(); i++) cdt.insert_constraint(cons[i].first, cons[i].second);

                CGAL::draw(cdt);

                //remove the constraints that were inserted
                for(int i=0; i<cons.size(); i++){
                    for(auto e: cdt.constrained_edges()){
                        Face_handle f = e.first;
                        int index = e.second;
                        Point p1 = f->vertex(CDT::ccw(index))->point();
                        Point p2 = f->vertex(CDT::cw(index))->point();
                        if((p1 == cons[i].first && p2 == cons[i].second) || (p1 == cons[i].second && p2 == cons[i].first)){
                            cdt.remove_constraint(e.first,e.second);
                        }

                    }
                }
            }
            else polv.clear();

        }
        if (found) break;

    }


}