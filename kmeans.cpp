// kmeans.cpp, from TP3, modified
#include "kmeans.hpp"
#include <iostream>
#include <cassert>
#include <cmath>    // for sqrt, fabs
#include <cfloat>    // for DBL_MAX
#include <cstdlib>    // for rand, srand
#include <ctime>    // for rand seed
#include <fstream>
#include <cstdio>    // for EOF
#include <string>
#include <algorithm>    // for count
#include <vector>
#include <random>

//#include <gtkmm/application.h>
//#include <gtkmm/window.h>
//#include <gtkmm/drawingarea.h>


int point::d;

void point::operator/=(const double x) {
	for (int i = 0; i < d; i++) this->coords[i] /= x;
}

bool point::operator==(const point& p) {
	for (int i = 0; i < d; i++)
		if (p.coords[i] != this->coords[i]) return false;
	return true;
}

bool point::operator!=(const point& p) {
	return !(*this == p);
}

bool point::arr_equals(point* p, point* q, const int len) {
	for (int i = 0; i < len; i++)
		if (p[i] != q[i]) return false;
	return true;
}

point point::operator+(const point& p) {
	point pp;
	for (int i = 0; i < point::d; i++) {
		pp.coords[i] = this->coords[i] + p.coords[i];
	}
	return pp;
}

void point::operator+=(const point& p) {
	for (int i = 0; i < point::d; i++)
		this->coords[i] += p.coords[i];
	return;
}

point::point() {
	//	d = 0;
	coords = new double[d];
	for (int i = 0; i < d; ++i) {
		coords[i] = 0;
	}
	label = 0;
}

point::~point() {
	delete coords;
}

void point::print() {
	for (int i = 0; i < d - 1; ++i) {
		std::cout << this->coords[i] << '\t';
	}
	if (d > 0)
		std::cout << this->coords[d - 1] << std::endl;
	else std::cout << std::endl;
}


double point::sq_dist(const point& q) const {
	double dist = 0;
	for (int i = 0; i < d; ++i) {
		dist += (this->coords[i] - q.coords[i]) * (this->coords[i] - q.coords[i]);
	}
	return dist;
}


double point::dist(point& q) {
	return sqrt(this->sq_dist(q));
}


double cloud::get_sihouette() {
	double sum = 0;
	for (int i = 0; i < n; i++) {
		sum += (get_outcluster_coeff(i) - get_incluster_coeff(i)) /
			((get_outcluster_coeff(i) > get_incluster_coeff(i)) ? get_outcluster_coeff(i) : get_incluster_coeff(i));
	}
	return sum / (double)n;
}

double cloud::get_bi(int c) {
	double sum = 0;
	int count = 0;
	for (int i = 0; i < n; i++) {
		if (points[i].label == c) {
			sum += get_outcluster_coeff(i);
			count++;
		}
	}
	return sum / (double)count;
}



double cloud::get_ai(int c) {
	double sum = 0;
	int count = 0;
	for (int i = 0; i < n; i++) {
		if (points[i].label == c) {
			sum += get_incluster_coeff(i);
			count++;
		}
	}
	return sum / (double)count;
}


double cloud::get_incluster_coeff(int target_i) {
	double sum = 0;
	int count = 0;
	for (int i = 0; i < n; i++) {
		if (points[i].label == points[target_i].label) {
			sum += points[target_i].dist(points[i]);
			count++;
		}
	}
	return sum / (double)(count - 1); // dist to all the *other* samples.
}

double cloud::get_outcluster_coeff(int target_i) {
	std::vector<double>sum;
	std::vector<int>count;
	sum.resize(k); count.resize(k);
	//double sum[k];
	//int count[k];

	for (int i = 0; i < k; i++) {
		sum[i] = 0; count[i] = 0;
	}
	for (int i = 0; i < n; i++) {
		if (points[i].label != points[target_i].label) {
			sum[points[i].label] += points[target_i].dist(points[i]);
			count[points[i].label] ++;
		}
	}

	double min = DBL_MAX;
	for (int i = 0; i < k; i++) {
		if (i != points[target_i].label) {
			sum[i] /= (double)count[i];
			if (sum[i] < min) min = sum[i];
		}
	}

	return min;
}


void cloud::init_random_partition() {
	std::random_device rd;
	std::uniform_int_distribution<int> d(0, k - 1);
	for (int i = 0; i < n; i++) {
		points[i].label = d(rd);
	}
	set_centroid_centers();
}



double cloud::sq_dist_cluster(const point p, const point* Cs, const int l) {
	double dist = std::numeric_limits<double>::max();
	for (int i = 0; i < l; ++i) {
		if (dist > Cs[i].sq_dist(p)) dist = Cs[i].sq_dist(p);
	}
	return dist;
}

void cloud::init_plusplus() {
	// init
//    srand(time(nullptr));
//    int c = rand() % n;
	std::random_device rd;
	std::uniform_int_distribution<int> d(0, n - 1);
	int c = d(rd);
	set_center(points[c], 0);

	// calc proba distribution
//    std::vector<point> vec;
//    vec.push_back(points[c]);

	//double sq_dist[n];
	std::vector<double> sq_dist;
	sq_dist.resize(n);
	for (int i = 0; i < n; i++)
		sq_dist[i] = points[i].sq_dist(points[c]);

	for (int i = 1; i < k; i++) {
		double sum = 0;
		for (int j = 0; j < n; j++) sum += sq_dist[j];

		std::uniform_real_distribution<> dist(0, sum);
		double x = dist(rd);
		double tmp_sum = 0;
		int j;
		for (j = 0; j < n; j++) {
			tmp_sum += sq_dist[j];
			if (x < tmp_sum)break;
		}
		set_center(points[j], i);

		// update distance
		for (int jj = 0; jj < n; jj++) {
			if (points[jj].sq_dist(points[j]) < sq_dist[jj])
				sq_dist[jj] = points[jj].sq_dist(points[j]);
		}

	}

}

void cloud::init_forgy() {

	std::random_device rd;
	std::uniform_int_distribution<int> d(0, n - 1);

	//bool is_chosen[n];
	std::vector<bool> is_chosen;
	is_chosen.resize(n);
	for (int i = 0; i < n; i++) is_chosen[n] = false;
	//    std::srand((unsigned int) std::time(nullptr));
	int c = 0;
	for (; c < k;) {
		int choice = d(rd);
		//        std::cout<<choice<<'\n';
		if (!is_chosen[choice]) {
			set_center(points[choice], c);
			c++;
			is_chosen[choice] = true;
		}
	}
}

void cloud::kmeans() {
	//
	init_plusplus();
	//point _cen[k];
	std::vector<point> _cen;
	_cen.resize(k);
	do {
		for (int c = 0; c < k; c++)
			for (int i = 0; i < d; i++)
				_cen[c].coords[i] = centers[c].coords[i];
		set_voronoi_labels();
		set_centroid_centers();
	} while (!point::arr_equals(centers, _cen.data(), k));
}

void cloud::set_voronoi_labels() {
	for (int i = 0; i < n; i++) {
		for (int center = 0; center < k; center++) {
			if (points[i].dist(centers[points[i].label]) > points[i].dist(centers[center]))
				points[i].label = center;
		}
	}
}

void cloud::set_centroid_centers() {

	//    point sum[k];
	//    int count[k];
	//    for(int i=0;i<k;i++) count[i] = 0;
	//
	//    for(int i=0;i<n;i++){
	//        sum[points[i].label] += points[i];
	//        count[points[i].label] += 1;
	//    }
	//
	//    for(int c=0;c<k;c++){
	//        if(count[c]){
	//            sum[c] /= count[c];
	//            set_center(sum[c], c);
	//        }
	//    }
		//int lbl_count[k];
		//for (int i = 0; i < k; ++i) lbl_count[i] = 0;
	std::vector<int> lbl_count(k, 0);
	//
		// point sum[k];
	std::vector<point> sum;
	sum.resize(k);
	////	std::cout << "+++ centroid" << '\n' << "------------" << std::endl;
	////	std::cout << n <<"=n, k="<< k << '\n' << "------------" << std::endl;
	//
	for (int i = 0; i < n; i++) {
		//		std::cout << "=== "<<i <<" with lbl: " <<points[i].label << "-" << std::endl;
		//		sum[points[i].label].print();
		sum[points[i].label] += points[i];
		//		sum[points[i].label].print();
		lbl_count[points[i].label]++;
	}
	////    std::cout << "==========" << std::endl;
	//
	for (int i = 0; i < k; i++) {
		if (lbl_count[i]) {
			sum[i] /= lbl_count[i];
			//		for(int j=0; j<point::d; j++)
			//			sum[i].coords[j] /= lbl_count[i];
			//		std::cout<<i<<", ";
			//		sum[i].print();

			set_center(sum[i], i);
		}
	}
}

//void cloud::test_set_centroid_centers(){
//}

// test functions
void test_intracluster_variance() {
	// tolerance for comparison of doubles
	const double eps = 0.0001;

	// dimension used for tests
	point::d = 1;

	// temporary container
	point p;

	std::cout << "Testing function intracluster_variance()...";

	// test case 1
	const double dist_onepoint_zerodist = 0.0;
	cloud onepoint_zerodist(1, 1, 1);
	p.coords[0] = 0.0;
	onepoint_zerodist.add_point(p, 0);
	assert(std::fabs(onepoint_zerodist.intracluster_variance() - dist_onepoint_zerodist) < eps);

	// test case 2
	const double dist_onepoint_posdist = 0.25;
	cloud onepoint_posdist(1, 1, 1);
	p.coords[0] = 0.5;
	onepoint_posdist.add_point(p, 0);
	assert(std::fabs(onepoint_posdist.intracluster_variance() - dist_onepoint_posdist) < eps);

	// test case 3
	const double dist_twopoints = 0.625;
	cloud twopoints(1, 2, 1);
	p.coords[0] = -1.0;
	twopoints.add_point(p, 0);
	p.coords[0] = 0.5;
	twopoints.add_point(p, 0);
	p.coords[0] = -0.5;
	twopoints.set_center(p, 0);
	assert(std::fabs(twopoints.intracluster_variance() - dist_twopoints) < eps);

	// test case 4
	const double dist_twoclusters = 6.8125;
	cloud twoclusters(1, 4, 2);
	p.coords[0] = -1.0;
	twoclusters.add_point(p, 0);
	p.coords[0] = 0.5;
	twoclusters.add_point(p, 0);
	p.coords[0] = -0.5;
	twoclusters.set_center(p, 0);
	p.coords[0] = -2.0;
	twoclusters.add_point(p, 1);
	p.coords[0] = 2.0;
	twoclusters.add_point(p, 1);
	p.coords[0] = -3.0;
	twoclusters.set_center(p, 1);
	assert(std::fabs(twoclusters.intracluster_variance() - dist_twoclusters) < eps);

	std::cout << "\t[OK]" << std::endl;
}

void test_kmeans() {
	// TODO
}

void test_init_forgy() {
	// number of random experiments
	const int K = 10000;
	// tolerance in probability
	const double delta = 0.0625;

	// dimenstion used for tests
	point::d = 1;

	// temporary container
	point p;

	std::cout << "Testing function init_forgy()...";

	const double prob_threepoints = 0.3333;
	cloud threepoints(1, 3, 1);
	p.coords[0] = 0.0;
	threepoints.add_point(p, 0);
	p.coords[0] = 1.0;
	threepoints.add_point(p, 0);
	p.coords[0] = 2.0;
	threepoints.add_point(p, 0);
	int cnt = 0;
	for (int k = 0; k < K; k++) {
		threepoints.init_forgy();
		if (threepoints.get_center(0).coords[0] == 1.0)
			cnt++;
	}
	assert(std::fabs(cnt / (double)K - prob_threepoints) < delta);

	std::cout << "\t\t[OK]" << std::endl;
}

void test_init_plusplus() {
	// number of random experiments
	const int K = 10000;
	// tolerance in probability
	const double delta = 0.0625;

	// dimenstion used for tests
	point::d = 1;

	// temporary container
	point p;

	std::cout << "Testing function init_plusplus()...";

	// test case 1
	const double prob_threepoints = 0.3333;
	cloud threepoints(1, 3, 1);
	p.coords[0] = 0.0;
	threepoints.add_point(p, 0);
	p.coords[0] = 1.0;
	threepoints.add_point(p, 0);
	p.coords[0] = 2.0;
	threepoints.add_point(p, 0);
	int cnt = 0;
	for (int k = 0; k < K; k++) {
		threepoints.init_plusplus();
		if (threepoints.get_center(0).coords[0] == 1.0)
			cnt++;
	}
	assert(std::fabs(cnt / (double)K - prob_threepoints) < delta);

	// test case 2
	const double prob_twoclusters = 0.125;
	cloud twoclusters(1, 4, 2);
	p.coords[0] = 0.0;
	twoclusters.add_point(p, 0);
	p.coords[0] = 0.0;
	twoclusters.add_point(p, 0);
	p.coords[0] = 1.0;
	twoclusters.add_point(p, 0);
	p.coords[0] = 2.0;
	twoclusters.add_point(p, 0);
	cnt = 0;
	for (int k = 0; k < K; k++) {
		twoclusters.init_plusplus();
		if (twoclusters.get_center(1).coords[0] == 1.0)
			cnt++;
	}
	assert(std::fabs(cnt / (double)K - prob_twoclusters) < delta);

	std::cout << "\t\t[OK]" << std::endl;
}

void test_init_random_partition() {
	// number of random experiments
	const int K = 10000;
	// tolerance in probability
	const double delta = 0.0625;

	// dimenstion used for tests
	point::d = 1;

	// temporary container
	point p;

	std::cout << "Testing function init_random_parition()...";

	const double prob_threepoints = 0.3333;
	cloud threepoints(1, 3, 3);
	p.coords[0] = 0.0;
	threepoints.add_point(p, 0);
	p.coords[0] = 1.0;
	threepoints.add_point(p, 0);
	p.coords[0] = 2.0;
	threepoints.add_point(p, 0);
	int cnt = 0;
	for (int k = 0; k < K; k++) {
		threepoints.init_random_partition();
		if (threepoints.get_point(2).label == 1)
			cnt++;
	}
	assert(std::fabs(cnt / (double)K - prob_threepoints) < delta);

	std::cout << "\t[OK]" << std::endl;
}


// for graphical interface
//class MyArea : public Gtk::DrawingArea {
//private:
//    cloud *c;
//    int x_column;
//    int y_column;
//
//public:
//    MyArea(cloud *_c, int _x_column, int _y_column) {
//        c = _c;
//        x_column = _x_column;
//        y_column = _y_column;
//    }
//
//    virtual ~MyArea() {}
//
//protected:
//    //Override default signal handler:
//    bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr) override;
//};

/**
 * Counts the number of tab-separated columns in the given line.
 */


void test_point() {
	//	point::d = 2;
	point::d = 1;
	cloud test(1, 3, 2);
	point p;
	p.coords[0] = 0.0;
	p.print();
	test.add_point(p, 0);
	p.coords[0] = 1.0;
	p.print();
	test.add_point(p, 0);
	p.coords[0] = 2.0;
	test.add_point(p, 0);
	p.print();
	test.print_points();

	test.set_centroid_centers();

	//	p.print();
}

int test_TD3(int argc, char** argv) {
	test_point();
	bool run_tests = false;

	if (argc < 5 || argc > 6) {
		std::cerr << "Usage: " << argv[0] << " csv nb_clusters x_column y_column [ test ]" << std::endl;
		std::exit(1);
	}
	std::string csv_filename = argv[1];
	int nb_clusters = std::stoi(argv[2]);
	int x_column = std::stoi(argv[3]);
	int y_column = std::stoi(argv[4]);

	if (argc >= 6)
		run_tests = true;

	srand(time(NULL));

	if (run_tests) {
		test_intracluster_variance();
		test_kmeans();
		//        test_init_forgy();
		test_init_plusplus();
		test_init_random_partition();
	}

	// open data file
	std::ifstream is(csv_filename);
	assert(is.is_open());

	// read header line
	std::string header_line;
	std::getline(is, header_line);
	std::vector<std::string> names;

	const int d = nb_columns(header_line) - 1;
	const int nmax = 150;
	const int k = nb_clusters;

	// construct point cloud
	cloud c(d, nmax, k);

	// point to read into
	point p;

	// labels to cycle through
	int label = 0;

	// while not at end of file
	while (is.peek() != EOF) {
		// read new points
		for (int m = 0; m < d; m++) {
			is >> p.coords[m];
		}

		c.add_point(p, label);

		label = (label + 1) % k;

		// read ground-truth labels
		// unused in normal operation
		std::string next_name;
		is >> next_name;
		names.push_back(next_name);

		// consume \n
		is.get();
	}

	// execute k-means algorithm
	std::cout << "Intracluster variance before k-means: " << c.intracluster_variance() << std::endl;
	c.kmeans();
	std::cout << "Intracluster variance after k-means: " << c.intracluster_variance() << std::endl;
	std::cout << "Sihouette value of k=" << k << " is: " << c.get_sihouette() << std::endl;

	std::cout << "Saving clustering into \"output.csv\"... ";
	std::ofstream os("output.csv");
	assert(os.is_open());
	os << header_line << '\n';
	for (int i = 0; i < c.get_n(); ++i) {
		for (int j = 0; j < c.get_d(); ++j) {
			os << c.get_point(i).coords[j] << '\t';
		}
		os << names[i] << "_Label_" << c.get_point(i).label;
		if (i != c.get_n() - 1)
			os << '\n';
	}
	std::cout << "done" << std::endl;

	// launch graphical interface
	//int gtk_argc = 1;
	//Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(gtk_argc, argv, "inf442.td3");

	//Gtk::Window win;
	//win.set_title("TD 3");
	//win.set_default_size(400, 400);

	//MyArea area(&c, x_column, y_column);
	//win.add(area);
	//area.show();

	//return app->run(win);
}


//bool MyArea::on_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
//    Gtk::Allocation allocation = get_allocation();
//    const int width = allocation.get_width();
//    const int height = allocation.get_height();
//
//    // find min and max on each axis
//    double x_min = DBL_MAX;
//    double x_max = DBL_MIN;
//    double y_min = DBL_MAX;
//    double y_max = DBL_MIN;
//    for (int i = 0; i < c->get_n(); i++) {
//        if (c->get_point(i).coords[x_column] < x_min)
//            x_min = c->get_point(i).coords[x_column];
//
//        if (c->get_point(i).coords[x_column] > x_max)
//            x_max = c->get_point(i).coords[x_column];
//
//        if (c->get_point(i).coords[y_column] < y_min)
//            y_min = c->get_point(i).coords[y_column];
//
//        if (c->get_point(i).coords[y_column] > y_max)
//            y_max = c->get_point(i).coords[y_column];
//    }
//
//    // plot all points
//    for (int i = 0; i < c->get_n(); i++) {
//        cr->save(); // save current drawing context (opaque black)
//        cr->arc((c->get_point(i).coords[x_column] - x_min) * width / (x_max - x_min),
//                (c->get_point(i).coords[y_column] - y_min) * height / (y_max - y_min), 10.0, 0.0,
//                2.0 * M_PI); // full circle
//
//        // choose color depending on label
//        switch (c->get_point(i).label) {
//            case 0:
//                cr->set_source_rgba(1.0, 0.0, 0.0, 0.6); // red, partially translucent
//                break;
//
//            case 1:
//                cr->set_source_rgba(0.0, 0.0, 0.8, 0.6); // 0.8 blue, partially translucent
//                break;
//
//            case 2:
//                cr->set_source_rgba(0.0, 1.0, 0.0, 0.6); // green, partially translucent
//                break;
//
//            default:
//                double shade = c->get_point(i).label / (double) c->get_k();
//                cr->set_source_rgba(shade, shade, shade, 1.0); // gray
//                break;
//        }
//
//        cr->fill_preserve();
//        cr->restore();  // restore drawing context to opaque black
//        cr->stroke();
//    }
//
//    return true;
//}
