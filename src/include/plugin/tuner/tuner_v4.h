/*************************************************************************
 * Copyright (c) 2023, NVIDIA CORPORATION. All rights reserved.
 * Copyright (c) 2023, Meta Platforms, Inc. and affiliates.
 *
 * See LICENSE.txt for license information
 ************************************************************************/

#ifndef TUNER_V4_H_
#define TUNER_V4_H_

// API to be implemented by external tuner
typedef struct {
  // Name of the tuner
  const char* name;

  // Initializes tuner states.
  // Inputs:
  //   - nRanks: number of ranks in current communicator. Each communicator initialize its own tuner.
  //   - nNodes: number of nodes in current communicator.
  //   - logFunction: a logFunction can be useful to integrate logging together with NCCL core.
  // Outputs:
  //   - context: tuner context object
  ncclResult_t (*init)(size_t nRanks, size_t nNodes, ncclDebugLogger_t logFunction, void **context);

  // Gets info (algo, protocol, number of ctas and threads) for a given collective.
  // Inputs:
  //   - context: tuner context object
  //   - collType: collective type , e.g., allreduce, allgather…
  //   - nBytes: collective size in bytes
  //   - numPipeOps: number of operations in the group
  //   - numAlgo: number of algorithms in collCostTable
  //   - numProto: number of protocols in collCostTable
  //   - regBuff: can register user buffer
  //
  // Outputs:
  //   - nChannels: number of channels (hence SMs) to be used.
  //
  // InOut:
  //   - collCostTable: collective cost table, generated by NCCL core, containing algo|proto|time entries for collType.
  //                    NCCL core sets ignored algo/proto cost table entries to -1.0 (NCCL_ALGO_PROTO_IGNORE).
  //
  // If getCollInfo() does not return ncclSuccess, NCCL will fall back to the
  // default tuning for the given collective.
  // Also, the plugin is allowed to not set any output, or set only the
  // algorithm and protocol, but not only the algorithm or only the protocol.
  // Unset fields will be set automatically by NCCL.
  ncclResult_t (*getCollInfo)(void* context, ncclFunc_t collType, size_t nBytes,
                              int numPipeOps, float** collCostTable, int numAlgo, int numProto,
                              int regBuff, int* nChannels);

  // Terminates the plugin and cleans up any resources that the plugin allocated.
  // context: tuner context object
  ncclResult_t (*destroy)(void* context);
} ncclTuner_v4_t;

#endif
