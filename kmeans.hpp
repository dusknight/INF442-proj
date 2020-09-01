#pragma once
#ifndef INF442_P3_KMEANS_HPP
#define INF442_P3_KMEANS_HPP

#include <algorithm>
#include <cassert>
#include <iostream>

struct point {
    static int d;
    double* coords;
    int label;

    point();
    ~point();

    void print();

    double dist(point& q);
    double sq_dist(const point& q) const;

    point operator+(const point& p);
    void operator+=(const point& p);
    bool operator==(const point& p);
    bool operator!=(const point& p);
    void operator/=(const double x);

    static bool arr_equals(point* p, point* q, const int len);
};

class cloud {
   private:
    int d;
    int n;
    int k;

    // maximum possible number of points
    int nmax;

    point* points;
    point* centers;

   public:
    cloud(int _d, int _nmax, int _k) {
        d = _d;
        point::d = _d;
        n = 0;
        k = _k;

        nmax = _nmax;

        points = new point[nmax];
        centers = new point[k];
    }

    ~cloud() {
        delete[] centers;
        delete[] points;
    }

    void add_point(point& p, int label) {
        assert(n < nmax);

        for (int m = 0; m < d; m++) {
            points[n].coords[m] = p.coords[m];
        }

        points[n].label = label;
        n++;
    }

    void print_points() {
        for (int in = 0; in < n; in++) {
            {
                std::cout << "Point[" << in << "] = ";
                points[in].print();
            }
        }
    }

    int get_d() { return d; }

    int get_n() { return n; }

    int get_k() { return k; }

    point& get_point(int i) { return points[i]; }

    point& get_center(int j) { return centers[j]; }

    void set_center(point& p, int j) {
        for (int m = 0; m < d; m++) centers[j].coords[m] = p.coords[m];
    }

    double intracluster_variance() {
        double dist = 0;
        for (int i = 0; i < n; ++i) {
            dist += (points[i].dist(centers[points[i].label])) *
                    (points[i].dist(centers[points[i].label]));
        }
        return dist / n;
    }

    void set_voronoi_labels();
    void set_centroid_centers();

    void kmeans();
    void _kmeans_calc();

    void init_forgy();
    void init_plusplus();
    void init_random_partition();

    static double sq_dist_cluster(const point p, const point* Cs, const int l);

    double get_sihouette();

    double get_ai(int c);
    double get_bi(int c);
    double get_incluster_coeff(int i);
    double get_outcluster_coeff(int i);
};

inline int nb_columns(const std::string& line) {
    return std::count(line.begin(), line.end(), '\t') + 1;
}

int test_TD3(int argc, char** argv);

#endif  // INF442_P3_KMEANS_HPP
