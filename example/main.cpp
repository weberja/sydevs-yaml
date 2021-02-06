#include "nodes/adder_node.h" // To register the node, it need to be included!

#include <sydevs-yaml/generics/run_simulation.h>
#include <iostream>

using namespace sydevs;

int main(int argc, const char* argv[])
{
    sydevs::generics::realtime rt("example/s.yaml");
    rt.mainloop(20_s, 0, std::cout, false);
    return 0;
}