#ifndef PROCESS_EVENTS_H
#define PROCESS_EVENTS_H

#include "beachline.h"
#include "my_dcel.h"
#include "math_utils.h"

#include <queue>

void process_site_event(beachline::Event* site_event,
                        std::priority_queue<beachline::Event*, std::vector<beachline::Event*>, beachline::EventComparator>* event_queue,
                        beachline::Beachline* beachline, my_dcel::DCEL* voronoi_diagram, qreal sweepline);

void process_circle_event(beachline::Event* circle_event,
                          std::priority_queue<beachline::Event*, std::vector<beachline::Event*>, beachline::EventComparator>* event_queue,
                          beachline::Beachline* beachline, my_dcel::DCEL* voronoi_diagram, qreal sweepline);

void check_new_circle_event( beachline::BeachlineElement* arc1, beachline::BeachlineElement* arc2,  beachline::BeachlineElement*  arc3, qreal sweepline,
                            std::priority_queue<beachline::Event*, std::vector<beachline::Event*>, beachline::EventComparator>* event_queue);

#endif // PROCESS_EVENTS_H
