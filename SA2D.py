

"""
This program implement the Simulated Annealing Algorithm (maximization version)
Author: Byron Cholo
"""
import numpy as np
import matplotlib.pyplot as plt
import matplotlib
matplotlib.use('TkAgg')

## Acceptance Criterion
def acceptance_probability(delta, T):
    """
    Acceptance probability for maximization
    delta = f_new - f_current
    """
    if delta >= 0:
        return 1.0
    else:
        return np.exp(delta/T)


def simulatedAnnealing(f, x_min, x_max, y_min, y_max, T0, alpha, N_steps, step_size, seed=None):

    ########### Initial solution ##############
    current = np.array([np.random.uniform(x_min, x_max),
                np.random.uniform(y_min, y_max)
                        ])
    f_current = f(current)

    ################# best solution #################
    best_point = current.copy()
    best_value = f_current
    print("Initial point", current, "profit:", f_current)

    point_hist = [current.copy()]
    value_hist = [f_current]

    T = T0
    iterations = 0

    for i in range(N_steps):
        # Random neighbor
        candidate = current + np.random.uniform(-step_size, step_size, size=2)

        f_candidate = f(candidate)
        delta = f_candidate -f_current

        # Rejection Criterion
        if np.random.rand() < acceptance_probability(delta, T):
            current = candidate
            f_current = f_candidate

        #Update best found solution

        if f_current > best_value:
            best_point = current.copy()
            best_value = f_current

        point_hist.append(current.copy())
        value_hist.append(f_current)

        T = T*alpha
        iterations += 1

        if T < 1e-6:
            break
    return best_point, best_value, point_hist, value_hist, iterations
# Function f1
# def objective_function(v):
#     x1, x2 = v
#     return -(x1 ** 2 + x2 ** 2 - np.cos(18 * x1) - np.cos(18 * x2))


## Function f2
# def objective_function(v):
#     x1, x2 = v
#     term1 = (1 + (x1 + x2 + 1)**2 * (19 - 14*x1 + 3*x1**2 - 14*x2 + 6*x1*x2 + 3*x2**2))
#     term2 = (30 + (2*x1 - 3*x2)**2 * (18 - 32*x1 + 12*x1**2 + 48*x2 - 36*x1*x2 + 27*x2**2))
#     return -(term1*term2)

# Function f3
# def objective_function(v):
#
#     return - (100 * (x2 - x1 ** 2) ** 2 + (1 - x1) ** 2)

def objective_function(v):
    x1, x2 = v
    term_sin = np.sin(x1)**2 + np.sin(x2)**2
    term_exp_center = np.exp(-(x1**2 + x2**2))
    term_exp_outer = np.exp(-(np.sin(np.sqrt(np.abs(x1)))**2 + np.sin(np.sqrt(np.abs(x2)))**2))
    return -(1e4 * (1 + (term_sin - term_exp_center) * term_exp_outer))




if __name__ == '__main__':

    # Problem Set up
    x = np.random.uniform(0, 1)
    # Define the support
    x_min = -5
    x_max = 5
    y_min = -5
    y_max = 5
    T0 = 10.0
    alpha = 0.99
    N_steps = 2000
    step_size = 5
    seed = None
    best_point, best_value, point_data, value_data, iterations = simulatedAnnealing(objective_function, x_min,x_max, y_min, y_max, T0, alpha, N_steps, step_size, seed)

    print("Number of iterations: ", iterations)
    print("Best x found:", best_point)
    print("Best f(x) found:", best_value)

    # Plot the objective function surface
    x_vals = np.linspace(x_min,x_max, 2000)
    y_vals = np.linspace(y_min, y_max, 2000)
    X, Y = np.meshgrid(x_vals, y_vals)
    w = np.array([X,Y])

    Z = objective_function(w)

    point_hist_arr = np.array(point_data)
    # Plot
    plt.figure(figsize = (8, 6))
    contour = plt.contourf(X,Y,Z, levels = 60)
    plt.colorbar(contour)

    plt.plot(point_hist_arr[:,0], point_hist_arr[:,1], 'b.-', linewidth=1, markersize = 2, label = 'SA path')
    plt.scatter(best_point[0], best_point[1], c='r', s=80, label='best found')

    plt.xlabel('x')
    plt.ylabel('y')
    plt.show()

    # Objective value history
    plt.figure(figsize = (8, 6))
    plt.plot(value_data)

    plt.xlabel("Iteration")
    plt.ylabel("Objective value")
    plt.title("Evolution of objective value")
    plt.grid(True)
    plt.show()






