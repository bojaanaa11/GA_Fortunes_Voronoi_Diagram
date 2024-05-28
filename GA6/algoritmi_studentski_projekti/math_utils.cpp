#include "math_utils.h"


QPointF find_intersection_parabolas(QPointF f1, QPointF f2, qreal d)
{
    if(fabs(d - f1.y()) < EPSILON) {
        qreal a2 = (f2.y() < d) ? -1.0 / (2 * (f2.y() - d)) : 1.0 / (2 * (f2.y() - d));
        qreal b2 = -f2.x() / (f2.y() - d);
        qreal c2 = (f2.x() * f2.x() + f2.y() * f2.y() - d * d) / (2 * (f2.y() - d));

        qreal x = f1.x();
        qreal y = a2 * (x * x) + b2 * x + c2;

        return QPointF(x, y);
    } else if(fabs(d - f2.y()) < EPSILON) {
        qreal a1 = (f1.y() < d) ? -1.0 / (2 * (f1.y() - d)) : 1.0 / (2 * (f1.y() - d));
        qreal b1 = -f1.x() / (f1.y() - d);
        qreal c1 = (f1.x() * f1.x() + f1.y() * f1.y() - d*d) / (2 * (f1.y() - d));

        qreal x = f2.x();
        qreal y = a1 * (x * x) + b1 * x + c1;

        return QPointF(x, y);
    } else {
        qreal a1 = (f1.y() < d) ? -1.0 / (2 * (f1.y() - d)) : 1.0 / (2 * (f1.y() - d));
        qreal b1 = -f1.x() / (f1.y() - d);
        qreal c1 = (f1.x() * f1.x() + f1.y() * f1.y() - d*d) / (2 * (f1.y() - d));

        qreal a2 = (f2.y() < d) ? -1.0 / (2 * (f2.y() - d)) : 1.0 / (2 * (f2.y() - d));
        qreal b2 = -f2.x() / (f2.y() - d);
        qreal c2 = (f2.x() * f2.x() + f2.y() * f2.y() - d*d) / (2 * (f2.y() - d));

        qreal A = a1 - a2;
        qreal B = b1 - b2;
        qreal C = c1 - c2;

        qreal discriminant = B * B - 4 * A * C;

        qreal x1 = (-B + qSqrt(discriminant)) / (2 * A);
        qreal x2 = (-B - qSqrt(discriminant)) / (2 * A);

        qreal y1 = a1 * (x1 * x1) + b1 * x1 + c1;
        qreal y2 = a1 * (x2 * x2) + b1 * x2 + c1;

        if(x1 > x2)
        {
            int tmp_x = x1;
            x1 = x2;
            x2 = tmp_x;

            int tmp_y = y1;
            y1 = y2;
            y2 = tmp_y;
        }

        return (f1.y() > f2.y()) ? QPointF(x1, y1) : QPointF(x2, y2);
    }
}

qreal distance(QPointF p1, QPointF p2)
{
    return qSqrt((p1.x() - p2.x()) * (p1.x() - p2.x()) + (p1.y() - p2.y()) * (p1.y() - p2.y()));
}


QPointF calculate_circle_lowest_point(QPointF p1, QPointF p2, QPointF p3)
{
    QPointF center = calculate_circle_center(p1, p2, p3);
    qreal radius = distance(center, p1);

    qreal vertical_line_x = center.x();

    qreal intersection_y1 = center.y() + qSqrt(radius * radius - (vertical_line_x - center.x()) * (vertical_line_x - center.x()));
    qreal intersection_y2 = center.y() - qSqrt(radius * radius - (vertical_line_x - center.x()) * (vertical_line_x - center.x()));

    qreal intersection_y = (intersection_y1 < intersection_y2) ? intersection_y1 : intersection_y2;

    return QPointF(vertical_line_x, intersection_y);
}

QPointF calculate_circle_center(QPointF p1, QPointF p2, QPointF p3)
{
    qreal midAB_x = (p1.x() + p2.x()) / 2;
    qreal midAB_y = (p1.y() + p2.y()) / 2;

    qreal midBC_x = (p2.x() + p3.x()) / 2;
    qreal midBC_y = (p2.y() + p3.y()) / 2;

    qreal slope_AB = (p2.y() - p1.y()) / (p2.x() - p1.x());
    qreal slope_BC = (p3.y() - p2.y()) / (p3.x() - p2.x());

    qreal perpSlopeAB = -1 / slope_AB;
    qreal perpSlopeBC = -1 / slope_BC;

    qreal y_intercept_AB = midAB_y - perpSlopeAB * midAB_x;
    qreal y_intercept_BC = midBC_y - perpSlopeBC * midBC_x;

    qreal circumcenter_x = (y_intercept_BC - y_intercept_AB) / (perpSlopeAB - perpSlopeBC);
    qreal circumcenter_y = perpSlopeAB * circumcenter_x + y_intercept_AB;

    return QPointF(circumcenter_x, circumcenter_y);
}

bool are_collinear(QPointF p1, QPointF p2, QPointF p3)
{
    qreal area = (p2.x() - p1.x()) * (p3.y() - p1.y()) - (p3.x() - p1.x()) * (p2.y() - p1.y());

    return std::abs(area) < EPSILON;
}

std::pair<QPointF, QPointF> find_breakpoints(QPointF f1, QPointF f2, qreal d)
{
    qreal a1 = (f1.y() < d) ? -1.0 / (2 * (f1.y() - d)) : 1.0 / (2 * (f1.y() - d));
    qreal b1 = -f1.x() / (f1.y() - d);
    qreal c1 = (f1.x() * f1.x() + f1.y() * f1.y() - d*d) / (2 * (f1.y() - d));

    qreal a2 = (f2.y() < d) ? -1.0 / (2 * (f2.y() - d)) : 1.0 / (2 * (f2.y() - d));
    qreal b2 = -f2.x() / (f2.y() - d);
    qreal c2 = (f2.x() * f2.x() + f2.y() * f2.y() - d*d) / (2 * (f2.y() - d));

    qreal A = a1 - a2;
    qreal B = b1 - b2;
    qreal C = c1 - c2;

    qreal discriminant = B * B - 4 * A * C;

    qreal x1 = (-B + qSqrt(discriminant)) / (2 * A);
    qreal x2 = (-B - qSqrt(discriminant)) / (2 * A);

    qreal y1 = a1 * (x1 * x1) + b1 * x1 + c1;
    qreal y2 = a1 * (x2 * x2) + b1 * x2 + c1;

    if(x1 > x2)
    {
        int tmp_x = x1;
        x1 = x2;
        x2 = tmp_x;

        int tmp_y = y1;
        y1 = y2;
        y2 = tmp_y;
    }


    QPointF int_1 =  QPointF(x1, y1);
    QPointF int_2 = QPointF(x2, y2);

    return std::make_pair(int_1, int_2);
}

bool between(QPointF left, QPointF mid, QPointF right)
{
    return left.x() < mid.x() && mid.x() < right.x();
}

qreal area(qreal radius)
{
    return radius * radius * M_PI;
}

bool converge_towards_common_point(QPointF p1, QPointF p2, QPointF p3, qreal d)
{
    // // Find breakpoints
    // std::pair<QPointF, QPointF> bpts12 = find_breakpoints(p1, p2, d);
    // std::pair<QPointF, QPointF> bpts23 = find_breakpoints(p2, p3, d);

    // // Check if any of them are equal
    // qreal points_treshold = 2;
    // qreal dist_11 = distance(bpts12.first, bpts23.first);
    // qreal dist_12 = distance(bpts12.first, bpts23.second);
    // qreal dist_21 = distance(bpts12.second, bpts23.first);
    // qreal dist_22 = distance(bpts12.second, bpts23.second);
    // if(dist_11 < points_treshold) {
    //     //if(between(p1, bpts12.first, p3) && between(p1, bpts23.first, p3))
    //         return true;
    // } else if(dist_12 < points_treshold) {
    //     //if(between(p1, bpts12.first, p3) && between(p1, bpts23.second, p3))
    //         return true;
    // } else if(dist_21 < points_treshold) {
    //     //if(between(p1, bpts12.second, p3) && between(p1, bpts23.first, p3))
    //         return true;
    // } else if(dist_22 < points_treshold) {
    //     //if(between(p1, bpts12.second, p3) && between(p1, bpts23.second, p3))
    //         return true;
    // }

    // return false;

    // qreal points_treshold = 2;

    // std::pair<QPointF, QPointF> bpts12 = find_breakpoints(p1, p2, d);
    // QPointF bp1;
    // if(between(p1, bpts12.first, p3))
    //     bp1 = bpts12.first;
    // else
    //     bp1 = bpts12.second;

    // std::pair<QPointF, QPointF> bpts23 = find_breakpoints(p2, p3, d);
    // QPointF bp2;
    // if(between(p1, bpts23.first, p3))
    //     bp2 = bpts23.first;
    // else
    //     bp2 = bpts23.second;

    // qreal dist = distance(bp1, bp2);
    // return dist < points_treshold;

    qreal points_treshold = 2;

    QPointF bp1 = find_intersection_parabolas(p1, p2, d);
    QPointF bp2 = find_intersection_parabolas(p2, p3, d);

    qreal dist = distance(bp1, bp2);
    return dist < points_treshold;
}

bool triple_disappears_later(QPointF left_focus, QPointF old_left_focus, QPointF old_right_focus,QPointF right_focus)
{
    return !(left_focus == old_left_focus) || !(right_focus == old_right_focus);
}
