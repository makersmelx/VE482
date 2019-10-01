from pylab import *
import numpy as np
X = np.linspace(0, 13, 14, endpoint=True)
p = [0.25, 0.6, 0.9]
A = [0, 0, 0]
for i in range(0, 3):
    A[i] = 1 - p[i]**X
    tmp = str(p[i]*100) + '%'
    plot(X, A[i], label=tmp)
xlabel('n')
ylabel('CPU utilization')
legend(loc='lower right')
show()
