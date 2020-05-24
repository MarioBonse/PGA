# PGA
A framework for implement genetic algorithm with parallel programming
## Usage
In order to use this framework you have to implement your own implementation of the class agnet with these method and object(given your application):
`fitness ->`
`simluate: method that given a DNA and an anviroment silmulate the life and return a fitness (or value)`
`policy: function that given a list of agents implenets the policy that, usually using the fitness, returns a list of pair(agents, probability) that we will use in order to pick the random parents for the reprudiction fase`
`reprudction: given the list above, after the polciy return the new population for the simulation`
