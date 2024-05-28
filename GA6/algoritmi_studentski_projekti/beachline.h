#ifndef BEACHLINE_H
#define BEACHLINE_H

#include <QPointF>
#include <vector>
#include "math_utils.h"
#include <iostream>
#include <QPainter>
#include <QPainterPath>
#include "my_dcel.h"

namespace beachline
{

class BeachlineElement;

enum event_type {
    SITE = 0,
    CIRCLE = 1,
    NONE = 2,
};

class Event
{
public:
    Event();
    Event(QPointF site);
    Event(QPointF lowest_point, QPointF left_focus, QPointF right_focus, QPointF center, beachline::BeachlineElement* squeezed_arc);
    ~Event();

    qreal _y;
    int _event_type = event_type::NONE;

    /* Attributes for site event */
    QPointF _site;

    /* Attributes for circle event */
    QPointF _lowest_point;
    QPointF _center;
    QPointF _left_focus;
    QPointF _right_focus;
    beachline::BeachlineElement* _squeezed_arc = nullptr;
};

struct EventComparator
{
    bool operator()(const Event* e1, const Event* e2)
    {
        return e1->_y < e2->_y;
    }
};

class Arc
{
public:
    Arc(QPointF focus) : _focus(focus) {}

    QPointF _focus;
};

class Breakpoint
{
public:
    Breakpoint(QPointF first_focus, QPointF second_focus) : _first_focus(first_focus), _second_focus(second_focus) {}

    QPointF _first_focus;
    QPointF _second_focus;
};

class BeachlineElement
{
public:
    BeachlineElement(Arc* arc);
    BeachlineElement(Breakpoint* breakpoint);
    ~BeachlineElement();

    Arc* _arc;
    BeachlineElement* _next = nullptr;
    BeachlineElement* _previous = nullptr;

    Breakpoint* _breakpoint;

    my_dcel::HalfEdge* _left_halfedge = nullptr;
    my_dcel::HalfEdge* _right_halfedge = nullptr;

    Event* _circle_event = nullptr;

    BeachlineElement* _left = nullptr;
    BeachlineElement* _right = nullptr;
    BeachlineElement* _parent = nullptr;
};

class Beachline
{
public:
    my_dcel::DCEL* voronoi_diagram;

    Beachline();
    ~Beachline();

    bool empty()
    {
        return root == nullptr;
    }

    void insert_new_root(Arc* arc)
    {
        root = new BeachlineElement(arc);
    }

    BeachlineElement* find_arc_above(QPointF arc, qreal sweepline)
    {
        return _find_arc_above(root, arc, sweepline);
    }

    void add_subtree(BeachlineElement* new_arc, BeachlineElement* node)
    {
        _add_subtree(new_arc, node);
    }

    void remove(BeachlineElement* node, qreal sweepline) {
        _remove(node, sweepline);
    }

private:
    BeachlineElement* root = nullptr;

    BeachlineElement* _find_arc_above(BeachlineElement* node, QPointF arc, qreal sweepline);
    void _add_subtree(BeachlineElement* new_arc, BeachlineElement* node);
    void _remove(BeachlineElement* node, qreal sweepline);
};

}

#endif // BEACHLINE_H
