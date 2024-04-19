--[============================================================[
        JKRGUI v2 - ALL RIGHT RESERVED (c)

*userdata - means table in which you cannot insert elements.
    This is similar to class in C++

*factory function - These are the functions that are prefixed
    CreateXXXX, that means you are creating an object.
    *A table is returned by the factory function,

Notes:
1. All the factory functions that is not in the namespace Jkr,
    (is local to this file), will return userdata, that is
    you cannot extend the table. And those which are in the
    Jkr namespace will return a table which can be extended
    with your functionality.


CODING STANDARDS
    -- always  for member functions that are not meant to be used
            use "m" prefix, like mNumber, mComplex
    -- if the argument type is a table make it plural
            like inNumbers, inKeyframes etc


CREATING A FACTORY (CLASS PRODUCER)

Namespace.CreateCLASSNAME = function(inArgument1, inArgument2)
    local o = {}
    o.mArgument1 = inArgument1
    o.Argument2 = inArgument2

    o.AFunction = function()
        -- What the function does
    end

    return o
end


]============================================================]
