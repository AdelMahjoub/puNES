################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/openGL/cube3d.c \
../src/openGL/no_effect.c 

OBJS += \
./src/openGL/cube3d.o \
./src/openGL/no_effect.o 

C_DEPS += \
./src/openGL/cube3d.d \
./src/openGL/no_effect.d 


# Each subdirectory must supply rules for building sources it contributes
src/openGL/%.o: ../src/openGL/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	x86_64-w64-mingw32-gcc -DMINGW64 -DGLEW_STATIC -I../src -I/usr/x86_64-w64-mingw32/usr/include/SDL -O3 -Wall -ffast-math -c -fmessage-length=0 -finline-functions -Winline -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


