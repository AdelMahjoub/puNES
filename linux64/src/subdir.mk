################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/apu.c \
../src/audio_quality.c \
../src/blip_buf.c \
../src/cfg_file.c \
../src/cmd_line.c \
../src/cpu.c \
../src/emu.c \
../src/external_calls.c \
../src/fds.c \
../src/fps.c \
../src/gamegenie.c \
../src/ines.c \
../src/input.c \
../src/irqA12.c \
../src/irql2f.c \
../src/main.c \
../src/mappers.c \
../src/opengl.c \
../src/ppu.c \
../src/savestate.c \
../src/sdlgfx.c \
../src/sdlsnd.c \
../src/sdltext.c \
../src/sha1.c \
../src/tas.c \
../src/timeline.c 

OBJS += \
./src/apu.o \
./src/audio_quality.o \
./src/blip_buf.o \
./src/cfg_file.o \
./src/cmd_line.o \
./src/cpu.o \
./src/emu.o \
./src/external_calls.o \
./src/fds.o \
./src/fps.o \
./src/gamegenie.o \
./src/ines.o \
./src/input.o \
./src/irqA12.o \
./src/irql2f.o \
./src/main.o \
./src/mappers.o \
./src/opengl.o \
./src/ppu.o \
./src/savestate.o \
./src/sdlgfx.o \
./src/sdlsnd.o \
./src/sdltext.o \
./src/sha1.o \
./src/tas.o \
./src/timeline.o 

C_DEPS += \
./src/apu.d \
./src/audio_quality.d \
./src/blip_buf.d \
./src/cfg_file.d \
./src/cmd_line.d \
./src/cpu.d \
./src/emu.d \
./src/external_calls.d \
./src/fds.d \
./src/fps.d \
./src/gamegenie.d \
./src/ines.d \
./src/input.d \
./src/irqA12.d \
./src/irql2f.d \
./src/main.d \
./src/mappers.d \
./src/opengl.d \
./src/ppu.d \
./src/savestate.d \
./src/sdlgfx.d \
./src/sdlsnd.d \
./src/sdltext.d \
./src/sha1.d \
./src/tas.d \
./src/timeline.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	x86_64-glibc2.12.2-linux-gnu-gcc -DGTK -DGLEW_STATIC -I../src -I/usr/x86_64-glibc2.12.2-linux-gnu/usr/include/SDL -O3 -Wall -ffast-math -msse2 -mfpmath=sse -c -fmessage-length=0 -finline-functions -Winline `x86_64-glibc2.12.2-linux-gnu-pkg-config --cflags gtk+-2.0` -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


