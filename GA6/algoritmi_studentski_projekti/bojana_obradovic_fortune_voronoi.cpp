#include "bojana_obradovic_fortune_voronoi.h"

namespace Voronoi
{

FortuneVoronoi::FortuneVoronoi(QWidget *pCrtanje,
                               int pauzaKoraka,
                               const bool &naivni,
                               std::string imeDatoteke,
                               int brojTacaka)
    : AlgoritamBaza(pCrtanje, pauzaKoraka, naivni), _yPoz(_pCrtanje->height()),
    sweepline(_pCrtanje->height()), voronoi_diagram(new my_dcel::DCEL()), beachline(new beachline::Beachline())
{
    if (imeDatoteke == "")
        _tacke = generisiNasumicneTacke(brojTacaka);
    else
        _tacke = ucitajPodatkeIzDatoteke(imeDatoteke);

    beachline->voronoi_diagram = voronoi_diagram;
}


void FortuneVoronoi::pokreniAlgoritam()
{
    /* Switch points from QPoint to QPointF */
    std::vector<QPointF> points;
    for(auto& pt : _tacke)
    {
        points.push_back(QPointF(pt.x() * 1.0, pt.y() * 1.0));
    }

    /* Define event queue and put all given points as site events in it */
    std::priority_queue<beachline::Event*, std::vector<beachline::Event*>, beachline::EventComparator> event_queue;

    for(auto& pt : points)
    {
        event_queue.push(new beachline::Event(pt));
    }

    AlgoritamBaza_updateCanvasAndBlock()

    /* Until all events are processed */
    while(!event_queue.empty())
    {
        /* Take an event */
        beachline::Event* new_event = event_queue.top();

        _yPoz = sweepline;

        /* If sweepline reached the event pop it and process it */
        if(sweepline <= new_event->_y) /* It should be absolute value of the difference less then epsilon */
        {
            event_queue.pop();

            /* Depending on the type of the event, proccess it differently */
            if(new_event->_event_type == beachline::event_type::SITE)
            {
                process_site_event(new_event, &event_queue, beachline, voronoi_diagram, sweepline);
                AlgoritamBaza_updateCanvasAndBlock()
            } else if (new_event->_event_type == beachline::event_type::CIRCLE)
            {
                process_circle_event(new_event, &event_queue, beachline, voronoi_diagram, sweepline);
                AlgoritamBaza_updateCanvasAndBlock()
            }
        }

        sweepline -= SWEEPLINE_STEP;
    }

    // for(auto &he : voronoi_diagram->_halfedges) {
    //     if(he->_origin && he->_twin && !he->_twin->_origin && he->start_is_vertex) {
    //         qreal dx = he->_origin->_coordinates.x() - he->_start.x();
    //         qreal dy = he->_origin->_coordinates.y() - he->_start.y();

    //         dx = -50 * dx;
    //         dy = -50 * dy;

    //         he->_twin->_origin = new my_dcel::Vertex(he->_start + QPointF(dx, dy));
    //     }
    // }

    AlgoritamBaza_updateCanvasAndBlock()

    emit animacijaZavrsila();
}

void FortuneVoronoi::crtajAlgoritam(QPainter *painter) const
{
    if (!painter) return;

    QPen p = painter->pen();
    p.setColor(Qt::magenta);
    p.setWidth(2);
    p.setCapStyle(Qt::RoundCap);

    painter->setPen(p);
    painter->drawLine(0, _yPoz, _pCrtanje->width(), _yPoz);

    p.setWidth(5);
    for(const QPoint &pt : _tacke)
    {
        if (pt.y() < _yPoz)
            p.setColor(Qt::magenta);
        else
            p.setColor(Qt::black);

        painter->setPen(p);
        painter->drawPoint(pt);
    }

    /* Draw */

    p.setColor(Qt::darkRed);
    p.setWidth(5);
    for(auto &he : voronoi_diagram->_halfedges) {
        painter->setPen(p);
        if(he->_origin) {
            painter->drawLine(he->_start, he->_origin->_coordinates);
        }
    }

    p.setColor(Qt::cyan);
    p.setWidth(12);
    for(auto &vertex : voronoi_diagram->_vertices) {
        painter->setPen(p);
        painter->drawPoint(vertex->_coordinates);
    }
}

void FortuneVoronoi::pokreniNaivniAlgoritam()
{
    emit animacijaZavrsila();
}

void FortuneVoronoi::crtajNaivniAlgoritam(QPainter *painter) const
{
    if (!painter) return;
}

}
