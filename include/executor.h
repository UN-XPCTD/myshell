/*
 * FILE: executor.h
 * Purpose: interface for running pipelines
 */

#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "types.h"

//Execute a parsed pipeline
void executor_run(pipeline_t *pipeline);

#endif
