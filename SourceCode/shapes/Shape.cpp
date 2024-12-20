#include "Shape.h"
#include "Point.h"
#include "Rectangle.h"
#include "Circle.h"
#include "Triangle.h"
#include "../Utils.h"
#include <algorithm>

using namespace std;

/**
 * @file Shape.cpp
 * @brief All functions and implementations are defined here.
 */

bool checkOverlap(const Point *p1, const Point *p2) {
	return (p1->x == p2->x) && (p1->y == p2->y);
}

bool checkOverlap(const Point *p, const Rectangle *r) {
	return (r->x1 <= p->x && p->x <= r->x2) && (r->y1 <= p->y && p->y <= r->y2);
}

bool checkOverlap(const Point *p, const Circle *c) {
	return (c->r * c->r) <= Point::dist2((*p), Point(c->x, c->y));
}

bool checkOverlap(const Rectangle *r1, const Rectangle *r2) {
	return !(r1->x2 < r2->x1 || r2->x2 < r1->x1 || r1->y2 < r2->y1 || r2->y2 < r1->y1);
}

bool checkOverlap(const Rectangle *r, const Circle *c) {
	double x = max(r->x1, min(c->x, r->x2));
	double y = max(r->y1, min(c->y, r->y2));
	return (c->r * c->r) >= Point::dist2(Point(c->x, c->y), Point(x, y));
}

bool checkOverlap(const Circle *c1, const Circle *c2) {
	double d = c1->r + c2->r;
	return (d * d) >= Point::dist2(Point(c1->x, c1->y), Point(c2->x, c2->y));
}

bool checkOverlap(const Triangle *t, const Rectangle *r) {
	return t->contain(r->x1, r->y1) || t->contain(r->x2, r->y1) || t->contain(r->x1, r->y2) || t->contain(r->x2, r->y2)\
	||( (r->x1 <= t->x1 && t->x1 <= r->x2) && (r->y1 <= t->y1 && t->y1 <= r->y2)) \
	|| ((r->x1 <= t->x2 && t->x2 <= r->x2) && (r->y1 <= t->y2 && t->y2 <= r->y2)) \
	|| ((r->x1 <= t->x3 && t->x3 <= r->x2) && (r->y1 <= t->y3 && t->y3 <= r->y2));


}

bool checkOverlap(const Triangle *t, const Point *p) {
	return t->contain(p->x, p->y);
}

bool
Point::overlap(const Shape &s) const {
	switch(s.getType()) {
		case ShapeType::POINT: return checkOverlap(this, static_cast<const Point*>(&s));
		case ShapeType::RECTANGLE: return checkOverlap(this, static_cast<const Rectangle*>(&s));
		case ShapeType::CIRCLE: return checkOverlap(this, static_cast<const Circle*>(&s));
		case ShapeType::TRIANGLE: return checkOverlap(static_cast<const Triangle*>(&s), this);
	}
	GAME_ASSERT(false, "Unknown ShapeType.");
}

bool
Rectangle::overlap(const Shape &s) const {
	switch(s.getType()) {
		case ShapeType::POINT: return checkOverlap(static_cast<const Point*>(&s), this);
		case ShapeType::RECTANGLE: return checkOverlap(this, static_cast<const Rectangle*>(&s));
		case ShapeType::CIRCLE: return checkOverlap(this, static_cast<const Circle*>(&s));
		case ShapeType::TRIANGLE: return checkOverlap(static_cast<const Triangle*>(&s), this);
	}
	GAME_ASSERT(false, "Unknown ShapeType.");
}

bool
Circle::overlap(const Shape &s) const {
	switch(s.getType()) {
		case ShapeType::POINT: return checkOverlap(static_cast<const Point*>(&s), this);
		case ShapeType::RECTANGLE: return checkOverlap(static_cast<const Rectangle*>(&s), this);
		case ShapeType::CIRCLE: return checkOverlap(this, static_cast<const Circle*>(&s));
	}
	GAME_ASSERT(false, "Unknown ShapeType.");
}

bool
Triangle::overlap(const Shape &s) const {
	switch(s.getType()) {
		case ShapeType::POINT: return checkOverlap(this,static_cast<const Point*>(&s));
		case ShapeType::RECTANGLE: return checkOverlap(this,static_cast<const Rectangle*>(&s));
	}
}