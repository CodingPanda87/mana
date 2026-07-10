#pragma once

#include <cmath>
#include <vector>
#include <optional>
#include <memory>
#include <algorithm>
#include <limits>

namespace mana::util::geo {

// Constants
inline constexpr double earth_radius_m = 6371000.0;
inline constexpr double pi = 3.14159265358979323846;

// 2D Point
struct Point {
    double x = 0, y = 0;

    double distance_to(const Point& other) const {
        double dx = x - other.x;
        double dy = y - other.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    constexpr Point operator+(const Point& other) const {
        return {x + other.x, y + other.y};
    }

    constexpr Point operator-(const Point& other) const {
        return {x - other.x, y - other.y};
    }

    constexpr bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

// 2D Rectangle
struct Rect {
    Point min, max;

    constexpr double width() const { return max.x - min.x; }
    constexpr double height() const { return max.y - min.y; }
    constexpr double area() const { return width() * height(); }

    constexpr bool contains(const Point& p) const {
        return p.x >= min.x && p.x <= max.x &&
               p.y >= min.y && p.y <= max.y;
    }

    constexpr bool intersects(const Rect& other) const {
        return min.x <= other.max.x && max.x >= other.min.x &&
               min.y <= other.max.y && max.y >= other.min.y;
    }

    constexpr Rect intersection(const Rect& other) const {
        return {
            {std::max(min.x, other.min.x), std::max(min.y, other.min.y)},
            {std::min(max.x, other.max.x), std::min(max.y, other.max.y)}
        };
    }
};

// 2D Circle
struct Circle {
    Point center;
    double radius = 0;

    constexpr double area() const { return pi * radius * radius; }
    constexpr double circumference() const { return 2 * pi * radius; }

    bool contains(const Point& p) const {
        return center.distance_to(p) <= radius;
    }

    bool intersects(const Circle& other) const {
        double dist = center.distance_to(other.center);
        return dist <= radius + other.radius;
    }
};

// 2D Line Segment
struct Segment {
    Point a, b;

    double length() const { return a.distance_to(b); }

    double distance_to(const Point& p) const {
        double dx = b.x - a.x;
        double dy = b.y - a.y;
        double len_sq = dx * dx + dy * dy;
        if (len_sq == 0) return a.distance_to(p);

        double t = ((p.x - a.x) * dx + (p.y - a.y) * dy) / len_sq;
        t = std::max(0.0, std::min(1.0, t));

        Point projection = {a.x + t * dx, a.y + t * dy};
        return p.distance_to(projection);
    }

    bool intersects(const Segment& other) const {
        auto cross = [](Point p1, Point p2) {
            return p1.x * p2.y - p1.y * p2.x;
        };

        Point d1 = b - a;
        Point d2 = other.b - other.a;
        double denom = cross(d1, d2);

        if (std::abs(denom) < 1e-10) return false;

        Point d3 = other.a - a;
        double t = cross(d3, d2) / denom;
        double u = cross(d3, d1) / denom;

        return t >= 0 && t <= 1 && u >= 0 && u <= 1;
    }

    std::optional<Point> intersection(const Segment& other) const {
        auto cross = [](Point p1, Point p2) {
            return p1.x * p2.y - p1.y * p2.x;
        };

        Point d1 = b - a;
        Point d2 = other.b - other.a;
        double denom = cross(d1, d2);

        if (std::abs(denom) < 1e-10) return std::nullopt;

        Point d3 = other.a - a;
        double t = cross(d3, d2) / denom;
        double u = cross(d3, d1) / denom;

        if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
            return Point{a.x + t * d1.x, a.y + t * d1.y};
        }
        return std::nullopt;
    }
};

// Geographic coordinate
struct GeoPoint {
    double latitude = 0;   // [-90, 90]
    double longitude = 0;  // [-180, 180]
};

// Coordinate system enum
enum class CoordSystem {
    WGS84,
    GCJ02,
    BD09
};

// Haversine distance (meters)
inline double haversine_distance(const GeoPoint& a, const GeoPoint& b) {
    auto to_rad = [](double deg) { return deg * pi / 180.0; };

    double lat1 = to_rad(a.latitude);
    double lat2 = to_rad(b.latitude);
    double dlat = to_rad(b.latitude - a.latitude);
    double dlon = to_rad(b.longitude - a.longitude);

    double h = std::sin(dlat / 2) * std::sin(dlat / 2) +
               std::cos(lat1) * std::cos(lat2) *
               std::sin(dlon / 2) * std::sin(dlon / 2);

    return 2 * earth_radius_m * std::asin(std::sqrt(h));
}

// Bearing (radians)
inline double bearing(const GeoPoint& from, const GeoPoint& to) {
    auto to_rad = [](double deg) { return deg * pi / 180.0; };

    double lat1 = to_rad(from.latitude);
    double lat2 = to_rad(to.latitude);
    double dlon = to_rad(to.longitude - from.longitude);

    double y = std::sin(dlon) * std::cos(lat2);
    double x = std::cos(lat1) * std::sin(lat2) -
               std::sin(lat1) * std::cos(lat2) * std::cos(dlon);

    return std::atan2(y, x);
}

// Destination point
inline GeoPoint destination(const GeoPoint& from, double distance, double bearing_rad) {
    auto to_rad = [](double deg) { return deg * pi / 180.0; };
    auto to_deg = [](double rad) { return rad * 180.0 / pi; };

    double lat1 = to_rad(from.latitude);
    double lon1 = to_rad(from.longitude);
    double d = distance / earth_radius_m;

    double lat2 = std::asin(std::sin(lat1) * std::cos(d) +
                            std::cos(lat1) * std::sin(d) * std::cos(bearing_rad));
    double lon2 = lon1 + std::atan2(std::sin(bearing_rad) * std::sin(d) * std::cos(lat1),
                                     std::cos(d) - std::sin(lat1) * std::sin(lat2));

    return {to_deg(lat2), to_deg(lon2)};
}

// GCJ02 offset (simplified)
namespace detail {
    inline bool in_china(double lat, double lon) {
        return lat >= 0.8293 && lat <= 55.8271 && lon >= 72.004 && lon <= 137.8347;
    }

    inline double transform_lat(double x, double y) {
        double ret = -100.0 + 2.0 * x + 3.0 * y + 0.2 * y * y + 0.1 * x * y + 0.2 * std::sqrt(std::abs(x));
        ret += (20.0 * std::sin(6.0 * x * pi) + 20.0 * std::sin(2.0 * x * pi)) * 2.0 / 3.0;
        ret += (20.0 * std::sin(y * pi) + 40.0 * std::sin(y / 3.0 * pi)) * 2.0 / 3.0;
        ret += (160.0 * std::sin(y / 12.0 * pi) + 320 * std::sin(y * pi / 30.0)) * 2.0 / 3.0;
        return ret;
    }

    inline double transform_lon(double x, double y) {
        double ret = 300.0 + x + 2.0 * y + 0.1 * x * x + 0.1 * x * y + 0.1 * std::sqrt(std::abs(x));
        ret += (20.0 * std::sin(6.0 * x * pi) + 20.0 * std::sin(2.0 * x * pi)) * 2.0 / 3.0;
        ret += (20.0 * std::sin(x * pi) + 40.0 * std::sin(x / 3.0 * pi)) * 2.0 / 3.0;
        ret += (150.0 * std::sin(x / 12.0 * pi) + 300.0 * std::sin(x / 30.0 * pi)) * 2.0 / 3.0;
        return ret;
    }
} // namespace detail

// WGS84 to GCJ02
inline GeoPoint wgs84_to_gcj02(const GeoPoint& point) {
    if (!detail::in_china(point.latitude, point.longitude)) {
        return point;
    }

    double dlat = detail::transform_lat(point.longitude - 105.0, point.latitude - 35.0);
    double dlon = detail::transform_lon(point.longitude - 105.0, point.latitude - 35.0);

    double radlat = point.latitude / 180.0 * pi;
    double magic = std::sin(radlat);
    magic = 1 - 0.00669342162296594323 * magic * magic;

    double sqrtmagic = std::sqrt(magic);
    dlat = (dlat * 180.0) / ((6335552.717000426 * (1 - 0.00669342162296594323)) / (magic * sqrtmagic) * pi);
    dlon = (dlon * 180.0) / (6378245.0 / sqrtmagic * std::cos(radlat) * pi);

    return {point.latitude + dlat, point.longitude + dlon};
}

// GCJ02 to WGS84 (reverse)
inline GeoPoint gcj02_to_wgs84(const GeoPoint& point) {
    GeoPoint gcj = wgs84_to_gcj02(point);
    return {2 * point.latitude - gcj.latitude, 2 * point.longitude - gcj.longitude};
}

// GCJ02 to BD09
inline GeoPoint gcj02_to_bd09(const GeoPoint& point) {
    double x = point.longitude;
    double y = point.latitude;
    double z = std::sqrt(x * x + y * y) + 0.00002 * std::sin(y * pi);
    double theta = std::atan2(y, x) + 0.000003 * std::cos(x * pi);
    return {z * std::sin(theta) + 0.006, z * std::cos(theta) + 0.0065};
}

// BD09 to GCJ02
inline GeoPoint bd09_to_gcj02(const GeoPoint& point) {
    double x = point.longitude - 0.0065;
    double y = point.latitude - 0.006;
    double z = std::sqrt(x * x + y * y) - 0.00002 * std::sin(y * pi);
    double theta = std::atan2(y, x) - 0.000003 * std::cos(x * pi);
    return {z * std::sin(theta), z * std::cos(theta)};
}

// Generic convert
inline GeoPoint convert(const GeoPoint& point, CoordSystem from, CoordSystem to) {
    if (from == to) return point;

    // Convert to GCJ02 first
    GeoPoint gcj;
    switch (from) {
        case CoordSystem::WGS84: gcj = wgs84_to_gcj02(point); break;
        case CoordSystem::GCJ02: gcj = point; break;
        case CoordSystem::BD09: gcj = bd09_to_gcj02(point); break;
    }

    // Convert from GCJ02 to target
    switch (to) {
        case CoordSystem::WGS84: return gcj02_to_wgs84(gcj);
        case CoordSystem::GCJ02: return gcj;
        case CoordSystem::BD09: return gcj02_to_bd09(gcj);
    }

    return point; // Should not reach here
}

// QuadTree
class QuadTree {
public:
    QuadTree(const Rect& bounds, size_t capacity = 8)
        : bounds_(bounds), capacity_(capacity), root_(std::make_unique<Node>()) {}

    bool insert(const Point& point) {
        if (!bounds_.contains(point)) return false;
        return insert_impl(root_.get(), point, bounds_, 0);
    }

    std::vector<Point> query(const Rect& range) const {
        std::vector<Point> result;
        query_impl(root_.get(), bounds_, range, result);
        return result;
    }

    void clear() {
        root_ = std::make_unique<Node>();
        size_ = 0;
    }

    size_t size() const { return size_; }

private:
    static constexpr int max_depth = 10;

    struct Node {
        std::vector<Point> points;
        std::unique_ptr<Node> children[4]; // NW, NE, SW, SE
        bool is_leaf = true;
    };

    Node* get_quadrant(Node* node, const Rect& bounds, const Point& point) const {
        double mid_x = (bounds.min.x + bounds.max.x) / 2;
        double mid_y = (bounds.min.y + bounds.max.y) / 2;

        if (point.x < mid_x) {
            return point.y < mid_y ? node->children[0].get() : node->children[2].get();
        } else {
            return point.y < mid_y ? node->children[1].get() : node->children[3].get();
        }
    }

    Rect get_quadrant_bounds(const Rect& bounds, int index) const {
        double mid_x = (bounds.min.x + bounds.max.x) / 2;
        double mid_y = (bounds.min.y + bounds.max.y) / 2;

        switch (index) {
            case 0: return {{bounds.min.x, bounds.min.y}, {mid_x, mid_y}};  // SW
            case 1: return {{mid_x, bounds.min.y}, {bounds.max.x, mid_y}};  // SE
            case 2: return {{bounds.min.x, mid_y}, {mid_x, bounds.max.y}};  // NW
            case 3: return {{mid_x, mid_y}, {bounds.max.x, bounds.max.y}};  // NE
            default: return bounds;
        }
    }

    bool insert_impl(Node* node, const Point& point, const Rect& bounds, int depth) {
        if (node->is_leaf) {
            if (node->points.size() < capacity_ || depth >= max_depth) {
                node->points.push_back(point);
                ++size_;
                return true;
            }

            // Split node
            node->is_leaf = false;
            for (int i = 0; i < 4; ++i) {
                node->children[i] = std::make_unique<Node>();
            }

            // Redistribute existing points (they were already counted in size_)
            size_ -= node->points.size();
            for (const auto& p : node->points) {
                int q = 0;
                double mid_x = (bounds.min.x + bounds.max.x) / 2;
                double mid_y = (bounds.min.y + bounds.max.y) / 2;
                if (p.x >= mid_x) q += 1;
                if (p.y >= mid_y) q += 2;
                insert_impl(node->children[q].get(), p, get_quadrant_bounds(bounds, q), depth + 1);
            }
            node->points.clear();
        }

        int quadrant = 0;
        double mid_x = (bounds.min.x + bounds.max.x) / 2;
        double mid_y = (bounds.min.y + bounds.max.y) / 2;

        if (point.x >= mid_x) quadrant += 1;
        if (point.y >= mid_y) quadrant += 2;

        return insert_impl(node->children[quadrant].get(), point,
                          get_quadrant_bounds(bounds, quadrant), depth + 1);
    }

    void query_impl(Node* node, const Rect& bounds, const Rect& range, std::vector<Point>& result) const {
        if (!bounds.intersects(range)) return;

        for (const auto& p : node->points) {
            if (range.contains(p)) {
                result.push_back(p);
            }
        }

        if (!node->is_leaf) {
            for (int i = 0; i < 4; ++i) {
                query_impl(node->children[i].get(), get_quadrant_bounds(bounds, i), range, result);
            }
        }
    }

    Rect bounds_;
    size_t capacity_;
    size_t size_ = 0;
    std::unique_ptr<Node> root_;
};

} // namespace mana::util::geo
