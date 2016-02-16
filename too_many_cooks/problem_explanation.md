# Explanations
### Mutexes & condition variables
  - In what cases can a deadlock occur? Is there a reliable way to predict a deadlock in advance and avoid such scenarios?
    The deadlock may occurs when order is arriving too slow and all chefs when to sleep. The solution is for customer to wake up chefs.
    
  - What is the most fair way to avoid starvation? In what cases is it reasonable to sacrifice performance/parallelism to avoid starvation?
    Starvation occurs when orders arrive too fast than chefs can handle. The best way to avoid starvation is to add more chefs or tell customers to sleep for a moment.
