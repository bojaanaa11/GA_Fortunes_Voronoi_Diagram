#ifndef BOJANA_OBRADOVIC_FORTUNE_VORONOI_H
#define BOJANA_OBRADOVIC_FORTUNE_VORONOI_H

#include "algoritambaza.h"
#include "config.h" // For CANVAS_WIDTH and CANVAS_HEIGHT
#include "qpainterpath.h"
#include <vector>
#include <queue>

#include "my_dcel.h"
#include "beachline.h"
#include "process_events.h"

namespace Voronoi
{

#define SWEEPLINE_STEP 0.000001

class FortuneVoronoi : public AlgoritamBaza
{
public:
    FortuneVoronoi(QWidget *pCrtanje,
                   int pauzaKoraka,
                   const bool &naivni = false,
                   std::string imeDatoteke = "",
                   int brojTacaka = BROJ_SLUCAJNIH_OBJEKATA);

    void pokreniAlgoritam() final;
    void crtajAlgoritam(QPainter *painter) const final;
    void pokreniNaivniAlgoritam() final;
    void crtajNaivniAlgoritam(QPainter *painter) const final;

private:
    std::vector<QPoint> _tacke;
    int _yPoz;

    double sweepline;
    my_dcel::DCEL* voronoi_diagram;
    beachline::Beachline* beachline;
};

}

#endif // BOJANA_OBRADOVIC_FORTUNE_VORONOI_H
