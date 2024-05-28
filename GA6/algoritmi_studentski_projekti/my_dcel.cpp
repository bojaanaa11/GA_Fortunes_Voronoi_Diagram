#include "my_dcel.h"

my_dcel::Vertex::Vertex(QPointF coordinates) : _coordinates(coordinates) {}

my_dcel::Vertex::~Vertex() {}

my_dcel::DCEL::DCEL() {}

my_dcel::DCEL::~DCEL() {}


my_dcel::HalfEdge::HalfEdge(QPointF start) : _start(start) {}
