#pragma once

#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>
#include <vector>
#include <memory>
#include "read.hpp"
#include "chunk.hpp"
#include <Eigen/Core>

class OpenCLContext {
public:
	OpenCLContext();

	void initialise(std::string source_path);

	void update_cells(Eigen::Array < unsigned int, Chunk::rows, Chunk::columns, Eigen::RowMajor >& neighbour_count, Eigen::Array < bool, Chunk::rows, Chunk::columns, Eigen::RowMajor >& cells);

	bool success(cl_int errcode_ret);
	
	void initialise_cells_buffer();

	void run_update_cells_kernel();
	
	bool is_valid_context;

 	cl_platform_id platform_id;
	cl_device_id device_id;
	cl_context context;
	cl_command_queue command_queue;
	cl_program program;


	cl_kernel update_cells_kernel;
	constexpr static size_t buffer_size = Chunk::rows * Chunk::columns;
	cl_mem neighbour_count_buffer;
	cl_mem cells_result_buffer;
	cl_mem cells_input_buffer;
	cl_uint* update_cells_kernel_result;
	size_t update_cells_result_size;
};