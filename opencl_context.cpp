#pragma once

#include "opencl_context.hpp"
#include <iostream>


using Eigen::Array;
using Eigen::Dynamic;

OpenCLContext::OpenCLContext() : 
is_valid_context(false)
{
	
}

bool OpenCLContext::success(cl_int errcode_ret) {
	return errcode_ret == CL_SUCCESS;
}



void OpenCLContext::initialise(std::string source_path) {
	/*
	cl_int errcode_ret;
	
	clGetPlatformIDs(1, &platform_id, NULL);

	clGetDeviceIDs(platform_id,
	               CL_DEVICE_TYPE_GPU,
	               1,
	               &device_id, 
	               NULL);
	context = clCreateContext(NULL,
	                          1,
	                          &device_id,
	                          NULL, 
	                          NULL,
	                          NULL);
	
#if CL_TARGET_OPENCL_VERSION >= 200 
		const cl_queue_properties queue_properties = CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE;
		command_queue = clCreateCommandQueueWithProperties(context, device_id, &queue_properties, &errcode_ret);
#else
		cl_command_queue_properties queue_properties = CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE;
		command_queue = clCreateCommandQueue(context, device_id, queue_properties, &errcode_ret);
#endif
	if(!success(errcode_ret)) {
		std::cout<< "OPENCL_CONTEXT::INITIALISE(). Error creating command queue.\n";
		is_valid_context = false;
		return;
	}

	std::string file_contents = read_from_file_into_std_string(source_path);
	const char* source = file_contents.c_str();
	std::cout << "file contents: \n" << source << "\n";
	program = clCreateProgramWithSource( context,
	                                               1,
	                                               &source,
	                                               NULL, NULL );

	clBuildProgram(program, 1, &device_id, NULL, NULL, NULL );

	update_cells_kernel = clCreateKernel(program, "update_cells", NULL);


	initialise_cells_buffer();

	is_valid_context = true;
	*/
}


void OpenCLContext::initialise_cells_buffer() {	
	cells_result_buffer = clCreateBuffer(
		context,
		CL_MEM_WRITE_ONLY, buffer_size * sizeof(bool),
		NULL, 
		NULL);

	clSetKernelArg(
		update_cells_kernel,
		0, 
		sizeof(cl_mem), 
		(void*) &cells_result_buffer);

	neighbour_count_buffer = clCreateBuffer(context, 
	                                        CL_MEM_READ_ONLY, buffer_size * sizeof(cl_uint), 
	                                        NULL, 
	                                        NULL);
	clSetKernelArg(
		update_cells_kernel,
		1, 
		sizeof(cl_mem), 
		(void*) &neighbour_count_buffer);

	cells_input_buffer = clCreateBuffer(
		context,
		CL_MEM_READ_ONLY, buffer_size * sizeof(bool),
		NULL, 
		NULL);

	clSetKernelArg(
		update_cells_kernel,
		2, 
		sizeof(cl_mem), 
		(void*) &cells_input_buffer);
}



void OpenCLContext::update_cells(Array < unsigned int, Chunk::rows, Chunk::columns, Eigen::RowMajor >& neighbour_count, Array < bool, Chunk::rows, Chunk::columns, Eigen::RowMajor >& cells) {
	ZoneScoped;
	unsigned int* neighbour_count_data = neighbour_count.data();
	/*
		cl_uint neighbour_count_data_converted[buffer_size];
	for (int i = 0; i < neighbour_count.size(); i++) {
		neighbour_count_data_converted[i] = (cl_uint) neighbour_count_data[i];
	}
	*/
	clEnqueueWriteBuffer(command_queue, 
	                     neighbour_count_buffer,
	                     CL_FALSE,
	                     0,
	                     sizeof(cl_uint) * buffer_size,
	                     (void *) neighbour_count_data,
	                     0,
	                     NULL,
	                     NULL);
		

	bool* cells_input_data = cells.data();
	clEnqueueWriteBuffer(command_queue, 
	                     cells_input_buffer,
	                     CL_FALSE,
	                     0,
	                     sizeof(bool) * buffer_size,
	                     (void *) cells_input_data,
	                     0,
	                     NULL,
	                     NULL);

	clFinish(command_queue);

	clEnqueueNDRangeKernel(
		command_queue, 
		update_cells_kernel,
		1,
		NULL,
		&buffer_size,
		NULL,
		0,
		NULL, 
		NULL
	);

	// blocking read
	bool cells_output_data[buffer_size];
	clEnqueueReadBuffer(command_queue,
	                    cells_result_buffer,
	                    CL_FALSE,
	                    0,
	                    sizeof(bool) * buffer_size,
	                    (void *) cells_output_data,
	                    0,
	                    NULL,
	                    NULL);
	

	/*
	std::cout << "########################\n";
	for (int r = 0; r < Chunk::rows; r++) {
		for (int c = 0; c < Chunk::columns; c++) {
			std::cout << cells_output_data[r*Chunk::columns + c] << " ";
		}
		std::cout << "\n";
	}
	std::cout << "########################\n";
	*/

	for (int r = 0; r < Chunk::rows; r++) {
		for (int c = 0; c < Chunk::columns; c++) {
			cells(r, c) = cells_output_data[r*Chunk::columns + c];
		}
	}
}