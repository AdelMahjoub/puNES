################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/video/sdl/gfx.c \
../src/video/sdl/opengl.c 

OBJS += \
./src/video/sdl/gfx.o \
./src/video/sdl/opengl.o 

C_DEPS += \
./src/video/sdl/gfx.d \
./src/video/sdl/opengl.d 


# Each subdirectory must supply rules for building sources it contributes
src/video/sdl/%.o: ../src/video/sdl/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	i686-w64-mingw32-gcc -DSDL -DGLEW_STATIC -I../src -I../src/core -I../src/gui -I../src/gui/windows -I../src/video/sdl -I/usr/i686-w64-mingw32/usr/include/SDL -I/usr/i686-w64-mingw32/usr/include/qt4 -I/home/fhorse/sviluppo/personale/punes/misc/DXSDK/Include -I/home/fhorse/sviluppo/personale/punes/misc/DXSDK/vc10/include -O3 -Wall -ffast-math -mmmx -msse -msse2 -msse3 -mfpmath=sse -c -fmessage-length=0 -finline-functions -Winline -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


