#include "ir_base.hpp"
#include "parser.hpp"

class Driver : virtual public Ir_base, virtual public Parser {
    class Driver(std::istream& inS)
        : Ir_base(inS)
    {
    }
};