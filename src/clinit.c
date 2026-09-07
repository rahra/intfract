/* Copyright 2026 Bernhard R. Fischer, 4096R/8E24F29D <bf@abenteuerland.at>
 *
 * IntFract is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * IntFract is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with IntFract. If not, see <http://www.gnu.org/licenses/>.
 */

/* \file clinit.c
 * This file contains the initialization routines for OpenCL device.
 *
 * @author Bernhard R. Fischer, <bf@abenteuerland.at>
 * @date 2026/09/02
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "intfract.h"


#ifdef USE_OPENCL
/*! Get a valid OpenCL device id.
 */
cl_device_id init_cl_dev()
{
   cl_platform_id platform;
   cl_device_id dev;

   /* Identify a platform */
   if (clGetPlatformIDs(1, &platform, NULL) != CL_SUCCESS)
      fprintf(stderr, "clGetPlatformIDs() failed\n"), exit(1);

   // get GPU
   if (clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL) == CL_SUCCESS)
      return dev;
   
   // get CPU if getting GPU failed
   if (clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL) == CL_SUCCESS)
      return dev;

   fprintf(stderr, "clGetDeviceIDs() failed\n"), exit(1);
}


/*! Load OpenCL program from a file and compile it.
 */
cl_program init_cl_program(cl_context ctx, cl_device_id dev, const char *clsrc)
{
   cl_program program;
   struct stat st;
   char *buf;
   int fd, len;
   cl_int err;

   // open cl source file
   if ((fd = open(clsrc, O_RDONLY)) == -1)
      perror("open()"), exit(1);

   // get file meta data
   if (fstat(fd, &st) == -1)
      perror("fstat()"), exit(1);

   if ((buf = malloc(st.st_size)) == NULL)
      perror("malloc()"), exit(1);

   if ((len = read(fd, buf, st.st_size)) == -1)
      perror("read()"), exit(1);

   close(fd);

   if (len < st.st_size)
      fprintf(stderr, "read() truncated\n"), exit(1);

   program = clCreateProgramWithSource(ctx, 1, (const char**) &buf, (size_t*) &st.st_size, &err);
   if (err != CL_SUCCESS)
      fprintf(stderr, "clCreateProgramWithSource() failed\n"), exit(1);
   
   free(buf);

   if (clBuildProgram(program, 0, NULL, "-I .", NULL, NULL) != CL_SUCCESS)
   {
      size_t log_size;

      clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

      if ((buf = malloc(log_size)) == NULL)
         perror("malloc()"), exit(1);

      clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 
            log_size, buf, NULL);

      printf("%.*s\n", (int) log_size, buf);
      free(buf);

      exit(1);
   }

   return program;
}


/*! Initialize everything for OpenCL. This function is called in intfract.c.
 * @return The function returns a pointer to a if_cl_t structure (defined in
 * intfract.h) which holds several handles for executing programs on the OpenCL
 * device. This function always returns a valid pointer. If an error occurs,
 * the function exits the program with exit code 1.
 */
if_cl_t *prepare_cl(void)
{
   if_cl_t *ifcl;
   cl_int err;

   if ((ifcl = malloc(sizeof(*ifcl))) == NULL)
      perror("malloc()"), exit(1);

   /* Create device and context   */
   ifcl->device = init_cl_dev();
   if ((ifcl->context = clCreateContext(NULL, 1, &ifcl->device, NULL, NULL, &err)) == NULL)
      fprintf(stderr, "clCreateContext() failed: %d\n", err), exit(1);

   /* Build program */
   ifcl->program = init_cl_program(ifcl->context, ifcl->device, "iterate.cl");

   /* Create a command queue */
   if ((ifcl->queue = clCreateCommandQueueWithProperties(ifcl->context, ifcl->device, 0, &err)) == NULL)
      fprintf(stderr, "clCreateCommandQueueWithProperties() failed: %d\n", err), exit(1);

   return ifcl;
}


/*! Release all OpenCL handles.
 * @param ifcl Pointer to if_cl_t structure as initialized by prepare_cl().
 */
void release_cl(if_cl_t *ifcl)
{
   clReleaseCommandQueue(ifcl->queue);
   clReleaseProgram(ifcl->program);
   clReleaseContext(ifcl->context);
   free(ifcl);
}
#endif
