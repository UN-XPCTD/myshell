/**
 * @file executor.h
 * @brief Executor interface for running pipelines.
 */

#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "types.h"

/**
 * @brief Execute a parsed pipeline.
 *
 * Handles built-in commands, single external commands, and multi-stage
 * pipelines. Applies I/O redirection and background execution as needed.
 *
 * @param pipeline The pipeline to execute.
 */
void executor_run(pipeline_t *pipeline);

#endif
