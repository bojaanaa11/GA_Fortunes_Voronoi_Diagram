#ifndef MY_DCEL_H
#define MY_DCEL_H

#include<vector>
#include<QPointF>

namespace my_dcel
{

class Vertex;
class HalfEdge;
class Face;

class DCEL
{
public:
    DCEL();
    ~DCEL();

    std::vector<Vertex *> _vertices;
    std::vector<HalfEdge *> _halfedges;
    std::vector<Face *> _faces;
};

class Vertex
{
public:
    Vertex(QPointF coordinates);
    ~Vertex();

    QPointF _coordinates;
    HalfEdge *_incident_halfedge = nullptr; ; // The edge that points towards this vertex [-->O]
};

class HalfEdge
{
public:
    HalfEdge(QPointF start);
    ~HalfEdge();

    QPointF _start;
    Vertex *_origin = nullptr;
    HalfEdge *_twin = nullptr;
    HalfEdge *_next = nullptr;
    HalfEdge *_previous = nullptr;
    Face *_incident_face = nullptr;
};

class Face{
public:
    Face();
    ~Face();

    HalfEdge *_outer_component = nullptr;
    std::vector<HalfEdge *> _inner_components;
};

}

#endif // MY_DCEL_H
