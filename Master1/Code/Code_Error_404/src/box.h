#include <src/gKit/mat.h>
#include <src/gKit/vec.h>

#ifndef BOX_H
#define BOX_H
class Box {
public:
    Box();
    Box(const Point& pmin, const Point& pmax);

    bool collides(const Box& rhs);
    Point pmin, pmax;
    Transform T;
};

#endif
