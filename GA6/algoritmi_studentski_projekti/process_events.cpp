#include "process_events.h"

void process_site_event(beachline::Event *site_event,
                        std::priority_queue<beachline::Event *, std::vector<beachline::Event *>, beachline::EventComparator> *event_queue,
                        beachline::Beachline *beachline, my_dcel::DCEL *voronoi_diagram, qreal sweepline)
{
    if(beachline->empty())
    {
        beachline::Arc* new_root = new beachline::Arc(site_event->_site);
        beachline->insert_new_root(new_root);
    } else {
        /* Update beachline */
        QPointF new_arc_focus = site_event->_site;

        beachline::BeachlineElement* arc_above = beachline->find_arc_above(new_arc_focus, sweepline);
        QPointF arc_above_focus = arc_above->_arc->_focus;

        if(arc_above->_circle_event) {
            arc_above->_circle_event->_event_type = beachline::event_type::NONE;
        }

        beachline::BeachlineElement* new_arc = new beachline::BeachlineElement(new beachline::Arc(new_arc_focus));
        beachline->add_subtree(new_arc, arc_above);

        /* Update Voronoi Diagram */
        QPointF int_1 = find_intersection_parabolas(arc_above_focus, new_arc_focus, sweepline);
        my_dcel::HalfEdge* he1 = new my_dcel::HalfEdge(int_1);

        QPointF int_2 = find_intersection_parabolas(new_arc_focus, arc_above_focus, sweepline);
        my_dcel::HalfEdge* he2 = new my_dcel::HalfEdge(int_2);

        voronoi_diagram->_halfedges.push_back(he1);
        voronoi_diagram->_halfedges.push_back(he2);

        new_arc->_left_halfedge = he1;
        new_arc->_right_halfedge = he2;

        new_arc->_previous->_right_halfedge = new my_dcel::HalfEdge(he1->_start);
        new_arc->_next->_left_halfedge = new my_dcel::HalfEdge(he2->_start);
        voronoi_diagram->_halfedges.push_back(new_arc->_previous->_right_halfedge);
        voronoi_diagram->_halfedges.push_back(new_arc->_next->_left_halfedge);

        he1->_twin = he2;
        he2->_twin = he1;

        /* Check for new circle events */
        beachline::BeachlineElement* left_arc = nullptr;
        beachline::BeachlineElement*  right_arc = nullptr;
        beachline::BeachlineElement* left_of_left_arc = nullptr;
        beachline::BeachlineElement*  right_of_right_arc = nullptr;
        if(new_arc->_previous) {
            left_arc = new_arc->_previous;
            if(new_arc->_previous->_previous) {
                left_of_left_arc = new_arc->_previous->_previous;
            }
        }

        if(new_arc->_next) {
            right_arc = new_arc->_next;
            if(new_arc->_next->_next) {
                right_of_right_arc = new_arc->_next->_next;
            }
        }

        if(left_arc && left_of_left_arc) {
            check_new_circle_event(left_of_left_arc, left_arc, new_arc, sweepline, event_queue);
        }

        if(right_arc && right_of_right_arc) {
            check_new_circle_event(new_arc, right_arc, right_of_right_arc, sweepline, event_queue);
        }
    }
}

void process_circle_event(beachline::Event *circle_event,
                          std::priority_queue<beachline::Event *, std::vector<beachline::Event *>, beachline::EventComparator> *event_queue,
                          beachline::Beachline *beachline, my_dcel::DCEL *voronoi_diagram, qreal sweepline)
{

    beachline::BeachlineElement* left_arc= circle_event->_squeezed_arc->_previous;
    beachline::BeachlineElement* right_arc = circle_event->_squeezed_arc->_next;
    beachline::BeachlineElement* left_of_left_arc= left_arc->_previous;
    beachline::BeachlineElement* right_of_right_arc = right_arc->_next;
    QPointF center = circle_event->_center;

    if(!left_arc || !right_arc)
        return;

    if(left_arc && left_arc->_circle_event) {
        left_arc->_circle_event->_event_type = beachline::event_type::NONE;
    }

    if(right_arc && right_arc->_circle_event) {
        right_arc->_circle_event->_event_type = beachline::event_type::NONE;
    }

    if(!converge_towards_common_point(left_arc->_arc->_focus, circle_event->_squeezed_arc->_arc->_focus, right_arc->_arc->_focus, sweepline)) {
        return;
    }

    /* Add new vertex to voroni which is the center of circle */
    my_dcel::Vertex* vertex = new my_dcel::Vertex(center);
    voronoi_diagram->_vertices.push_back(vertex);

    if(circle_event->_squeezed_arc->_left_halfedge && circle_event->_squeezed_arc->_right_halfedge) {
        circle_event->_squeezed_arc->_left_halfedge->_origin = vertex;
        circle_event->_squeezed_arc->_right_halfedge->_origin = vertex;
    }

    /* Delete arc being squeezed out and its parent and adjust grandparent */
    beachline->remove(circle_event->_squeezed_arc, sweepline);

    if(left_arc) {
        if(left_arc->_right_halfedge)  {
            if(!left_arc->_right_halfedge->_origin) {
                left_arc->_right_halfedge->_origin = vertex;

                my_dcel::HalfEdge* new_right_halfedge = new my_dcel::HalfEdge(vertex->_coordinates);
                left_arc->_right_halfedge = new_right_halfedge;
                voronoi_diagram->_halfedges.push_back(new_right_halfedge);
            }
        }
    }

    if(right_arc) {
        if(right_arc->_left_halfedge)  {
            if(!right_arc->_left_halfedge->_origin) {
                right_arc->_left_halfedge->_origin = vertex;

                my_dcel::HalfEdge* new_left_halfedge = new my_dcel::HalfEdge(vertex->_coordinates);
                right_arc->_left_halfedge = new_left_halfedge;
                voronoi_diagram->_halfedges.push_back(new_left_halfedge);
            }
        }
    }

    /* Check for new circle events */
    if(left_of_left_arc && left_arc && right_arc) {
        check_new_circle_event(left_of_left_arc, left_arc, right_arc, sweepline, event_queue);
    }

    if(left_of_left_arc && right_arc && right_of_right_arc) {
        check_new_circle_event(left_arc, right_arc, right_of_right_arc, sweepline, event_queue);
    }
}

void check_new_circle_event(beachline::BeachlineElement*  arc1, beachline::BeachlineElement *arc2, beachline::BeachlineElement* arc3, qreal sweepline,
                            std::priority_queue<beachline::Event *, std::vector<beachline::Event *>, beachline::EventComparator> *event_queue)
{
    if(!arc1 || !arc3 || arc1->_arc->_focus == arc3->_arc->_focus) return;

    if(are_collinear(arc1->_arc->_focus, arc2->_arc->_focus, arc3->_arc->_focus)) return;

    QPointF lowest_point = calculate_circle_lowest_point(arc1->_arc->_focus, arc2->_arc->_focus, arc3->_arc->_focus);

    QPointF center = calculate_circle_center(arc1->_arc->_focus, arc2->_arc->_focus, arc3->_arc->_focus);

    if(lowest_point.y() <= sweepline) {
        QPointF left_focus = arc1->_arc->_focus;
        QPointF right_focus = arc3->_arc->_focus;
        beachline::Event* circle_event = new beachline::Event(lowest_point, left_focus, right_focus, center, arc2);
        event_queue->push(circle_event);
        arc2->_circle_event = circle_event;
    }
}

