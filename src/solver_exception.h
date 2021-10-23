#ifndef SOLVER_EXCEPTION_H
#define SOLVER_EXCEPTION_H

#include <stdexcept>

class PropagationException : public std::runtime_error{
    using std::runtime_error::runtime_error;
};

class TrueClauseException : public PropagationException{
    using PropagationException::PropagationException;
};

class NoLiteralToPropagateException : public PropagationException{
    using PropagationException::PropagationException;
};

class ConflictClauseException : public std::runtime_error{
    using std::runtime_error::runtime_error;
};


#endif