#include "beachline.h"

beachline::Event::Event() {}

beachline::Event::Event(QPointF site) : _site(site)
{
    _y = site.y();
    _event_type = event_type::SITE;
}

beachline::Event::Event(QPointF lowest_point, QPointF left_focus, QPointF right_focus, QPointF center, beachline::BeachlineElement* squeezed_arc)
{
    _lowest_point = lowest_point;
    _y = lowest_point.y();
    _event_type = event_type::CIRCLE;
    _left_focus = left_focus;
    _right_focus = right_focus;
    _center = center;
    _squeezed_arc = squeezed_arc;
}

beachline::Event::~Event() {}

beachline::BeachlineElement::BeachlineElement(Arc *arc) : _arc(arc), _breakpoint(nullptr) {}

beachline::BeachlineElement::BeachlineElement(Breakpoint *breakpoint) : _arc(nullptr), _breakpoint(breakpoint) {}

beachline::BeachlineElement::~BeachlineElement() {}

beachline::Beachline::Beachline() {}

beachline::Beachline::~Beachline() {}

beachline::BeachlineElement *beachline::Beachline::_find_arc_above(BeachlineElement *node, QPointF arc, double sweepline)
{
    if(node->_breakpoint) {
        QPointF intersection = find_intersection_parabolas(node->_breakpoint->_first_focus, node->_breakpoint->_second_focus, sweepline);

        if(arc.x() > intersection.x()) {
            return _find_arc_above(node->_right, arc, sweepline);
        } else {
            return _find_arc_above(node->_left, arc, sweepline);
        }
    }

    if(node->_arc) {
        QPointF intersect = find_intersection_parabolas(node->_arc->_focus, arc, sweepline);

        QPointF int_left;
        bool has_left = false;
        if(node->_previous) {
            has_left = true;
            int_left = find_intersection_parabolas(node->_previous->_arc->_focus, arc, sweepline);
        }

        QPointF int_right;
        bool has_right = false;
        if(node->_next) {
            has_right = true;
            int_right = find_intersection_parabolas(arc, node->_next->_arc->_focus, sweepline);
        }

        if(has_left && has_right) {
            bool left_is_closer = false;
            if(int_left.y() < int_right.y()) {
                left_is_closer = true;
            }

            if(left_is_closer) {
                if(int_left.y() < intersect.y()) {
                    return node->_previous;
                } else {
                    return node;
                }
            } else {
                if(int_right.y() < intersect.y()) {
                    return node->_next;
                } else {
                    return node;
                }
            }
        } else if(!has_left && has_right) {
            if(int_right.y() < intersect.y()) {
                return node->_next;
            } else {
                return node;
            }
        } else if(has_left && !has_right) {
            if(int_left.y() < intersect.y()) {
                return node->_previous;
            } else {
                return node;
            }
        } else {
            return node;
        }
    }

    return node;
}

void beachline::Beachline::_add_subtree(BeachlineElement* new_arc, BeachlineElement *node)
{
    QPointF new_focus = new_arc->_arc->_focus;
    QPointF focus = node->_arc->_focus;

    BeachlineElement* first_breakpoint = new BeachlineElement(new Breakpoint(focus, new_focus));
    BeachlineElement* second_breakpoint = new BeachlineElement(new Breakpoint(new_focus, focus));
    BeachlineElement* old_arc_1 = new BeachlineElement(new Arc(focus));
    BeachlineElement* old_arc_2 = new BeachlineElement(new Arc(focus));

    BeachlineElement* node_parent = node->_parent;
    if(!node->_parent) {
        root = first_breakpoint;
    } else {
        if(node_parent->_left == node) {
            node_parent->_left = first_breakpoint;
            first_breakpoint->_parent = node_parent;
        } else if(node_parent->_right == node) {
            node_parent->_right = first_breakpoint;
            first_breakpoint->_parent = node_parent;
        }
    }

    first_breakpoint->_left = old_arc_1;
    first_breakpoint->_right = second_breakpoint;

    second_breakpoint->_parent = first_breakpoint;
    second_breakpoint->_left = new_arc;
    second_breakpoint->_right = old_arc_2;

    old_arc_1->_previous = node->_previous;
    old_arc_2->_next = node->_next;
    if(node->_previous) node->_previous->_next = old_arc_1;
    if(node->_next) node->_next->_previous = old_arc_2;

    old_arc_1->_next = new_arc;
    old_arc_1->_parent = first_breakpoint;

    old_arc_2->_previous = new_arc;
    old_arc_2->_parent = second_breakpoint;

    new_arc->_parent = second_breakpoint;
    new_arc->_previous = old_arc_1;
    new_arc->_next = old_arc_2;

    if(node->_left_halfedge) {
        old_arc_1->_left_halfedge = new my_dcel::HalfEdge(node->_left_halfedge->_start);
        voronoi_diagram->_halfedges.push_back(old_arc_1->_left_halfedge);
    }

    if(node->_right_halfedge) {
        old_arc_2->_right_halfedge = new my_dcel::HalfEdge(node->_right_halfedge->_start);
        voronoi_diagram->_halfedges.push_back(old_arc_2->_right_halfedge);
    }
}

void beachline::Beachline::_remove(BeachlineElement *node, qreal sweepline)
{
    /* If element is not already squeezed */
    if(node) {
        if(node->_parent != root) {
            BeachlineElement* parent = node->_parent;
            BeachlineElement* grandparent = parent->_parent;
            BeachlineElement* previous = node->_previous;
            BeachlineElement* next = node->_next;

            previous->_next = next;
            next->_previous = previous;

            BeachlineElement* new_grandparent_child = nullptr;
            if(parent->_left == node) {
                new_grandparent_child = parent->_right;
            } else if(parent->_right == node) {
                new_grandparent_child = parent->_left;
            }

            if(grandparent->_left == parent) {
                grandparent->_left = new_grandparent_child;
                new_grandparent_child->_parent = grandparent;

                if(!(grandparent->_breakpoint->_first_focus == parent->_breakpoint->_second_focus && grandparent->_breakpoint->_second_focus == parent->_breakpoint->_first_focus)) {
                    if(parent->_breakpoint->_second_focus == node->_arc->_focus)
                        grandparent->_breakpoint->_first_focus = parent->_breakpoint->_first_focus;
                    else
                        grandparent->_breakpoint->_first_focus = parent->_breakpoint->_second_focus;
                }

            } else if(grandparent->_right == parent) {
                grandparent->_right = new_grandparent_child;
                new_grandparent_child->_parent = grandparent;

                if(!(grandparent->_breakpoint->_first_focus == parent->_breakpoint->_second_focus && grandparent->_breakpoint->_second_focus == parent->_breakpoint->_first_focus)) {
                    if(parent->_breakpoint->_first_focus == node->_arc->_focus)
                        grandparent->_breakpoint->_second_focus = parent->_breakpoint->_second_focus;
                    else
                        grandparent->_breakpoint->_second_focus = parent->_breakpoint->_first_focus;
                }
            }

            node->_circle_event->_event_type = event_type::NONE;
            node->_circle_event->_squeezed_arc->_arc = nullptr;
            node->_arc = nullptr;
        } else {
            BeachlineElement* parent = node->_parent;
            BeachlineElement* previous = node->_previous;
            BeachlineElement* next = node->_next;

            previous->_next = next;
            next->_previous = previous;

            BeachlineElement* new_root = nullptr;
            if(parent->_left == node) {
                new_root = parent->_right;
            } else if(parent->_right == node) {
                new_root = parent->_left;
            }

            root = new_root;
            node->_circle_event->_event_type = event_type::NONE;
            node->_circle_event->_squeezed_arc->_arc = nullptr;
            node->_arc = nullptr;
        }
    }
}
