# SyDEVS-yaml

This Framework can be used to create and describe a simulation with a `.yaml` file and run it without the need to
recompile every time a change is needed.

This project was created to be a connecting peace between a GUI and [SyDEVS](https://autodesk.github.io/sydevs/) and to eliminate the need to recompile the source code
every time connection between nodes or settings of nodes change.

It will be used in my master thesis, so not all functionalities are implemented and tested to its hearts content. Please be aware of this.
If you have idea, problems i want to invite you to create a issue!

In difference to SyDEVS this project is a **C++17** project.

## The idea
At the beginning there were two usecases.

1. A library of Nodes are given to a user and the user can change and rearrange the nodes as he please. To run this simulation there is no need to generate C++ Code and compile it. Only a `.yaml`/`.yml` will be generated and this library provides method to run it.
2. In addition to 1. Nodes can be completely written in LUA and loaded and run on the fly.

## What can SyDEVS-yaml?
As stated above the implementation is not complete and tested. So you can lockup here if the feature is currently implemented

* Create GenericNodes
    * GenericAtomicNode - Class `generic_atomic_node`
    * GenericCollectionNode - Class `generic_closed_system`
* Link Ports in and with a GenericCollectionNode
* Load and Run `.lua` for each Node and the GenericInteractivSystem(`generic_interactic_system`)    
    * set/get the value of the Ports of current Port.
    * check if a new Value is set at a Port
    * Set Initial values for the flow_input ports for the main `generic_closed_system`
    
### What SyDEVS-yaml can't do
Currently the following things are not implemented or a not in current focus of my work:
* Manny quality of life function of SyDEVS are currently not callable from LUA. An Example are the `sydevs/core/units.h` and `sydevs/core/quantity.h`
* Use the `run_simulation.h` to run a normal simulation. The stubs are created, but aren't implemented yet.
* Every thing which is printed in lua ignore the given output stream

### What will come?
For the use in my master thesis these feature are bound to implemented - probably:
* Register your own, in C++ written Nodes, and use them within the the `.yaml`
    * Within this, there will be the option to declare config_parameter. These will also be settable with in the `.yaml`
* Run a normal simulation with the SyDEVS-yaml
* Set the log level
* Initialize the value of ports within in the `.yaml`
* Change the name of the main `composite_node`/`generic_closed_system`
    
## Example / How to use .yaml
In the folder `example` there is a really simple example. The simulation it self only use two atomic nodes. 
One which counts up every second and the other one which sum up all the values of the counter up till now.

Paths, if they are relative paths, always interpreted from the point of the file. It the path is in C++, 
the it will be use the path of the executable and a path in the `.yaml` with use the path of the `.yaml` as its root.

The example use the following files, and the SyDEVS-yaml.
```text
- example
| - counter
| | - lua
| | | - adder.lua
| | | - counter.lua
| | | - simulation.lua
| | - s.yaml
| - main.cpp
```

The main.cpp is really small. It only set the path to `s.yaml` and start the simulation

```c++
#include <generics/run_simulation.h>
#include <iostream>
using namespace sydevs;

int main(int argc, const char* argv[])
{
    sydevs::generics::realtime rt("example/s.yaml");
    rt.mainloop(20_s, 0, std::cout, true); //duration of the simulation, seed, ostream for the output, if update_time_synchronization
    return 0;
}
```
### Describe a simulation with `.yaml`
#### Bare minimum
The YAML-file is a little bit bigger so i will divide it in section. All node names must be uniq in the simulation! The port names must be uniq only in the same node.

Before i describe the example. I will show you what is the complete minimum to create a runable simulation:

```yaml
name: 'bouncing_ball'
count_agent: 2
lua: 'lua/simulation.lua'
time_precision: unit
simulation_main_node: 'main'
composite_nodes:
  - name: 'main'
    time_precision: unit
    nodes:
    ports:
    connections:
```

Every simulation must have a composite_node with the name `main`. In this node the other nodes will be placed and ports linked. Currently there is no way to replace this node with a note of an other type.

#### The Example

The first thing in the configuration the the simulation itself:

```yaml
name: 'bouncing_ball' # currently ignored
count_agent: 1 # currently ignored
lua: 'lua/simulation.lua' # main use is to set the init value of the ports
time_precision: unit
simulation_main_node: 'main' # must be a name of a collection_node! Currently IGNORDED!
real_time: true # currently ignored
```

A `generic_atomic_node` with the name `adder` and the lua file `adder.lua` can be described as follow:

```yaml
- name: 'adder' # name of the node
    time_precision: unit
    lua: 'lua/adder.lua'
    ports: # The number of ports is not limited
      - port_type: flow # port_type
        direction: input # port_direction
        data_type: int # currently ignored
        name: "starter" # name of the port
      - port_type: message
        direction: input
        data_type: int
        name: "adder"
      - port_type: message
        direction: output
        data_type: int
        name: "result"
```
And with a secound node `counter` the YAML looks likes this:

```yaml
name: 'bouncing_ball'
count_agent: 2
lua: 'lua/simulation.lua'
time_precision: unit
simulation_main_node: 'main'
real_time: true
atomic_nodes:
  - name: 'adder'
    time_precision: unit
    lua: 'lua/adder.lua'
    ports:
      - port_type: flow
        direction: input
        data_type: int
        name: "starter"
      - port_type: message
        direction: input
        data_type: int
        name: "adder"
      - port_type: message
        direction: output
        data_type: int
        name: "result"
  - name: 'counter'
    time_precision: unit
    lua: 'lua/counter.lua'
    ports:
      - port_type: flow
        direction: input
        data_type: int
        name: 'starter'
      - port_type: message
        direction: output
        data_type: int
        name: 'counter'
```

The last thing is the describe the `main` `composite_node` with its ports and link the ports of `main` with the ports of the `atomic_node` we described beforehand.

The declaration of the node is similar to the other nodes. A `composite_node` has no lua file, because it cant run any code. 

```yaml
  - name: 'main'
    time_precision: unit
    nodes: # List all node names with are IN this node. Only the listed will be initialized
      - adder
      - counter
    ports: # This Ports can be used in the simulation.lua / interactive_system
      - port_type: flow # This type of port must be set with a value to create the simulation! 
        direction: input
        data_type: int
        name: 'init'
      - port_type: message
        direction: output
        data_type: int
        name: 'adder'
      - port_type: message
        direction: output
        data_type: int
        name: 'counter'
```

Now we need to describe the connection between the ports. For this SyDEVS-yaml use a special syntax.
```yaml
FROM_NODE_NAME.FROM_PORT_NAME > TO_NODE_NAME.TO_PORT_NAME
```

For Example `main.init > counter.starter` links the Port `init` of the node `main` with the port `starter` of the node `counter`. SyDEVS-yaml will check if the ports can be connected, otherwise a exception will be thrown, but it will not check if the type of the ports are compatible!

After this the complete yaml file look like this:

```yaml
name: 'bouncing_ball'
count_agent: 2
lua: 'lua/simulation.lua'
time_precision: unit
simulation_main_node: 'main'
real_time: true
atomic_nodes:
  - name: 'adder'
    time_precision: unit
    lua: 'lua/adder.lua'
    ports:
      - port_type: flow
        direction: input
        data_type: int
        name: "starter"
      - port_type: message
        direction: input
        data_type: int
        name: "adder"
      - port_type: message
        direction: output
        data_type: int
        name: "result"
  - name: 'counter'
    time_precision: unit
    lua: 'lua/counter.lua'
    ports:
      - port_type: flow
        direction: input
        data_type: int
        name: 'starter'
      - port_type: message
        direction: output
        data_type: int
        name: 'counter'
composite_nodes:
  - name: 'main'
    time_precision: unit
    nodes:
      - adder
      - counter
    ports:
      - port_type: flow
        direction: input
        data_type: int
        name: 'init'
      - port_type: message
        direction: output
        data_type: int
        name: 'adder'
      - port_type: message
        direction: output
        data_type: int
        name: 'counter'
    connections:
      - main.init > counter.starter
      - main.init > adder.starter
      - counter.counter > adder.adder
      - adder.result > main.adder
      - counter.counter > main.counter
```

### Use LUA in the simulation
SyDEVS-yaml make the following object/things from SyDEVS callebal from lua.

Types:
* `generic_port` 
    * `set_port(PORT_NAME, PORT_VALUE)`
    * `port_received(PORT_NAME)`
    * `get_string_port(PORT_NAME)`
    * `get_double_port(PORT_NAME)`
    * `get_duration_port(PORT_NAME)`
    * `get_bool_port(PORT_NAME)`

The ports of the current node is in the object `ports` and for example a port can be set with the following command:
```lua
ports:set_port("init",any.new(2.0))
```
Import is the `any.new(VALUE)`. It represent the std::any with is used in the inner code of SyDEVS-yaml. `VALUE` can be a `string`, `int` or `double`.

* `scale` - from `sydevs/core/scale.h`
with the following representations already translated to lua:
```lua
scales = {
    yocto = scale.new(-8),
    zepto = scale.new(-7),
    atto = scale.new(-6),
    femto = scale.new(-5),
    pico = scale.new(-4),
    nano = scale.new(-3),
    micro = scale.new(-2),
    milli = scale.new(-1),
    unit = scale.new(0),
    kilo = scale.new(1),
    mega = scale.new(2),
    giga = scale.new(3),
    tera = scale.new(4),
    peta = scale.new(5),
    exa = scale.new(6),
    zetta = scale.new(7),
    yotta = scale.new(8)
}
```

* `duration` - a `quantity` from `sydevs/core/quantity.h` - the function name are identical to SyDEVS
    * `fixed_at(SCALE)`
    * `multiplier`
    * `precision`
    * `to_number`

#### 


### Write your own node in C++ and use it
Currently not implemented

## What can be and Future
Currently LUA is used a truly generic simulation. If C++ and Clang is used instead functionalities of SyDEVS can be used without a problem. LUA it self was just used to get this project going.

## Projects used in this project
* [Lua](https://github.com/lua/lua)
* [lua-cmake](https://github.com/weberja/lua-cmake) by [lubgr](https://github.com/lubgr/lua-cmake)
* [Registerer](https://github.com/Xadeck/Registerer)
* [sol2 v3](https://github.com/ThePhD/sol2)
* [SyDEVS](https://autodesk.github.io/sydevs/)
* [yaml-cpp](https://github.com/jbeder/yaml-cpp)