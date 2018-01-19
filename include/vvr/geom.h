#ifndef VVR_GEOM_H
#define VVR_GEOM_H

#include "vvrframework_DLL.h"
#include <vvr/drawing.h>

namespace vvr
{
    template <class T>
    void snap_to_grid(T &x, T &y, T gs)
    {
        const double dxn = floor(fabs((double)x / gs) + 0.5);
        const double dyn = floor(fabs((double)y / gs) + 0.5);
        x = (x < 0) ? (-dxn * gs) : (dxn * gs);
        y = (y < 0) ? (-dyn * gs) : (dyn * gs);
    }

    bool operator==(const Point2D &lhs, const Point2D &rhs)
    {
        return (lhs.x == rhs.x) && (lhs.y == rhs.y);
    }

    template <typename point>
    bool cw(const point &a, const point &b, const point &c) 
    {
        return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x) < 0;
    }

    template <typename point>
    bool ccw(const point &a, const point &b, const point &c) 
    {
        return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x) > 0;
    }

    template <typename point>
    auto area(const point &a, const point &b, const point &c)
    {
        return abs((b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x));
    }

    template <typename point, typename real = decltype(point::x)>
    auto dist(const point &a, const point &b)
    {
        return (real)hypot(a.x - b.x, a.y - b.y);
    }

    template <typename point>
    std::vector<point> convex_hull(std::vector<point> p)
    {
        int n = p.size();
        if (n <= 1) return p;
        int k = 0;
        std::sort(p.begin(), p.end(), [](auto &lhs, auto &rhs) {
            return (lhs.x < rhs.x) || ((lhs.x == rhs.x) && (lhs.y < rhs.y));
        });
        std::vector<point> q(n * 2);
        for (int i = 0; i < n; q[k++] = p[i++]) {
            for (; k >= 2 && !ccw(q[k - 2], q[k - 1], p[i]); --k);
        }
        for (int i = n - 2, t = k; i >= 0; q[k++] = p[i--]) {
            for (; k > t && !ccw(q[k - 2], q[k - 1], p[i]); --k);
        }
        q.resize(k - 1 - (q[0] == q[1]));
        return q;
    }

    template <typename point, typename real=decltype(point::x)>
    real convex_diameter(const std::vector<point> &h, size_t &i1, size_t &i2)
    {
        size_t n = h.size();
        if (n == 1) return 0;
        if (n == 2) return dist(h[0], h[1]);
        size_t k = 1;
        while (area(h[n - 1], h[0], h[(k + 1) % n]) > area(h[n - 1], h[0], h[k])) k++;
        real res = 0, d;
        for (size_t i = 0, j = k; i <= k && j < n; i++) {
            if ((d = dist(h[i], h[j])) > res) {
                res = d; 
                i1 = i;
                i2 = j;
            }
            while (j < n && area(h[i], h[(i + 1) % n], h[(j + 1) % n]) > area(h[i], h[(i + 1) % n], h[j])) {
                if ((d = dist(h[i], h[(j + 1) % n])) > res) {
                    res = d; 
                    i1 = i;
                    i2 = j+1;
                }
                ++j;
            }
        }
        return res;
    }

    template <typename point, typename real = decltype(point::x)>
    real convex_width(const std::vector<point> &h, size_t &i1, size_t &i2)
    {
        const size_t n = h.size();
        if (n < 3) return 0;

        auto cmp = [](const point &lhs, const point &rhs) -> bool {
            return (lhs.x < rhs.x) || ((lhs.x == rhs.x) && (lhs.y > rhs.y));
        };

        C2DLine calipera, caliperb;
        size_t ia = std::min_element(h.begin(), h.end(), cmp) - h.begin();
        size_t ib = std::max_element(h.begin(), h.end(), cmp) - h.begin();
        calipera.vector = C2DVector(0, -1);
        caliperb.vector = C2DVector(0, +1);
        real minwidth = std::numeric_limits<real>::max();
        size_t reps = n;
        while (reps--)
        {
            C2DPoint pa(h[ia].x, h[ia].y);
            C2DPoint pb(h[ib].x, h[ib].y);
            C2DPoint paa(h[(ia+1)%n].x, h[(ia+1)%n].y);
            C2DPoint pbb(h[(ib+1)%n].x, h[(ib+1)%n].y);
            C2DVector edgea = paa - pa; edgea.MakeUnit();
            C2DVector edgeb = pbb - pb; edgeb.MakeUnit();
            real cosa = calipera.vector.Dot(edgea);
            real cosb = caliperb.vector.Dot(edgeb);
            calipera.point = pa;
            caliperb.point = pb;

            real width;
            size_t min_i1, min_i2;
            if (cosa > cosb)
            {
                calipera.vector = edgea;
                caliperb.vector = calipera.vector * -1;
                width = calipera.DistanceAsRay(caliperb.point);
                min_i1 = ia;
                min_i2 = ib;
                ia = (ia + 1) % n;
            }
            else
            {
                caliperb.vector = edgeb;
                calipera.vector = caliperb.vector * -1;
                width = caliperb.DistanceAsRay(calipera.point);
                min_i1 = ib;
                min_i2 = ia;
                ib = (ib + 1) % n;
            }

            if (width < minwidth)
            {
                minwidth = width;
                i1 = min_i1;
                i2 = min_i2;
            }
        }

        return minwidth;
    }
}

#endif
