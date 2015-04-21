all: compile_all_threads
	gcc -o mips_pthreads_simulator \include/src/main.c \include/obj/a.o \include/obj/alu.o \include/obj/alu_control.o \include/obj/aluout_register.o \include/obj/and_or.o \include/obj/b.o \include/obj/control_unit.o \include/obj/instruction_register.o \include/obj/main_memory.o \include/obj/memory_data_register.o \include/obj/mux_2_alusrca.o \include/obj/mux_2_iord.o \include/obj/mux_2_memtoreg.o \include/obj/mux_2_regdst.o \include/obj/mux_3_pcsource.o \include/obj/mux_4_alusrcb.o \include/obj/pc_shift_left.o \include/obj/program_counter.o \include/obj/register_bank.o \include/obj/shift_left_after_se.o \include/obj/sign_extend.o -lpthread

compile_all_threads:
	gcc -c \include/src/a.c -o \include/obj/a.o
	gcc -c \include/src/alu.c -o \include/obj/alu.o
	gcc -c \include/src/alu_control.c -o \include/obj/alu_control.o
	gcc -c \include/src/aluout_register.c -o \include/obj/aluout_register.o
	gcc -c \include/src/and_or.c -o \include/obj/and_or.o
	gcc -c \include/src/b.c -o \include/obj/b.o
	gcc -c \include/src/control_unit.c -o \include/obj/control_unit.o
	gcc -c \include/src/instruction_register.c -o \include/obj/instruction_register.o
	gcc -c \include/src/main_memory.c -o \include/obj/main_memory.o
	gcc -c \include/src/memory_data_register.c -o \include/obj/memory_data_register.o
	gcc -c \include/src/mux_2_alusrca.c -o \include/obj/mux_2_alusrca.o
	gcc -c \include/src/mux_2_iord.c -o \include/obj/mux_2_iord.o
	gcc -c \include/src/mux_2_memtoreg.c -o \include/obj/mux_2_memtoreg.o
	gcc -c \include/src/mux_2_regdst.c -o \include/obj/mux_2_regdst.o
	gcc -c \include/src/mux_3_pcsource.c -o \include/obj/mux_3_pcsource.o
	gcc -c \include/src/mux_4_alusrcb.c -o \include/obj/mux_4_alusrcb.o
	gcc -c \include/src/pc_shift_left.c -o \include/obj/pc_shift_left.o
	gcc -c \include/src/program_counter.c -o \include/obj/program_counter.o
	gcc -c \include/src/register_bank.c -o \include/obj/register_bank.o
	gcc -c \include/src/shift_left_after_se.c -o \include/obj/shift_left_after_se.o
	gcc -c \include/src/sign_extend.c -o \include/obj/sign_extend.o
clean:
	rm \include/obj/*.o mips_pthreads_simulator
run:
	./mips_pthreads_simulator
