#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/draw_triangulation_2.h>
#include <iostream>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Constrained_Delaunay_triangulation_2<K> CDT;
typedef CDT::Point Point;

int main() {
    CDT cdt;

    // Insert some constrained edges (a simple polygon)
    std::cout << "Inserting a polygon with constraints..." << std::endl;
    cdt.insert_constraint(Point(0, 0), Point(16, 0));
    cdt.insert_constraint(Point(16, 0), Point(4, 4));
    cdt.insert_constraint(Point(4, 4), Point(0, 4));
    cdt.insert_constraint(Point(0, 4), Point(0, 0));

    // Insert some internal points to create more triangles
    cdt.insert(Point(1, 1));
    cdt.insert(Point(2, 2));
    cdt.insert(Point(3, 1));

    std::cout << "Constrained Delaunay Triangulation completed." << std::endl;

    // Draw initial triangulation
    CGAL::draw(cdt);


    // CDT cdt;
    
    // std:: vector<Point> points = {
    //     Point(632,1588), Point(1330, 1097), Point(3051,470), Point(5040,1077),
    //     Point(5883,2766), Point(8130, 3629), Point(9280, 2836), Point(9613, 4963),
    //     Point(9422, 6363), Point(8996, 7327), Point(8020, 7611), Point(8467, 9720),
    //     Point(6735,9183), Point(4674,7865), Point(2519, 7692), Point(973,9797),
    //     Point(1205,6005), Point(1929, 5812), Point(3203, 6301),Point(5345,2923)
    // };

    // for(const Point& p :points){
    //     cdt.insert(p);
    // }

    // std::vector<std::pair<int,int>> constraints = {
    //     {3,4} ,{5,6}, {9,10},{10,11},{11,12},{12,13},{13,14},
    //     {14,15},{15,16},{18,19},{19,0}
    // };

    // for(const auto& constraint:constraints){
    //     cdt.insert_constraint(points[constraint.first], points[constraint.second]);
    // }


    // CGAL::draw(cdt);

    // // Iterate through finite edges and flip if possible
    for (auto e : cdt.finite_edges()) {
        // Check if the edge can be flipped
        if (cdt.is_constrained(e)) {
            std::cout << "Im in" << std::endl;
            // cdt.flip(e.first,e.second);  // Perform the flip operation
        }
    }

    std::cout << "Edge flipping completed." << std::endl;

    // Draw the triangulation after edge flipping
    CGAL::draw(cdt);
    return 0;
}