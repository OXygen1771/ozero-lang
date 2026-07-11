#pragma once

// All types of allocator operations.
// Used for OZ_ERR_UNSUPPORTED_ALLOCATION_OPERATION
typedef enum OzeroAllocationOperation {
    OZ_ALLOC_OP_ALLOC,
    OZ_ALLOC_OP_FREE,
    OZ_ALLOC_OP_REALLOC,
    OZ_ALLOC_OP_FREE_ALL,
} OzeroAllocationOperation;
