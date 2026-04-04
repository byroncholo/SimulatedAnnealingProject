

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


def simulatedAnnealing(f, x_min, x_max, T0, alpha, N_steps, step_size, seed):

    ########### Initial solution ##############
    x_current = np.random.uniform(x_min, x_max)
    f_current = f(x_current)

    # best solution
    x_best = x_current
    f_best = f_current

    print('x_current', x_current, 'profit', f_current)

    x_hist = [x_current]
    f_hist = [f_current]

    T = T0

    for i in range(N_steps):
        # Choose a neighborhood
        x_candidate = x_current + np.random.uniform(-step_size, step_size)

        f_candidate = f(x_candidate)

        delta = f_candidate -f_current

        # Rejection Criterion
        if np.random.rand() < acceptance_probability(delta, T):
            x_current = x_candidate
            f_current = f_candidate

        #Update best found solution

        if f_current > f_best:
            x_best = x_current
            f_best = f_current

        x_hist.append(x_current)
        f_hist.append(f_current)

        if T < 1e-12:
            break
    return x_best, f_best, x_hist, f_hist

def objective_function(x):
    return np.sin(20 * x) * np.cos(7 * x) + 0.1 * x


if __name__ == '__main__':

    # Problem Set up
    x = np.random.uniform(0, 1)

    x_min = -10
    x_max = 10
    T0=10.0
    alpha = 0.99
    N_steps = 1000
    step_size = 0.5
    seed = None
    best_x, best_f, x_data, y_data = simulatedAnnealing(objective_function, x_min,x_max, T0, alpha, N_steps, step_size, seed)

    print("Best x found:", best_x)
    print("Best f(x) found:", best_f)

    # Plot the objective function
    xs = np.linspace(-500,500, 2000)
    ys = objective_function(xs)

    plt.figure(figsize = (10,5))
    plt.plot(xs, ys, label = "f(x)")
    plt.scatter([best_x], [best_f], c='r', s=80, label="best found")
    plt.show()




