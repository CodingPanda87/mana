#include <mana/util/geo.hpp>
#include <cassert>
#include <iostream>
#include <cmath>

void test_point() {
    mana::util::geo::Point p1{0, 0};
    mana::util::geo::Point p2{3, 4};

    assert(std::abs(p1.distance_to(p2) - 5.0) < 1e-10);

    auto p3 = p1 + p2;
    assert(p3.x == 3 && p3.y == 4);

    auto p4 = p2 - p1;
    assert(p4.x == 3 && p4.y == 4);
}

void test_rect() {
    mana::util::geo::Rect rect{{0, 0}, {10, 10}};

    assert(rect.width() == 10);
    assert(rect.height() == 10);
    assert(rect.area() == 100);

    assert(rect.contains({5, 5}) == true);
    assert(rect.contains({15, 5}) == false);

    mana::util::geo::Rect other{{5, 5}, {15, 15}};
    assert(rect.intersects(other) == true);

    mana::util::geo::Rect far{{20, 20}, {30, 30}};
    assert(rect.intersects(far) == false);
}

void test_circle() {
    mana::util::geo::Circle circle{{0, 0}, 5};

    assert(std::abs(circle.area() - 78.53981633974483) < 1e-10);
    assert(std::abs(circle.circumference() - 31.41592653589793) < 1e-10);

    assert(circle.contains({3, 4}) == true);
    assert(circle.contains({6, 0}) == false);

    mana::util::geo::Circle other{{10, 0}, 5};
    assert(circle.intersects(other) == true);

    mana::util::geo::Circle far{{20, 0}, 5};
    assert(circle.intersects(far) == false);
}

void test_segment() {
    mana::util::geo::Segment seg{{0, 0}, {10, 0}};

    assert(seg.length() == 10);

    assert(std::abs(seg.distance_to({5, 3}) - 3.0) < 1e-10);
    assert(std::abs(seg.distance_to({-1, 0}) - 1.0) < 1e-10);

    mana::util::geo::Segment other{{5, -5}, {5, 5}};
    assert(seg.intersects(other) == true);

    mana::util::geo::Segment far{{5, 5}, {5, 10}};
    assert(seg.intersects(far) == false);
}

void test_haversine() {
    // Beijing to Shanghai (~1068 km)
    mana::util::geo::GeoPoint beijing{39.9042, 116.4074};
    mana::util::geo::GeoPoint shanghai{31.2304, 121.4737};

    double dist = mana::util::geo::haversine_distance(beijing, shanghai);
    assert(dist > 1060000 && dist < 1080000); // ~1068 km
}

void test_bearing() {
    mana::util::geo::GeoPoint north{0, 0};
    mana::util::geo::GeoPoint east{0, 90};

    double b = mana::util::geo::bearing(north, east);
    assert(std::abs(b - mana::util::geo::pi / 2) < 0.1); // ~90 degrees
}

void test_destination() {
    mana::util::geo::GeoPoint start{0, 0};
    auto dest = mana::util::geo::destination(start, 111320, 0); // ~1 degree north

    assert(std::abs(dest.latitude - 1.0) < 0.1);
    assert(std::abs(dest.longitude) < 0.1);
}

void test_coord_convert() {
    mana::util::geo::GeoPoint wgs{39.9042, 116.4074};

    // WGS84 -> GCJ02 -> WGS84 should be close to original
    auto gcj = mana::util::geo::wgs84_to_gcj02(wgs);
    auto back = mana::util::geo::gcj02_to_wgs84(gcj);

    assert(std::abs(back.latitude - wgs.latitude) < 0.001);
    assert(std::abs(back.longitude - wgs.longitude) < 0.001);

    // GCJ02 -> BD09 -> GCJ02 should be close to original
    auto bd = mana::util::geo::gcj02_to_bd09(gcj);
    auto gcj_back = mana::util::geo::bd09_to_gcj02(bd);

    assert(std::abs(gcj_back.latitude - gcj.latitude) < 0.001);
    assert(std::abs(gcj_back.longitude - gcj.longitude) < 0.001);
}

void test_quadtree() {
    mana::util::geo::Rect bounds{{0, 0}, {100, 100}};
    mana::util::geo::QuadTree qt(bounds, 4);

    // Insert points
    assert(qt.insert({10, 10}) == true);
    assert(qt.insert({20, 20}) == true);
    assert(qt.insert({30, 30}) == true);
    assert(qt.insert({40, 40}) == true);
    assert(qt.insert({50, 50}) == true);

    assert(qt.size() == 5);

    // Insert outside bounds
    assert(qt.insert({200, 200}) == false);
    assert(qt.size() == 5);

    // Range query
    mana::util::geo::Rect range{{0, 0}, {25, 25}};
    auto results = qt.query(range);
    assert(results.size() == 2);

    // Clear
    qt.clear();
    assert(qt.size() == 0);
}

// Tests are called from test_util.cpp main()
