################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/video/d3d9/gfx.c 

OBJS += \
./src/video/d3d9/gfx.o 

C_DEPS += \
./src/video/d3d9/gfx.d 


# Each subdirectory must supply rules for building sources it contributes
src/video/d3d9/%.o: ../src/video/d3d9/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	x86_64-w64-mingw32-gcc -DD3D9 -I../src -I../src/core -I../src/gui -I../src/gui/windows -I../src/video/d3d9 -I/usr/x86_64-w64-mingw32/usr/include/qt4 -I/home/fhorse/sviluppo/personale/punes/misc/DXSDK/Include -I/home/fhorse/sviluppo/personale/punes/misc/DXSDK/vc10/include -O3 -Wall -ffast-math -mmmx -msse -msse2 -msse3 -mfpmath=sse -c -fmessage-length=0 -finline-functions -Winline -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


