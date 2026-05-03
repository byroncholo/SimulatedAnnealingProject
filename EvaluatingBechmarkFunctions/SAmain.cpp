
/*
This program implement the Simulated Annealing Algorithm in its maximization version.


// Problem:
Trivial: Find a good neigobor, so stupid 
Author : Byron Cholo Z.
Date : 4/9/2026
*/

#include <iostream>
#include <cmath>
#include <vector>
#include <random>
#include <functional>
#include <iomanip>

using namespace std;



// structure for a 2D point in the plane
struct Point {
  double x_1;
  double x_2;
};

// Output structure
struct SAResults {
Point best_point;
double best_value;
vector<Point> point_hist;
vector<double> value_hist;
int iterations;
};


// Function prototypes
double objective_function(const Point& v);
double acceptance_probability(double delta, double T);

SAResults simulatedAnnealing(
  function<double(const Point&)> f, double x_min, double x_max,
  double y_min, double y_max, double T0, double alpha,
  int N_steps, double step_size
);


// Acceptance probability
double acceptance_probability(double delta, double T){
  if (delta >= 0.0)
    return 1.0;
  else
    return exp(delta/T);  
}

// Simulated Annealing algorithm
SAResults simulatedAnnealing(
  function<double(const Point&)> f, double x_min, double x_max,
  double y_min, double y_max, double T0, double alpha,
  int N_steps, double step_size
)
{

  mt19937 gen(random_device{}());
  

  // Create the distribution objects
  uniform_real_distribution<double> dist_x(x_min, x_max);
  uniform_real_distribution<double> dist_y(y_min, y_max);
  uniform_real_distribution<double> dist_step(-step_size, step_size);
  uniform_real_distribution<double> dist_pro(0.0, 0.1);

  // Initial Solution
  Point current =  {dist_x(gen), dist_y(gen)};
  double f_current = f(current);

  // Best Solution
  Point best_point = current;
  double best_value = f_current;


  cout << fixed << setprecision(6);
  cout << "Initial point: "<< "(" << current.x_1 << ", " << current.x_2 << " )" << endl;
  cout << "Initial profit: " << f_current << endl;

  vector<Point> point_hist;
  vector<double> value_hist;

  // Store data: point and value
  point_hist.push_back(current);
  value_hist.push_back(f_current);

  // intial temperature
  double T  = T0;
  // initialize interation count
  int iterations = 0;

  for (int i  = 0; i < N_steps; i++){

    // Generate a random neighborhood
    Point candidate = {
      current.x_1 + dist_step(gen),
      current.x_2 + dist_step(gen)
    };

    double f_candidate = f(candidate);
    double delta = f_candidate - f_current;

    // Accetance criterion
    double u = dist_pro(gen);
    //cout << "Probability: "<< u << endl;
    //cout << "Acceptance probability: "<< acceptance_probability(delta, T) << endl;
    if (u < acceptance_probability(delta, T)){
      current =  candidate;
      f_current = f_candidate;
    }

    // Update best solution
    if (f_current > best_value){
      best_point = current;
      best_value = f_current;
    }

    // printing test
    //cout << "Current point: "<< "(" << current.x_1 << ", " << current.x_2 << " )" << endl;
    //cout << "Current profit: " << f_current << endl;

    point_hist.push_back(current);
    value_hist.push_back(f_current);

    T = T*alpha;
    iterations++;

    if (T < 1e-12)
      break;
  }

  return {best_point, best_value, point_hist, value_hist, iterations};
};

// Objective Fucntion definition
double objective_function(const Point& v){

  double x1 = v.x_1;
  double x2 = v.x_2;

  double term_sin = pow(sin(x1), 2) + pow(sin(x2), 2);
  double term_exp1 = exp(-(x1 * x1 + x2 * x2));
  double term_exp2 = exp(-(pow(sin(sqrt(abs(x1))), 2) + pow(sin(sqrt(abs(x2))), 2)));

  return -(1e4 * (1.0 + (term_sin - term_exp1) * term_exp2));
}




int main(){
  // Problem Setup
  double x_min = -10.0;
  double x_max = 10.0;
  double y_min = -10.0;
  double y_max = 10.0;
  double T0 = 10.0;
  double alpha = 0.99;
  int N_steps = 3000;
  double step_size = 5;

  // SA call
  SAResults results = simulatedAnnealing(
    objective_function,
    x_min, x_max,
    y_min, y_max,
    T0, alpha,
    N_steps, step_size
  );

  cout << "Number of iterations: " << results.iterations << endl;
  cout << "Best x found: " << "(" << results.best_point.x_1 << ", " << results.best_point.x_2 << " )" << endl;
  cout << "Best f(x) found: " << results.best_value << endl;
  return 0;
}

























