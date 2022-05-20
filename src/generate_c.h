#ifndef GENERATE_C_H
#define GENERATE_C_H

#include "common.h"
#include "ast.h"
#include "core/str.h"

Str generate_c(const ExprArray ast);

#endif // GENERATE_C_H
