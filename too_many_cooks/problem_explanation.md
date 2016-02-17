# Explanations
### Mutexes & condition variables
  - In what cases can a deadlock occur? Is there a reliable way to predict a deadlock in advance and avoid such scenarios?
    The deadlock may occurs when two recipe both trying to access next stages of the other's. The solution is to predict, wait and skip it.

  - What is the most fair way to avoid starvation? In what cases is it reasonable to sacrifice performance/parallelism to avoid starvation?
    Starvation occurs when chef wait for ever. 
