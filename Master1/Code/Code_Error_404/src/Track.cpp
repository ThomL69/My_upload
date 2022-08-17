#include "Track.h"
#include <Tools.h>

// Code from https://en.wikipedia.org/wiki/Centripetal_Catmull%E2%80%93Rom_spline
float GetT(const float t, const float alpha, const Point& p0, const Point& p1) {
    auto d = p1 - p0;
    float a = dot(d, d);
    float b = std::pow(a, alpha * .5f);
    return (b + t);
}

// Code from https://en.wikipedia.org/wiki/Centripetal_Catmull%E2%80%93Rom_spline
Point CatMullRom(const Point& p0, const Point& p1, const Point& p2, const Point& p3, float t, float alpha = 0.5f) {
    float t0 = 0.0f;
    float t1 = GetT(t0, alpha, p0, p1);
    float t2 = GetT(t1, alpha, p1, p2);
    float t3 = GetT(t2, alpha, p2, p3);
    t = lerp(t1, t2, t);
    Point A1 = (t1 - t) / (t1 - t0) * p0 + (t - t0) / (t1 - t0) * p1;
    Point A2 = (t2 - t) / (t2 - t1) * p1 + (t - t1) / (t2 - t1) * p2;
    Point A3 = (t3 - t) / (t3 - t2) * p2 + (t - t2) / (t3 - t2) * p3;
    Point B1 = (t2 - t) / (t2 - t0) * A1 + (t - t0) / (t2 - t0) * A2;
    Point B2 = (t3 - t) / (t3 - t1) * A2 + (t - t1) / (t3 - t1) * A3;
    Point C = (t2 - t) / (t2 - t1) * B1 + (t - t1) / (t2 - t1) * B2;
    return C;
}

// t [0-1]
const Point Track::getPosition(const float t) const {
    return CatMullRom(
        t_path[0],
        t_path[1],
        t_path[2],
        t_path[3],
        t);
}

// t [0-1]
const Vector Track::getNormal(const float t) const {
    int id = (int)(t_normals.size() * t);
    return t_normals[id];
}

void Track::buildPath2(const vector<Point>& path, const int precision, bool debug) {
    t_path = path;
    // boucle pour générer notre chemin
    // puissance de 2 pour avoir une "precision" qui à la même logique qu'avec subdivision
    int nbpoint = pow(2, precision);
    for (int i = 0; i <= nbpoint; i++) {
        Point p = CatMullRom(
                path[0],
                path[1],
                path[2],
                path[3],
                ((float)i) / ((float)nbpoint));
        t_points.push_back(p);
    }

    // fait une approximation de la longueur du chemin
    for (int i = 1; i < t_points.size(); i++) {
        t_length += length(t_points[i] - t_points[i - 1]);
    }

    if (debug) {
        // construction des segments pour la subdivision n
        for (int j = 0; j < t_points.size() - 1; ++j) {
            this->t_curve.vertex(t_points[j]);
            this->t_curve.vertex(t_points[j + 1]);
        }
    }
}

// Pas utilisé dans le projet car provoque des artéfacts lors de la "fusion" des tubes
void Track::buildPath(const vector<Point>& path, const int precision, bool debug) {
    this->t_points = path;

    // boucle de subdivision
    std::vector<Point> temps;
    Point p_temp;
    Vector v_temp;
    for (int i = 0; i < precision; i++) {
        temps.push_back(t_points[0]);
        for (int j = 0; j < t_points.size() - 1; ++j) {
            v_temp = t_points[j + 1] - t_points[j];
            p_temp = t_points[j] + v_temp * 0.25;
            temps.push_back(p_temp);
            p_temp = t_points[j] + v_temp * 0.75;
            temps.push_back(p_temp);
        }
        temps.push_back(t_points[t_points.size()-1]);
        t_points = temps;
        temps.clear();
    }

    if (debug) {
        // construction des segments pour la subdivision n
        for (int j = 0; j < t_points.size() - 1; ++j) {
            this->t_curve.vertex(t_points[j]);
            this->t_curve.vertex(t_points[j + 1]);
        }
    }
}

void Track::buildNormals(Vector firstNormal, const float c_size, bool debug) {

    Vector a;
    Vector b;
    Transform r;

    //première normale
    Vector normal = Vector(firstNormal);

    t_normals.push_back(normal);

    if (debug) {
        t_curve.vertex(t_points[0]);
        t_curve.vertex(t_points[0] + normal * (c_size + 3));
    }

    for (int p = 1; p < t_points.size(); ++p) {
        // rotation de la normale le long du tube
        if (p < t_points.size() - 1) {
            //calcul de la normale
            a = t_points[p] - t_points[p - 1];
            b = t_points[p + 1] - t_points[p];
            r = Rotation(a, b);
            normal = normalize(r(normal));
        }

        t_normals.push_back(normal);

        if (debug) {
            t_curve.vertex(t_points[p]);
            t_curve.vertex(t_points[p] + normal * (c_size + 3));
        }
    }
}



void Track::buildCylinder(const int nb_sides, const float c_size, bool debug) {
   
    // Construction du mesh
    std::vector<Vector> c_vertices;
    std::vector<Vector> c_verticesPred;
    Transform r;
    Vector tmp;
    for(int n = 0; n < t_normals.size(); n++) {

        tmp = t_normals[n];
        c_vertices.push_back(Vector(t_points[n].x, t_points[n].y, t_points[n].z) + tmp * c_size);

        if (n == 0) {
            r = Rotation(normalize(t_points[1] - t_points[0]), 360 / nb_sides);
        }
        else {
            r = Rotation(normalize(t_points[n] - t_points[n - 1]), 360 / nb_sides);
        }
        // construction d'un cercle du tube
        for (int c = 1; c <= nb_sides; ++c) {
            tmp = r(tmp);
            c_vertices.push_back(Vector(t_points[n].x, t_points[n].y, t_points[n].z) + tmp * c_size);
            
            // affichage pour le sol
            t_edges.vertex(c_vertices[c % nb_sides]);
            t_edges.vertex(c_vertices[c - 1]);
        }

        if (n > 0) {
            // prend le cercle courant et précédent pour construire un morceau (un cylindre) du tube 
            for (int c = 0; c < nb_sides; ++c) {
                int next = (c + 1) % nb_sides;
                t_cylinder.vertex(c_verticesPred[c]);
                t_cylinder.vertex(c_vertices[next]);
                t_cylinder.vertex(c_vertices[c]);

                t_cylinder.vertex(c_verticesPred[c]);
                t_cylinder.vertex(c_verticesPred[next]);
                t_cylinder.vertex(c_vertices[next]);

            }
        }
        
        c_verticesPred = c_vertices;
        c_vertices.clear();
    }
    c_vertices.clear();
    c_verticesPred.clear();
}



Track::Track(const vector<Point>& path, Vector firstNormal, const int precision, const int nb_sides, const float c_size, bool debug) {
    this->t_length = 0;

    this->t_curve = Mesh(GL_LINES); // normals + path
    this->t_curve.color(1, 0, 0);

    this->t_edges = Mesh(GL_LINES); // edges + circles
    this->t_edges.color(0, 0.5, 0);

    this->t_cylinder = Mesh(GL_TRIANGLES); // tube
    this->t_cylinder.color(0, 0, 0);

    buildPath2(path, precision, debug);
    buildNormals(firstNormal, c_size, debug);
    buildCylinder(nb_sides, c_size, debug);
}


void Track::draw_cylinder(Camera* camera) {
    draw(t_cylinder, Identity(), camera->T(), camera->proj());
}
void Track::draw_edges(Camera* camera) {
    draw(t_edges, Identity(), camera->T(), camera->proj());
}
void Track::draw_curve(Camera* camera) {
    draw(t_curve, Identity(), camera->T(), camera->proj());
}
void Track::draw_zones(Camera* camera) {
    for each (Zone* d in t_zones)
    {
        d->draw_zone(camera);
    }
}

Point Track::getLastPostion() {
    return t_points[t_points.size() - 1];
}

Vector Track::getLastNormal() {
    return t_normals[t_normals.size() - 1];
}

const float Track::track_length() const {
    return  t_length;
}

const int Track::size() const {
    return t_points.size();
}

void Track::addZone(Zone* zone) {
    t_zones.push_back(zone);
}

Track::~Track() {
    t_path.clear();
    t_points.clear();
    t_normals.clear();
    for each (Zone * d in t_zones)
        delete d;
    t_zones.clear();
    t_cylinder.clear();
    t_edges.clear();
    t_curve.clear();
}