#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <QPointF>
#include "qfloat16.h"

#define EPSILON 0.00001

QPointF find_intersection_parabolas(QPointF f1, QPointF f2, qreal d);

qreal distance(QPointF p1, QPointF p2);

QPointF calculate_circle_lowest_point(QPointF p1, QPointF p2, QPointF p3);

QPointF calculate_circle_center(QPointF p1, QPointF p2, QPointF p3);

bool are_collinear(QPointF p1, QPointF p2, QPointF p3);

std::pair<QPointF, QPointF> find_breakpoints(QPointF f1, QPointF f2, QPointF f3, qreal d);

bool between(QPointF left, QPointF mid, QPointF right);

bool converge_towards_common_point(QPointF p1, QPointF p2, QPointF p3, qreal d);

bool triple_disappears_later(QPointF left_focus, QPointF old_left_focus, QPointF old_right_focus,QPointF right_focus);

#endif // MATH_UTILS_H
